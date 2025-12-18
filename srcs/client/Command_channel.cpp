/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command_channel.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 17:24:03 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/18 17:26:01 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "Server.hpp"
#include "Bot.hpp"

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
			return;
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