/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc_server.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: geuyoon <geuyoon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 16:32:01 by geuyoon           #+#    #+#             */
/*   Updated: 2025/10/23 14:00:08 by geuyoon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_IRC_SERVER_HPP
# define FT_IRC_SERVER_HPP

// # include "../../includes/ft_irc.hpp"
# include "../../includes/ft_irc_define.hpp"
# include "../../includes/ft_irc_include.hpp"
# include "../../includes/ft_irc_numeric_err.hpp"
# include "../../includes/ft_irc_numeric_rpl.hpp"

# include "./ft_irc_client.hpp"
# include "./ft_irc_channel.hpp"

# define MAXWAITINGCONNECTIONS 10
# define MAXTIMEOUT 1000

class	Client;
class	Channel;

class	Server
{
	private:
		int							port_;
		int							serverSock_;

		std::string					password_;
		std::string					serverName_;
		std::vector<struct pollfd>	fds_;
		std::vector<Client *>		clients_;
		std::vector<Channel *>		channels_;

		std::map<std::string, int>	targetMin_;
		std::map<std::string, int>	targetMax_;

		void	setPort_(int port);
		void	setServerFd_(int fd);
		void	setPassword_(const std::string &password);
		void	setFds_(std::vector<struct pollfd> fds);

		static const int	commandSize = 13;
		typedef void	(Server::*tCommandFunc)(Client *client, const std::vector<std::string> &args);
		tCommandFunc	commandFuncs[commandSize];

		static const std::string	invalNickChar_;
		static const std::string	invalNickStartChar_;
		static const std::string	channelPrefix_;
		static const std::string	commandList_[commandSize];

	protected:
		void	acceptClient(void);

		void	initClientConnect(Client *client);
		void	initChannel(Client *client, const std::string &channelName);

		void	commandParsor(Client *client, const std::string& msg);
		void	commandKick(Client *client, const std::vector<std::string> &args);
		void	commandInvite(Client *client, const std::vector<std::string> &args);
		void	commandTopic(Client *client, const std::vector<std::string> &args);
		void	commandMode(Client *client, const std::vector<std::string> &args);
		void	commandNick(Client *client, const std::vector<std::string> &args);
		void	commandPrivmsg(Client *client, const std::vector<std::string> &args);
		void	commandUser(Client *client, const std::vector<std::string> &args);
		void	commandPong(Client *client, const std::vector<std::string> &args);
		void	commandPass(Client *client, const std::vector<std::string> &args);
		void	commandJoin(Client *client, const std::vector<std::string> &args);
		void	commandPart(Client *client, const std::vector<std::string> &args);
		void	commandQuit(Client *client, const std::vector<std::string> &args);
		void	commandCap(Client *client, const std::vector<std::string> &args);

		bool	commandNickValid(Client *client, const std::string &nick);

		Client	*findClient(const std::string &nickName);
		Client	*findClientFd(int fd);
		Channel	*findChannel(const std::string &channelName);


		int		isTargetMatch(Client *client, const std::string &cmd, const std::vector<std::string> &args) const;

		int		getTargetMinCommand(const std::string &cmd) const;
		int		getTargetMaxCommand(const std::string &cmd) const;

		void	sendEventMsg(Client *client, const std::string &msg);
		void	sendMsgClient(Client *client, const std::string &clientName, const std::string &channelName, \
			const std::string &cmd, const std::string &arg, int code);
		void	broadcastChannel(Channel *channel, const std::string &msg, Client *client, bool clientSend);
	public:
		Server(char **argv);
		~Server(void);
	
		int	getPort(void) const;
		int	getServerFd(void) const;
		std::string	getServerName(void) const;
		std::string	getPassword(void) const;
		std::vector<struct pollfd> getFds(void) const;
		std::vector<struct Client> getClient(void) const;

		void	initServer(void);
		void	runServer(void);
};

std::vector<std::string>	tokenizeLine(const std::string& msg);

#endif
