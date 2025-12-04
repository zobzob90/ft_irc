/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel_Management.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 16:02:08 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/03 14:50:22 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Channel* Server::getChannel(const std::string& name)
{
	std::map<std::string, Channel*>::iterator it = _channels.find(name);
	if (it != _channels.end())
		return (it->second);
	return (NULL);
}

Channel* Server::createChannel(const std::string& name, Client* creator)
{
	Channel* newChannel = new Channel(name);

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
		std::cout << "📢 Bye Bye Channel  : " << name << std::cout;
	}
}
void Server::removeClientFromAllChannels(Client* client){
	std::map<std::string, Channel*>::iterator it = _channels.begin();

	while (it != _channels.end()){
		Channel* channel = it->second;
		if (channel->isMember(client)){
			channel->removeMember(client);

			// Si le channel est vide, on le supprime
			if (channel->getMembersCount() == 0){
				delete it->second;
				_channels.erase(it++); // Erase l'ancien it, puis avance
			} else {
				++it; // Avancer normalement si pas de suppression
			}
		} else {
			++it; // Avancer si le client n'est pas membre
		}
	}
}

void Server::markForDisconnect(Client* client){
	// Envoyer un message ERROR au client avant de le déconnecter
	std::string errorMsg = "ERROR :Closing connection\r\n";
	send(client->getFd(), errorMsg.c_str(), errorMsg.length(), 0);

	//Fermer la connexion
	removeClient(client->getFd());
}