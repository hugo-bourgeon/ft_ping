/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 00:12:42 by hubourge          #+#    #+#             */
/*   Updated: 2025/03/12 13:19:56 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void	error(int code, t_ping *ping)
{
	if (ping->socketfd > 0)
		close(ping->socketfd);
	if (ping->stats->print)
		print_stats(ping);
	if (ping->ip)
		free(ping->ip);
	if (ping->host)
		free(ping->host);
	if (ping->stats)
		free(ping->stats);
	if (ping->flags)
		free(ping->flags);
	if (ping)
		free(ping);
	if (code == EXIT_FAILURE || code == EXIT_SUCCESS)
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
		ping->stats->print = 1;
		error(EXIT_SUCCESS, ping);
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
