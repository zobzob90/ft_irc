/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command_utils.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 20:40:50 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/17 14:30:22 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "Server.hpp"

Channel*	Command::getChannelOrError(const std::string& name)
{
	if (name[0] != '#')
	{
		sendError(403, name + " :No such channel");
		return (NULL);
	}
	Channel* channel = _server->getChannel(name);
	if (!channel)
	{
		sendError(403, name + " :No such channel");
		return (NULL);
	}
	return (channel);
}

bool	Command::checkChannelMembership(Channel* channel, const std::string& channelName)
{
	if (!channel->isMember(_client))
	{
		sendError(442, channelName + " :You're not on that channel");
		return (false);
	}
	return (true);
}

bool Command::checkChannelOperator(Channel* channel, const std::string& channelName)
{
	if (!channel->isOperator(_client))
	{
		sendError(482, channelName + " :You're not channel operator");
		return (false);
	}
	return (true);
}

Client*	Command::findClientByNick(const std::string& nickname)
{
	std::map<int, Client*>& clients = _server->getClients();
	for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
		if (it->second->getNickname() == nickname)
			return (it->second);
	sendError(401, nickname + " :No such nick/channel");
	return (NULL);
}

void Command::removeFromChannelAndCleanup(Channel* channel, Client* client, const std::string& channelName)
{
	channel->removeMember(client);

	if (channel->getMembersCount() == 0)
		_server->destroyChannel(channelName);
}

bool Command::checkParamSize(size_t required, const std::string& cmdName)
{
	if (_params.size() < required)
	{
		if (cmdName == "PASS" || cmdName == "NICK" || cmdName == "USER" || 
			cmdName == "JOIN" || cmdName == "PART" || cmdName == "KICK" || 
			cmdName == "INVITE" || cmdName == "TOPIC" || cmdName == "MODE")
		{
			sendError(461, cmdName + " :Not enough parameters");
		}
		else if (cmdName == "PRIVMSG")
		{
			if (required == 1)
				sendError(411, ":No recipient given (PRIVMSG)");
			else
				sendError(412, ":No text to send");
		}
		return (false);
    }
    return (true);
}

void	Command::sendError(int code, const std::string& message)
{
	std::ostringstream oss;

	oss << ":" << "server" << " " << code << " ";
	if (!_client->getNickname().empty())
		oss << _client->getNickname() << " ";
	else
		oss << "* ";
	oss << message;
	std::string response = oss.str();
	_server->sendToUser(_client, response);
}

void	Command::sendReply(int code, const std::string& message)
{
	std::ostringstream oss;

	oss << ":" << "\033[1;31m[IRC]\033[0m" << " ";
	if (code < 10)
		oss << "00" << code;
	else if (code < 100)
		oss << "0" << code;
	else
		oss << code;
	oss << " " << _client->getNickname() << " " << message;
	std::string response = oss.str();	
	_server->sendToUser(_client, response);
}

// =============== MODE HELPERS ===============

void Command::displayChannelModes(Channel* channel, const std::string& channelName)
{
	std::string modes = "+";
	std::string params = "";

	if (channel->isInviteOnly()) 
		modes += "i";
	if (channel->isTopicRestricted()) 
		modes += "t";
	if (channel->hasPassword()) 
	{
		modes += "k";
		params += " " + channel->getPass();
	}
	if (channel->hasUserLimit())
	{
		modes += "l";
		std::ostringstream oss;
		oss << channel->getUserLimit();
		params += " " + oss.str();
	}
	
	sendReply(324, channelName + " " + modes + params);
}

bool Command::applyModeI(Channel* channel, bool adding, std::string& applied)
{
	channel->setInviteOnly(adding);
	applied += (adding ? "+" : "-");
	applied += "i";
	return true;
}

bool Command::applyModeT(Channel* channel, bool adding, std::string& applied)
{
	channel->setTopicRestricted(adding);
	applied += (adding ? "+" : "-");
	applied += "t";
	return true;
}

bool Command::applyModeK(Channel* channel, bool adding, size_t& paramIdx, std::string& applied, std::string& appliedParams)
{
	if (adding)
	{
		if (paramIdx >= _params.size())
		{
			sendError(461, "MODE +k :Not enough parameters");
			return false;
		}
		channel->setPassword(_params[paramIdx]);
		applied += "+k";
		appliedParams += " " + _params[paramIdx++];
	}
	else
	{
		channel->setPassword("");
		applied += "-k";
	}
	return true;
}

bool Command::applyModeL(Channel* channel, bool adding, size_t& paramIdx, std::string& applied, std::string& appliedParams)
{
	if (adding)
	{
		if (paramIdx >= _params.size())
		{
			sendError(461, "MODE +l :Not enough parameters");
			return false;
		}
		int limit = atoi(_params[paramIdx].c_str());
		if (limit <= 0)
		{
			sendError(461, "MODE +l :Invalid limit");
			return false;
		}
		channel->setUserLimit(limit);
		applied += "+l";
		appliedParams += " " + _params[paramIdx++];
	}
	else
	{
		channel->setUserLimit(0);
		applied += "-l";
	}
	return true;
}

bool Command::applyModeO(Channel* channel, bool adding, size_t& paramIdx, const std::string& channelName, std::string& applied, std::string& appliedParams)
{
	if (paramIdx >= _params.size())
	{
		sendError(461, "MODE +o/-o :Not enough parameters");
		return false;
	}
	Client* target = findClientByNick(_params[paramIdx]);
	if (!target)
		return false;
	if (!channel->isMember(target))
	{
		sendError(441, _params[paramIdx] + " " + channelName + " :They aren't on that channel");
		return false;
	}
	if (adding)
		channel->addOperator(target);
	else
		channel->removeOperator(target);
	applied += (adding ? "+" : "-");
	applied += "o";
	appliedParams += " " + _params[paramIdx++];
	return true;
}
