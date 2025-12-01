/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/01 17:30:55 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/01 19:11:09 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

void Channel::addMember(Client* client)
{
	if (!isMember(client))
		_members.push_back(client);
}

void Channel::removeMember(Client* client)
{
	std::vector<Client *>::iterator it = std::find(_members.begin(), _members.end(), client);
	if (it != _members.end())
		_members.erase(it);
	// On retire aussi des operateurs si c'etait un OP
	removeOperator(client);
}

bool Channel::isMember(Client* client) const
{
	return (std::find(_members.begin(), _members.end(), client) != _members.end());
}

void Channel::addOperator(Client *client)
{
	if (!isOperator(client))
		_operators.push_back(client);
}

void Channel::removeOperator(Client* client)
{
	std::vector<Client *>::iterator it = std::find(_operators.begin(), _operators.end(), client);
	if (it != _operators.end())
		_operators.erase(it);
}

bool Channel::isOperator(Client* client) const
{
	return (std::find(_operators.begin(), _operators.end(), client) != _operators.end());
}

void Channel::addInvite(Client *client)
{
	if (!isOperator(client))
		_invitedUsers.push_back(client);
}

void Channel::removeInvite(Client* client)
{
	std::vector<Client *>::iterator it = std::find(_invitedUsers.begin(), _invitedUsers.end(), client);
	if (it != _invitedUsers.end())
		_invitedUsers.erase(it);
}

bool Channel::isInvited(Client* client) const
{
	return (std::find(_invitedUsers.begin(), _invitedUsers.end(), client) != _invitedUsers.end());
}

void Channel::broadcast(const std::string& message, Client *exclude)
{
	for (size_t i = 0; i < _members.size(); i++)
	{
		if (_members[i] != exclude)
		{
			std::string msg = message + "\r\n";
			send(_members[i]->getFd(), msg.c_str(), msg.length(), 0);
		}
	}
}

std::string Channel::getMemberList() const
{
	std::string list;
	for (size_t i = 0; i < _members.size(); ++i)
	{
		if (isOperator(_members[i]))
			list += "@";
		list += _members[i]->getNickname();
		if (i < _members.size() - 1)
			list += " ";
	}
	return (list);
}
