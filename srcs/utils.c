/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 00:12:42 by hubourge          #+#    #+#             */
/*   Updated: 2025/02/15 17:18:11 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void	error(int code, t_ping *ping)
{
	if (ping->socketfd > 0)
		close(ping->socketfd);
	if (ping->print_stats)
	{
		ping->print_stats = 0;
		print_stats(ping);
	}
	if (code != -1)
		exit(code);
}

void	handle_sigint(int sig)
{
    (void)sig;
    g_stop_code = STOP;
}

void	check_sigint(t_ping *ping)
{
	if (g_stop_code == STOP)
	{
		ping->print_stats = 1;
		error(-1, ping);
	}
}

unsigned short	checksum(void *packet, int len)
{
	unsigned short	*buffer	= packet;
	unsigned int	sum		= 0;
	unsigned short	result;
	
	// Sum all 16 bits words 
	// ex: 0800 + 0000 + 0001 + FFFF
	for (sum = 0; len > 1; len -= 2)
		sum += *(buffer)++;
	
	// If there is a byte left, add it
	if (len == 1)
		sum += *(unsigned char*)buffer;

	// Add overflow bit
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	
	// one's complement
	result = ~sum;
	return (result);
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

void	init_imcp_packet(t_ping *ping)
{
	// ICMP packet
	ping->dest_icmp = (struct icmphdr *)ping->packet;	
	memset(ping->packet, 0, PACKET_SIZE);
    ping->dest_icmp->type				= ICMP_ECHO;
    ping->dest_icmp->code				= 0;
    ping->dest_icmp->checksum			= 0;
    ping->dest_icmp->un.echo.id			= getpid();
    ping->dest_icmp->un.echo.sequence	= 1;
	
	ping->dest_icmp->checksum = checksum(ping->packet, sizeof(ping->packet));
}

// --- google.com ping statistics ---
// 3 packets transmitted, 3 packets received, 0.0% packet loss
// round-trip min/avg/max/stddev = 10.829/12.891/15.009/1.707 ms

void	print_stats(t_ping *ping)
{
	ping->print_stats = 0;
	printf("\n--- %s ping statistics ---\n", ping->host);
	
	printf("%zu packets transmitted, %zu packets received, %.1f%% packet loss\n", \
	ping->nb_sequence, ping->nb_received, (double)(ping->nb_sequence - ping->nb_received) / ping->nb_sequence * 100);
	
	printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", 0.0, 0.0, 0.0, 0.0);
}
