/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/11 21:43:13 by hubourge          #+#    #+#             */
/*   Updated: 2025/02/15 17:00:50 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

int g_stop_code = PROCESS;

int	main(int ac, char **av)
{
	t_ping	ping;
	
	init_struct(&ping);
	parsing(ac, av, &ping);
	init_socket_dest(&ping);
	init_imcp_packet(&ping);
	process(&ping);
	
	if(ping.socketfd > 0)
		close(ping.socketfd);
	return (0);
}
