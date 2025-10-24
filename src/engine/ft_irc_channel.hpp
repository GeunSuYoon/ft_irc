/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc_channel.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: geuyoon <geuyoon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 08:31:59 by geuyoon           #+#    #+#             */
/*   Updated: 2025/10/24 10:10:43 by geuyoon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_IRC_CHANNEL_HPP
# define FT_IRC_CHANNEL_HPP

// # include "../../includes/ft_irc.hpp"
# include "../../includes/ft_irc_define.hpp"
# include "../../includes/ft_irc_include.hpp"
// # include "../../includes/ft_irc_numeric_err.hpp"
// # include "../../includes/ft_irc_numeric_rpl.hpp"
# include "./ft_irc_client.hpp"

class	Client;

// std::string	modeFlags[] = {
// 	"+i", "+t", "+k", "+o", "+l", "-i", "-t", "-k", "-o", "-l"
// };

class	Channel
{
	private:
		std::string				channelName_;
		bool					modeInviteOnly_;
		// bool					modeKey_;
		bool					modeTopicOperator;
		int						userLimit_;
		std::string				topic_;
		std::string				password_;

		std::vector<Client *>	channelOperator_;
		std::vector<Client *>	channelMembers_;

		static const std::string	modeFlags_[];

	public:
		Channel(const std::string &channelName, Client *client);
		~Channel(void);

		const std::string	&getChannelName(void) const;
		bool				getModeInviteOnly(void) const;
		// bool				getModeInviteOnly(void) const;
		int					getUserLimit(void) const;
		const std::string	&getTopic(void) const;
		const std::string	&getPassword(void) const;
		const std::vector<Client *>	&getChannelMembers(void) const;
		
		std::string	getChannelMembersName(void) const;
		// Client				*getChannelOperator(void) const;

		void	setChannelName(const std::string &channelName);
		void	setModeInviteOnly(bool inviteOnly);
		void	setUserLimit(int userLimit);
		void	setTopic(const std::string &topic);
		void	setPassword(const std::string &password);
		void	addChannelMember(Client *client);
		void	removeChannelMember(Client *client);
		void	addChannelOperator(Client *client);
		void	removeChannelOperator(Client *client);

		int	partChannelMember(Client *client);
		int	modeChannel(Client *client, const std::vector<std::string> &args);
		int	joinChannel(Client *client, const std::vector<std::string> &args);
		// client function
		int	inviteMember(Client *client, Client *targetClient);
		int	kickMember(Client *client, Client *targetClient);
		int	topicChannel(Client *client, const std::string &topic);

		bool	isOperator(Client *client) const;

		void	changeInviteOnly(void);

		// void	broadcastMsg(Server *server, const std::string &msg);

		Client	*findTargetClient(const std::string &nickName);
};

#endif