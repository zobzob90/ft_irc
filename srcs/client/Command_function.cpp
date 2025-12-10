/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command_function.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 19:48:34 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/09 19:51:16 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "Server.hpp"
#include "Bot.hpp"

// AUTHENT PART
void	Command::executePass()
{
	if (!checkParamSize(1, "PASS"))
		return ;
	if (_client->isAuthenticated())
	{
		sendError(462, ": You may not reregistered");
		return ;
	}
	if (_params[0] == _server->getPassword())
		_client->setAuthenticated(true);
	else
		sendError(464, ": Password Incorrect");
}

void Command::executeNick()
{
	if (!checkParamSize(1, "NICK"))
		return ;
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
	if(!checkParamSize(4, "USER"))
		return;
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
	_client->setUsername(_params[0]);
	_client->setRealname(_params[3]);
	_client->setRegistered(true);

	sendReply(1, ":Welcome to IRC " + _client->getPrefix());
}

// CHANNEL PART
void	Command::executeJoin()
{
	if (!checkParamSize(1, "JOIN"))
		return ;

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
	
	// Si le client avait une invitation, la retirer (elle a été utilisée)
	if (channel->isInvited(_client))
		channel->removeInvite(_client);
	
	// Envoyer la confirmation à tous (y compris le client)
	std::string joinMsg = ":" + _client->getPrefix() + " JOIN " + channelName;
	channel->broadcast(joinMsg, NULL); //NULL = envoyer à tous
	if(_server->getBot())
		_server->getBot()->onUserJoin(channel, _client);
	//Envoyer le topic s'il existe
	if (!channel->getTopic().empty())
		sendReply(332, channelName + " :" + channel->getTopic());
	// Envoyer la liste des membres
	sendReply(353, "= " + channelName + " :" + channel->getMemberList());
	sendReply(366, channelName + " :End of /NAMES list");

}

void	Command::executePrivmsg()
{
	if (!checkParamSize(1, "PRIVMSG"))
		return;
	if (!checkParamSize(2, "PRIVMSG"))
		return;
	std::string target = _params[0];
	std::string message = _params[1];

	if (target[0] == '#')
	{
		Channel* channel = getChannelOrError(target);
		if (!channel || !checkChannelMembership(channel, target))
			return;
		if (_server->getBot())
			_server->getBot()->onMessage(channel, _client, message);
		channel = _server->getChannel(target);
		if (!channel)
			return ;
		if (!channel->isMember(_client))
			return;
		std::string ircMessage = ":" + _client->getPrefix() + " PRIVMSG " + target + " :" + message;
		channel->broadcast(ircMessage, _client);
	}
	else
	{
		Client* recipient = findClientByNick(target);
		if (!recipient)
			return;
		std::string ircMessage = ":" + _client->getPrefix() + " PRIVMSG " + target + " :" + message + "\r\n";
		send(recipient->getFd(), ircMessage.c_str(), ircMessage.length(), 0);
	}
}

void 	Command::executeQuit()
{
	std::string leaveMsg = "Client Quit";
	
	if (_params.size() > 0)
		leaveMsg = _params[0];
	std::string quitMessage = _client->getPrefix() + " QUIT : " + leaveMsg;
	std::map<std::string, Channel*>& channels = _server->getChannels();
	for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		Channel* channel = it->second;
		if (channel->isMember(_client))
			channel->broadcast(quitMessage, NULL); // Envoyer à tous (même au client qui quitte)
	}
	_server->removeClientFromAllChannels(_client);
	_server->markForDisconnect(_client);
}

void	Command::executePart()
{
	if (!checkParamSize(1, "PART"))
		return;
	std::string channelName = _params[0];
	std::string partMessage = "Leaving";
	if (_params.size() >= 2)
		partMessage = _params[1];
	Channel* channel = getChannelOrError(channelName);
	if (!channel || !checkChannelMembership(channel, channelName))
		return;
	std::string partMsg = ":" + _client->getPrefix() + " PART " + channelName + " :" + partMessage;
	channel->broadcast(partMsg, NULL);
	removeFromChannelAndCleanup(channel, _client, channelName);
}

void	Command::executeKick()
{
    if (!checkParamSize(2, "KICK"))
		return;
	std::string channelName = _params[0];
	std::string targetNick = _params[1];
	std::string reason = "No reason given";
	
	if (_params.size() >= 3)
		reason = _params[2];
	Channel* channel = getChannelOrError(channelName);
	if (!channel || !checkChannelMembership(channel, channelName) || !checkChannelOperator(channel, channelName))
		return;
	Client* target = findClientByNick(targetNick);
	if (!target)
		return;
	if (!channel->isMember(target))
	{
		sendError(441, targetNick + " " + channelName + " :They aren't on that channel");
		return;
	}
	std::string kickMsg = ":" + _client->getPrefix() + " KICK " + channelName + " " + targetNick + " :" + reason;
	channel->broadcast(kickMsg, NULL);
	removeFromChannelAndCleanup(channel, target, channelName);
}

void	Command::executeInvite()
{
	// Vérifier les paramètres : INVITE <nickname> <channel>
	if (!checkParamSize(2, "INVITE"))
		return;
	
	std::string targetNick = _params[0];
	std::string channelName = _params[1];

	// Vérifier que le channel existe et commence par #
	Channel* channel = getChannelOrError(channelName);
	if (!channel)
		return;

	// Vérifier que l'inviteur est membre du channel
	if (!checkChannelMembership(channel, channelName))
		return;

	// Si le channel est en mode +i (invite-only), seuls les OPs peuvent inviter
	if (channel->isInviteOnly() && !checkChannelOperator(channel, channelName))
		return;

	// Trouver l'utilisateur cible
	Client* target = findClientByNick(targetNick);
	if (!target)
		return;

	// Vérifier que la cible n'est pas déjà sur le channel
	if (channel->isMember(target)){
		sendError(443, targetNick + " " + channelName + " :is already on channel");
		return;
	}

	// Ajouter l'invitation
	channel->addInvite(target);

	// Envoyer la notification à la cible
	std::string inviteMsg = ":" + _client->getPrefix() + " INVITE " + targetNick + " " + channelName;
	_server->sendToUser(target, inviteMsg);

	// Confirmer à l'inviteur (code 341)
	sendReply(341, targetNick + " " + channelName);
}

void		Command::executeTopic(){
	// Vérifier qu'on a au moins le nom du channel
	if (!checkParamSize(1, "TOPIC"))
		return;
	std::string channelName = _params[0];

	// Vérifier que le channel existe et commence par #
	Channel* channel = getChannelOrError(channelName);
	if (!channel)
		return;

	// Vérifier que le client est membre du channel
	if (!checkChannelMembership(channel, channelName))
		return;
	
	// CAS 1 : Afficher le tropic actuel (pas de 2ème paramètre)
	if (_params.size() == 1){
		std::string currentTopic = channel->getTopic();

		if (currentTopic.empty()){
			sendReply(331, channelName + " :No topic is set");
		} else {
			sendReply(332, channelName + " :" + currentTopic);
		}
		return;
	}

	// CAS 2 : Modifier le topic (_params.size() >= 2)
	std::string newTopic = _params[1];

	// Si le channel a le mode +t (topicRestrict), seuls les OPs peuvent changer
	if (channel->isTopicRestricted() && !checkChannelOperator(channel, channelName))
		return;
	
	// Changer le topic
	channel->setTopic(newTopic);

	// Notifier tous les membres du channel du changement de topic
	std::string topicMsg = ":" + _client->getPrefix() + " TOPIC " + channelName + " : " + newTopic;
	channel->broadcast(topicMsg, NULL);
}

void		Command::executeMode(){
	// a coder
}

