/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 17:53:55 by ertrigna          #+#    #+#             */
/*   Updated: 2025/11/03 18:14:01 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"

int main(int ac, char *av[])
{

	if (ac > 3)
	{
		throw std::runtime_error("Usage: ./ircserv <port> <password>") << std::endl;
		return (1);
	}

	int port = atoi(av[1]);
	std::string password = av[2];

	try
	{
		Server server(port, password);
		server.run();
	}
	catch
		std::cerr << "Error: " << e.what() << std::endl;
	
	return (0);
}
