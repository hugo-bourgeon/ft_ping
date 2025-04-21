/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 17:52:27 by hubourge          #+#    #+#             */
/*   Updated: 2025/04/21 15:08:57 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void	init_struct(t_ping **ping)
{
	*ping = malloc(sizeof(t_ping));
	if (!(*ping))
	{
		perror("malloc");
		free_all(EXIT_FAILURE, *ping);
	}

	(*ping)->socketfd		= -1;
	(*ping)->addr_len		= 0;
	(*ping)->ip				= NULL;
	(*ping)->host			= NULL;
	(*ping)->dest_icmp		= NULL;
	(*ping)->recv_icmp		= NULL;
	memset(&(*ping)->time_now, 0, sizeof(struct timeval));
	memset(&(*ping)->time_last, 0, sizeof(struct timeval));

	init_stats(*ping);
	init_flags(*ping);
}

void	init_stats(t_ping *ping)
{
	ping->stats = malloc(sizeof(t_stats));
	if (!ping->stats)
	{
		perror("malloc");
		free_all(EXIT_FAILURE, ping);
	}
	ping->stats->print = 0;
	ping->stats->min = 999999;
	ping->stats->max = 0;
	ping->stats->avg = 0;
	ping->stats->mdev = 0;
	ping->stats->nb_sent = 0;
	ping->stats->nb_received = 0;
	ping->stats->nb_lost = 0;
}

void	init_flags(t_ping *ping)
{
	ping->flags = malloc(sizeof(t_flags));
	if (!ping->flags)
	{
		perror("malloc");
		free_all(EXIT_FAILURE, ping);
	}
	ping->flags->v = NOT_SET;
	ping->flags->f = NOT_SET;
	ping->flags->l = NOT_SET;
	ping->flags->n = NOT_SET;
	ping->flags->w = NOT_SET;
	ping->flags->W = 1;
	ping->flags->p = NULL;
	ping->flags->r = NOT_SET;
	ping->flags->s = 64;
	ping->flags->T = NOT_SET;
	ping->flags->ttl = NOT_SET;
}


void	init_socket_dest(t_ping *ping)
{
	// Create a raw socket
	if ((ping->socketfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
	{
		perror("socket");
		free_all(EXIT_FAILURE, ping);
	}

	// Set the TTL (Time To Live) option
	int ttl = ping->flags->ttl;
	if (setsockopt(ping->socketfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)
	{
		perror("setsockopt TTL");
		free_all(EXIT_FAILURE, ping);
	}

	// SO_DONTROUTE if -r flag is set
	if (ping->flags->r != NOT_SET)
	{
		int flag = 1;
		if (setsockopt(ping->socketfd, SOL_SOCKET, SO_DONTROUTE, &flag, sizeof(flag)) < 0)
		{
			perror("setsockopt SO_DONTROUTE");
			free_all(EXIT_FAILURE, ping);
		}
	}

	// Set the TOS (Type of Service) if specified
	if (ping->flags->T != NOT_SET)
	{
		int tos = ping->flags->T;
		if (setsockopt(ping->socketfd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos)) < 0)
		{
			perror("setsockopt TOS");
			free_all(EXIT_FAILURE, ping);
		}
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
	memset(ping->packet, 0, PACKET_MAX_SIZE);
	ping->dest_icmp = (struct icmphdr *)ping->packet;	
	ping->dest_icmp->type				= ICMP_ECHO;
	ping->dest_icmp->code				= 0;
	ping->dest_icmp->checksum			= 0;
	ping->dest_icmp->un.echo.id			= getpid();
	ping->dest_icmp->un.echo.sequence	= -1;
	
	// ICMP payload
	unsigned char	*payload		= ping->packet + sizeof(struct icmphdr) + DEFAULT_PAYLOAD;
	size_t			payload_size	= ping->flags->s - sizeof(struct icmphdr) - DEFAULT_PAYLOAD;
	if (ping->flags->p && \
		ping->flags->s > (long long int)(sizeof(struct icmphdr) + DEFAULT_PAYLOAD))
		fill_pattern(payload, ping->flags->p, payload_size);

	ping->dest_icmp->checksum = checksum(ping->packet, sizeof(ping->packet));
}
