/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 17:53:55 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/08 13:15:35 by ertrigna         ###   ########.fr       */
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
	try
	{
		if(port <= 1023 || port > 65535)
			throw std::runtime_error("Error : Invalid Port Number");
		Server server(port, password);
		signal(SIGINT, signalHandler);
		signal(SIGQUIT, signalHandler);
		server.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);	
	}
	
	return (0);
}
