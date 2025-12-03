/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_Management.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 17:52:17 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/03 14:05:57 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Command.hpp"

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
	
	pollfd p;
	
	p.fd = clientFd;
	p.events = POLLIN; // POLLIN  surveille les donnees entrante a lire
	p.revents = 0; // initialise le champ de retour a 0
	_pollFds.push_back(p);
	
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

	for (size_t i = 0; i < _pollFds.size(); i++)
	{
		if (_pollFds[i].fd == fd)
		{
			_pollFds.erase(_pollFds.begin() + i);
			break;
		}
	}

	close (fd);
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

		if (message.empty())
			continue ;
		std::cout << "[fd " << fd << "] " << message << std::endl;
		Command cmd(this, client, message);
		cmd.execute();
	}
}

void	Server::run()
{
	pollfd p;
	p.fd = _serverSocket;
	p.events = POLLIN;
	p.revents = 0;
	_pollFds.push_back(p);

	while (true)
	{
		int ret = poll(_pollFds.data(), _pollFds.size(), -1);
		if (ret < 0)
			throw std::runtime_error("poll() failed");
		
		for (size_t i = 0; i < _pollFds.size(); ++i)
		{
			pollfd &pfd = _pollFds[i];
			
			if (pfd.revents & POLLIN)
			{
				if (pfd.fd == _serverSocket)
					handleNewConnection();
				else
					handleClientMessage(pfd.fd);
			}
			if (pfd.revents & (POLLHUP | POLLERR))
				removeClient(pfd.fd);
		}
	}
}
