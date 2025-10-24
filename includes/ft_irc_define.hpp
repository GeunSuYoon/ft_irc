/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc_define.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: geuyoon <geuyoon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/14 21:37:50 by geuyoon           #+#    #+#             */
/*   Updated: 2025/10/24 14:35:41 by geuyoon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_IRC_DEFINE_H
# define FT_IRC_DEFINE_H

# define MAX_BUF_SIZE		1024
# define CHANNELLEN			16
# define NICKLEN			8

# define CRLF "\r\n"
// Commands
# define COMMAND_KICK		"KICK"
# define COMMAND_INVITE		"INVITE"
# define COMMAND_TOPIC		"TOPIC"
# define COMMAND_MODE		"MODE"
# define COMMAND_NICK		"NICK"
# define COMMAND_PRIVMSG	"PRIVMSG"
# define COMMAND_USER		"USER"
# define COMMAND_PING		"PING"
# define COMMAND_PASS		"PASS"
# define COMMAND_JOIN		"JOIN"
# define COMMAND_PART		"PART"
# define COMMAND_QUIT		"QUIT"
# define COMMAND_CAP		"CAP"
// targetmin parameter
# define TARGETMINKICK		3
# define TARGETMININVITE	2
# define TARGETMINTOPIC		1
# define TARGETMINMODE		2
# define TARGETMINNICK		1
# define TARGETMINPRIVMSG	2
# define TARGETMINUSER		2
# define TARGETMINPING		1
# define TARGETMINPASS		1
# define TARGETMINJOIN		1
# define TARGETMINPART		1
# define TARGETMINQUIT		0
# define TARGETMINCAP		-1
// targetmax parameter
# define TARGETMAXKICK		3
# define TARGETMAXINVITE	2
# define TARGETMAXTOPIC		2
# define TARGETMAXMODE		3
# define TARGETMAXNICK		1
# define TARGETMAXPRIVMSG	2
# define TARGETMAXUSER		4
# define TARGETMAXPING		1
# define TARGETMAXPASS		1
# define TARGETMAXJOIN		5
# define TARGETMAXPART		1
# define TARGETMAXQUIT		-1
# define TARGETMAXCAP		-1

# define MODEMSG(server, channel, modes) ((":") + server + " MODE #" + channel + " " + modes + " ")
# define MODEMSGPARAM(server, channel, modes, parameter) ((":") + server + " MODE #" + channel + " " + modes + " " + parameter)

# define PRIVMSG(client, target, msg) ((":") + client + " " + target + " :" + msg);

#endif
