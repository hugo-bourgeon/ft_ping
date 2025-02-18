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
	if (ping->stats->print)
	{
		ping->stats->print = 0;
		print_stats(ping);
	}
	if (code != -1)
	{
		if (ping->stats)
			free(ping->stats);
		if (ping)
			free(ping);
		exit(code);
	}
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

void	print_stats(t_ping *ping)
{
	ping->stats->print = 0;
	printf("--- %s ping statistics ---\n", ping->host);
	
	printf("%zu packets transmitted, %zu packets received, %.0f%% packet loss\n", \
	ping->stats->nb_sent, ping->stats->nb_received, (double)(ping->stats->nb_sent - ping->stats->nb_received) / ping->stats->nb_sent * 100);
	

	double avg = ping->stats->avg / ping->stats->nb_received;
	double mdev = sqrt((ping->stats->mdev / ping->stats->nb_received) - (avg * avg));
	
	printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", \
	ping->stats->min, avg, ping->stats->max, mdev);
}
