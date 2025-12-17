/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 16:12:04 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/16 16:40:33 by ertrigna         ###   ########.fr       */
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
    // Chercher \r\n (standard IRC)
    size_t pos = _buffer.find("\r\n");
    
    // Si pas trouvé, chercher juste \n (netcat)
    if (pos == std::string::npos)
        pos = _buffer.find("\n");
    
    if (pos == std::string::npos)
        return ("");
    
    std::string message = _buffer.substr(0, pos);
    
    // Effacer le message + le délimiteur
    if (_buffer[pos] == '\r' && pos + 1 < _buffer.size() && _buffer[pos + 1] == '\n')
        _buffer.erase(0, pos + 2);  // \r\n
    else
        _buffer.erase(0, pos + 1);  // \n seul
    
    return (message);
}

bool Client::hasCompleteMessage() const
{
    // Accepter soit \r\n (standard) soit \n seul (netcat)
    if (_buffer.find("\r\n") != std::string::npos)
        return (true);
    if (_buffer.find("\n") != std::string::npos)
        return (true);
    return (false);
}

std::string Client::getPrefix() const
{
	return _nickname + "!" + _username + "@student.42.fr";
}

// OUTPUT BUFFER MANAGEMENT
void Client::appendOutputBuffer(const std::string& data)
{
	_outputBuffer += data;
}

bool Client::hasOutputPending() const
{
	return !_outputBuffer.empty();
}

std::string Client::getOutputBuffer() const
{
	return _outputBuffer;
}

void Client::clearOutputBuffer(size_t bytes)
{
	if (bytes >= _outputBuffer.size())
		_outputBuffer.clear();
	else
		_outputBuffer.erase(0, bytes);
}
