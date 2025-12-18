/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel_Management.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 16:02:08 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/18 14:00:44 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

#define MAX_CHANNELS 100

// Fonction pour valider le nom d'un channel selon RFC IRC
static bool isValidChannelName(const std::string& name)
{
	if (name.empty() || name[0] != '#')
		return (false);
	if (name.length() < 2 || name.length() > 50)
		return (false);
	if (name == "#")
		return (false);
	if (name.length() >= 2 && name[1] == '#')
		return (false);
	if (name.find(' ') != std::string::npos)
		return (false);
	for (size_t i = 1; i < name.length(); i++)
	{
		char c = name[i];
		if (c == ' ' || c == ',' || c == 7 || c == '\r' || c == '\n' || c < 32)
			return (false);
		if (c == '@' || c == '!')
			return (false);
	}
	return (true);
}

Channel* Server::getChannel(const std::string& name)
{
	std::map<std::string, Channel*>::iterator it = _channels.find(name);
	if (it != _channels.end())
		return (it->second);
	return (NULL);
}

Channel* Server::createChannel(const std::string& name, Client* creator)
{
	if (!isValidChannelName(name))
	{
		std::string errorMsg = ":server 403 " + creator->getNickname() + " " + name + " :Invalid channel name";
		sendToUser(creator, errorMsg);
		return NULL;
	}
	if (_channels.size() >= MAX_CHANNELS)
	{
		std::string errorMsg = ":server 405 " + creator->getNickname() + " " + name + " :You have joined too many channels";
		sendToUser(creator, errorMsg);
		return NULL;
	}
	Channel* newChannel = new Channel(name, this);  // Passer le pointeur du serveur
	newChannel->addMember(creator);
	newChannel->addOperator(creator);
	_channels[name] = newChannel;
	std::cout << "📢 New channel is here : " << name << " by " << creator->getNickname() << std::endl; 
	return (newChannel);
}

void Server::destroyChannel(const std::string& name)
{
	std::map<std::string, Channel*>::iterator it = _channels.find(name);
	
	if (it != _channels.end())
	{
		delete it->second;
		_channels.erase(it);
		std::cout << "📢 Bye Bye Channel  : " << name << std::endl;
	}
}

void Server::removeClientFromAllChannels(Client* client)
{
	std::map<std::string, Channel*>::iterator it = _channels.begin();

	while (it != _channels.end())
	{
		Channel* channel = it->second;
		if (channel->isMember(client))
		{
			channel->removeMember(client);
			if (channel->getMembersCount() == 0)
			{
				delete it->second;
				_channels.erase(it++);
			} 
			else 
				++it;
		} 
		else
			++it;
	}
}

void Server::markForDisconnect(Client* client)
{
	std::string errorMsg = "ERROR :Closing connection";
	sendToUser(client, errorMsg);
	removeClient(client->getFd());
}
