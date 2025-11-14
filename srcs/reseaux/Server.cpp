/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 17:52:17 by ertrigna          #+#    #+#             */
/*   Updated: 2025/11/14 15:33:31 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

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
	if (fd < 0 || fd == _serverSocket)
		return ;
	std::cout << "❌ Client disconnected (fd=" << fd << ")" << std::endl;
	// Fermer la connexion
	close(fd);
	// Retirer du set de lecture
	FD_CLR(fd, &_readFds);
	// Recalculer _maxFd si nécessaire
	if (fd == _maxFd)
	{
		_maxFd = _serverSocket;
		for (int i = _serverSocket + 1; i < fd; ++i)
		{
			if (FD_ISSET(i, &_readFds))
				_maxFd = i;
		}
	}
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
	if (send(fd, buffer, bytes, 0) < 0)
		removeClient(fd); 
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
