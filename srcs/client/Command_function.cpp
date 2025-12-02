/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command_function.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 19:48:34 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/02 15:40:23 by ertrigna         ###   ########.fr       */
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
		sendError(464, ":You may not reregistered");
		return ;
	}

	if (_client->isRegistered())
	{
		sendError(462, ":Password needed");
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

	//Recuperer ou creer le channel
	
	//Ajouter le client au channel

	// Envoyer la confirmation au client

	// Envoyer la liste des membres

	// A faire quand on sera sur channel
}

void 	Command::executePrivmsg()
{
	
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
