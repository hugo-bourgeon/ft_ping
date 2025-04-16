/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_receive.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 15:19:38 by hubourge          #+#    #+#             */
/*   Updated: 2025/04/16 18:22:42 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void	handle_receive(t_ping *ping)
{
	int	bytes_received;

	receive_packet(&bytes_received, ping);
	gettimeofday(&ping->time_now, NULL);

	// Extract header IP et ICMP
	ping->recv_header	= (struct iphdr *)ping->recv_buffer;
	ping->recv_icmp		= (struct icmphdr *)(ping->recv_buffer + (ping->recv_header->ihl * 4)); // ignore IP header

	if (check_checksum(bytes_received, ping) == -1)
		return ;

	if (ping->recv_icmp->type == ICMP_ECHO)	// Echo request
		retry_receive_packet(&bytes_received, ping); // Self ping condition 

	if ((ping->recv_icmp->type == ICMP_ECHOREPLY && ping->recv_icmp->code == 0)) // Echo reply
		handle_icmp_echo_reply(ping, bytes_received);
	else if (ping->recv_icmp->type == ICMP_TIME_EXCEEDED) // Time exceeded
	{
		handle_icmp_time_exceeded(ping, bytes_received);
		ping->stats->nb_lost++;
	}
	else
	{
		printf("%d bytes from %s: Destination Host Unreachable\n", \
			bytes_received, \
			inet_ntoa(ping->recv_addr.sin_addr));
		ping->stats->nb_lost++;
	}
}

void	receive_packet(int *bytes_received, t_ping *ping)
{
	ping->addr_len = sizeof(ping->recv_addr);
	if (ping->flags->f != NOTSET || ping->flags->l == NOTSET)
		*bytes_received = recvfrom(ping->socketfd, ping->recv_buffer, ping->flags->s, 0, (struct sockaddr *)&ping->recv_addr, &ping->addr_len);
	else
		*bytes_received = recvfrom(ping->socketfd, ping->recv_buffer, ping->flags->s, MSG_DONTWAIT, (struct sockaddr *)&ping->recv_addr, &ping->addr_len);
	if ((*bytes_received) < 0)
	{
		perror("recvfrom");
		free_all(EXIT_FAILURE, ping);
	}
}

int	check_checksum(int bytes_received, t_ping *ping)
{
	uint16_t received_checksum = ping->recv_icmp->checksum;
	ping->recv_icmp->checksum = 0;
	uint16_t calculated_checksum = checksum((uint16_t *)ping->recv_icmp, bytes_received - (ping->recv_header->ihl * 4));
	
	if (received_checksum != calculated_checksum)
	{
		fprintf(stderr, "ping: received packet with bad checksum (corrupted packet)\n");
		ping->stats->nb_lost++;
		return (-1);
	}

	return (0);
}

void retry_receive_packet(int *bytes_received, t_ping *ping)
{
	*bytes_received = recvfrom(ping->socketfd, ping->recv_buffer, ping->flags->s, MSG_DONTWAIT, (struct sockaddr *)&ping->recv_addr, &ping->addr_len);
	gettimeofday(&ping->time_now, NULL);
	if ((*bytes_received) < 0)
	{
		perror("recvfrom");
		free_all(EXIT_FAILURE, ping);
	}

	ping->recv_header	= (struct iphdr *)ping->recv_buffer;
	ping->recv_icmp		= (struct icmphdr *)(ping->recv_buffer + (ping->recv_header->ihl * 4)); // ignore IP header
}

void	handle_icmp_echo_reply(t_ping *ping, int bytes_received)
{
	int		ttl	= ping->recv_header->ttl;
	double	rtt	= (ping->time_now.tv_sec - ping->time_last.tv_sec) * 1000.0 + 
				(ping->time_now.tv_usec - ping->time_last.tv_usec) / 1000.0;

	if (ping->flags->f == NOTSET)
		printf("%d bytes from %s: icmp_seq=%u ttl=%d time=%.3f ms\n", \
		bytes_received, \
		ping->ip, \
		ntohs(ping->dest_icmp->un.echo.sequence), \
		ttl, \
		rtt);
	
	handle_stats(ping, rtt);
	ping->stats->nb_received++;
}

void	handle_icmp_time_exceeded(t_ping *ping, int bytes_received)
{
	if (ping->flags->n != NOTSET)
		printf("%d bytes from %s: Time to live exceeded\n", bytes_received, inet_ntoa(ping->recv_addr.sin_addr));
	else
	{
		struct in_addr	source_addr;
		source_addr.s_addr = ping->recv_header->saddr;
	
		// DNS reverse lookup
		struct hostent *host_entry_source = gethostbyaddr(&source_addr, sizeof(source_addr), AF_INET);
		if (host_entry_source != NULL)
			printf("%d bytes from %s (%s): Time to live exceeded\n", bytes_received, host_entry_source->h_name, inet_ntoa(ping->recv_addr.sin_addr));
		else
			printf("%d bytes from %s: Time to live exceeded\n", bytes_received, inet_ntoa(ping->recv_addr.sin_addr));
	}
	if (ping->flags->v != NOTSET) // Verbose mode
	{
		struct iphdr	*original_ip_header = (struct iphdr *)((uint8_t *)ping->recv_icmp + sizeof(struct icmphdr));
		struct icmphdr	*original_icmp_header = (struct icmphdr *)((uint8_t *)original_ip_header + (original_ip_header->ihl * 4));

		print_dump(original_ip_header, original_icmp_header, bytes_received, ping);
	}
}
