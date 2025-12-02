/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 17:52:20 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/02 16:15:14 by ertrigna         ###   ########.fr       */
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include "Client.hpp"
#include "Channel.hpp"

class Server
{
	private:
	
		int			_serverSocket; // <- socket principal
		int			_port; // <- port
		std::string	_password; // <- password
		fd_set		_readFds; // <- Fd lu
		int			_maxFd; // <- Nombre max de Fd
		std::map<int, Client*> _clients;
		std::map<std::string, Channel*> _channels; 

		void	createSocket();
		void	configAddr();
		void	setUpServerSocket();

	public:
	
		//CONSTRUCTEUR + DESTRUCTEUR
		Server(int port, const std::string &password): _port(port), _password(password) 
		{
			setUpServerSocket(); // <- prepare le socket ici
		};

		virtual	~Server()
		{
			for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
			{
				close(it->first);
				delete (it->second);
			}
			close (_serverSocket);
		};
		
		// GETTER
		std::string	getPassword() const {return _password; }

		//CHANNEL MANAGEMENTS : Creer / Gerer / Detruire
		Channel*	getChannel(const std::string& name);
		Channel*	createChannel(const std::string& name, Client *creator);
		Channel*	destroyChannel(const std::string& name);
		
		//CHANNEL UTILITIES
		Channel*	broadcastToUserChannels(Client* clients, const std::string msg);
		void		removeClientFromAllChannels(Client* client);
		void		markForDisconnect(Client* client);

		//SERVER MANAGEMENTS
		void	handleNewConnection();
		void	handleClientMessage(int fd);
		void	removeClient(int fd);
		void	run(); // fonction principale pour runner le server
} ;

#endif