/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 15:42:47 by ertrigna          #+#    #+#             */
/*   Updated: 2025/11/18 14:37:56 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>

class Client
{
	private:
		int			_fd; 		// <- FD du socket
		std::string	_nickname;	// Pseudo IRC (NICK CMD)
		std::string	_username;	// (USER CMD)
		std::string	_realname;	// (USER CMD)
		std::string	_hostname;	// IP/Hostname
		std::string	_buffer;	// Buffer pour messages partiels
		bool		_authenticated;	// A-t-il envoye le bon PASS(word)
		bool		_registered;	// A-t-il fait NICK+USER
	
	public:
		// Constructeur + Destructeur
		Client(int fd): _fd(fd), _nickname(""), _username(""), _realname(""), _hostname(""), _buffer(""), _authenticated(false), _registered(false) {};
		~Client() {};

		// GETTER
		int			getFd()				const { return (_fd); }
		std::string	getNickname()		const { return (_nickname); }
		std::string	getUsername()		const { return (_username); }
		std::string getRealname()		const { return (_realname); }
		std::string	getHostname()		const { return (_hostname); }
		std::string	getBuffer()			const { return (_buffer); }
		bool		isAuthenticated()	const { return (_authenticated); }
		bool		isRegistered()		const { return (_registered); }
		
		// SETTER
		void		setNickname(const std::string& nick);
		void		setUsername(const std::string& user);
		void 		setRealname(const std::string& real);
		void		setHostname(const std::string& host);
		void		setAuthenticated(bool auth);
		void		setRegistered(bool reg);

		// BUFFER MANAGEMENTS
		// On accumule tout les messages dans un buffer, si on trouve un \r\n on display le message
		void		appendBuffer(const std::string& data);
		void		clearBuffer();
		bool		hasCompleteMessage() const;
		std::string	extractMessage();

		//UTILS
		std::string	getPrefix() const;
} ;

#endif