/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc_numeric_rpl.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: geuyoon <geuyoon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 12:39:56 by geuyoon           #+#    #+#             */
/*   Updated: 2025/10/16 09:31:36 by geuyoon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_IRC_NUMERIC_rpl_HPP
# define FT_IRC_NUMERIC_rpl_HPP

# include "ft_irc_include.hpp"

# define RPL_WELCOME(server, client, networkname, nick, user, host) ((":") + server + " 001 " + client + " :Welcome to the " + networkname + " Network, " + nick) //001

# define RPL_AWAY(server, client, nickname, msg) ((":") + server + " 301 " + client + " " + nickname + " :" + msg) //301

# define RPL_NOTOPIC(server, client, channel) ((":") + server + " 331 " + client + " " + channel + " :No topic is set") //331
# define RPL_TOPIC(server, client, channel, topic) ((":") + server + " 332 " + client + " " + channel + " :" + topic) //332

# define RPL_INVITING(server, client, nickname, channel) ((":") + server + " 341 " + client + " " + nickname + " " + channel) //341

# define RPL_MOTD(server, client, msg) ((":") + server + " 372 " + client + " :" + msg) //372
# define RPL_MOTDSTART(server, client) ((":") + server + " 375 " + client + " :" + server + " message of the day") //375
# define RPL_MOTDEND(server, client) ((":") + server + " 376 " + client + " :End of message of the day.") //376

#endif
