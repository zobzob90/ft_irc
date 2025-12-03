/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 17:53:55 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/03 18:25:45 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"
#include <csignal>

Server* g_server = NULL;

void	handle_sigint(int sig)
{
	(void)sig;

	std::cout << "\n⚠️ I saw a SIGINT, time to sleep ..." << std::endl;
	
	if (g_server)
	{
		std::map<int, Client*> clients = g_server->getClients();
		for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
		{
			g_server->removeClient(it->first);
		}
		std::map<std::string, Channel*> channels = g_server->getChannels(); // ajouter un getter
	for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
			delete it->second;
	}
	std::cout << "Server is sleeping ..." << std::endl;
	std::_Exit(0);
}

int main(int ac, char *av[])
{

	if (ac != 3)
	{
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
		return (1);
	}

	int port = atoi(av[1]);
	std::string password = av[2];

	if (port <= 0 || port > 65535)
	{
		std::cerr << "Error: Invalid port number" << std::endl;
		return (1);
	}

	try
	{
		Server server(port, password);
		g_server = &server;
		std::signal(SIGINT, handle_sigint);
		server.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);	
	}
	
	return (0);
}

// int main()
// {
// 	Server server(6667, "password");
// 	server.run();
// 	return (0);
// }
