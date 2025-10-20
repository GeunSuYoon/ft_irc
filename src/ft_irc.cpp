/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: geuyoon <geuyoon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 12:22:57 by geuyoon           #+#    #+#             */
/*   Updated: 2025/10/16 10:43:49 by geuyoon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_irc.hpp"
#include "./engine/ft_irc_server.hpp"

int	main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Error: Invalid program arguments" << std::endl;
		return (1);
	}
	try
	{
		Server	ircServ(argv + 1);

		ircServ.initServer();
		ircServ.runServer();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (errno);
	}
}
