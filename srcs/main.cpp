/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 17:53:55 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/05 16:29:39 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"
#include <csignal>

extern volatile sig_atomic_t g_stop;

void	signalHandler(int signum)
{
	if (signum == SIGINT || signum == SIGQUIT)
		g_stop = 1;
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
    signal(SIGINT, signalHandler);
    signal(SIGQUIT, signalHandler);
	try
	{
		Server server(port, password);
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
