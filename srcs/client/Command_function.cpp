/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command_function.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 19:48:34 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/03 15:57:32 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "Server.hpp"

// AUTHENT PART
void	Command::executePass()
{
	if (_params.size() < 1)
	{
		sendError(461, "PASS: Not enough parameters");
		return ;
	}

	if (_client->isAuthenticated())
	{
		sendError(462, ": You may not reregistered");
		return ;
	}

	std::string password = _params[0];
	if (password == _server->getPassword())
	{
		_client->setAuthenticated(true);
	}
	else
		sendError(464, ": Password Incorrect");
}

void Command::executeNick()
{
	if (_params.size() < 1)
	{
		sendError(461, "NICK: Not enough parameters");
		return ;
	}

	std::string newNickname = _params[0];
	
	if (newNickname.empty())
	{
		sendError(431, ":No Nickname Given");
		return ;
	}

	//TODO -> Verifier doublon de Nick

	_client->setNickname(newNickname);
	if (_client->isAuthenticated() && !_client->getUsername().empty() && !_client->isRegistered())
	{
		_client->setRegistered(true);
		std::string welcome = "Welcome to the IRC Network " + _client->getPrefix();
		sendReply(1, welcome);
	}
}

void Command::executeUser()
{
	if (_params.size() < 4)
	{
		sendError(461, "USER: Not enough parameters");
		return ;
	}

	if (!_client->isAuthenticated())
	{
		sendError(464, ":Password Incorrect !");
		return ;
	}

	if (_client->isRegistered())
	{
		sendError(462, ":Password needed !");
		return ;
	}

	if (_client->getNickname().empty())
	{
		sendError(431, ":No nickname given");
		return ;
	}	

	std::string username = _params[0];
	std::string realname = _params[3];

	_client->setUsername(username);
	_client->setRealname(realname);
	_client->setRegistered(true);

	std::string welcome = ":Welcome to IRC " + _client->getPrefix();
	sendReply(1, welcome);
}

// CHANNEL PART
void	Command::executeJoin()
{
	if (_params.size() < 1)
	{
		sendError(461, "JOIN: Not enough parameters");
		return ;
	}

	std::string channelName = _params[0];
	std::string key = (_params.size() >= 2) ? _params[1] : "";
	
	if (channelName[0] != '#')
	{
		sendError(403, channelName + " :No such channel");
		return ;
	}

	// Recuperer ou creer le channel
	Channel *channel = _server->getChannel(channelName);
	if (!channel)
		channel = _server->createChannel(channelName, _client);
	
	// Verification d'accès
	if (channel->isInviteOnly() && !channel->isInvited(_client)){
		sendError(473, channelName + " :Cannot join channel (+i)");
		return;
	}

	if (channel->hasPassword() && key != channel->getPass()){
		sendError(475, channelName + " :Cannot join channel (+k)");
		return;
	}

	if (channel->hasUserLimit() && channel->getMembersCount() >= channel->getUserLimit()){
		sendError(471, channelName + " :Cannot join channel (+l)");
		return;
	}
	
	// Ajouter le client au channel
	channel->addMember(_client);

	// Envoyer la confirmation à tous (y compris le client)
	std::string joinMsg = ";" + _client->getPrefix() + " JOIN " + channelName;
	channel->broadcast(joinMsg, NULL); //NULL = envoyer à tous

	//Envoyer le topic s'il existe
	if (!channel->getTopic().empty()){
		sendReply(332, channelName + " :" + channel->getTopic());
	}

	// Envoyer la liste des membres
	sendReply(353, "= " + channelName + " :" + channel->getMemberList());
	sendReply(366, channelName + "End of /NAMES list");

}

void 	Command::executePrivmsg()
{
	
	// Verifier les paramètres (411, 412)
	if (_params.size() < 1){
		sendError(411, ":No recipient given (PRIVMSG)");
		return;
	}
	if (_params.size() < 2){
		sendError(412, ":No text to send");
		return;
	}

	// Extraire target et message
	std::string target = _params[0];
	std::string message = _params[0];

	// Accéder aux clients du serveur
	std::map<int, Client*>& clients = _server->getClients();

	// Déterminer si c'est un channel ou un user
	if (target[0] == '#'){
		// CAS CHANNEL
		// - Récupérer le channel (401 si inexistant)
		Channel* channel = _server->getChannel(target);
		if (!channel){
			sendError(401, target + " :No such nick/channel");
			return;
		}
		// - Vérifier membership (404 si non membre)
		if (!channel->isMember(_client)){
			sendError(404, target + " :Cannot send to channel");
			return;
		}
		// - Broadcast au channel (exclude l'expéditeur)
		std::string ircMessage = _client->getPrefix() + " PRIVMSG " + target + " :" + message;
		channel->broadcast(ircMessage, _client);
	}else{
		// CAS USER
		// - Trouver le destinataire dans _clients (401 si inexistant)
		std::map<int, Client*>& clients = _server->getClients();
		Client* recipient = NULL;

		for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it){
			if (it->second->getNickname() == target){
				recipient = it->second;
				break;
			}
		}

		if (!recipient){
			sendError(401, target + " :No such nick/channel");
			return;
		}

		// - Envoyer le message directement
		std::string ircMessage = _client->getPrefix() + " PRIVMSG " + target + " :" + message + "\r\n";
		send(recipient->getFd(), ircMessage.c_str(), ircMessage.length(), 0);
	}
}

void 	Command::executeQuit()
{
	std::string leaveMsg = "Client Quit";
	
	if (_params.size() > 0)
		leaveMsg = _params[0];
	
	//Broadcast aux channels ou le client est present
	
	//Retirer le client de tous ses channels
	
	// Marquer le client pour deconnexion
}
