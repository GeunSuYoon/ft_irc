/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc_server.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: geuyoon <geuyoon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 16:41:51 by geuyoon           #+#    #+#             */
/*   Updated: 2025/10/24 09:48:35 by geuyoon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// TODO
// Kick func fix
// Topic func fix
// Maybe invite func too

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
	// 무언가 입력이 들어올 때까지 대기
	while (poll(this->fds_.data(), this->fds_.size(), -1))
	{
		// fds 돌면서 입력 들어온거 확인
		for(size_t fdsCnt = 0; fdsCnt < this->fds_.size(); fdsCnt++)
		{
			if (this->fds_[fdsCnt].revents && POLLIN)
			{
				// 해당 fds의 fd가 클라언트일 경우
				if (this->fds_[fdsCnt].fd != this->serverSock_)
				{
					int		bytes;
					char	buffer[MAX_BUF_SIZE];
					Client	*targetClient = this->clients_[fdsCnt - 1];

					// client fd에서 받은 문자 기존 문자열에 추가하기
					bytes = recv(this->fds_[fdsCnt].fd, buffer, MAX_BUF_SIZE, 0);
					if (bytes > 0)
					{
						std::string	buf(buffer, bytes);
						int			targetClientFd = targetClient->getFd();

						targetClient->appendBuffer(buf, bytes);
						// CRLF 기준으로 자르면서 cmd 실행
						while (targetClient && targetClient->isCompleteMsg())
						{
							std::cout << "Before buffer [" << targetClient->getCmd() << "]" << std::endl;
							this->commandParsor(targetClient, targetClient->getCmd());
							std::cout << "Finish cmd" << std::endl;
							// cmd 실행 도중 client가 없어졌을 가능성이 존재하므로 fd 기준으로 다시 갱신
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

	// client socket 초기화
	if (clientSock == -1)
	{
		throw (std::runtime_error("Error: Client Cannot Accept"));
	}
	if (fcntl(clientSock, F_SETFL, O_NONBLOCK) == -1)
	{
		throw (std::runtime_error("Error: Client Socket Cannot change Non-Blocking mode"));
	}

	// client 및 client pollfd 초기화
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
	// 첫 연결 시 motd 보내기
	client->sendMsg(RPL_WELCOME(this->serverName_, client->getNickName(), this->serverName_, client->getSendString()));
	client->sendMsg(RPL_YOURHOST(this->serverName_, client->getNickName(), "GeuIrc-1.0"));
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
	std::vector<std::string>	args = tokenizeLine(msg);
	std::string					cmd(args[0]);

	// 들어온 명령어를 parsing한 뒤 해당하는 커멘드 있는지 확인
	for (int cmdFinder = 0; cmdFinder < commandSize; cmdFinder++)
	{
		// 있으면 매개변수 개수 확인 후 실행
		if (args[0] == this->commandList_[cmdFinder])
		{
			if (this->isTargetMatch(client, args[0], args))
			{
				(this->*commandFuncs[cmdFinder])(client, args);
			}
			return ;
		}
	}
	// 없을 때 client에게 에러 전송
	if (client->getNickName().size())
		client->sendMsg(ERR_UNKNOWNCOMMAND(this->serverName_, client->getNickName(), cmd));
	else
		client->sendMsg(ERR_UNKNOWNCOMMAND(this->serverName_, "*", cmd));
}

void	Server::commandPass(Client *client, const std::vector<std::string> &args)
{
	// 입력한 비밀번호 맞는지 확인
	if (this->password_ != args[1])
	{
		this->sendMsgClient(client, client->getNickName(), "", args[0], "", 464);
		// client->sendMsg(ERR_PASSWDMISMATCH(this->serverName_, client->getNickName()));
		return ;
	}
	if (client->getIsRegister() || client->getIsPass())
	{
		this->sendMsgClient(client, client->getNickName(), "", args[0], "", 462);
		// client->sendMsg(ERR_ALREADYREGISTERED(this->serverName_, client->getNickName()));
		return ;
	}
	client->setIsPass(true);
}

void	Server::commandNick(Client *client, const std::vector<std::string> &args)
{
	std::string	nick(args[1]);

	// 입력한 닉네임이 이전 닉네임과 같으면 무시
	if (client->getNickName() == nick)
		return ;
	// 닉네임 유효성 확인 후 적절한 행동
	int	code = client->isValideNick(nick);
	switch (code)
	{
		case (0):
		{
			// 유효하지만 이미 존재하는 닉네임이면 에러, 없으면 닉네임 설정
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
				// client->sendMsg(ERR_NONICKNAMEGIVEN(this->serverName_, client->getNickName()));
			else
				this->sendMsgClient(client, "*", "", "", "", code);
				// client->sendMsg(ERR_NONICKNAMEGIVEN(this->serverName_, "*"));
			return ;
		}
	}
	// 이미 등록된 클라이언트라면 해당 사실 고지
	// 아니라면 초입 3단계 확인 후 등록 및 motd
	if (client->getIsRegister())
	{
		const std::string	&msg = client->getSendString() + " " + client->getCmd();
		
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
	// 비밀번호 제대로 안쳤으면 에러
	if (!client->getIsPass())
	{
		if (client->getNickName().size())
			this->sendMsgClient(client, client->getNickName(), "", "", "", 464);
			// client->sendMsg(ERR_PASSWDMISMATCH(this->serverName_, client->getNickName()));
		else
			this->sendMsgClient(client, "*", "", "", "", 464);
			// client->sendMsg(ERR_PASSWDMISMATCH(this->serverName_, "*"));
		return ;
	}
	// 입력이 제대로 들어왔으면 적절한 행동
	// 아니라면 에러
	if (args[1].size() && args[args.size() - 1].size())
	{
		client->setUserName(args[1]);
		client->setHostName(args[2]);
		client->setRealName(args[args.size() - 1]);
		// 등록되지 않은 클라이언트면 3단계 확인 후 등록
		// 등록됐다면 관련 사항 고지
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
			const std::string	&msg = client->getSendString() + " " + client->getCmd();

			client->sendMsg(msg);
		}
	}
	else
		this->sendMsgClient(client, client->getNickName(), "", COMMAND_USER, "", 464);
		// client->sendMsg(ERR_NEEDMOREPARAMS(this->serverName_, client->getNickName(), COMMAND_USER));
}
void	Server::commandKick(Client *client, const std::vector<std::string> &args)
{
	// 목표 채널 및 클라이언트 유효성 검사
	std::string	targetChannelName(args[1]);
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
	// 있으면 해당하는 클라이언트 밴 혹은 오류메시지
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
	// 목표 채널 및 클라이언트 유효성 검사
	std::string	targetClientName(args[1]);
	Client		*targetClient = this->findClient(targetClientName);

	if (!targetClient)
	{
		client->sendMsg(ERR_NOSUCHNICK(this->serverName_, client->getNickName(), targetClientName));
		return ;
	}

	std::string	targetChannelName(args[2]);
	Channel		*targetChannel = this->findChannel(targetChannelName);

	if (!targetChannel)
	{
		client->sendMsg(ERR_NOSUCHCHANNEL(this->serverName_, client->getNickName(), targetChannelName));
		return ;
	}
	// 멤버 초대해보기
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
			std::string	topic(args[2]);
			switch (targetChannel->topicChannel(client, topic))
			{
				case (482):
				{
					client->sendMsg(ERR_CHANOPRIVSNEEDED(this->getServerName(), client->getNickName(), targetChannelName));
					return ;
				}
			
				default:
				{
					std::string	msg = client->getSendString() + " " + client->getCmd();
					
					this->broadcastChannel(targetChannel, msg, client, true);
					return ;
				}
			}
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
	std::string			cmd(args[0]);
	std::string			recip(args[1]);
	const std::string	&msg = client->getSendString() + " " + client->getCmd();

	if (args[args.size() - 1][0] != ':')
	{
		client->sendMsg(ERR_NOTEXTTOSEND(this->serverName_, client->getNickName()));
		return ;
	}
	if (this->channelPrefix_.find(recip[0]) != std::string::npos)
	{
		Channel	*targetChannel = this->findChannel(recip);

		if (targetChannel)
		{
			if (targetChannel->getChannelMembers().size() != 1)
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
	std::string	targetChannelName(args[1]);
	Channel		*targetChannel = this->findChannel(targetChannelName);

	if (targetChannelName[0] == ':')
	{
		if (client->getNickName().size())
			client->sendMsg(ERR_NEEDMOREPARAMS(this->getServerName(), client->getNickName(), args[0]));
		else
			client->sendMsg(ERR_NOTREGISTERED(this->getServerName(), args[0]));
	}
	else if (!targetChannel)
	{
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
		const std::string	&msg = client->getSendString() + " " + client->getCmd();

		client->sendMsg(msg);
		targetChannel->addChannelMember(client);
		client->joinChannel(targetChannel);
		if (targetChannel->getTopic().size())
			client->sendMsg(RPL_TOPIC(this->serverName_, client->getNickName(), targetChannelName, targetChannel->getTopic()));
		client->sendMsg(RPL_NAMREPLY(this->serverName_, client->getNickName(), targetChannelName, targetChannel->getChannelMembersName()));
		client->sendMsg(RPL_ENDOFNAMES(this->serverName_, client->getNickName(), targetChannelName));
		this->broadcastChannel(targetChannel, msg, client, false);
	}
}

void	Server::commandPart(Client *client, const std::vector<std::string> &args)
{
	const std::string	&msg = client->getSendString() + " " + client->getCmd();
	const std::string	&targetChannelName(args[1]);
	Channel				*targetChannel = this->findChannel(targetChannelName);

	if (targetChannel)
	{
		int	code = targetChannel->partChannelMember(client);
		switch (code)
		{
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
				break;
			}

			default:
			{
				this->sendMsgClient(client, client->getNickName(), targetChannelName, args[0], "", code);
				break;
				// client->sendMsg(ERR_NOTONCHANNEL(this->serverName_, client->getNickName(), targetChannelName));
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

	// quit 명령어 실행 시 channel들에서 client 제거
	while (channelList != this->channels_.end())
	{
		(*channelList)->removeChannelMember(client);
		client->leaveChannel(*channelList);
		// channel이 비면 제거
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
	// client 제거
	this->clients_.erase(std::remove(this->clients_.begin(), \
		this->clients_.end(), client), this->clients_.end());
	// client에 대항하는 pollfd 제거
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
	// 딱히 유효한 CAP 없음
	client->sendMsg(this->serverName_ + " CAP * LS");
}

bool	Server::commandNickValid(Client *client, const std::string &nick)
{
	// 입력받은 nickname 유효성 확인
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

// nickname 기준 client 탐색
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

// fd 기준 client 탐색
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

// channelname 기준 channel 탐색
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

// cmd 인자 유효성 확인
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
			client->sendMsg(ERR_NOTREGISTERED(this->serverName_, cmd));
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
			// client->sendMsg(ERR_NONICKNAMEGIVEN(this->serverName_, clientName));
			break;
		}
	}
}

// channel 내부 모든 client에게 메시지 보내기
void	Server::broadcastChannel(Channel *channel, const std::string &msg, Client *client, bool clientSend)
{
	const std::vector<Client *>	&channelClient = channel->getChannelMembers();

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
