/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 13:12:12 by hubourge          #+#    #+#             */
/*   Updated: 2025/03/12 20:33:17 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void	printf_header(t_ping *ping)
{
	printf("PING %s (%s): %d data bytes", ping->host, ping->ip, PACKET_SIZE - 8);
	if (ping->flags->v != NOTSET)
		printf(", id Ox%x = %d", ping->dest_icmp->un.echo.id, ping->dest_icmp->un.echo.id);
	printf("\n");
}

void	print_stats(t_ping *ping)
{
	printf("--- %s ping statistics ---\n", ping->host);
	
	printf("%zu packets transmitted, %zu packets received, %.0f%% packet loss\n", \
	ping->stats->nb_sent, ping->stats->nb_received, (double)(ping->stats->nb_sent - ping->stats->nb_received) / ping->stats->nb_sent * 100);
	

	double avg = ping->stats->avg / ping->stats->nb_received;
	double mdev = sqrt((ping->stats->mdev / ping->stats->nb_received) - (avg * avg));
	
	if (ping->stats->nb_received)
		printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", \
		ping->stats->min, avg, ping->stats->max, mdev);
}
