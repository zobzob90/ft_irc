/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 17:52:17 by ertrigna          #+#    #+#             */
/*   Updated: 2025/11/19 18:09:22 by ertrigna         ###   ########.fr       */
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
	
	Client* newClient = new Client(clientFd);

	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &clientAddr.sin_addr, ip, INET_ADDRSTRLEN);
	newClient->setHostname(ip);

	_clients[clientFd] = newClient;
	
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

	std::map<int, Client*>::iterator it = _clients.find(fd);
	if (it != _clients.end())
	{
		delete it->second;
		_clients.erase(it);
	}

	close (fd);
	FD_CLR(fd, &_readFds);

	if (fd == _maxFd)
	{
		_maxFd = _serverSocket;
		for (int i = _serverSocket +1; i < fd; ++i)
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

	Client* client = _clients[fd];
	
	client->appendBuffer(buffer);

	while (client->hasCompleteMessage())
	{
		std::string message = client->extractMessage();
		std::cout << "[fd " << fd << "] " << message << std::endl;
		//ADD PARSER ET EXECUTER CMD IRC

		std::string reponse = message + "\r\n";
		send(fd, reponse.c_str(), reponse.length(), 0);
	}
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
