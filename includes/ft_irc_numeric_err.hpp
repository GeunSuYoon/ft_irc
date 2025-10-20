/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc_numeric_err.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: geuyoon <geuyoon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 12:34:45 by geuyoon           #+#    #+#             */
/*   Updated: 2025/10/16 09:31:36 by geuyoon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_IRC_NUMERIC_ERR_HPP
# define FT_IRC_NUMERIC_ERR_HPP

// # include "ft_irc_include.hpp"

// 400
# define ERR_UNKNOWNERROR(server, client, command, info) ((":") + server + " 400 " + client + " " + command + ":" + info) 
// 401
# define ERR_NOSUCHNICK(server, client, nickname) ((":") + server + " 401 " + client + " " + nickname + ":No such nick") 
// 403
# define ERR_NOSUCHCHANNEL(server, client, channel) ((":") + server + " 403 " + client + " " + channel + " :No such channel") 
// 407
# define ERR_TOOMANYTARGETS(server, client) ((":") + server + " 407 " + client + " :Too many targets") 

// 411
# define ERR_NORECIPIENT(server, client, command) ((":") + server + " 411 " + client + " " + command + " :No recipient given") 
// 412
# define ERR_NOTEXTTOSEND(server, client) ((":") + server + " 412 " + client + " :No text to send") 

// 421
# define ERR_UNKNOWNCOMMAND_UNKOWNUSER(server, command) ((":") + server + " 421 * " + command + " :Unknown command") 
# define ERR_UNKNOWNCOMMAND(server, client, command) ((":") + server + " 421 " + client + " " + command + " :Unknown command") 

// 431
# define ERR_NONICKNAMEGIVEN(server, client) ((":") + server + " 431 " + client + " :No nickbname given") 
// 432
# define ERR_ERRONEUSNICKNAME(server, client, nick) ((":") + server + " 432 " + client + " " + nick + " :Erroneus nickname") 
// 433
# define ERR_NICKNAMEINUSE(server, client, nick) ((":") + server + " 433 " + client + " " + nick + " :Nickname is already in use") 

// 442
# define ERR_NOTONCHANNEL(server, client, channel) ((":") + server + " 442 " + client + " " + channel + " :You're not on that channel") 
// 443
# define ERR_USERONCHANNEL(server, client, nickname, channel) ((":") + server + " 443 " + client + " " + nickname + " " + channel + ":is already on channel") 

// 451
# define ERR_NOTREGISTERED(server, command) ((":") + server + " 451 * " + command + " :You have not registered")

// 461
# define ERR_NEEDMOREPARAMS(server, client, command) ((":") + server + " 461 " + client + " " + command + " :Not enough parameters") 
// 462
# define ERR_ALREADYREGISTERED(server, client) ((":") + server + " 462 " + client + " :You may not reregister") 
// 464
# define ERR_PASSWDMISMATCH(server, client) ((":") + server + " 464 " + client + " :Password incorrect") 

// 482
# define ERR_CHANOPRIVSNEEDED(server, client, channel) ((":") + server + " 482 " + client + " " + channel + " :You're not channel operator") 

// 501
# define ERR_UMODEUNKNOWNFLAG(server, client) ((":") + server + " 501 " + client + " :Unknown MODE flag") 

// # define ERR_INVALIDMODEPARAM(client, channel, mode)

#endif
