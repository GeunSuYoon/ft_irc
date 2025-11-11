/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc_server.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: geuyoon <geuyoon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 16:41:51 by geuyoon           #+#    #+#             */
/*   Updated: 2025/11/11 10:52:20 by geuyoon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./ft_irc_server.hpp"

const std::string	Server::invalNickChar_ = " ,*?!@";
const std::string	Server::invalNickStartChar_ = "$:";
const std::string	Server::channelPrefix_ = "#&";
const std::string	Server::commandList_[commandSize] = {
	COMMAND_KICK,
	COMMAND_INVITE,
	COMMAND_TOPIC,
	COMMAND_MODE,
	COMMAND_NICK,
	COMMAND_PRIVMSG,
	COMMAND_USER,
	COMMAND_PING,
	COMMAND_PASS,
	COMMAND_JOIN,
	COMMAND_PART,
	COMMAND_QUIT,
	COMMAND_CAP
};

Server::Server(char **argv)
	: port_(atoi(argv[0])), password_(argv[1]), serverName_("GeuIrc"), version_("1.0"), usermod_("i"), chanmod_("itkol"), tokens_("CHANNELLEN=16 NICKLEN=8")
{
	std::time_t	serverCreateTime = std::time(NULL);
	std::tm		*time = std::localtime(&serverCreateTime);
	std::strftime(this->serverCreateTime_, sizeof(this->serverCreateTime_), "%Y-%m-%d %H:%M:%S", time);

	if (this->port_ < 1 || this->port_ > 65535)
		throw std::out_of_range("Error: Port number is Invalid");
	this->serverSock_ = socket(AF_INET, SOCK_STREAM, 0);
	if (this->serverSock_ == -1)
		throw std::runtime_error("Error: Server Fd Cannot allocation");
    fcntl(this->serverSock_, F_SETFL, O_NONBLOCK);
	// command function list init
	commandFuncs[0] = &Server::commandKick;
	commandFuncs[1] = &Server::commandInvite;
	commandFuncs[2] = &Server::commandTopic;
	commandFuncs[3] = &Server::commandMode;
	commandFuncs[4] = &Server::commandNick;
	commandFuncs[5] = &Server::commandPrivmsg;
	commandFuncs[6] = &Server::commandUser;
	commandFuncs[7] = &Server::commandPong;
	commandFuncs[8] = &Server::commandPass;
	commandFuncs[9] = &Server::commandJoin;
	commandFuncs[10] = &Server::commandPart;
	commandFuncs[11] = &Server::commandQuit;
	commandFuncs[12] = &Server::commandCap;
	// targetmin parameter init
	targetMin_[COMMAND_KICK] = TARGETMINKICK;
	targetMin_[COMMAND_INVITE] = TARGETMININVITE;
	targetMin_[COMMAND_TOPIC] = TARGETMINTOPIC;
	targetMin_[COMMAND_MODE] = TARGETMINMODE;
	targetMin_[COMMAND_NICK] = TARGETMINNICK;
	targetMin_[COMMAND_PRIVMSG] = TARGETMINPRIVMSG;
	targetMin_[COMMAND_USER] = TARGETMINUSER;
	targetMin_[COMMAND_PING] = TARGETMINPING;
	targetMin_[COMMAND_PASS] = TARGETMINPASS;
	targetMin_[COMMAND_JOIN] = TARGETMINJOIN;
	targetMin_[COMMAND_PART] = TARGETMINPART;
	targetMin_[COMMAND_QUIT] = TARGETMINQUIT;
	targetMin_[COMMAND_CAP] = TARGETMINCAP;
	// targetmax parameter init
	targetMax_[COMMAND_KICK] = TARGETMAXKICK;
	targetMax_[COMMAND_INVITE] = TARGETMAXINVITE;
	targetMax_[COMMAND_TOPIC] = TARGETMAXTOPIC;
	targetMax_[COMMAND_MODE] = TARGETMAXMODE;
	targetMax_[COMMAND_NICK] = TARGETMAXNICK;
	targetMax_[COMMAND_PRIVMSG] = TARGETMAXPRIVMSG;
	targetMax_[COMMAND_USER] = TARGETMAXUSER;
	targetMax_[COMMAND_PING] = TARGETMAXPING;
	targetMax_[COMMAND_PASS] = TARGETMAXPASS;
	targetMax_[COMMAND_JOIN] = TARGETMAXJOIN;
	targetMax_[COMMAND_PART] = TARGETMAXPART;
	targetMax_[COMMAND_QUIT] = TARGETMAXQUIT;
	targetMax_[COMMAND_CAP] = TARGETMAXCAP;
}

Server::~Server(void)
{
	for (std::vector<Client *>::iterator clientList = this->clients_.begin(); clientList != this->clients_.end(); clientList++)
	{
		if (*clientList)
		{
			close((*clientList)->getFd());
			delete (*clientList);
		}
	}
	this->clients_.clear();
	for (std::vector<Channel *>::iterator channelList = this->channels_.begin(); channelList != this->channels_.end(); channelList++)
	{
		if (*channelList)
		{
			delete (*channelList);
		}
	}
	this->channels_.clear();
	close(this->serverSock_);
}

int	Server::getPort(void) const
{
	return (this->port_);
}

int	Server::getServerFd(void) const
{
	return (this->serverSock_);
}

std::string	Server::getServerName(void) const
{
	return (this->serverName_);
}

std::string	Server::getPassword(void) const
{
	return (this->password_);
}

std::vector<struct pollfd> Server::getFds(void) const
{
	return (this->fds_);
}

void	Server::setPort_(int port)
{
	this->port_ = port;
}

void	Server::setServerFd_(int fd)
{
	this->serverSock_ = fd;

}

void	Server::setPassword_(const std::string &password)
{
	this->password_ = password;
}

void	Server::setFds_(std::vector<struct pollfd> fds)
{
	this->fds_ = fds;
}

void	Server::initServer(void)
{
	struct sockaddr_in	serverSockAddr_;
	socklen_t 			addrLen;

	serverSockAddr_.sin_family = AF_INET;
	serverSockAddr_.sin_addr.s_addr = INADDR_ANY;
	serverSockAddr_.sin_port = htons(this->port_);
	addrLen = sizeof(serverSockAddr_);
	if (bind(this->serverSock_, reinterpret_cast<struct sockaddr*>(&serverSockAddr_), addrLen) == -1)
	{
		close(this->serverSock_);
		throw std::runtime_error("Error: Server Bind Fail");
	}
	if (getsockname(this->serverSock_, reinterpret_cast<struct sockaddr*>(&serverSockAddr_), &addrLen) == -1)
	{
		close(this->serverSock_);
		throw std::runtime_error("Error: Server getsockname Fail");
	}
	if (listen(this->serverSock_, MAXWAITINGCONNECTIONS) == -1)
	{
		close(this->serverSock_);
		throw std::runtime_error("Error: Server Listen Fail");
	}
	struct pollfd	pfd;
	
	pfd.fd = this->serverSock_;
	pfd.events = POLLIN;
	pfd.revents = 0;
	this->fds_.push_back(pfd);
}

void	Server::runServer(void)
{
	std::cout << "Server start" << std::endl;
	while (poll(this->fds_.data(), this->fds_.size(), -1))
	{
		for(size_t fdsCnt = 0; fdsCnt < this->fds_.size(); fdsCnt++)
		{
			// check what pollfd has event, and it is server fd or not
			if (this->fds_[fdsCnt].revents && POLLIN)
			{
				if (this->fds_[fdsCnt].fd != this->serverSock_)
				{
					int		bytes;
					char	buffer[MAX_BUF_SIZE];
					Client	*targetClient = this->clients_[fdsCnt - 1];

					// read buffer and append to remain client buffer
					bytes = recv(this->fds_[fdsCnt].fd, buffer, MAX_BUF_SIZE, 0);
					if (bytes > 0)
					{
						std::string	buf(buffer, bytes);
						int			targetClientFd = targetClient->getFd();

						targetClient->appendBuffer(buf, bytes);
						// while CRLF if leaving, parse the client buffer
						while (targetClient && targetClient->isCompleteMsg())
						{
							this->commandParsor(targetClient, targetClient->getCmd());
							// check if client is deleted while command
							targetClient = this->findClientFd(targetClientFd);
							if (targetClient)
								targetClient->clearCmd();
							else
								break ;
						}
					}
					else
					{
						int	targetClientFd = targetClient->getFd();

						this->clients_.erase(std::remove(this->clients_.begin(), \
							this->clients_.end(), targetClient), this->clients_.end());
						delete (targetClient);
						for (std::vector<pollfd>::iterator it = this->fds_.begin(); it != this->fds_.end(); ++it)
						{
							if (it->fd == targetClientFd)
							{
								this->fds_.erase(it);
								break ;
							}
						}
						close(targetClientFd);
						std::cerr << "ERROR: Client has disconnected: recv" << std::endl;
					}
				}
				else
					this->acceptClient();
			}
		}
	}
	
}

void	Server::acceptClient(void)
{
	sockaddr_in	clientAddr;
	socklen_t	clientAddrLen = sizeof(clientAddr);
	int			clientSock = accept(this->serverSock_, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrLen);

	if (clientSock == -1)
	{
		throw (std::runtime_error("Error: Client Cannot Accept"));
	}
	if (fcntl(clientSock, F_SETFL, O_NONBLOCK) == -1)
	{
		throw (std::runtime_error("Error: Client Socket Cannot change Non-Blocking mode"));
	}

	Client			*client = new Client(clientSock);
	struct pollfd	pfd;

	if (!client)
		throw (std::runtime_error("Error: Client init Fail"));
	pfd.fd = clientSock;
	pfd.events = POLLIN;
	pfd.revents = 0;

	this->fds_.push_back(pfd);
	client->setPort(ntohs(clientAddr.sin_port));
	client->setIpAddr(inet_ntoa(clientAddr.sin_addr));
	this->clients_.push_back(client);
}

void	Server::initClientConnect(Client *client)
{
	client->sendMsg(RPL_WELCOME(this->serverName_, client->getNickName(), this->serverName_, client->getSendString()));
	client->sendMsg(RPL_YOURHOST(this->serverName_, client->getNickName(), this->version_));
	client->sendMsg(RPL_CREATED(this->serverName_, client->getNickName(), this->serverCreateTime_));
	client->sendMsg(RPL_MYINFO(this->serverName_, client->getNickName(), this->version_, this->usermod_, this->chanmod_));
	client->sendMsg(RPL_ISUPPORT(this->serverName_, client->getNickName(), this->tokens_));
	client->sendMsg(RPL_MOTDSTART(this->serverName_, client->getNickName()));
	client->sendMsg(RPL_MOTD(this->serverName_, client->getNickName(), "**************************************************"));
	client->sendMsg(RPL_MOTD(this->serverName_, client->getNickName(), "*             H    E    L    L    O              *"));
	client->sendMsg(RPL_MOTD(this->serverName_, client->getNickName(), "*               Welcome to GeuIrc                *"));
	client->sendMsg(RPL_MOTD(this->serverName_, client->getNickName(), "*           This is private irc server           *"));
	client->sendMsg(RPL_MOTD(this->serverName_, client->getNickName(), "**************************************************"));
	client->sendMsg(RPL_MOTDEND(this->serverName_, client->getNickName()));
}

Channel	*Server::initChannel(Client *client, const std::string &channelName)
{
	Channel	*newChannel = new Channel(channelName, client);

	if (!newChannel)
		throw (std::runtime_error("Error: Channel init Fail"));
	this->channels_.push_back(newChannel);
	client->joinChannel(newChannel);
	return (newChannel);
}

void	Server::commandParsor(Client *client, const std::string& msg)
{
	if (!msg.size())
		return ;
	std::vector<std::string>	args = tokenizeLine(msg);
	std::string					cmd(args[0]);

	for (int cmdFinder = 0; cmdFinder < commandSize; cmdFinder++)
	{
		if (args[0] == this->commandList_[cmdFinder])
		{
			if (this->isTargetMatch(client, args[0], args))
			{
				(this->*commandFuncs[cmdFinder])(client, args);
			}
			return ;
		}
	}
	if (client->getNickName().size())
		client->sendMsg(ERR_UNKNOWNCOMMAND(this->serverName_, client->getNickName(), cmd));
	else
		client->sendMsg(ERR_UNKNOWNCOMMAND(this->serverName_, "*", cmd));
}

void	Server::commandPass(Client *client, const std::vector<std::string> &args)
{
	if (client->getIsRegister())
	{
		client->sendMsg(ERR_ALREADYREGISTERED(this->serverName_, client->getNickName()));
		return ;
	}
	if (client->getIsPass())
		return ;
	if (this->password_ != args[1])
	{
		if (client->getIsNick())
			client->sendMsg(ERR_PASSWDMISMATCH(this->serverName_, client->getNickName()));
		else
			client->sendMsg(ERR_PASSWDMISMATCH(this->serverName_, "*"));
		return ;
	}
	client->setIsPass(true);
}

void	Server::commandNick(Client *client, const std::vector<std::string> &args)
{
	std::string			nick(args[1]);
	const std::string	&msg = client->getSendString() + " " + args[0] + " :" + nick;

	if (client->getNickName() == nick)
		return ;
	int	code = client->isValideNick(nick);
	switch (code)
	{
		case (0):
		{
			if (this->findClient(nick))
			{
				if (client->getNickName().size())
					client->sendMsg(ERR_NICKNAMEINUSE(this->serverName_, client->getNickName(), nick));
				else
					client->sendMsg(ERR_NICKNAMEINUSE(this->serverName_, "*", nick));
				return ;
			}
			else
			{
				client->setNickName(nick);
				client->setIsNick(true);
			}
			break ;
		}
	
		default:
		{
			if (client->getNickName().size())
				this->sendMsgClient(client, client->getNickName(), "", "", "", code);
			else
			{
				int	clientFd = client->getFd();
				
				this->sendMsgClient(client, "*", "", "", "", code);
				this->clients_.erase(std::remove(this->clients_.begin(), \
					this->clients_.end(), client), this->clients_.end());
				delete (client);
				for (std::vector<pollfd>::iterator it = this->fds_.begin(); it != this->fds_.end(); ++it)
				{
					if (it->fd == clientFd)
					{
						this->fds_.erase(it);
						break ;
					}
				}
				close(clientFd);
			}
			return ;
		}
	}
	if (client->getIsRegister())
	{
		std::cout << msg << std::endl;
		client->sendMsg(msg);
		for (std::vector<Channel *>::const_iterator clientChannelList = client->getJoinedChannels().begin(); \
			clientChannelList != client->getJoinedChannels().end(); clientChannelList++)
		{
			this->broadcastChannel(*(clientChannelList), msg, client, false);
		}
	}
	// if it is not registered but all register process is done, change to register
	else if (client->getIsPass() && client->getIsNick() && client->getIsUser())
	{
		client->setIsRegister(true);
		initClientConnect(client);
	}
}

void	Server::commandUser(Client *client, const std::vector<std::string> &args)
{
	if (!client->getIsPass())
	{
		if (client->getNickName().size())
			client->sendMsg(ERR_PASSWDMISMATCH(this->serverName_, client->getNickName()));
		else
			client->sendMsg(ERR_PASSWDMISMATCH(this->serverName_, "*"));
		return ;
	}
	if (args[1].size() && args[args.size() - 1].size())
	{
		client->setUserName(args[1]);
		client->setHostName(args[2]);
		client->setRealName(args[args.size() - 1]);
		if (!client->getIsRegister())
		{
			client->setIsUser(true);
			// if it is not registered but all register process is done, change to register
			if (client->getIsPass() && client->getIsNick() && client->getIsUser())
			{
				client->setIsRegister(true);
				initClientConnect(client);
			}
		}
		else
		{
			const std::string	&msg = client->getSendString() + " " + client->getCmd();

			client->sendMsg(msg);
		}
	}
	else
		client->sendMsg(ERR_NEEDMOREPARAMS(this->serverName_, client->getNickName(), COMMAND_USER));
}
void	Server::commandKick(Client *client, const std::vector<std::string> &args)
{
	if (!client->getIsRegister())
	{
		if (client->getNickName().size())
			client->sendMsg(ERR_NOTREGISTERED(this->serverName_, client->getNickName(), args[0]));
		else
			client->sendMsg(ERR_NOTREGISTERED(this->serverName_, "*", args[0]));
		return ;
	}
	
	std::string	targetChannelName(args[1]);
	if (targetChannelName.size() > CHANNELLEN)
	{
		client->sendMsg(ERR_BADCHANMASK(this->serverName_, client->getNickName(), targetChannelName));
		return ;
	}
	Channel		*targetChannel = this->findChannel(targetChannelName);

	if (!targetChannel)
	{
		client->sendMsg(ERR_NOSUCHCHANNEL(this->serverName_, client->getNickName(), targetChannelName));
		return ;
	}

	std::string	targetClientName(args[2]);
	Client		*targetClient = this->findClient(targetClientName);

	if (!targetClient)
	{
		client->sendMsg(ERR_NOSUCHNICK(this->serverName_, client->getNickName(), targetClientName));
		return ;
	}
	
	int	codes = targetChannel->kickMember(client, targetClient);

	switch (codes)
	{
		case (0):
		{
			std::string	msg = client->getSendString() + " " + client->getCmd();
			
			this->broadcastChannel(targetChannel, msg, client, true);
			targetClient->leaveChannel(targetChannel);
			targetChannel->removeChannelMember(targetClient);
			return ;
		}
		default:
		{
			this->sendMsgClient(client, client->getNickName(), targetChannelName, args[0], targetClientName, codes);
			return ;
		}
	}

}

void	Server::commandInvite(Client *client, const std::vector<std::string> &args)
{
	if (!client->getIsRegister())
	{
		if (client->getNickName().size())
			client->sendMsg(ERR_NOTREGISTERED(this->serverName_, client->getNickName(), args[0]));
		else
			client->sendMsg(ERR_NOTREGISTERED(this->serverName_, "*", args[0]));
		return ;
	}
	
	std::string	targetClientName(args[1]);
	Client		*targetClient = this->findClient(targetClientName);

	if (!targetClient)
	{
		client->sendMsg(ERR_NOSUCHNICK(this->serverName_, client->getNickName(), targetClientName));
		return ;
	}

	std::string	targetChannelName(args[2]);
	if (targetChannelName.size() > CHANNELLEN)
	{
		client->sendMsg(ERR_BADCHANMASK(this->serverName_, client->getNickName(), targetChannelName));
		return ;
	}
	Channel		*targetChannel = this->findChannel(targetChannelName);

	if (!targetChannel)
	{
		client->sendMsg(ERR_NOSUCHCHANNEL(this->serverName_, client->getNickName(), targetChannelName));
		return ;
	}
	
	int	code = targetChannel->inviteMember(client, targetClient);
	
	switch (code)
	{
		case (0):
		{
			std::string	msg = client->getSendString() + " " + args[0] + " " + targetClientName + " :" + targetChannelName;
			
			this->broadcastChannel(targetChannel, msg, client, false);
			targetClient->joinChannel(targetChannel);
			targetChannel->addChannelMember(targetClient);
			client->sendMsg(RPL_INVITING(this->serverName_, client->getNickName(), targetClientName, targetChannelName));
			targetClient->sendMsg(msg);
			return ;
		}
		default:
		{
			this->sendMsgClient(client, client->getNickName(), targetChannelName, args[0], targetClientName, code);
			return ;
		}
	}
}

void	Server::commandTopic(Client *client, const std::vector<std::string> &args)
{
	if (!client->getIsRegister())
	{
		if (client->getNickName().size())
			client->sendMsg(ERR_NOTREGISTERED(this->serverName_, client->getNickName(), args[0]));
		else
			client->sendMsg(ERR_NOTREGISTERED(this->serverName_, "*", args[0]));
		return ;
	}

	std::string	targetChannelName(args[1]);
	if (targetChannelName.size() > CHANNELLEN)
	{
		client->sendMsg(ERR_BADCHANMASK(this->serverName_, client->getNickName(), targetChannelName));
		return ;
	}
	Channel		*targetChannel = this->findChannel(targetChannelName);

	if (!targetChannel)
	{
		client->sendMsg(ERR_NOSUCHCHANNEL(this->serverName_, client->getNickName(), targetChannelName));
		return ;
	}
	
	switch (args.size())
	{
		// 3 means there is new topic
		case (3):
		{
			std::string	topic(args[2]);
			int			code = targetChannel->topicChannel(client, topic);

			switch (code)
			{
				case (0):
				{
					std::string	msg = client->getSendString() + " " + client->getCmd();

					this->broadcastChannel(targetChannel, msg, client, true);
					return ;
				}
			
				default:
				{
					this->sendMsgClient(client, client->getNickName(), targetChannelName, args[0], "", code);
				}
			}
			return ;
		}

		// otherwise, 2, means client requests now topic
		default:
		{
			std::string	channelTopic = targetChannel->getTopic();
	
			if (targetChannel->findTargetClient(client->getNickName()))
			{
				if (channelTopic.size())
					client->sendMsg(RPL_TOPIC(this->serverName_, client->getNickName(), targetChannelName, channelTopic));
				else
					client->sendMsg(RPL_NOTOPIC(this->serverName_, client->getNickName(), targetChannelName));
			}
			else
				client->sendMsg(ERR_NOTONCHANNEL(this->serverName_, client->getNickName(), targetChannelName));
			return ;
		}
	}
}

void	Server::commandMode(Client *client, const std::vector<std::string> &args)
{
	if (!client->getIsRegister())
	{
		if (client->getNickName().size())
			client->sendMsg(ERR_NOTREGISTERED(this->serverName_, client->getNickName(), args[0]));
		else
			client->sendMsg(ERR_NOTREGISTERED(this->serverName_, "*", args[0]));
		return ;
	}

	std::string	targetChannelName(args[1]);
	if (targetChannelName.size() > CHANNELLEN)
	{
		client->sendMsg(ERR_BADCHANMASK(this->serverName_, client->getNickName(), targetChannelName));
		return ;
	}
	Channel		*targetChannel = this->findChannel(targetChannelName);

	if (this->findClient(targetChannelName))
	{
		if (targetChannelName != client->getNickName())
		{
			client->sendMsg(ERR_USERSDONTMATCH(this->serverName_, client->getNickName()));
			return ;
		}
		else if (args[2] == "+i")
		{
			std::string	msg = client->getSendString() + " " + client->getCmd();

			client->setIsInvisible(true);
			client->sendMsg(msg);
			return ;
		}
		else if (args[2] == "-i")
		{
			std::string	msg = client->getSendString() + " " + client->getCmd();

			client->setIsInvisible(false);
			client->sendMsg(msg);
			return ;
		}
		client->sendMsg(ERR_UMODEUNKNOWNFLAG(this->serverName_, client->getNickName()));
		return ;
	}

	if (!targetChannel)
	{
		client->sendMsg(ERR_NOSUCHCHANNEL(this->serverName_, client->getNickName(), targetChannelName));
		return ;
	}
	
	int	code = targetChannel->modeChannel(client, args);

	switch (code)
	{
		case (0):
		{
			std::string	msg = client->getSendString() + " " + client->getCmd();
			
			this->broadcastChannel(targetChannel, msg, client, true);
			return ;
		}
		default:
		{
			this->sendMsgClient(client, client->getNickName(), targetChannelName, args[0], "", code);
			return ;
		}
	}
}

void	Server::commandPrivmsg(Client *client, const std::vector<std::string> &args)
{
	if (!client->getIsRegister())
	{
		if (client->getNickName().size())
			client->sendMsg(ERR_NOTREGISTERED(this->serverName_, client->getNickName(), args[0]));
		else
			client->sendMsg(ERR_NOTREGISTERED(this->serverName_, "*", args[0]));
		return ;
	}

	std::string			cmd(args[0]);
	std::string			recip(args[1]);
	const std::string	&msg = client->getSendString() + " " + client->getCmd();

	if (args[args.size() - 1][0] != ':')
	{
		client->sendMsg(ERR_NOTEXTTOSEND(this->serverName_, client->getNickName()));
		return ;
	}
	
	// check if target is channel or client
	if (this->channelPrefix_.find(recip[0]) != std::string::npos)
	{
		Channel	*targetChannel = this->findChannel(recip);

		if (recip.size() > CHANNELLEN)
		{
			client->sendMsg(ERR_BADCHANMASK(this->serverName_, client->getNickName(), recip));
			return ;
		}
		if (targetChannel)
		{
			this->broadcastChannel(targetChannel, msg, client, false);
			return ;
		}
		else
		{
			client->sendMsg(ERR_NOSUCHCHANNEL(this->serverName_, client->getNickName(), recip));
			return ;
		}
	}
	else
	{	
		Client	*targetClient = this->findClient(recip);
		
		if (targetClient)
		{
			targetClient->sendMsg(msg);
			return ;
		}
		else
		{
			client->sendMsg(ERR_NOSUCHNICK(this->serverName_, client->getNickName(), recip));
			return ;
		}
	}
}


void	Server::commandPong(Client *client, const std::vector<std::string> &args)
{
	client->sendMsg(":" + this->serverName_ + " PONG " + args[1] + " :" + this->serverName_);
}

void	Server::commandJoin(Client *client, const std::vector<std::string> &args)
{
	if (!client->getIsRegister())
	{
		if (client->getNickName().size())
			client->sendMsg(ERR_NOTREGISTERED(this->serverName_, client->getNickName(), args[0]));
		else
			client->sendMsg(ERR_NOTREGISTERED(this->serverName_, "*", args[0]));
		return ;
	}

	std::string	targetChannelName(args[1]);
	if (targetChannelName.size() > CHANNELLEN)
	{
		client->sendMsg(ERR_BADCHANMASK(this->serverName_, client->getNickName(), targetChannelName));
		return ;
	}
	Channel		*targetChannel = this->findChannel(targetChannelName);

	if (targetChannelName[0] == ':')
	{
		client->sendMsg(ERR_NEEDMOREPARAMS(this->getServerName(), client->getNickName(), args[0]));
	}
	else if (!targetChannel)
	{
		// join the new channel
		const std::string	&msg = client->getSendString() + " " + client->getCmd();
		
		targetChannel = initChannel(client, targetChannelName);
		client->sendMsg(msg);
		if (targetChannel->getTopic().size())
			client->sendMsg(RPL_TOPIC(this->serverName_, client->getNickName(), targetChannelName, targetChannel->getTopic()));
		client->sendMsg(RPL_NAMREPLY(this->serverName_, client->getNickName(), targetChannelName, targetChannel->getChannelMembersName()));
		client->sendMsg(RPL_ENDOFNAMES(this->serverName_, client->getNickName(), targetChannelName));
	}
	else
	{
		// join the exist channel
		int	code = targetChannel->joinChannel(client, args);

		switch (code)
		{
			case (0):
			{
				const std::string	&msg = client->getSendString() + " " + client->getCmd();

				client->sendMsg(msg);
				targetChannel->addChannelMember(client);
				client->joinChannel(targetChannel);
				if (targetChannel->getTopic().size())
					client->sendMsg(RPL_TOPIC(this->serverName_, client->getNickName(), targetChannelName, targetChannel->getTopic()));
				client->sendMsg(RPL_NAMREPLY(this->serverName_, client->getNickName(), targetChannelName, targetChannel->getChannelMembersName()));
				client->sendMsg(RPL_ENDOFNAMES(this->serverName_, client->getNickName(), targetChannelName));
				this->broadcastChannel(targetChannel, msg, client, false);
				return ;
			}
			
			default:
			{
				this->sendMsgClient(client, client->getNickName(), targetChannelName, args[0], "", code);
				return ;
			}
		}
	}
}

void	Server::commandPart(Client *client, const std::vector<std::string> &args)
{
	if (!client->getIsRegister())
	{
		if (client->getNickName().size())
			client->sendMsg(ERR_NOTREGISTERED(this->serverName_, client->getNickName(), args[0]));
		else
			client->sendMsg(ERR_NOTREGISTERED(this->serverName_, "*", args[0]));
		return ;
	}

	const std::string	&targetChannelName(args[1]);
	if (targetChannelName.size() > CHANNELLEN)
	{
		client->sendMsg(ERR_BADCHANMASK(this->serverName_, client->getNickName(), targetChannelName));
		return ;
	}
	Channel				*targetChannel = this->findChannel(targetChannelName);
	const std::string	&msg = client->getSendString() + " " + client->getCmd();

	if (targetChannel)
	{
		int	code = targetChannel->partChannelMember(client);

		switch (code)
		{
			// part(leave) channel then check if channel is empty or not
			case (0):
			{
				this->broadcastChannel(targetChannel, msg, client, true);
				targetChannel->removeChannelMember(client);
				client->leaveChannel(targetChannel);
				if (!targetChannel->getChannelMembers().size())
				{
					this->channels_.erase(std::remove(this->channels_.begin(), \
						this->channels_.end(), targetChannel), this->channels_.end());
					delete	(targetChannel);
				}
				return ;
			}

			default:
			{
				this->sendMsgClient(client, client->getNickName(), targetChannelName, args[0], "", code);
				return ;
			}
		}
	}
	else
		client->sendMsg(ERR_NOSUCHCHANNEL(this->serverName_, client->getNickName(), targetChannelName));
}

void	Server::commandQuit(Client *client, const std::vector<std::string> &args)
{
	(void)args;
	Client								*targetClient = client;
	int									clientFd = targetClient->getFd();
	std::vector<Channel *>::iterator	channelList = this->channels_.begin();
	Channel								*targetChannel;

	while (channelList != this->channels_.end())
	{
		(*channelList)->removeChannelMember(client);
		client->leaveChannel(*channelList);
		if ((*channelList)->getChannelMembers().empty())
		{
			targetChannel = *(channelList);
			channelList = this->channels_.erase(channelList);
			delete (targetChannel);
		}
		else
		{
			channelList++;
		}
	}

	this->clients_.erase(std::remove(this->clients_.begin(), \
		this->clients_.end(), client), this->clients_.end());
	for (std::vector<pollfd>::iterator it = this->fds_.begin(); it != this->fds_.end(); ++it)
	{
		if (it->fd == clientFd)
		{
			this->fds_.erase(it);
			break;
		}
	}
	close(clientFd);
	delete (targetClient);
}

void	Server::commandCap(Client *client, const std::vector<std::string> &args)
{
	(void)args;
	client->sendMsg(this->serverName_ + " CAP * LS");
}

bool	Server::commandNickValid(Client *client, const std::string &nick)
{
	if (invalNickStartChar_.find(nick[0]) != std::string::npos || \
		nick.find_first_of(invalNickChar_) != std::string::npos)
	{
		client->sendMsg(ERR_ERRONEUSNICKNAME(this->serverName_, client->getUserName(), nick));
		return (false);
	}
	if (this->findClient(nick))
	{
		client->sendMsg(ERR_NICKNAMEINUSE(this->serverName_, client->getNickName(), nick));
		return (false);
	}
	return (true);
}

Client	*Server::findClient(const std::string &nickName)
{
	for (std::vector<Client *>::iterator clientList = this->clients_.begin(); clientList != this->clients_.end(); clientList++)
	{
		if (nickName == (*clientList)->getNickName())
		{
			return (*clientList);
		}
	}
	return (NULL);
}

Client	*Server::findClientFd(int fd)
{
	for (std::vector<Client *>::iterator clientList = this->clients_.begin(); clientList != this->clients_.end(); clientList++)
	{
		if (fd == (*clientList)->getFd())
		{
			return (*clientList);
		}
	}
	return (NULL);
}

Channel	*Server::findChannel(const std::string &channelName)
{
	for (std::vector<Channel *>::iterator channelList = this->channels_.begin(); channelList != this->channels_.end(); channelList++)
	{
		if (channelName == (*channelList)->getChannelName())
		{
			return (*channelList);
		}
	}
	return (NULL);
}

int	Server::isTargetMatch(Client *client, const std::string &cmd, const std::vector<std::string> &args) const
{
	int	targetMin = this->getTargetMinCommand(cmd);
	int	targetMax = this->getTargetMaxCommand(cmd);
	
	if (static_cast<int>(args.size()) < targetMin + 1)
	{
		client->sendMsg(ERR_NEEDMOREPARAMS(this->serverName_, client->getNickName(), cmd));
		return (false);
	}
	if (targetMax != -1 && (static_cast<int>(args.size()) > targetMax + 1))
	{
		client->sendMsg(ERR_TOOMANYTARGETS(this->serverName_, client->getNickName()));
		return (false);
	}
	return (true);
}

int	Server::getTargetMinCommand(const std::string &cmd) const
{
	std::map<std::string, int>::const_iterator	target = this->targetMin_.find(cmd);

	if (target != this->targetMin_.end())
		return (target->second);
	return (-1);
}

int	Server::getTargetMaxCommand(const std::string &cmd) const
{
	std::map<std::string, int>::const_iterator	target = this->targetMax_.find(cmd);

	if (target != this->targetMax_.end())
		return (target->second);
	return (-1);
}

void	Server::sendMsgClient(Client *client, const std::string &clientName, const std::string &channelName, \
			const std::string &cmd, const std::string &arg, int code)
{
	switch (code)
	{
		case (301):
		{
			client->sendMsg(RPL_AWAY(this->serverName_, clientName, clientName, arg));
			break;
		}
		
		case (331):
		{
			client->sendMsg(RPL_NOTOPIC(this->serverName_, clientName, channelName));
			break;
		}
		
		case (332):
		{
			client->sendMsg(RPL_TOPIC(this->serverName_, clientName, channelName, arg));
			break;
		}
		
		case (341):
		{
			client->sendMsg(RPL_INVITING(this->serverName_, clientName, arg, channelName));
			break;
		}
		
		case (400):
		{
			client->sendMsg(ERR_UNKNOWNERROR(this->serverName_, clientName, cmd, arg));
			break;
		}
		
		case (401):
		{
			client->sendMsg(ERR_NOSUCHNICK(this->serverName_, clientName, arg));
			break;
		}
		
		case (403):
		{
			client->sendMsg(ERR_NOSUCHCHANNEL(this->serverName_, clientName, arg));
			break;
		}
		
		case (407):
		{
			client->sendMsg(ERR_TOOMANYTARGETS(this->serverName_, clientName));
			break;
		}
		
		case (411):
		{
			client->sendMsg(ERR_NORECIPIENT(this->serverName_, clientName, cmd));
			break;
		}
		
		case (412):
		{
			client->sendMsg(ERR_NOTEXTTOSEND(this->serverName_, clientName));
			break;
		}
		
		case (421):
		{
			client->sendMsg(ERR_UNKNOWNCOMMAND(this->serverName_, clientName, cmd));
			break;
		}
		
		case (431):
		{
			client->sendMsg(ERR_NONICKNAMEGIVEN(this->serverName_, clientName));
			break;
		}
		
		case (432):
		{
			client->sendMsg(ERR_ERRONEUSNICKNAME(this->serverName_, clientName, arg));
			break;
		}
		
		case (433):
		{
			client->sendMsg(ERR_NICKNAMEINUSE(this->serverName_, clientName, arg));
			break;
		}
		
		case (441):
		{
			client->sendMsg(ERR_USERNOTINCHANNEL(this->serverName_, clientName, channelName, arg));
			break;
		}

		case (442):
		{
			client->sendMsg(ERR_NOTONCHANNEL(this->serverName_, clientName, channelName));
			break;
		}
		
		case (443):
		{
			client->sendMsg(ERR_USERONCHANNEL(this->serverName_, clientName, arg, channelName));
			break;
		}
		
		case (451):
		{
			client->sendMsg(ERR_NOTREGISTERED(this->serverName_, clientName, cmd));
			break;
		}
		
		case (461):
		{
			client->sendMsg(ERR_NEEDMOREPARAMS(this->serverName_, clientName, cmd));
			break;
		}
		
		case (462):
		{
			client->sendMsg(ERR_ALREADYREGISTERED(this->serverName_, clientName));
			break;
		}
		
		case (464):
		{
			client->sendMsg(ERR_PASSWDMISMATCH(this->serverName_, clientName));
			break;
		}
		
		case (471):
		{
			client->sendMsg(ERR_CHANNELISFULL(this->serverName_, clientName, channelName));
			break;
		}
		
		case (473):
		{
			client->sendMsg(ERR_INVITEONLYCHAN(this->serverName_, clientName, channelName));
			break;
		}
		
		case (475):
		{
			client->sendMsg(ERR_BADCHANNELKEY(this->serverName_, clientName, channelName));
			break;
		}
		
		case (476):
		{
			client->sendMsg(ERR_BADCHANMASK(this->serverName_, clientName, channelName));
			break;
		}
		
		case (482):
		{
			client->sendMsg(ERR_CHANOPRIVSNEEDED(this->serverName_, clientName, channelName));
			break;
		}
		
		case (501):
		{
			client->sendMsg(ERR_UMODEUNKNOWNFLAG(this->serverName_, clientName));
			break;
		}
		
		default:
		{
			client->sendMsg(ERR_UNKNOWNERROR(this->serverName_, clientName, cmd, arg));
			break;
		}
	}
}

void	Server::broadcastChannel(Channel *channel, const std::string &msg, Client *client, bool clientSend)
{
	const std::vector<Client *>	&channelClient = channel->getChannelMembers();

	if (channel->findTargetClient(client->getNickName()))
	{
		if (clientSend)
		{
			for (std::vector<Client *>::const_iterator channelClientList = channelClient.begin(); channelClientList != channelClient.end(); channelClientList++)
			{
				(*channelClientList)->sendMsg(msg);
			}
		}
		else
		{
			for (std::vector<Client *>::const_iterator channelClientList = channelClient.begin(); channelClientList != channelClient.end(); channelClientList++)
			{
				if ((*channelClientList) != client)
					(*channelClientList)->sendMsg(msg);
			}
		}
	}
	else
		client->sendMsg(ERR_CANNOTSENDTOCHAN(this->serverName_, client->getNickName(), channel->getChannelName()));
}

std::vector<std::string>	tokenizeLine(const std::string& msg)
{
    std::istringstream			tokenParsor(msg);
    std::string 				token;
    std::vector<std::string>	args;

    while (tokenParsor >> token) {
        if (!args.empty() && args.back()[0] == ':')
		{
            args.back() += " " + token;
        }
		else
		{
            args.push_back(token);
        }
    }
    return (args);
}
