/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command_function.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 19:48:34 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/09 17:59:35 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "Server.hpp"
#include "Bot.hpp"

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
	
	// Gestion doublon de Nickname
	std::map<int, Client*>& clients = _server->getClients();
	for (std::map<int, Client *>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second != _client && it->second->getNickname() == newNickname)
		{
			sendError(433, newNickname + " :Nickname is already use please take another one !");
			return ;
		}
	}
	
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
	if (channel->isInviteOnly() && !channel->isInvited(_client))
	{
		sendError(473, channelName + " :Cannot join channel (+i)");
		return;
	}

	if (channel->hasPassword() && key != channel->getPass())
	{
		sendError(475, channelName + " :Cannot join channel (+k)");
		return;
	}

	if (channel->hasUserLimit() && channel->getMembersCount() >= channel->getUserLimit())
	{
		sendError(471, channelName + " :Cannot join channel (+l)");
		return;
	}
	
	// Ajouter le client au channel
	channel->addMember(_client);

	// Envoyer la confirmation à tous (y compris le client)
	std::string joinMsg = ":" + _client->getPrefix() + " JOIN " + channelName;
	channel->broadcast(joinMsg, NULL); //NULL = envoyer à tous

	if(_server->getBot())
		_server->getBot()->onUserJoin(channel, _client);

	//Envoyer le topic s'il existe
	if (!channel->getTopic().empty())
	{
		sendReply(332, channelName + " :" + channel->getTopic());
	}

	// Envoyer la liste des membres
	sendReply(353, "= " + channelName + " :" + channel->getMemberList());
	sendReply(366, channelName + " :End of /NAMES list");

}

void 	Command::executePrivmsg()
{
	
	// Verifier les paramètres (411, 412)
	if (_params.size() < 1)
	{
		sendError(411, ":No recipient given (PRIVMSG)");
		return;
	}
	if (_params.size() < 2)
	{
		sendError(412, ":No text to send");
		return;
	}

	// Extraire target et message
	std::string target = _params[0];
	std::string message = _params[1];
	// Déterminer si c'est un channel ou un user
	if (target[0] == '#')
	{
		// CAS CHANNEL
		// - Récupérer le channel (401 si inexistant)
		Channel* channel = _server->getChannel(target);
		if (!channel)
		{
			sendError(401, target + " :No such nick/channel");
			return;
		}
		// - Vérifier membership (404 si non membre)
		if (!channel->isMember(_client))
		{
			sendError(404, target + " :Cannot send to channel");
			return;
		}
        Bot* bot = _server->getBot();
        if (bot)
            bot->onMessage(channel, _client, message);

		// - Broadcast au channel (exclude l'expéditeur)
		std::string ircMessage = ":" + _client->getPrefix() + " PRIVMSG " + target + " :" + message;
		channel->broadcast(ircMessage, _client);
	}
	else
	{
		// CAS USER
		// - Trouver le destinataire dans _clients (401 si inexistant)
		std::map<int, Client*>& clients = _server->getClients();
		Client* recipient = NULL;

		for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
		{
			if (it->second->getNickname() == target)
			{
				recipient = it->second;
				break;
			}
		}

		if (!recipient)
		{
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

	// Construire le message QUIT au format IRC
	std::string quitMessage = _client->getPrefix() + " QUIT : " + leaveMsg;
	
	//Broadcast aux channels où le client est present
	std::map<std::string, Channel*>& channels = _server->getChannels();
	for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it){
		Channel* channel = it->second;
		if (channel->isMember(_client)){
			channel->broadcast(quitMessage, NULL); // Envoyer à tous (même au client qui quitte)
		}
	}
	
	//Retirer le client de tous ses channels
	_server->removeClientFromAllChannels(_client);

	// Marquer le client pour deconnexion
	_server->markForDisconnect(_client);
}

void	Command::executePart()
{
	if (_params.size() < 1)
	{
		sendError(461, "PART :Not enough parameters");
		return;
	}

	// Extraire le nom du channel et le message optionnel
	std::string channelName = _params[0];
	std::string partMessage = "Leaving"; // Message par défaut
	if (_params.size() >= 2)
		partMessage = _params[1];

	// Vérifier que c'est bien un channel
	if (channelName[0] != '#')
	{
		sendError(403, channelName + " :No such channel");
		return;
	}

	// Récupérer le channel
	Channel* channel = _server->getChannel(channelName);
	if (!channel)
	{
		sendError(403, channelName + " :No such channel");
		return;
	}

	// Vérifier que le client est membre du channel
	if (!channel->isMember(_client))
	{
		sendError(442, channelName + " :You're not on that channel");
		return;
	}

	// Construire le message PART au format IRC
	std::string partMsg = _client->getPrefix() + " PART " + channelName + " :" + partMessage;

	// Broadcaster à tous les membres (y compris celui qui part)
	channel->broadcast(partMsg, NULL);

	// Retirer le client du channel
	channel->removeMember(_client);

	// Supprimer le channel, si il est vide
	if (channel->getMembersCount() == 0)
		_server->destroyChannel(channelName);
}

void	Command::executeKick()
{
	// Vérification du nombre de paramètres
	if (_params.size() < 2){

	}
	// Extraire les paramètres
	std::string channelName = _params[0];
	std::string targetNick = _params[1];
	std::string reason = "No reason given";

	if (_params.size() >= 3)
		reason = _params[2];

	// Vérifier que c'est bien un channel
	if (channelName[0] != '#')
	{
		sendError(403, channelName + " :No such channel");
		return;
	}

	// Récupérer le Channel
	Channel* channel = _server->getChannel(channelName);
	if (!channel)
	{
		sendError(403, channelName + " :Nos such channel");
		return;
	}

	// Vérifier que l'expéditeur est membre du channel
	if (!channel->isMember(_client)){
		sendError(442, channelName + " :You're not on that channel");
		return;
	}

	// Vérifier que l'expéditeur est opérateur
	if (!channel->isOperator(_client)){
		sendError(482, channelName + " :You're not channel operator");
		return;
	}

	// Trouver la cible (le client a kicker)
	std::map<int, Client*>& clients = _server->getClients();
	Client* target = NULL;
	for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second->getNickname() == targetNick){
			target = it->second;
			break;
		}
	}

	// Vérifier que la cible existe
	if (!target)
	{
		sendError(401, targetNick + " :No such nick/channel");
		return;
	}
	
	// Vérifier que la cible est membre du channel
	if (!channel->isMember(target))
	{
		sendError(441, targetNick + " " + channelName + " :They aren't on that channel");
		return;
	}

	// Construire le message KICK au format IRC
	std::string kickMsg = _client->getPrefix() + " KICK " + channelName + " " + targetNick + " :" + reason;

	// Broadcaster à tous les membres (y compris la cible)
	channel->broadcast(kickMsg, NULL);

	// Retirer la cible du channel
	channel->removeMember(target);

	// Si le channel est vide, le supprimer
	if (channel->getMembersCount() == 0)
	{
		_server->destroyChannel(channelName);
	}
}