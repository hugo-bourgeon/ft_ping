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
	printf("PING %s (%s): %lld data bytes", ping->host, ping->ip, ping->flags->s - 8);
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

void	print_usage()
{
	printf("Usage: ping [-nrvf?] [-T NUM] [-w N] [-W N] [-l NUMBER]\n"); 
	printf("            [-p PATTERN] [-s NUMBER] [--ttl=N] [--help] [--usage]\n");
	printf("            HOST ...\n");
}

void	print_help()
{
	printf("Usage: ft_ping [OPTION...] HOST ...\n");
	printf("Send ICMP ECHO_REQUEST packets to network hosts.\n");
	printf("\n Options valid for all request types:\n\n");

	PRINT_OPT_S("-n,", "do not resolve host addresses");
	PRINT_OPT_S("-r,", "send directly to a host on an attached network");
	PRINT_OPT_L("--ttl=N", "specify N as time-to-live");
	PRINT_OPT_S("-T,", "set type of service (TOS) to NUM");
	PRINT_OPT_S("-v,", "verbose output");
	PRINT_OPT_S("-w,", "stop after N seconds");
	PRINT_OPT_S("-W,", "number of seconds to wait for response");

	printf("\n Options valid for --echo requests:\n\n");

	PRINT_OPT_S("-f,", "flood ping (root only)");
	PRINT_OPT_S("-l,", "send NUMBER packets as fast as possible before");
	printf("                             falling into normal mode of behavior (root only)\n");
	PRINT_OPT_S("-p,", "fill ICMP packet with given pattern (hex)");
	PRINT_OPT_S("-s,", "send NUMBER data octets");

	printf("\n");

	PRINT_OPT("-?,", "--help", "give this help list");
	PRINT_OPT_L("--usage", "give a short usage message");
	PRINT_OPT_S("-V,", "print program version");
}
