/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/11 21:43:13 by hubourge          #+#    #+#             */
/*   Updated: 2025/05/14 14:36:11 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

int		g_stop_code = PROCESS;

int	main(int ac, char **av)
{
	t_ping	*ping = NULL;

	init_struct(&ping);
	parsing(ac, av, ping);
	init_socket_dest(ping);
	init_icmp_packet(ping);
	process(ping);
	
	free_all(NOT_EXIT, ping);
	return (0);
}
