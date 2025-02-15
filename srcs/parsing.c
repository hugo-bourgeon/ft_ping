/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 17:52:27 by hubourge          #+#    #+#             */
/*   Updated: 2025/02/15 15:38:39 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void	init_struct(t_ping *ping)
{
	ping->ip			= NULL;
	ping->host			= NULL;
	ping->socketfd		= -1;
	ping->nb_sequence	= 0;
	ping->nb_received	= 0;
	ping->print_stats	= 0;
	memset(&ping->time_now, 0, sizeof(struct timeval));
	memset(&ping->time_last, 0, sizeof(struct timeval));
}

void	parsing(int ac, char **av, t_ping *ping)
{
	if (ac < 2)
	{
		ft_putstr_fd("Usage: ft_ping [hostname]\n", STDERR_FILENO);
		error(EXIT_FAILURE, ping);
	}

	struct hostent *host_entity = gethostbyname(av[1]);
	if (!host_entity)
	{
		ft_printf(STDERR_FILENO, "ping: cannot resolve %s: Unknown host\n", av[1]);
		error(EXIT_FAILURE, ping);
	}
	ping->ip			= inet_ntoa(*(struct in_addr *)host_entity->h_addr);
	ping->host			= av[1];
}
