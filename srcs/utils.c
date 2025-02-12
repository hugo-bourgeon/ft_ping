/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 00:12:42 by hubourge          #+#    #+#             */
/*   Updated: 2025/02/12 20:36:58 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void	error(int code, t_ping *ping)
{
	if (ping->socketfd > 0)
		close(ping->socketfd);
	exit(code);
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

void	create_imcp_packet(t_ping *ping)
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
