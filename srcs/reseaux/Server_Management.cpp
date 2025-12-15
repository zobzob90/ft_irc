/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_Management.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 17:52:17 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/09 19:54:21 by ertrigna         ###   ########.fr       */
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

	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0)
    {
        std::cerr << "fcntl() failed on client socket" << std::endl;
        close(clientFd);
        return;
    }

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
	
	std::cout << "New client connected (fd=" << clientFd << ")" << std::endl;
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

std::vector<Channel*> Server::getClientChannels(Client* client)
{
    std::vector<Channel*> result;
    
    for (std::map<std::string, Channel*>::iterator it = _channels.begin(); 
         it != _channels.end(); ++it)
    {
        if (it->second->isMember(client))
            result.push_back(it->second);
    }
    
    return result;
}

void	Server::removeClient(int fd)
{
    if (fd < 0 || fd == _serverSocket)
        return ;

    std::map<int, Client*>::iterator it = _clients.find(fd);
    
    if (it != _clients.end())
    {
        Client* client = it->second;
        std::string nickname = client->getNickname();
        
        // Message QUIT conforme IRC
        std::string quitMsg = ":" + nickname + " QUIT :Client disconnected\r\n";
        
        // Notifier les channels où le client était présent
        std::vector<Channel*> clientChannels = getClientChannels(client);
        for (size_t i = 0; i < clientChannels.size(); i++)
        {
            clientChannels[i]->broadcast(quitMsg, NULL);
            clientChannels[i]->removeMember(client);
        }
        
        std::cout << "Client disconnected: " << nickname << " (fd=" << fd << ")" << std::endl;
        
        delete client;
        _clients.erase(it);
    }

    close(fd);

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
    memset(buffer, 0, sizeof(buffer));
    int		bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes <= 0)
    {
        removeClient(fd);
        return ;
    }
    
    std::map<int, Client*>::iterator it = _clients.find(fd);
    if (it == _clients.end())
        return;
    Client* client = it->second;
    if (!client)
        return;
    
    // Ajouter au buffer du client
    client->appendBuffer(std::string(buffer, bytes));
    
    // Traiter tous les messages complets
    while (client->hasCompleteMessage())
    {
        std::string message = client->extractMessage();
        
        if (message.empty())
            continue;
        
        // Nettoyer des caratère parasite
        std::string cleanMsg;
        for (size_t i = 0; i < message.length(); ++i)
        {
            if (message[i] != '\r' && message[i] != '\n' && message[i] >= 32)
                cleanMsg += message[i];
        }
        
        if (cleanMsg.empty())
            continue;
        
        std::cout << "[" << client->getNickname() << "] " << cleanMsg << std::endl;
        Command cmd(this, client, cleanMsg);
        cmd.execute();
        
        if (client->isMarkedForDisconnect())
        {
            std::cout << "Client kicked/removed: " << client->getNickname() << " (fd=" << fd << ")" << std::endl;
            removeClient(fd);
            return;
        }
    }
}

void	Server::signalHandler(int signum)
{
	if (signum == SIGINT || signum == SIGQUIT)
		g_stop = 1;
}

void	Server::closeServer()
{
	std::cout << "\nShutting down server..." << std::endl;

	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		std::string quitMsg = "ERROR :Server shutting down\r\n";
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
	std::cout << "Server closed successfully." << std::endl; 
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
