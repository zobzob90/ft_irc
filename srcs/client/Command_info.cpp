/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command_info.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 17:26:23 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/18 17:26:32 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "Server.hpp"
#include "Bot.hpp"

void	Command::executeWho()
{
	std::map<int, Client*>& clients = _server->getClients();
	sendReply(322, ":=== Connected Users ===");
	int count = 0;
	for (std::map<int, Client*>::iterator it =  clients.begin(); it != clients.end(); ++it)
	{
		Client* user = it->second;
		if (user->isRegistered())
		{
			count++;
			std::ostringstream oss;
			oss << count << ". " << user->getNickname() << " (" << user->getUsername() << "@" << user->getHostname() <<")";
			sendReply(322, ":" + oss.str());
		}
	}
	std::ostringstream total;
	total << ":Total: " << count << " user(s) online";
	sendReply(315, total.str());
}

void	Command::executeList()
{
	sendReply(321, ":=== Available Channels ===");
	std::map<std::string, Channel*>& channels = _server->getChannels();
	if (channels.empty())
		sendReply(322, ":No channels available for the moment");
	else
	{
		int count = 0;
		for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
		{
			Channel* chan = it->second;
			count++;
			std::ostringstream oss;
			oss << count << ". " << chan->getName() << "[ " << chan->getMembersCount() << " user(s)]";
			std::string topic = chan->getTopic();
			if (!topic.empty())
				oss << " - " << topic;
			sendReply(322, ":" + oss.str());
		}
	}
}
