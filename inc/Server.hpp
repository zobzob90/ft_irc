/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 17:52:20 by ertrigna          #+#    #+#             */
/*   Updated: 2025/11/14 15:01:07 by ertrigna         ###   ########.fr       */
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

class Server
{
	private:
	
		int			_serverSocket; // <- socket principal
		int			_port; // <- port
		std::string	_password; // <- password
		fd_set		_readFds; // <- Fd lu
		int			_maxFd; // <- Nombre max de Fd

		void	createSocket();
		void	configAddr();
		void	setUpServerSocket();

	public:
	
		Server(int port, const std::string &password): _port(port), _password(password) 
		{
			setUpServerSocket(); // <- prepare le socket ici
		};

		virtual	~Server()
		{
			if (_serverSocket != -1)
				close (_serverSocket);
		};
		
		void	handleNewConnection();
		void	handleClientMessage(int fd);
		void	removeClient(int fd);
		void	run(); // fonction principale pour runner le serveur
} ;

#endif