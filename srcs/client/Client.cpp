/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 16:12:04 by ertrigna          #+#    #+#             */
/*   Updated: 2025/11/19 17:57:19 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

void	Client::setNickname(const std::string& nick)
{
	_nickname = nick;
}

void	Client::setUsername(const std::string& user)
{
	_username = user;
}

void Client::setRealname(const std::string& real)
{
	_realname = real;
}

void Client::setHostname(const std::string& host)
{
	_hostname = host;
}

void Client::setAuthenticated(bool auth)
{
	_authenticated = auth;
}

void Client::setRegistered(bool reg)
{
	_registered = reg;
}

bool Client::hasCompleteMessage() const
{
	size_t pos = _buffer.find("\r\n");
	if (pos != std::string::npos)
		return (true);
	else
		return (false);
}

void Client::clearBuffer()
{
	_buffer.clear();
}

void Client::appendBuffer(const std::string& data)
{
	_buffer = _buffer + data;
}

std::string Client::extractMessage()
{
	size_t pos = _buffer.find("\r\n");

	if (pos == std::string::npos)
		return ("");
	
	std::string message = _buffer.substr(0, pos);
	_buffer.erase(0, pos + 2);

	return (message);
}

std::string Client::getPrefix() const
{
	return ":" + _nickname + "!" + _username + "@" + _hostname;
}
