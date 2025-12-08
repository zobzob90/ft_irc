/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/20 14:10:07 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/08 14:24:57 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <vector>
#include <sstream>
#include <sys/socket.h>
#include "Client.hpp"

class Server;

class Command
{
	private:
		Server* _server;
		Client* _client;
		std::string _command;
		std::vector<std::string> _params;

		void	parse(const std::string& message);
		
		// Commande pour authentification
		void	executePass();
		void	executeNick();
		void	executeUser();

		// Commande de channel
		void	executeJoin();
		void	executePart();
		void	executePrivmsg();
		void	executeKick();
		void	executeInvite();
		void	executeTopic();
		void	executeMode();
		
		// Utils
		void	executeQuit();

		void	sendReply(int code, const std::string& message);
		void	sendError(int code, const std::string& message);

	public:
		Command(Server* server, Client* client, const std::string message) : _server(server), _client(client), _command(""), _params()
		{
			parse(message);
		}
		~Command() {}

		

		void	execute();
};

#endif