/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 13:48:15 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/09 16:34:43 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOT_HPP
#define BOT_HPP

#include <vector>
#include <map>
#include <iostream>
#include <string>
#include "Client.hpp"
#include "Channel.hpp"

class Server;

class Bot
{
	private:
		Server* 					_serv;
		Client* 					_botClient;
		std::string					_name;
		std::vector<std::string>	_bannedWords;

	public:
		Bot(Server *server) : _serv(server), _name("BotCop") 
		{
			_botClient = new Client(-1);
			_botClient->setNickname(_name);
			_botClient->setUsername(_name);
			_botClient->setRealname("IRC ROBOCOP");

			_bannedWords.push_back("pute");
			_bannedWords.push_back("connard");
			_bannedWords.push_back("con");
			_bannedWords.push_back("fdp");
			_bannedWords.push_back("PTT");
		}
		~Bot() 
		{
			delete _botClient;
		}

		Client*			getClient() const {return _botClient;}
		std::string		getName() const {return _name;}

		void	onUserJoin(Channel *channel, Client* user);
		void	onMessage(Channel *channel, Client *user, const std::string& msg);

	private:
		bool	containsBadWord(const std::string& msg) const;
		void	sendToChannel(Channel* channel, const std::string& msg);
		void	sendToUser(Client* user, const std::string& reason);
		void	kickUser(Channel* channel, Client* user, const std::string& reason);
		void	dadJoke(Channel* channel);
};

#endif