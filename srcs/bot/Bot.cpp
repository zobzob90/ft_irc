/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 14:20:40 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/09 19:44:47 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bot.hpp"
#include "Server.hpp"

void	Bot::onUserJoin(Channel *channel, Client* user)
{
	std::string msg = "Welcome " + user->getNickname() + "! Respecte mon autorite 👮";
	sendToChannel(channel, msg);
}

void	Bot::sendToChannel(Channel* channel, const std::string& msg)
{
	std::string formattedMsg = ":" + _name + " PRIVMSG " + channel->getName() + " :" + msg;
	channel->broadcast(formattedMsg, NULL);
}

void	Bot::sendToUser(Client* user, const std::string& reason)
{
	std::string formattedMsg = ":" + _name + " PRIVMSG " + user->getNickname() + " :" + reason;
	_serv->sendToUser(user, formattedMsg);
}

void	Bot::onMessage(Channel* channel, Client* user, const std::string& msg)
{
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
	std::string msg = ":" + _name + " KICK " + channel->getName() + " " + user->getNickname() + " :" + reason + "\r\n";
	channel->broadcast(msg, NULL);
	channel->removeMember(user);
	if (channel->getMembersCount() == 0)
		_serv->destroyChannel(channel->getName());
	user->markForDisconnect();
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
