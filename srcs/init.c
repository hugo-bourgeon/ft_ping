/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
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
	memset(&ping->time_now, 0, sizeof(struct timeval));
	memset(&ping->time_last, 0, sizeof(struct timeval));

	ping->stats = malloc(sizeof(t_stats));
	if (!ping->stats)
		error(EXIT_FAILURE, ping);
	ping->stats->print = 0;
	ping->stats->min = 999999;
	ping->stats->max = 0;
	ping->stats->avg = 0;
	ping->stats->mdev = 0;
	ping->stats->nb_sent = 0;
	ping->stats->nb_received = 0;
	ping->stats->nb_lost = 0;
}

void	init_socket_dest(t_ping *ping)
{
	// Create a raw socket
	if ((ping->socketfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
	{
		perror("socket");
		error(EXIT_FAILURE, ping);
	}

	// Fill in the destination information
	memset(&ping->dest_addr, 0, sizeof(ping->dest_addr));
	ping->dest_addr.sin_family		= AF_INET;
	ping->dest_addr.sin_port		= 0;
	ping->dest_addr.sin_addr.s_addr	= inet_addr(ping->ip);
}

void	init_icmp_packet(t_ping *ping)
{
	// ICMP packet
	ping->dest_icmp = (struct icmphdr *)ping->packet;	
	memset(ping->packet, 0, PACKET_SIZE);
	ping->dest_icmp->type				= ICMP_ECHO;
	ping->dest_icmp->code				= 0;
	ping->dest_icmp->checksum			= 0;
	ping->dest_icmp->un.echo.id			= getpid();
	ping->dest_icmp->un.echo.sequence	= 0;

	ping->dest_icmp->checksum = checksum(ping->packet, sizeof(ping->packet));
}
