/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 17:52:27 by hubourge          #+#    #+#             */
/*   Updated: 2025/03/13 14:57:26 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void	init_struct(t_ping *ping)
{
	ping->socketfd		= -1;
	ping->addr_len		= 0;
	ping->ip			= NULL;
	ping->host			= NULL;
	ping->dest_icmp		= NULL;
	ping->recv_icmp		= NULL;
	ping->packet		= NULL;

	memset(&ping->time_now, 0, sizeof(struct timeval));
	memset(&ping->time_last, 0, sizeof(struct timeval));

	ping->stats = malloc(sizeof(t_stats));
	if (!ping->stats)
	{
		perror("malloc");
		error(EXIT_FAILURE, ping);
	}
	ping->stats->print = 0;
	ping->stats->min = 999999;
	ping->stats->max = 0;
	ping->stats->avg = 0;
	ping->stats->mdev = 0;
	ping->stats->nb_sent = 0;
	ping->stats->nb_received = 0;
	ping->stats->nb_lost = 0;

	ping->flags = malloc(sizeof(t_flags));
	if (!ping->flags)
	{
		perror("malloc");
		error(EXIT_FAILURE, ping);
	}
	ping->flags->v = NOTSET;
	ping->flags->f = NOTSET;
	ping->flags->l = NOTSET;
	ping->flags->n = NOTSET;
	ping->flags->w = NOTSET;
	ping->flags->W = 1;
	ping->flags->p = NULL;
	ping->flags->r = NOTSET;
	ping->flags->s = 64;
	ping->flags->T = NOTSET;
	ping->flags->ttl = NOTSET;
	// print_struct(ping);
}

void	init_socket_dest(t_ping *ping)
{
	// Create a raw socket
	if ((ping->socketfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
	{
		perror("socket");
		error(EXIT_FAILURE, ping);
	}

	// Set the TTL (Time To Live) option
	int ttl = ping->flags->ttl;
	if (setsockopt(ping->socketfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)
	{
		perror("setsockopt TTL");
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
	ping->packet = malloc(ping->flags->s);
	if (!ping->packet)
	{
		perror("malloc");
		error(EXIT_FAILURE, ping);
	}
	memset(ping->packet, 0, ping->flags->s);
	ping->dest_icmp = (struct icmphdr *)ping->packet;	
	ping->dest_icmp->type				= ICMP_ECHO;
	ping->dest_icmp->code				= 0;
	ping->dest_icmp->checksum			= 0;
	ping->dest_icmp->un.echo.id			= getpid();
	ping->dest_icmp->un.echo.sequence	= -1;
	
	// ICMP payload
	unsigned char	*payload		= ping->packet + sizeof(struct icmphdr) + 16;
	size_t			payload_size	= ping->flags->s - sizeof(struct icmphdr) - 16;
	
	if (ping->flags->p)
	fill_pattern(payload, ping->flags->p, payload_size);
	ping->dest_icmp->checksum 			= checksum(ping->packet, sizeof(ping->packet));
}
