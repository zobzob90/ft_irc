/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command_auth.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 19:48:34 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/18 17:26:42 by ertrigna         ###   ########.fr       */
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
			channel->broadcast(quitMessage, NULL);
	}
	_server->removeClientFromAllChannels(_client);
	_server->markForDisconnect(_client);
}
