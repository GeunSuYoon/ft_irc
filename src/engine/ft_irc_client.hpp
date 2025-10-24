/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc_client.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: geuyoon <geuyoon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 11:43:31 by geuyoon           #+#    #+#             */
/*   Updated: 2025/10/24 14:33:48 by geuyoon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_IRC_CLIENT_HPP
# define FT_IRC_CLIENT_HPP

# include "../../includes/ft_irc_define.hpp"
# include "../../includes/ft_irc_include.hpp"
# include "./ft_irc_channel.hpp"

class	Channel;

class	Client
{
	private:
		int			fd_;
		uint16_t	port_;
		bool		isPass_;
		bool		isNick_;
		bool		isUser_;
		bool		isRegister_;

		std::string	ipAddr_;
		std::string	userName_;
		std::string	nickName_;
		std::string	realName_;
		std::string	hostName_;

		std::string	password_;

		std::string	buffer_;
		std::string	cmd_;

		std::vector<Channel *>	joinedChannels_;

		static const std::string	invalNickChar_;
		static const std::string	invalNickStartChar_;
		
	public:
		Client(int socket);
		~Client(void);

		int					getFd(void) const;
		uint16_t			getPort(void) const;
		bool				getIsPass(void) const;
		bool				getIsNick(void) const;
		bool				getIsUser(void) const;
		bool				getIsRegister(void) const;
		std::string	const	&getIpAddr(void) const;
		std::string	const	&getUserName(void) const;
		std::string	const	&getNickName(void) const;
		std::string	const	&getRealName(void) const;
		std::string	const	&getHostName(void) const;
		std::string	const	&getPassword(void) const;
		std::string	const	&getBuffer(void) const;
		std::string	const	&getCmd(void) const;

		std::string			getSendString(void) const;

		void	setPort(uint16_t port);
		void	setIsPass(bool isPass);
		void	setIsNick(bool isNick);
		void	setIsUser(bool isUser);
		void	setIsRegister(bool isRegister);
		void	setIpAddr(const std::string &ipAddr);
		void	setUserName(const std::string &userName);
		void	setRealName(const std::string &realName);
		void	setNickName(const std::string &nickName);
		void	setHostName(const std::string &hostName);
		void	setPassword(const std::string &password);

		void	sendMsg(const std::string &msg);

		bool	isCompleteMsg(void);
		void	appendBuffer(const std::string &buffer, size_t bufSize);
		void	clearCmd(void);

		void	joinChannel(Channel *channel);
		void	leaveChannel(Channel *channel);

		int		isValideNick(const std::string &nick);

};

#endif
