/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc_numeric_rpl.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: geuyoon <geuyoon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 12:39:56 by geuyoon           #+#    #+#             */
/*   Updated: 2025/10/24 13:17:04 by geuyoon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_IRC_NUMERIC_rpl_HPP
# define FT_IRC_NUMERIC_rpl_HPP

# include "ft_irc_include.hpp"

// 001
# define RPL_WELCOME(server, client, networkname, nick) ((":") + server + " 001 " + client + " :Welcome to the " + networkname + " Network, " + nick)
// 002
# define RPL_YOURHOST(server, client, version) ((":") + server + " 002 " + client + " :Your host is " + server + ", running version " + version)
// 003
# define RPL_CREATED(server, client, datetime) ((":") + server + " 003 " + client + " This server was created " + datetime)
// 004
# define RPL_MYINFO(server, client, version, usermod, channmod) ((":") + server + " 004 " + client + server + version + usermod + channmod)
// 005
# define RPL_ISUPPORT(server, client, tokens) ((":") + server + " 005 " + client + " " + tokens + " :are supported by this server")

// 301
# define RPL_AWAY(server, client, nickname, msg) ((":") + server + " 301 " + client + " " + nickname + " :" + msg)

// 331
# define RPL_NOTOPIC(server, client, channel) ((":") + server + " 331 " + client + " " + channel + " :No topic is set")
// 332
# define RPL_TOPIC(server, client, channel, topic) ((":") + server + " 332 " + client + " " + channel + " :" + topic)

// 341
# define RPL_INVITING(server, client, nickname, channel) ((":") + server + " 341 " + client + " " + nickname + " :" + channel)

// 353
# define RPL_NAMREPLY(server, client, channel, clientList) ((":") + server + " 353 " + client + " = " + channel + " :" + clientList)

// 366
# define RPL_ENDOFNAMES(server, client, channel) ((":") + server + " 366 " + client + " " + channel + " :End of /NAMES list")

// 372
# define RPL_MOTD(server, client, msg) ((":") + server + " 372 " + client + " :" + msg)
// 375
# define RPL_MOTDSTART(server, client) ((":") + server + " 375 " + client + " :" + server + " message of the day")
// 376
# define RPL_MOTDEND(server, client) ((":") + server + " 376 " + client + " :End of message of the day.")

#endif
