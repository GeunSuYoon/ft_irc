/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc_client.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: geuyoon <geuyoon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 09:11:48 by geuyoon           #+#    #+#             */
/*   Updated: 2025/10/22 12:13:24 by geuyoon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./ft_irc_client.hpp"

const std::string	Client::invalNickChar_ = " ,*?!@";
const std::string	Client::invalNickStartChar_ = "$:";

Client::Client(int fd)
	: fd_(fd), port_(fd), passwdCorrect_(false), isRegister_(false), ipAddr_(""), userName_(""), nickName_(""), realName_(""), hostName_(""), password_(""), buffer_("")
{ }

Client::~Client(void)
{ }

int	Client::getFd(void) const
{
	return (this->fd_);
}

uint16_t	Client::getPort(void) const
{
	return (this->port_);
}

bool	Client::getPasswdCorrect(void) const
{
	return (this->passwdCorrect_);
}

bool	Client::getRegister(void) const
{
	return (this->isRegister_);
}

std::string	const	&Client::getIpAddr(void) const
{
	return (this->ipAddr_);
}

std::string	const	&Client::getUserName(void) const
{
	return (this->userName_);
}

std::string	const	&Client::getNickName(void) const
{
	return (this->nickName_);
}

std::string	const	&Client::getRealName(void) const
{
	return (this->realName_);
}

std::string	const	&Client::getHostName(void) const
{
	return (this->hostName_);
}

std::string	const	&Client::getPassword(void) const
{
	return (this->password_);
}

std::string	const	&Client::getBuffer(void) const
{
	return (this->buffer_);
}

std::string	const	&Client::getCmd(void) const
{
	return (this->cmd_);
}

std::string	Client::getSendString(void) const
{
	return (this->nickName_ + "!" + this->userName_ + "@" + this->hostName_);
}

void	Client::setPort(uint16_t port)
{
	this->port_ = port;
}

void	Client::setPasswdCorrect(bool passwdCorrect)
{
	this->passwdCorrect_ = passwdCorrect;
}

void	Client::setRegister(bool isRegister)
{
	this->isRegister_ = isRegister;
}

void	Client::setIpAddr(const std::string &ipAddr)
{
	this->ipAddr_ = ipAddr;
}

void	Client::setUserName(const std::string &userName)
{
	this->userName_ = userName;
}

void	Client::setRealName(const std::string &realName)
{
	this->realName_ = realName;
}

void	Client::setNickName(const std::string &nickName)
{
	this->nickName_ = nickName;
}

void	Client::setPassword(const std::string &password)
{
	this->password_ = password;
}

void	Client::sendMsg(const std::string &msg)
{
	std::string	message = msg + "\r\n";

	send(this->fd_, message.c_str(), message.size(), 0);
}

bool	Client::isCompleteMsg(void)
{
	size_t	pos = this->buffer_.find(CRLF);

	if (pos == std::string::npos)
		return (false);
	this->cmd_ = this->buffer_.substr(0, pos);
	this->buffer_.erase(pos + 2);
	return (true);
}

void	Client::appendBuffer(const std::string &buffer, size_t bufSize)
{
	size_t	len = std::min(bufSize, buffer.size());

	// std::cout << "fd [" << this->fd_ << "]" << std::endl;
	// std::cout << "Before buffer [" << this->buffer_ << "]" << std::endl;
	// this->buffer_.append(buffer, 0, len);
	try 
	{
        this->buffer_.append(buffer, 0, len);
    } 
	catch (const std::exception &e) 
	{
        std::cerr << "append failed: " << e.what() << std::endl;
    }
}

void	Client::clearCmd(void)
{
	this->cmd_.clear();
}

void	Client::joinChannel(Channel *channel)
{
	this->joinedChannels_.push_back(channel);
}

void	Client::leaveChannel(Channel *channel)
{
	this->joinedChannels_.erase(std::remove(this->joinedChannels_.begin(), this->joinedChannels_.end(), channel), \
		this->joinedChannels_.end());
	// this->joinedChannels_.push_back(channel);
}

int	Client::isValideNick(const std::string &nick)
{
	if (!nick.size())
		return (431);
	if (invalNickStartChar_.find(nick[0]) != std::string::npos || \
		nick.find_first_of(invalNickChar_) != std::string::npos)
		return (432);
	return (0);
}

// bool	Client::commandNickValid(const std::string &nick)
// {
// 	if (invalNickStartChar_.find(nick[0]) != std::string::npos || 
// 		nick.find_first_of(invalNickChar_) != std::string::npos)
// 	{
// 		client.sendMsg(ERR_ERRONEUSNICKNAME(this->serverName_, client.getUserName(), nick));
// 		return (false);
// 	}
// 	if (this->findClient(nick))
// 	{
// 		client.sendMsg(ERR_NICKNAMEINUSE(this->serverName_, client.getNickName(), nick));
// 		return (false);
// 	}
// 	return (true);
// }
