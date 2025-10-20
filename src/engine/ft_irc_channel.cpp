/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc_channel.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: geuyoon <geuyoon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 09:18:18 by geuyoon           #+#    #+#             */
/*   Updated: 2025/10/16 10:24:25 by geuyoon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./ft_irc_channel.hpp"

const std::string	Channel::modeFlags_[] = {
	"+i",
	"+t",
	"+k",
	"+o",
	"+l",
	"-i",
	"-t",
	"-k",
	"-o",
	"-l"
};


Channel::Channel(const std::string &channelName, Client *client)
	: channelName_(channelName)
{
	this->channelOperator_.push_back(client);
	this->channelMembers_.push_back(client);
}

Channel::~Channel(void)
{
	this->channelMembers_.clear();
}

const std::string	&Channel::getChannelName(void) const
{
	return (this->channelName_);
}

bool	Channel::getInviteOnly(void) const
{
	return (this->modeInviteOnly_);
}

int	Channel::getUserLimit(void) const
{
	return (this->userLimit_);
}

const std::string	&Channel::getTopic(void) const
{
	return (this->topic_);
}

const std::string	&Channel::getPassword(void) const
{
	return (this->password_);
}

const std::vector<Client *>	&Channel::getChannelMembers(void) const
{
	return (this->channelMembers_);
}

// Client	*Channel::getChannelOperator(void) const
// {
// 	return (this->channelOperator_);
// }

void	Channel::setChannelName(const std::string &channelName)
{
	this->channelName_ = channelName;
}

void	Channel::setInviteOnly(bool inviteOnly)
{
	this->modeInviteOnly_ = inviteOnly;
}

void	Channel::setUserLimit(int userLimit)
{
	this->userLimit_ = userLimit;
}

void	Channel::setTopic(const std::string &topic)
{
	this->topic_ = topic;
}

void	Channel::setPassword(const std::string &password)
{
	this->password_ = password;
}

void	Channel::addChannelMember(Client *client)
{
	if (!this->findTargetClient(client->getNickName()))
		this->channelMembers_.push_back(client);
}

void	Channel::removeChannelMember(Client *client)
{
	this->channelMembers_.erase(std::remove(this->channelMembers_.begin(), \
		this->channelMembers_.end(), client), this->channelMembers_.end());
}

void	Channel::addChannelOperator(Client *client)
{
	if (!this->isOperator(client))
		this->channelOperator_.push_back(client);
}

void	Channel::removeChannelOperator(Client *client)
{
	if (this->isOperator(client))
	{
		this->channelOperator_.erase(std::remove(this->channelOperator_.begin(), \
			this->channelOperator_.end(), client), this->channelOperator_.end());
	}
}

int	Channel::partChannelMember(Client *client)
{
	if (this->findTargetClient(client->getNickName()))
	{
		this->removeChannelMember(client);
		client->leaveChannel(this);
		return (0);
	}
	else
		return (442);
}

int	Channel::modeChannel(Client *client, const std::vector<std::string> &args)
{
	if (this->isOperator(client))
	{
		if (args[2].size() != 2)
		{
			return (501);
		}
		switch (static_cast<int>(args[2][0]))
		{
			case (static_cast<int>('+')):
			{
				switch (static_cast<int>(args[2][1]))
				{
					case (static_cast<int>('i')):
					{
						this->modeInviteOnly_ = true;
						return (0);
					}
					case (static_cast<int>('t')):
					{
						this->modeTopicOperator = true;
						return (0);
					}
					case (static_cast<int>('k')):
					{
						if (args.size() < 4)
							return (461);
						else
							this->password_ = args[3];
						return (0);
					}
					case (static_cast<int>('o')):
					{
						Client	*targetClient = this->findTargetClient(args[3]);
						
						if (!targetClient)
							return (442);
						addChannelOperator(targetClient);
						return (0);
					}
					case (static_cast<int>('l')):
						if (args.size() < 4)
							return (461);
						else
							this->userLimit_ = atoi(args[3].c_str());	
						return (0);
					
					default:
						return (501);
				}
			}
			case (static_cast<int>('-')):
			{
				switch (static_cast<int>(args[2][1]))
				{
					case (static_cast<int>('i')):
					{
						this->modeInviteOnly_ = false;
						return (0);
					}
					case (static_cast<int>('t')):
					{
						this->modeTopicOperator = false;
						return (0);
					}
					case (static_cast<int>('k')):
					{
						this->password_.clear();
						return (0);
					}
					case (static_cast<int>('o')):
					{
						Client	*targetClient = this->findTargetClient(args[3]);
						
						if (!targetClient)
							return (442);
						removeChannelOperator(targetClient);
						return (0);
					}
					case (static_cast<int>('l')):
					{
						this->userLimit_ = 0;	
						return (0);
					}
					default:
						return (501);
				}
			}
			default :
				return (501);
		}
	}
	return (482);
}

int	Channel::inviteMember(Client *client, Client *targetClient)
{
	// Client	*alreadyJoinedClient = this->findTargetClient(client->getNickName());

	// check if client is already in this channel
	if (this->isOperator(client))
	{
		if (this->findTargetClient(targetClient->getNickName()))
			return (443);
		targetClient->joinChannel(this);
		this->channelMembers_.push_back(targetClient);
		return (0);
	}
	return (482);
}

std::vector<int>	Channel::kickMember(Client *client, const std::vector<std::string> &args)
{
	std::vector<int>	codes;

	if (this->isOperator(client))
	{
		codes.push_back(0);
		for (size_t argCnt = 2; argCnt < args.size(); argCnt++)
		{
			Client	*targetClient = this->findTargetClient(args[argCnt]);

			if (targetClient)
			{
				targetClient->leaveChannel(this);
				this->channelMembers_.erase(std::remove(this->channelMembers_.begin(), \
					this->channelMembers_.end(), targetClient), this->channelMembers_.end());
				codes.push_back(0);
			}
			else
				codes.push_back(442);
		}
	}
	else
		codes.push_back(482);
	return (codes);
}

bool	Channel::isOperator(Client *client)
{
	for (size_t channelOperatorCnt = 0; channelOperatorCnt < this->channelOperator_.size(); channelOperatorCnt++)
	{
		if (this->channelOperator_[channelOperatorCnt] == client)
			return (true);
	}
	return (false);
}

void	Channel::changeInviteOnly(void)
{
	this->modeInviteOnly_ = !this->modeInviteOnly_;
}

Client	*Channel::findTargetClient(const std::string &nickName)
{
	for (size_t memberCnt = 0; memberCnt < this->channelMembers_.size(); memberCnt++)
	{
		if (this->channelMembers_[memberCnt]->getNickName() == nickName)
		{
			return (this->channelMembers_[memberCnt]);
		}
	}
	return (NULL);
}
