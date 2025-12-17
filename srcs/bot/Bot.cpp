/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 14:20:40 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/16 11:41:01 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bot.hpp"
#include "Server.hpp"

void	Bot::onUserJoin(Channel *channel, Client* user)
{
	// Protection contre les pointeurs NULL
	if (!channel || !user)
	{
		std::cerr << "ERROR: Bot::onUserJoin - NULL pointer!" << std::endl;
		return;
	}
	
	// Protection contre les channels invalides
	if (channel->getName().empty() || channel->getName().length() < 2)
	{
		std::cerr << "ERROR: Bot::onUserJoin - Invalid channel name: " << channel->getName() << std::endl;
		return;
	}
	
	std::string msg = "Welcome " + user->getNickname() + " 👮 BotCop is watching you 👀";
	sendToChannel(channel, msg);
}

void	Bot::sendToChannel(Channel* channel, const std::string& msg)
{
	// Protection contre les pointeurs NULL
	if (!channel)
	{
		std::cerr << "ERROR: Bot::sendToChannel - NULL channel!" << std::endl;
		return;
	}
	
	std::string formattedMsg = ":" + _botClient->getPrefix() + " PRIVMSG " + channel->getName() + " :" + msg;
	channel->broadcast(formattedMsg, NULL);
}

void	Bot::sendToUser(Client* user, const std::string& reason)
{
	std::string formattedMsg = ":" + _botClient->getPrefix() + " PRIVMSG " + user->getNickname() + " :" + reason;
	_serv->sendToUser(user, formattedMsg);
}

void	Bot::onMessage(Channel* channel, Client* user, const std::string& msg)
{
	// Protection contre les pointeurs NULL
	if (!channel || !user)
	{
		std::cerr << "ERROR: Bot::onMessage - NULL pointer!" << std::endl;
		return;
	}
	
	if (containsBadWord(msg))
	{
		kickUser(channel, user, "Gros mots interdit 👮");
		return ;
	}
	if (msg == "!ping")
		sendToChannel(channel, "pong 🏓");
	else if (msg == "!dadjoke")
		dadJoke(channel);
	else if (msg == "!rules")
	{
		sendToChannel(channel, "Regle 1 : Pas d'insultes 👮\n");
		sendToChannel(channel, "Regle 2 : On ne parle pas du Fight Club\n");
	}
}

bool	Bot::containsBadWord(const std::string& msg) const
{
	for (size_t i = 0; i < _bannedWords.size(); ++i)
		if (msg.find(_bannedWords[i]) != std::string::npos)
			return (true);
	return (false);
}

void	Bot::kickUser(Channel* channel, Client* user, const std::string& reason)
{
	// IMPORTANT: Sauvegarder le nom avant de manipuler le channel
	std::string channelName = channel->getName();
	std::string msg = ":" + _botClient->getPrefix() + " KICK " + channelName + " " + user->getNickname() + " :" + reason;
	
	channel->broadcast(msg, NULL);
	std::cout << "🚨 " << _name << " kicked " << user->getNickname() << " from " << channelName << ": " << reason << std::endl;
	
	channel->removeMember(user);
	
	// Vérifier si le channel est vide APRÈS avoir fini de l'utiliser
	if (channel->getMembersCount() == 0)
	{
		// Ne plus utiliser le pointeur channel après cette ligne !
		_serv->destroyChannel(channelName);
	}
	// Ne pas déconnecter l'utilisateur, juste le retirer du channel
}

void	Bot::dadJoke(Channel* channel)
{

	std::vector< std::pair<std::string, std::string> > jokes;

	jokes.push_back(std::make_pair(
        "Quel est le comble pour un électricien ?",
        "De ne pas être au courant. ⚡"
	));
	jokes.push_back(std::make_pair(
		"Qu'est-ce qu'un canif?",
		"Un petit fien!"
	));
	jokes.push_back(std::make_pair(
		"Comment appelle-t-on un chat tombé dans un pot de peinture le jour de Noël?",
		"Un chat-peint de Noël!"
	));

	int idx = rand() % jokes.size();

	sendToChannel(channel, jokes[idx].first);
	sleep(2);
	sendToChannel(channel, jokes[idx].second);
}
