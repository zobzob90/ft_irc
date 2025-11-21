/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command_utils.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 20:40:50 by ertrigna          #+#    #+#             */
/*   Updated: 2025/11/21 21:06:46 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "Server.hpp"

void Command::sendError(int code, const std::string& message)
{
	std::ostringstream oss;

	oss << ":" << "server" << " " << code << " ";

	if (_client->getNickname().empty())
		oss << _client->getNickname() << " ";
	else
		oss << "*" << " ";
	
	oss << message << "\r\n";
	std::string response = oss.str();
	send(_client->getFd(), response.c_str(), response.length(), 0);
}

void Command::sendReply(int code, const std::string& message)
{
	std::ostringstream oss;

	oss << ":" << "server" << " ";
	
	if (code < 10)
		oss << "00" << code;
	else if (code < 100)
		oss << "0" << code;
	else
		oss << code;
	
	oss << " " << _client->getNickname() << " " << message << "\r\n";

	std::string response = oss.str();
	send(_client->getFd(), response.c_str(), response.length(), 0);
}
