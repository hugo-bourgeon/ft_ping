/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 19:33:55 by hubourge          #+#    #+#             */
/*   Updated: 2025/02/15 17:32:17 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void	process(t_ping *ping)
{
	struct timeval timeout;
	signal(SIGINT, handle_sigint);
	printf("PING %s (%s): %d data bytes\n", ping->host, ping->ip, PACKET_SIZE - 8);

	while (1)
	{
		check_sigint(ping);
		handle_send(ping);

		// Wait ICMP reply
		fd_set	read_fds;
		FD_ZERO(&read_fds);
		FD_SET(ping->socketfd, &read_fds);
		timeout.tv_sec	= 1; // Timeout 1 second
		timeout.tv_usec	= 0;
		
		int ret = select(ping->socketfd + 1, &read_fds, NULL, NULL, &timeout);
		if (ret > 0)		// Packet received
			handle_receive(ping);
		else if (ret == 0)	// Packet Timeout
			ping->stats->nb_lost++;
		else if (ret < 0)	// Ctrl+C
			g_stop_code = STOP;
		
		// Wait 1 second if necessary
		ret = select(0, NULL, NULL, NULL, &timeout);
		check_sigint(ping);
		if (ret < 0)		// Ctrl+C
			g_stop_code = STOP;
	}
}

void	handle_send(t_ping *ping)
{
	// Update ICMP sequence number
	ping->dest_icmp->un.echo.sequence++;
	ping->dest_icmp->checksum = 0;
	ping->dest_icmp->checksum = checksum(ping->packet, sizeof(ping->packet));
	gettimeofday(&ping->time_last, NULL);
	if (sendto(ping->socketfd, ping->packet, PACKET_SIZE, 0, (struct sockaddr *)&ping->dest_addr, sizeof(ping->dest_addr)) < 0)
	{
		perror("sendto");
		error(EXIT_FAILURE, ping);
	}
	ping->stats->nb_sent++;
}

void	handle_receive(t_ping *ping)
{
	ping->addr_len = sizeof(ping->recv_addr);

	int bytes_received = recvfrom(ping->socketfd, ping->recv_buffer, PACKET_SIZE, MSG_DONTWAIT, (struct sockaddr *)&ping->recv_addr, &ping->addr_len);
	gettimeofday(&ping->time_now, NULL);
	if (bytes_received < 0)
	{
		perror("recvfrom");
		error(EXIT_FAILURE, ping);
	}

	// Extract heander IP et ICMP
	struct iphdr *ip_header	= (struct iphdr *)ping->recv_buffer;
	ping->recv_icmp			= (struct icmphdr *)(ping->recv_buffer + (ip_header->ihl * 4)); // ignore IP header

	if ((ping->recv_icmp->type == ICMP_ECHOREPLY && ping->recv_icmp->code == 0) \
		|| (ping->recv_icmp->type == ICMP_ECHO && ping->recv_icmp->code == 0 && strncmp(ping->host,ping->ip, strlen(ping->ip)+1) == 0))
	{
		int		ttl	= ip_header->ttl;
		double	rtt	= (ping->time_now.tv_sec - ping->time_last.tv_sec) * 1000.0 + 
					(ping->time_now.tv_usec - ping->time_last.tv_usec) / 1000.0;

		printf("%d bytes from %s: icmp_seq=%u ttl=%d time=%.3f ms\n", \
		bytes_received, ping->ip, ping->dest_icmp->un.echo.sequence - 1, ttl, rtt);
		
		handle_stats(ping, rtt);
		ping->stats->nb_received++;
	}
	else
	{
		printf("%d bytes from %s (%s): Destination Host Unreachable\n", bytes_received, ping->host, inet_ntoa(ping->recv_addr.sin_addr));
		ping->stats->nb_lost++;
	}
	
}

void	handle_stats(t_ping *ping, double rtt)
{
	if (rtt < ping->stats->min)
		ping->stats->min = rtt;
	if (rtt > ping->stats->max)
		ping->stats->max = rtt;
	ping->stats->avg += rtt;
	ping->stats->mdev += rtt * rtt;
}