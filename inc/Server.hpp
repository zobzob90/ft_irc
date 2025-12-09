/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 17:52:20 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/09 17:13:26 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <map>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#include "Client.hpp"
#include "Channel.hpp"

class Bot;

class Server
{
	private:
	
		int								_serverSocket; // <- socket principal
		int								_port; // <- port
		std::string						_password; // <- password
		std::vector<pollfd>				_pollFds;
		std::map<int, Client*> 			_clients;
		std::map<std::string, Channel*> _channels;
		Bot* 							_bot;

		void	createSocket();
		void	configAddr();
		void	setUpServerSocket();

	public:
	
		//CONSTRUCTEUR + DESTRUCTEUR
		Server(int port, const std::string &password);
        virtual	~Server();
		
		// GETTER
		std::string							getPassword() const {return _password; }
		std::map<int, Client*>&				getClients() {return _clients; }
		std::map<std::string, Channel*>&	getChannels() {return _channels; }
		Bot*								getBot() const {return _bot; }

		//CHANNEL MANAGEMENTS : Creer / Gerer / Detruire
		Channel*	getChannel(const std::string& name);
		Channel*	createChannel(const std::string& name, Client *creator);
		void		destroyChannel(const std::string& name);
		
		//CHANNEL UTILITIES
		void		broadcastToUserChannels(Client* clients, const std::string msg);
		void		removeClientFromAllChannels(Client* client);
		void		markForDisconnect(Client* client);

		//SERVER MANAGEMENTS
		void		signalHandler(int signum); // fonction pour gerer les signaux
		void		closeServer(); // fonction pour fermer le server
		void		sendToUser(Client* user, const std::string& msg);
		void		handleNewConnection(); // gerer la connection
		void		handleClientMessage(int fd); // gerer les messages
		void		removeClient(int fd); // enlever un client
		void		run(); // fonction principale pour runner le server
} ;

#endif