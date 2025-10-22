/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc_server.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: geuyoon <geuyoon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 16:41:51 by geuyoon           #+#    #+#             */
/*   Updated: 2025/10/22 15:27:57 by geuyoon          ###   ########.fr       */
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
	: port_(atoi(argv[0])), password_(argv[1]), serverName_("GeuIrc")
{
	if (this->port_ < 1 || this->port_ > 65535)
		throw std::out_of_range("Error: Port number is Invalid");
	this->serverSock_ = socket(AF_INET, SOCK_STREAM, 0);
	if (this->serverSock_ == -1)
		throw std::runtime_error("Error: Server Fd Cannot allocation");
    int flags = fcntl(this->serverSock_, F_GETFL, 0);
    fcntl(this->serverSock_, F_SETFL, flags | O_NONBLOCK);
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
		throw std::runtime_error(std::string("Bind failed: ") + strerror(errno));
		// throw std::runtime_error("Error: Server Bind Fail");
	}
	else
		std::cout << "Bind Success" << std::endl;
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
	else
		std::cout << "Listen Success" << std::endl;
	struct pollfd	pfd;
	
	pfd.fd = this->serverSock_;
	pfd.events = POLLIN;
	pfd.revents = 0;

	this->fds_.push_back(pfd);
}

void	Server::runServer(void)
{
	while (poll(this->fds_.data(), this->fds_.size(), -1))
	{
		for(size_t fdsCnt = 0; fdsCnt < this->fds_.size(); fdsCnt++)
		{
			if (this->fds_[fdsCnt].revents && POLLIN)
			{
				if (this->fds_[fdsCnt].fd != this->serverSock_)
				{
					int		bytes;
					char	buffer[MAX_BUF_SIZE];
					Client	*targetClient = this->clients_[fdsCnt - 1];
					// char	*buffer;

					// client fd에서 받은 문자 기존 문자열에 추가하기
					bytes = recv(this->fds_[fdsCnt].fd, buffer, MAX_BUF_SIZE, 0);
					if (bytes > 0)
					{
						std::string	buf(buffer, bytes);
						int			targetClientFd = targetClient->getFd();

						targetClient->appendBuffer(buf, bytes);
						// crlf 있으면 커맨드 실행
						while (targetClient && targetClient->isCompleteMsg())
						{
							std::cout << "Before buffer [" << targetClient->getCmd() << "]" << std::endl;
							this->commandParsor(targetClient, targetClient->getCmd());
							std::cout << "Finish cmd" << std::endl;
							targetClient = this->findClientFd(targetClientFd);
							if (targetClient)
								targetClient->clearCmd();
							else
								break ;
						}
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
		std::cerr << "Error: Client Cannot Accept" << std::endl;
		return ;
	}

	int	clientFlag = fcntl(clientSock, F_GETFL, 0);
	if (clientFlag == -1)
	{
		std::cerr << "Error: Client Socket Flag Cannot Get" << std::endl;
		close(clientSock);
		return ;
	}
	if (fcntl(clientSock, F_SETFL, clientFlag | O_NONBLOCK) == -1)
	{
		std::cerr << "Error: Client Socket Cannot change Non-Blocking mode" << std::endl;
		close(clientSock);
		return ;
	}

	Client			*client = new Client(clientSock);
	struct pollfd	pfd;
	pfd.fd = clientSock;
	pfd.events = POLLIN;
	pfd.revents = 0;

	this->fds_.push_back(pfd);
	client->setPort(ntohs(clientAddr.sin_port));
	client->setIpAddr(inet_ntoa(clientAddr.sin_addr));
	// this->fds_.push_back(pfd);
	// this->fds_.push_back({clientSock, POLLIN, 0});
	this->clients_.push_back(client);
	// this->initClientConnect(client);
}

void	Server::initClientConnect(Client *client)
{
	client->sendMsg(RPL_MOTDSTART(this->serverName_, client->getNickName()));
	client->sendMsg(RPL_MOTD(this->serverName_, client->getNickName(), "**************************************************"));
	client->sendMsg(RPL_MOTD(this->serverName_, client->getNickName(), "*             H    E    L    L    O              *"));
	client->sendMsg(RPL_MOTD(this->serverName_, client->getNickName(), "*               Welcome to GeuIrc                *"));
	client->sendMsg(RPL_MOTD(this->serverName_, client->getNickName(), "*           This is private irc server           *"));
	client->sendMsg(RPL_MOTD(this->serverName_, client->getNickName(), "**************************************************"));
	client->sendMsg(RPL_MOTDEND(this->serverName_, client->getNickName()));
}

void	Server::initChannel(Client *client, const std::string &channelName)
{
	Channel	*newChannel = new Channel(channelName, client);

	this->channels_.push_back(newChannel);
}

void	Server::commandParsor(Client *client, const std::string& msg)
{
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
	if (this->password_ != args[1])
	{
		client->sendMsg(ERR_PASSWDMISMATCH(this->serverName_, client->getNickName()));
		return ;
	}
	if (client->getIsRegister() || client->getIsPass())
	{
		client->sendMsg(ERR_ALREADYREGISTERED(this->serverName_, client->getNickName()));
		return ;
	}
	client->setIsPass(true);
}

void	Server::commandNick(Client *client, const std::vector<std::string> &args)
{
	std::string	nick(args[1]);

	if (client->getNickName() == nick)
		return ;
	switch (client->isValideNick(nick))
	{
		case (431):
		{
			if (client->getNickName().size())
				client->sendMsg(ERR_NONICKNAMEGIVEN(this->serverName_, client->getNickName()));
			else
				client->sendMsg(ERR_NONICKNAMEGIVEN(this->serverName_, "*"));
			return ;
		}
	
		case (432):
		{
			if (client->getNickName().size())
				client->sendMsg(ERR_ERRONEUSNICKNAME(this->serverName_, client->getNickName(), nick));
			else
				client->sendMsg(ERR_ERRONEUSNICKNAME(this->serverName_, "*", nick));
			return ;
		}

		default:
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
		}
	}
	if (client->getIsRegister())
	{
		const std::string	&msg = ":" + client->getSendString() + " " + client->getCmd();
		
		client->sendMsg(msg);
	}
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
			if (client->getIsPass() && client->getIsNick() && client->getIsUser())
			{
				client->setIsRegister(true);
				initClientConnect(client);
			}
		}
		else
		{
			const std::string	&msg = ":" + client->getSendString() + " " + client->getCmd();

			client->sendMsg(msg);
		}
	}
	else
		client->sendMsg(ERR_NEEDMOREPARAMS(this->serverName_, client->getNickName(), COMMAND_USER));
}
void	Server::commandKick(Client *client, const std::vector<std::string> &args)
{
	std::string	targeChannelName(args[1]);
	Channel		*targetChannel = this->findChannel(targeChannelName);

	if (!targetChannel)
	{
		client->sendMsg(ERR_NOSUCHNICK(this->serverName_, client->getNickName(), targeChannelName));
	}

	std::vector<int>	codes = targetChannel->kickMember(client, args);

	for (size_t codeCnt = 0; codeCnt < codes.size(); codeCnt++)
	{
		switch (codes[codeCnt])
		{
			case (442):
			{
				client->sendMsg(ERR_NOSUCHNICK(this->serverName_, client->getNickName(), args[codeCnt + 1]));
				return ;
			}
			case (482):
			{
				client->sendMsg(ERR_CHANOPRIVSNEEDED(this->serverName_, client->getNickName(), targeChannelName));
				return ;
			}
			default:
				return ;
		}
	}
}

void	Server::commandInvite(Client *client, const std::vector<std::string> &args)
{
	std::string	targetClientName(args[1]);
	Client	*targetClient = this->findClient(targetClientName);

	if (!targetClient)
	{
		client->sendMsg(ERR_NOSUCHNICK(this->serverName_, client->getNickName(), targetClientName));
		return ;
	}

	std::string	targetChannelName(args[2]);
	Channel	*targetChannel = this->findChannel(targetChannelName);
	if (!targetChannel)
	{
		client->sendMsg(ERR_NOSUCHCHANNEL(this->serverName_, client->getNickName(), targetChannelName));
		return ;
	}
	switch (targetChannel->inviteMember(client, targetClient))
	{
		case (443):
		{
			client->sendMsg(ERR_USERONCHANNEL(this->serverName_, client->getNickName(), targetClientName, targetChannelName));
			return ;
		}
		case (482):
		{
			client->sendMsg(ERR_CHANOPRIVSNEEDED(this->serverName_, client->getNickName(), targetClientName));
			return ;
		}
		default:
		{
			client->sendMsg(RPL_INVITING(this->serverName_, client->getNickName(), targetClientName, targetChannelName));
			return ;
		}
	}
}

void	Server::commandTopic(Client *client, const std::vector<std::string> &args)
{
	std::string	targetChannelName(args[1]);
	Channel		*targetChannel = this->findChannel(targetChannelName);

	if (!targetChannel)
	{
		client->sendMsg(ERR_NOSUCHCHANNEL(this->serverName_, client->getNickName(), targetChannelName));
		return ;
	}
	switch (args.size())
	{
		case (3):
		{
			targetChannel->setTopic(args[2]);
			return ;
		}
		default:
		{
			std::string	channelTopic = targetChannel->getTopic();	
			if (channelTopic.size())
				std::cout << RPL_TOPIC(this->serverName_, client->getNickName(), targetChannelName, channelTopic);
			else
				std::cout << RPL_NOTOPIC(this->serverName_, client->getNickName(), targetChannelName);
			return ;
		}
	}
}

void	Server::commandMode(Client *client, const std::vector<std::string> &args)
{
	std::string	targetChannelName(args[1]);
	Channel		*targetChannel = this->findChannel(targetChannelName);

	if (this->findClient(targetChannelName))
	{
		return ;
	}
	if (!targetChannel)
	{
		client->sendMsg(ERR_NOSUCHCHANNEL(this->serverName_, client->getNickName(), targetChannelName));
		return ;
	}
	switch (targetChannel->modeChannel(client, args))
	{
		case (461):
		{
			client->sendMsg(ERR_NEEDMOREPARAMS(this->serverName_, client->getNickName(), targetChannel->getChannelName()));
			return ;
		}
		case (482):
		{
			client->sendMsg(ERR_CHANOPRIVSNEEDED(this->serverName_, client->getNickName(), targetChannel->getChannelName()));
			return ;
		}
		case (501):
		{
			client->sendMsg(ERR_UMODEUNKNOWNFLAG(this->serverName_, client->getNickName()));
			return ;
		}
		default:
		{
			if (static_cast<int>(args.size()) == this->targetMin_[args[0]] + 1)
				this->broadcastChannel(targetChannel, MODEMSG(this->serverName_, targetChannel->getChannelName(), args[2]));
			else
				this->broadcastChannel(targetChannel, MODEMSGPARAM(this->serverName_, targetChannel->getChannelName(), args[2], args[3]));
			return ;
		}
	}
}

void	Server::commandPrivmsg(Client *client, const std::vector<std::string> &args)
{
	std::string			cmd(args[0]);
	const std::string	&msg = ":" + client->getSendString() + " " + client->getCmd();

	if (args[args.size() - 1][0] != ':')
	{
		client->sendMsg(ERR_NOTEXTTOSEND(this->serverName_, client->getNickName()));
		return ;
	}
	if (static_cast<int>(args.size()) == this->getTargetMinCommand(cmd) + 1)
	{
		client->sendMsg(ERR_NORECIPIENT(this->serverName_, client->getNickName(), cmd));
		return ;
	}
	for (size_t recipCnt = 1; recipCnt < args.size() - 1; recipCnt++)
	{
		if (this->channelPrefix_.find(args[recipCnt][0]) != std::string::npos)
		{
			Channel	*targetChannel = this->findChannel(args[recipCnt]);

			if (targetChannel)
			{
				// client->sendMsg(RPL_AWAY(this->serverName_, client->getNickName(), targetChannel->getChannelName(), msg));
				if (targetChannel->getChannelMembers().size() != 1)
					this->broadcastChannel(targetChannel, msg);
				return ;
			}
			else
			{
				client->sendMsg(ERR_NOSUCHCHANNEL(this->serverName_, client->getNickName(), args[recipCnt]));
				return ;
			}
		}
		else
		{	
			Client	*targetClient = this->findClient(args[recipCnt]);

			if (targetClient)
			{
				targetClient->sendMsg(msg);
				return ;
			}
			else
			{
				client->sendMsg(ERR_NOSUCHNICK(this->serverName_, client->getNickName(), args[recipCnt]));
				return ;
			}
		}
	}
}


void	Server::commandPong(Client *client, const std::vector<std::string> &args)
{
	client->sendMsg(":" + this->serverName_ + " PONG " + args[1] + " :" + this->serverName_);
}

void	Server::commandJoin(Client *client, const std::vector<std::string> &args)
{
	std::string	targeChannelName(args[1]);
	Channel		*targetChannel = this->findChannel(targeChannelName);

	if (targeChannelName[0] == ':')
	{
		if (client->getNickName().size())
			client->sendMsg(ERR_NEEDMOREPARAMS(this->getServerName(), client->getNickName(), args[0]));
		else
			client->sendMsg(ERR_NOTREGISTERED(this->getServerName(), args[0]));
	}
	else if (!targetChannel)
		initChannel(client, targeChannelName);
	else
	{
		if (targetChannel->getModeInviteOnly() && !targetChannel->findTargetClient(client->getNickName()))
		{
			client->sendMsg(ERR_INVITEONLYCHAN(this->serverName_, client->getNickName(), targetChannel->getChannelName()));	
			return ;
		}
		if (targetChannel->getPassword().size() && (args.size() != 3 || targetChannel->getPassword() != args[2]))
		{
			client->sendMsg(ERR_BADCHANNELKEY(this->serverName_, client->getNickName(), targetChannel->getChannelName()));
			return ;
		}
		const std::string	&msg = ":" + client->getSendString() + " " + client->getCmd();

		targetChannel->addChannelMember(client);
		client->joinChannel(targetChannel);
		this->broadcastChannel(targetChannel, msg);
	}
}

void	Server::commandPart(Client *client, const std::vector<std::string> &args)
{
	const std::string	&msg = ":" + client->getSendString() + " " + client->getCmd();
	const std::string	&targetChannelName(args[1]);
	Channel				*targetChannel = this->findChannel(targetChannelName);

	if (targetChannel)
	{
		switch (targetChannel->partChannelMember(client))
		{
			case (442):
			{
				client->sendMsg(ERR_NOTONCHANNEL(this->serverName_, client->getNickName(), targetChannelName));
				break;
			}

			default:
			{
				client->sendMsg(msg);
				if (targetChannel->getChannelMembers().size())
				{
					this->broadcastChannel(targetChannel, msg);
				}
				else
				{
					this->channels_.erase(std::remove(this->channels_.begin(), \
						this->channels_.end(), targetChannel), this->channels_.end());
					delete	(targetChannel);
				}
				break;
			}
		}
	}
	else
		client->sendMsg(ERR_NOSUCHCHANNEL(this->serverName_, client->getNickName(), targetChannelName));
}

void	Server::commandQuit(Client *client, const std::vector<std::string> &args)
{
	(void)args;
	Client	*targetClient = client;
	int		clientFd = targetClient->getFd();
	std::vector<Channel *>::iterator channelList = this->channels_.begin();

	while (channelList != this->channels_.end())
	{
		(*channelList)->removeChannelMember(client);
		client->leaveChannel(*channelList);
		if ((*channelList)->getChannelMembers().empty())
		{
			channelList = this->channels_.erase(channelList);
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
	delete	(targetClient);
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
	// int	targetMax = this->getTargetMaxCommand(cmd);
	
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

void	Server::broadcastChannel(Channel *channel, const std::string &msg)
{
	const std::vector<Client *>	&channelClient = channel->getChannelMembers();

	for (std::vector<Client *>::const_iterator channelClientList = channelClient.begin(); channelClientList != channelClient.end(); channelClientList++)
	{
		(*channelClientList)->sendMsg(msg);
	}
	// for (size_t channelMemberCnt = 0; channelMemberCnt < channelClient.size(); channelMemberCnt++)
	// {
	// 	channelClient[channelMemberCnt]->sendMsg(msg);
	// }
}

// util funcs

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
