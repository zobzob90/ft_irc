/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 16:41:40 by ertrigna          #+#    #+#             */
/*   Updated: 2025/12/03 14:10:09 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sys/socket.h>
#include "Client.hpp"

class Channel
{
	private:
	
		// ATTRIBUTS - INFO CHANNEL
		std::string _name;
		std::string _topic;
		std::string _password;
		int			_userLimit;

		// LISTES - MEMBRES ET PERM
		std::vector<Client *> _members;
		std::vector<Client *> _operators;
		std::vector<Client *> _invitedUsers;

		// MODES - PARAM CHANNEL
		bool	_inviteOnly; // +i : Invite obligatoire
		bool	_topicRestrict; // +t : Seul OP change le topic
		bool	_hasPassword; // +k: PRotege par un mdp
		bool	_hasUserLimits; // +l : Limite User

	public:

		Channel(const std::string& name) :	_name(name),
											_topic(""),
											_password(""),
											_userLimit(0),
											_inviteOnly(false),
											_topicRestrict(false),
											_hasPassword(false),
											_hasUserLimits(false)
		{}
											
		~Channel() {}

		//GETTER - Infos
		std::string getName() const {return _name;}
		std::string getTopic() const {return _topic;}
		std::string getPass() const {return _password;}
		int			getUserLimit() const {return _userLimit;}
		int			getMembersCount() const {return _members.size();}

		// GETTER - Modes
		bool 		isInviteOnly() const { return _inviteOnly; }
        bool		isTopicRestricted() const { return _topicRestrict; }
        bool 		hasPassword() const { return _hasPassword; }
        bool 		hasUserLimit() const { return _hasUserLimits; }

		//SETTERS
		void		setTopic(const std::string& topic) 
		{
			_topic = topic;
		}
		
		void		setPassword(const std::string& password) 
		{
			_password = password;
			_hasPassword = !password.empty();
		}
		
		void		setUserLimit(int limit)
		{
			_userLimit = limit;
			_hasUserLimits = (limit > 0);	
		}

		// MEMBER MANAGEMENT
		void		addMember(Client* client); // -> Quand quelqu'un fait join
		void		removeMember(Client* client); // -> Quand quelqu'un fait PART #general ou  QUIT
		bool		isMember(Client* client) const; // -> Verifie si un client est dans le channel
		
	// OPERATOR MANAGEMENT
	void		addOperator(Client* client); // -> Cmd MODE #general +o Alice
	void		removeOperator(Client* client); // -> Cmd MODE #general -o Alice
	bool		isOperator(Client* client) const; // Verifie si un client peut KICK/MODE/TOPIC
	void		promoteFirstMemberIfNeeded(); // -> Promouvoir automatiquement si plus d'ops		// INVITE MANAGEMENT
		void		addInvite(Client* client); // -> Ajoute une invitation
		void		removeInvite(Client* client); // -> Remove une invitation
		bool		isInvited(Client* client) const; // Invite ou pas 

		// MODES MANAGEMENT
		void		setInviteOnly(bool value)
		{
			_inviteOnly = value;
		}
		void		setTopicRestricted(bool value)
		{
			_topicRestrict = value;
		}

		// UTILITIES
		void 		broadcast(const std::string& message, Client* exclude = NULL);
		std::string getMemberList() const;
};

#endif