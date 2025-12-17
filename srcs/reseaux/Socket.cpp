/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 11:26:37 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/15 15:12:36 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Bot.hpp"

Server::Server(int port, const std::string &password) : _port(port), _password(password), _bot(NULL)
{
   _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocket < 0)
        throw std::runtime_error("❌ socket() failed");

    // Mettre le socket serveur en mode non-bloquant
    if (fcntl(_serverSocket, F_SETFL, O_NONBLOCK) < 0)
    {
        close(_serverSocket);
        throw std::runtime_error("❌ fcntl() failed on server socket");
    }

    int opt = 1;
    if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        close(_serverSocket);
        throw std::runtime_error("❌ setsockopt() failed");
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(_serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        close(_serverSocket);
        throw std::runtime_error("❌ bind() failed");
    }

    if (listen(_serverSocket, 10) < 0)
    {
        close(_serverSocket);
        throw std::runtime_error("❌ listen() failed");
    }

    _bot = new Bot(this);
    std::cout << "✅ Server started on port " << _port << std::endl;
    std::cout << "🤖 Bot activated: " << _bot->getName() << std::endl;
}

Server::~Server()
{
    if (_bot)
        delete _bot;
}
