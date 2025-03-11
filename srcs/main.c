/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/11 21:43:13 by hubourge          #+#    #+#             */
/*   Updated: 2025/03/11 17:45:25 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

int g_stop_code = PROCESS;
void	print_struct(t_ping * ping);

int	main(int ac, char **av)
{
	t_ping	*ping = NULL;

	ping = malloc(sizeof(t_ping));
	if (!ping)
	{
		perror("malloc");
		error(EXIT_FAILURE, ping);
	}

	init_struct(ping);
	parsing(ac, av, ping);
	init_socket_dest(ping);
	init_icmp_packet(ping);
	process(ping);
	// print_struct(ping);
	
	error(-1, ping);
	return (0);
}

// void	print_struct(t_ping * ping)
// {
// 	printf("ip:   %s\n", ping->ip);
// 	printf("host: %s\n", ping->host);
// 	printf("flag v:    %d\n", ping->flags->v);
// 	printf("flag f:    %d\n", ping->flags->f);
// 	printf("flag l:    %d\n", ping->flags->l);
// 	printf("flag n:    %d\n", ping->flags->n);
// 	printf("flag w:    %d\n", ping->flags->w);
// 	printf("flag W:    %d\n", ping->flags->W);
// 	printf("flag p:    %d\n", ping->flags->p);
// 	printf("flag r:    %d\n", ping->flags->r);
// 	printf("flag s:    %d\n", ping->flags->s);
// 	printf("flag T:    %d\n", ping->flags->T);
// 	printf("flag ttl:  %d\n", ping->flags->ttl);
// }
