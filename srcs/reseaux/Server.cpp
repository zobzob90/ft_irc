/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 17:52:17 by ertrigna          #+#    #+#             */
/*   Updated: 2025/11/13 14:32:13 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::setUpServerSocket()
{
	_serverSocket = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP); // AF_INET(domain en IPv4), SOCK_STREAM (protocole TCP le plus fiable), IPPROTO_TCP (param par defaut)
	
	if (_serverSocket < 0)
		throw std::runtime_error("Error: socket() failed");
	
	int opt = 1; // valeur d'option de socket (1 = true, 0 =  false)
	if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) // active le mode reutiliser l'adrresse pour ce socket
		throw std::runtime_error("Error: setsocketoption failed");
	
	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(_port);

	if (bind(_serverSocket, (sockaddr *)&addr, sizeof(addr)) < 0)
		throw std::runtime_error("Error: bind() failed");
	
	if (listen(_serverSocket, 10) < 0)
		throw std::runtime_error("Error: listen() failed");
	std::cout << "✅ Server listening on port " << _port << std::endl;
}

void	Server::handleNewConnection()
{
	sockaddr_in clientAddr;
	
	socklen_t len = sizeof(clientAddr);
	int	clientFd = accept(_serverSocket, (sockaddr *)&clientAddr, &len);
	if (clientFd < 0)
		return ;
	
	FD_SET(clientFd, &_readFds);
	if (clientFd > _maxFd)
		_maxFd = clientFd;
	std::cout << "👤 New client connected (fd=" << clientFd << ")" << std::endl;
}

void	Server::removeClient(int fd)
{
	std::cout << "❌ Client disconnected (fd= " << fd << ")" << std::endl;
	close(fd);
	FD_CLR(fd, &_readFds);
}

void	Server::handleClientMessage(int fd)
{
	char	buffer[512];
	int		bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes <= 0)
	{
		removeClient(fd);
		return ;
	}
	
	buffer[bytes] = '\0';
	std::cout << "[fd " << fd << "]" << buffer;
	send(fd, buffer, bytes, 0); 
}

void	Server::run()
{
	FD_ZERO(&_readFds);
	FD_SET(_serverSocket, &_readFds);
	_maxFd = _serverSocket;

	while (true)
	{
		fd_set tmp = _readFds;
		if (select(_maxFd + 1, &tmp, NULL, NULL, NULL) < 0)
			throw std::runtime_error("select() failed");
		
		for (int fd = 0; fd <= _maxFd; ++fd)
		{
			if (FD_ISSET(fd, &tmp))
			{
				if (fd == _serverSocket)
					handleNewConnection();
				else
					handleClientMessage(fd);
			}
		}
	}
}
