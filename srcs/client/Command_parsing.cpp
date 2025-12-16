/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command_parsing.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 16:41:24 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/16 13:25:44 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include <iostream>

void Command::parse(const std::string& message)
{
	std::istringstream	iss(message);
	std::string			word;

	bool hasTrailing = false;

	if (iss >> _command)
	{
		for (size_t i = 0; i < _command.length(); ++i)
			_command[i] = std::toupper(_command[i]);
	}

	// Extraire le parametre
	while (iss >> word)
	{
		if (word[0] == ':')
		{
			//Extraire tout le reste de la ligne
			hasTrailing = true;
			std::string trailing = word.substr(1); // Enleve le ':'
			std::string	rest;
			std::getline(iss, rest);
			if (!rest.empty())
				trailing += rest;
			_params.push_back(trailing);
			break;
		}
		_params.push_back(word);
	}

	if (_command == "PRIVMSG")
	{
		if(_params.size() != 2 || !hasTrailing)
		{
			_params.clear();
			_command = "";
			return ;
		}
	}
}

void Command::execute()
{
	if (_command.empty())
	{
		sendError(461, "PRIVMSG :Bad message format");
		return ;
	}
	// Cmd pour authentification
	if (_command == "PASS")
		executePass();
	else if (_command == "USER")
		executeUser();
	else if (_command == "NICK")
		executeNick();
	// Cmd necessitant d'etre authentifie
	else if (!_client->isAuthenticated() || !_client->isRegistered())
	{
		sendError(451, "You have not registered");
		return ;
	}
	
	// Cmd de Channel
	else if (_command == "JOIN")
		executeJoin();
	else if (_command == "PART")
		executePart();
	else if (_command == "PRIVMSG")
		executePrivmsg();
	else if (_command == "INVITE")
		executeInvite();
	else if (_command == "TOPIC")
		executeTopic();
	else if (_command == "MODE")
		executeMode();
	else if (_command == "KICK")
		executeKick();
	else if (_command == "QUIT")
		executeQuit();
	else
		sendError(421, _command + " :Unknown command");
}
