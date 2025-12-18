/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command_function.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 19:48:34 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/18 17:16:08 by ertrigna         ###   ########.fr       */
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
	if (newNickname.length() > 30)
	{
		sendError(432, newNickname + " :Erroneous nickname (too long)");
		return;
	}
	if (newNickname[0] >= '0' && newNickname[0] <= '9')
	{
		sendError(432, newNickname + " :Erroneous nickname (cannot start with digit)");
		return;
	}
	for (size_t i = 0; i < newNickname.length(); i++)
	{
		char c = newNickname[i];
		bool isLetter = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
		bool isDigit = (c >= '0' && c <= '9');
		bool isSpecial = (c == '[' || c == ']' || c == '\\' || c == '`' || 
		                  c == '_' || c == '^' || c == '{' || c == '|' || c == '}' || c == '-');
		
		if (!isLetter && !isDigit && !isSpecial)
		{
			sendError(432, newNickname + " :Erroneous nickname (invalid characters)");
			return;
		}
	}
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
		std::string welcome = ":Welcome to the IRC Network " + _client->getPrefix();
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
	Channel *channel = _server->getChannel(channelName);
	if (!channel)
	{
		channel = _server->createChannel(channelName, _client);
		if (!channel)
			return; // L'erreur a déjà été envoyée par createChannel
	}
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
	channel->addMember(_client);
	if (channel->isInvited(_client))
		channel->removeInvite(_client);
	std::string joinMsg = ":" + _client->getPrefix() + " JOIN " + channelName;
	channel->broadcast(joinMsg, NULL);
	if(_server->getBot())
		_server->getBot()->onUserJoin(channel, _client);
	if (!channel->getTopic().empty())
		sendReply(332, channelName + " :" + channel->getTopic());
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
		std::string ircMessage = ":" + _client->getPrefix() + " PRIVMSG " + target + " :" + message;
		_server->sendToUser(recipient, ircMessage);
	}
}

void 	Command::executeQuit()
{
	std::string leaveMsg = "Client Quit";
	
	if (_params.size() > 0)
		leaveMsg = _params[0];
	std::string quitMessage = ":" + _client->getPrefix() + " QUIT :" + leaveMsg;
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
	if (!checkParamSize(2, "INVITE"))
		return;
	std::string targetNick = _params[0];
	std::string channelName = _params[1];
	Channel* channel = getChannelOrError(channelName);
	if (!channel)
		return;
	if (!checkChannelMembership(channel, channelName))
		return;
	if (channel->isInviteOnly() && !checkChannelOperator(channel, channelName))
		return;
	Client* target = findClientByNick(targetNick);
	if (!target)
		return;
	if (channel->isMember(target)){
		sendError(443, targetNick + " " + channelName + " :is already on channel");
		return;
	}
	channel->addInvite(target);
	std::string inviteMsg = ":" + _client->getPrefix() + " INVITE " + targetNick + " " + channelName;
	_server->sendToUser(target, inviteMsg);
	sendReply(341, targetNick + " " + channelName);
}

void		Command::executeTopic()
{
	if (!checkParamSize(1, "TOPIC"))
		return;
	std::string channelName = _params[0];
	Channel* channel = getChannelOrError(channelName);
	if (!channel)
		return;
	if (!checkChannelMembership(channel, channelName))
		return;
	if (_params.size() == 1)
	{
		std::string currentTopic = channel->getTopic();

		if (currentTopic.empty()){
			sendReply(331, channelName + " :No topic is set");
		} else {
			sendReply(332, channelName + " :" + currentTopic);
		}
		return;
	}
	std::string newTopic = _params[1];
	if (channel->isTopicRestricted() && !checkChannelOperator(channel, channelName))
		return;
	channel->setTopic(newTopic);
	std::string topicMsg = ":" + _client->getPrefix() + " TOPIC " + channelName + " :" + newTopic;
	channel->broadcast(topicMsg, NULL);
}

void	Command::executeMode()
{
	if (!checkParamSize(1, "MODE"))
		return;
	
	std::string channelName = _params[0];
	Channel* channel = getChannelOrError(channelName);
	if (!channel || !checkChannelMembership(channel, channelName))
		return;
	if (_params.size() == 1)
	{
		displayChannelModes(channel, channelName);
		return;
	}
	if (!checkChannelOperator(channel, channelName))
		return;
	std::string modeStr = _params[1];
	size_t paramIdx = 2;
	bool adding = true;
	std::string applied = "";
	std::string appliedParams = "";
	for (size_t i = 0; i < modeStr.length(); ++i)
	{
		char c = modeStr[i];
		
		if (c == '+') { adding = true; continue; }
		if (c == '-') { adding = false; continue; }
		bool success = false;
		if (c == 'i')
			success = applyModeI(channel, adding, applied);
		else if (c == 't')
			success = applyModeT(channel, adding, applied);
		else if (c == 'k')
			success = applyModeK(channel, adding, paramIdx, applied, appliedParams);
		else if (c == 'l')
			success = applyModeL(channel, adding, paramIdx, applied, appliedParams);
		else if (c == 'o')
			success = applyModeO(channel, adding, paramIdx, channelName, applied, appliedParams);
		else
		{
			sendError(472, std::string(1, c) + " :is unknown mode char to me");
			return;
		}
		if (!success)
			return;
	}
	if (!applied.empty())
	{
		std::string modeMsg = ":" + _client->getPrefix() + " MODE " + channelName + " " + applied + appliedParams;
		channel->broadcast(modeMsg, NULL);
	}
}

void	Command::executeWho()
{
	std::map<int, Client*>& clients = _server->getClients();
	sendReply(322, ":=== Connected Users ===");
	int count = 0;
	for (std::map<int, Client*>::iterator it =  clients.begin(); it != clients.end(); ++it)
	{
		Client* user = it->second;
		if (user->isRegistered())
		{
			count++;
			std::ostringstream oss;
			oss << count << ". " << user->getNickname() << " (" << user->getUsername() << "@" << user->getHostname() <<")";
			sendReply(322, ":" + oss.str());
		}
	}
	std::ostringstream total;
	total << ":Total: " << count << " user(s) online";
	sendReply(315, total.str());
}

void	Command::executeList()
{
	sendReply(321, ":=== Available Channels ===");
	std::map<std::string, Channel*>& channels = _server->getChannels();
	if (channels.empty())
		sendReply(322, ":No channels available for the moment");
	else
	{
		int count = 0;
		for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
		{
			Channel* chan = it->second;
			count++;
			std::ostringstream oss;
			oss << count << ". " << chan->getName() << "[ " << chan->getMembersCount() << " user(s)]";
			std::string topic = chan->getTopic();
			if (!topic.empty())
				oss << " - " << topic;
			sendReply(322, ":" + oss.str());
		}
	}
}
