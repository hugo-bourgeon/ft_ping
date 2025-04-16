/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 13:12:12 by hubourge          #+#    #+#             */
/*   Updated: 2025/04/16 18:09:19 by hubourge         ###   ########.fr       */
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
	if (ping->flags->f != NOTSET)
	{
		for (size_t i = 0; i < ping->stats->nb_lost; i++)
			printf(".");
	}

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

void	print_dump(struct iphdr *original_ip_header, struct icmphdr *original_icmp_header, int bytes_received, t_ping *ping)
{
	printf("IP Hdr Dump:\n");
	for (size_t i = 0; i < sizeof(struct iphdr); i += 2)
		printf(" %02x%02x", ((uint8_t *)original_ip_header)[i] & 0xFF, ((uint8_t *)original_ip_header)[i + 1] & 0xFF);
	printf("\n");

	printf("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src\tDst\tData\n");
	printf(" %1x  %1x  %02x %04x %04x   %1x %04x  %02x  %02x %04x %s  %s\n",
		original_ip_header->version,
		original_ip_header->ihl,
		original_ip_header->tos,
		ntohs(original_ip_header->tot_len),
		ntohs(original_ip_header->id),
		(ntohs(original_ip_header->frag_off) & 0xE000) >> 13,
		ntohs(original_ip_header->frag_off) & 0x1FFF,
		original_ip_header->ttl,
		original_ip_header->protocol,
		ntohs(original_ip_header->check),
		inet_ntoa(*(struct in_addr *)&original_ip_header->saddr),
		inet_ntoa(*(struct in_addr *)&original_ip_header->daddr));

	printf("ICMP: type %d, code %d, size %lu, id 0x%04x, seq 0x%04x\n",
		original_icmp_header->type,
		original_icmp_header->code,
		bytes_received - (ping->recv_header->ihl * 4) - sizeof(struct icmphdr),
		ntohs(original_icmp_header->un.echo.id),
		ntohs(original_icmp_header->un.echo.sequence));
}
	