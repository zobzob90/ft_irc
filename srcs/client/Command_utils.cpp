/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command_utils.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 20:40:50 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/09 19:18:15 by ertrigna         ###   ########.fr       */
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
	response += "\r\n";
	_server->sendToUser(_client, response);
}

void	Command::sendReply(int code, const std::string& message)
{
	std::ostringstream oss;

	oss << ":" << "server" << " ";
	
	if (code < 10)
		oss << "00" << code;
	else if (code < 100)
		oss << "0" << code;
	else
		oss << code;
	
	oss << " " << _client->getNickname() << " " << message << "\r\n";

	std::string response = oss.str();
	
	response += "\r\n";
	_server->sendToUser(_client, response);
}

