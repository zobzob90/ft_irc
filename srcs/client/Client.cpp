/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 16:12:04 by ertrigna          #+#    #+#             */
/*   Updated: 2025/11/14 17:12:19 by ertrigna         ###   ########.fr       */
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

