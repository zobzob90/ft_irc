/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_Utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 15:26:40 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/17 16:18:05 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Command.hpp"
#include "Bot.hpp"

std::string Server::cleanMessage(const std::string& msg)
{
	std::string cleanMsg;
	for (size_t i = 0; i < msg.length(); ++i)
		if (msg[i] != '\r' && msg[i] != '\n')
			cleanMsg += msg[i];
	return (cleanMsg);
}

void Server::removePollFd(int fd)
{
	for(size_t i = 0; i < _pollFds.size(); i++)
	{
		if (_pollFds[i].fd == fd)
		{
			_pollFds.erase(_pollFds.begin() + i);
			break;	
		}	
	}
}

Client* Server::getClientByFd(int fd)
{
	std::map<int, Client*>::iterator it = _clients.find(fd);
	if(it == _clients.end())
		return (NULL);
	return (it->second);
}

void Server::addPollFd(int fd, short events)
{
	pollfd p;
	p.fd = fd;
	p.events = events;
	p.revents = 0;
	_pollFds.push_back(p);
}

