/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/20 14:10:07 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/09 19:18:57 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <vector>
#include <sstream>
#include <sys/socket.h>
#include "Client.hpp"
#include "Channel.hpp"

class Server;

class Command
{
	private:
		Server* _server;
		Client* _client;
		std::string _command;
		std::vector<std::string> _params;

		void		parse(const std::string& message);
		
		// Commande utils (factorisation)
		Channel*	getChannelOrError(const std::string& name);
		bool		checkChannelMembership(Channel *channel, const std::string& channelName);
		bool		checkChannelOperator(Channel* channel, const std::string& channelName);
		Client*		findClientByNick(const std::string& nickname);
		void		removeFromChannelAndCleanup(Channel* channel, Client* client, const std::string& channelName);
		bool		checkParamSize(size_t required, const std::string& cmdName);

		// MODE helpers
		void		displayChannelModes(Channel* channel, const std::string& channelName);
		bool		applyModeI(Channel* channel, bool adding, std::string& applied);
		bool		applyModeT(Channel* channel, bool adding, std::string& applied);
		bool		applyModeK(Channel* channel, bool adding, size_t& paramIdx, std::string& applied, std::string& appliedParams);
		bool		applyModeL(Channel* channel, bool adding, size_t& paramIdx, std::string& applied, std::string& appliedParams);
		bool		applyModeO(Channel* channel, bool adding, size_t& paramIdx, const std::string& channelName, std::string& applied, std::string& appliedParams);

		// Commande pour authentification
		void		executePass();
		void		executeNick();
		void		executeUser();
		
		// Commande de channel
		void		executeJoin();
		void		executePart();
		void		executePrivmsg();
		void		executeKick();
		void		executeInvite();
		void		executeTopic();
		void		executeMode();
		
		// Utils
		void		executeQuit();

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