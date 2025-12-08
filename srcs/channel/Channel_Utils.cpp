/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel_Utils.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 16:35:14 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/08 16:36:54 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::broadcastToUserChannels(Client* clients, const std::string msg)
{
	for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		Channel* ch = it->second;

		if (ch->isMember(clients))
			ch->broadcast(msg, clients);
	}
}

void	Server::removeClientFromAllChannels(Client* client)
{
	for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end();)
	{
		Channel* ch = it->second;

		ch->removeMember(client);

		// supprime le channel si il est vide
		if (ch->getMembersCount() == 0)
		{
			delete ch;
			it = _channels.erase(it);
		}
		else
			++it;
	}
}

void	Server::markForDisconnect(Client *client)
{
	int fd = client->getFd();

	std::cout<< "⚠️ Marking client for disconnection : [FD: " << fd << " ]" <<std::endl;
	
	removeClientFromAllChannels(client);
	removeClient(fd);
}
