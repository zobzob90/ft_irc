/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 11:26:37 by ertrigna          #+#    #+#             */
/*   Updated: 2025/11/14 16:39:01 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::createSocket()
{
	_serverSocket = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP); // AF_INET(domain en IPv4), SOCK_STREAM (protocole TCP le plus fiable), IPPROTO_TCP (param par defaut)
	
	if (_serverSocket < 0)
		throw std::runtime_error("Error: socket() failed");
	
	int opt = 1; // valeur d'option de socket (1 = true, 0 =  false)
	if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) // active le mode reutiliser l'adrresse pour ce socket
		throw std::runtime_error("Error: setsocketoption failed");
}

void	Server::configAddr()
{
	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(_port);

	if (bind(_serverSocket, (sockaddr *)&addr, sizeof(addr)) < 0)
		throw std::runtime_error("Error: bind() failed");
	
	if (listen(_serverSocket, 10) < 0)
		throw std::runtime_error("Error: listen() failed");
}

void	Server::setUpServerSocket()
{
	createSocket();
	configAddr();
	std::cout << "✅ Server listening on port " << _port << std::endl;
}
