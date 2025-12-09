/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_Management.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 17:52:17 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/09 17:05:24 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Command.hpp"

volatile sig_atomic_t g_stop = 0;

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

void	Server::sendToUser(Client* user, const std::string& msg)
{
	if (!user)
		return ;
	int fd = user->getFd();
	if (fd < 0)
		return ;
	std::string formattedMsg = msg + "\r\n";
	send(fd, formattedMsg.c_str(), formattedMsg.size(), 0);
}

void	Server::removeClient(int fd)
{
	if (fd < 0 || fd == _serverSocket)
		return ;

	std::cout << "❌ Client disconnected (fd=" << fd << ")" << std::endl;
	close(fd);

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

void	Server::signalHandler(int signum)
{
	if (signum == SIGINT || signum == SIGQUIT)
		g_stop = 1;
}

void	Server::closeServer()
{
	std::cout << "\nClosing server ..." << std::endl;

	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		std::string quitMsg = ":server QUIT :Server shutting down\r\n";
		send(it->first, quitMsg.c_str(), quitMsg.size(), 0);
		close(it->first);
		delete it->second;
	}
	_clients.clear();

	for (std::map<std::string, Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		delete it->second;
	_channels.clear();

	if (_serverSocket >= 0)
	{
		close(_serverSocket);
		_serverSocket = -1;	
	}

	_pollFds.clear();
	std::cout << "✅ Server closed ! Bye bye." << std::endl; 
}

void	Server::run()
{
	pollfd p;
	p.fd = _serverSocket;
	p.events = POLLIN;
	p.revents = 0;
	_pollFds.push_back(p);

	while (!g_stop)
	{
		int ret = poll(_pollFds.data(), _pollFds.size(), -1);
		
		if (ret < 0)
		{
			if (g_stop)
				break;
			throw std::runtime_error("poll() failed");
		}
		
		size_t currentSize = _pollFds.size();
		
		for (size_t i = 0; i < currentSize; ++i)
		{
			int currentFd = _pollFds[i].fd;
			short currentRevents = _pollFds[i].revents;
			
			if (currentRevents & POLLIN)
			{
				if (currentFd == _serverSocket)
					handleNewConnection();
				else
					handleClientMessage(currentFd);
			}
			// if (currentRevents & (POLLHUP | POLLERR))
			// 	removeClient(currentFd);
		}
	}
	closeServer();
}
