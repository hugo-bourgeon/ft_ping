/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/11 21:43:13 by hubourge          #+#    #+#             */
/*   Updated: 2025/03/13 12:25:48 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

int		g_stop_code = PROCESS;
void	print_struct(t_ping *ping);

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

void	print_struct(t_ping *ping)
{
	printf("ip:   %s\n", ping->ip);
	printf("host: %s\n", ping->host);
	printf("flag v:    %d\n", ping->flags->v);
	printf("flag f:    %d\n", ping->flags->f);
	printf("flag l:    %lld\n", ping->flags->l);
	printf("flag n:    %d\n", ping->flags->n);
	printf("flag w:    %lld\n", ping->flags->w);
	printf("flag W:    %lld\n", ping->flags->W);
	printf("flag p:    %s\n", ping->flags->p);
	printf("flag r:    %d\n", ping->flags->r);
	printf("flag s:    %lld\n", ping->flags->s);
	printf("flag T:    %d\n", ping->flags->T);
	printf("flag ttl:  %d\n", ping->flags->ttl);

	printf("-----------------\n");
}
