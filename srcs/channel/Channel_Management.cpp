/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel_Management.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 16:02:08 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/05 13:56:52 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

#define MAX_CHANNELS 100

// Fonction pour valider le nom d'un channel selon RFC IRC
static bool isValidChannelName(const std::string& name)
{
	// Doit commencer par #
	if (name.empty() || name[0] != '#')
		return false;
	
	// Minimum 2 caractères (#a), maximum 50
	if (name.length() < 2 || name.length() > 50)
		return false;
	
	// Ne peut pas être juste '#'
	if (name == "#")
		return false;
	
	// Rejeter ## au début (channels comme ##test sont invalides)
	if (name.length() >= 2 && name[1] == '#')
		return false;
	
	// Vérifier explicitement qu'il n'y a pas d'espaces (car le parsing peut les ignorer)
	if (name.find(' ') != std::string::npos)
		return false;
	
	// Vérifier les caractères invalides (selon RFC 2812)
	for (size_t i = 1; i < name.length(); i++)
	{
		char c = name[i];
		// Interdire: espace, virgule, control chars, bell (0x07)
		if (c == ' ' || c == ',' || c == 7 || c == '\r' || c == '\n' || c < 32)
			return false;
		
		// Interdire certains caractères spéciaux problématiques
		// @ et ! sont utilisés dans les préfixes IRC, : pour les paramètres
		if (c == '@' || c == '!')
			return false;
	}
	
	return true;
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
	// Vérifier si le nom du channel est valide
	if (!isValidChannelName(name))
	{
		std::string errorMsg = ":server 403 " + creator->getNickname() + " " + name + " :Invalid channel name";
		sendToUser(creator, errorMsg);
		return NULL;
	}
	
	// Vérifier la limite de channels
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

			// Si le channel est vide, on le supprime
			if (channel->getMembersCount() == 0)
			{
				delete it->second;
				_channels.erase(it++); // Erase l'ancien it, puis avance
			} 
			else 
				++it; // Avancer normalement si pas de suppression
		} 
		else
			++it; // Avancer si le client n'est pas membre
	}
}

void Server::markForDisconnect(Client* client)
{
	std::string errorMsg = "ERROR :Closing connection";
	sendToUser(client, errorMsg);
	removeClient(client->getFd());
}
