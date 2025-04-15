/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 19:33:55 by hubourge          #+#    #+#             */
/*   Updated: 2025/03/13 21:25:50 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void	process(t_ping *ping)
{
	struct timeval	time_limit, timeout;
	fd_set			read_fds;
	int				bytes_recv;

	signal(SIGINT, handle_sigint);
	printf_header(ping);

	// Set total timeout
	if (ping->flags->w != NOTSET)
	{
		gettimeofday(&time_limit, NULL);
		time_limit.tv_sec += ping->flags->w;
	}

	while (1)
	{
		check_sigint(ping);
		handle_send(ping);

		// Check total timeout
		gettimeofday(&ping->time_now, NULL);
		if (ping->flags->w != NOTSET && 
			(ping->time_now.tv_sec > time_limit.tv_sec || 
			(ping->time_now.tv_sec == time_limit.tv_sec && ping->time_now.tv_usec >= time_limit.tv_usec))) 
		{
			g_stop_code = STOP;
			break;
		}

		// Wait ICMP reply
		FD_ZERO(&read_fds);
		FD_SET(ping->socketfd, &read_fds);
		timeout.tv_sec	= ping->flags->W;
		timeout.tv_usec	= 0;

		if (ping->flags->f != NOTSET)
		{
			timeout.tv_sec = 0;
			timeout.tv_usec = 25000;
		}

		// Check total timeout remaining time
		if (ping->flags->w != NOTSET)
		{
			long remaining_sec = time_limit.tv_sec - ping->time_now.tv_sec;
			long remaining_usec = time_limit.tv_usec - ping->time_now.tv_usec;
			if (remaining_usec < 0)
			{
				remaining_sec--;
				remaining_usec += 1000000;
			}
			if (remaining_sec < 1)
			{
				timeout.tv_sec--;
				timeout.tv_usec = remaining_usec;
			}
		}
	
		// Wait for packet
		bytes_recv = select(ping->socketfd + 1, &read_fds, NULL, NULL, &timeout);
		if (bytes_recv > 0)			// Packet received
			handle_receive(ping);
		else if (bytes_recv == 0)	// Packet Timeout
			ping->stats->nb_lost++;
		else if (bytes_recv < 0)	// Ctrl+C
			g_stop_code = STOP;

		if (ping->flags->f != NOTSET || ping->flags->l != NOTSET)
		{
			timeout.tv_sec = 0;
			timeout.tv_usec = 0;
		}
		check_sigint(ping);

		// Wait 1 second complement
		bytes_recv = select(0, NULL, NULL, NULL, &timeout);
		if (bytes_recv < 0)		// Ctrl+C
			g_stop_code = STOP;
	}
	check_sigint(ping);
}

void	handle_send(t_ping *ping)
{
	// Update ICMP sequence number
	ping->dest_icmp->checksum = 0;
	ping->dest_icmp->un.echo.sequence = htons(ntohs(ping->dest_icmp->un.echo.sequence) + 1);
	ping->dest_icmp->checksum = checksum(ping->packet, sizeof(ping->packet));
	gettimeofday(&ping->time_last, NULL);
	if (sendto(ping->socketfd, ping->packet, ping->flags->s, 0, (struct sockaddr *)&ping->dest_addr, sizeof(ping->dest_addr)) < 0)
	{
		perror("sendto");
		error(EXIT_FAILURE, ping);
	}
	if (ping->flags->l != NOTSET)
		ping->flags->l--;
	ping->stats->nb_sent++;
}

void	handle_receive(t_ping *ping)
{
	ping->addr_len = sizeof(ping->recv_addr);

	int bytes_received;
	if (ping->flags->f != NOTSET || ping->flags->l == NOTSET)
		bytes_received = recvfrom(ping->socketfd, ping->recv_buffer, ping->flags->s, 0, (struct sockaddr *)&ping->recv_addr, &ping->addr_len);
	else
		bytes_received = recvfrom(ping->socketfd, ping->recv_buffer, ping->flags->s, MSG_DONTWAIT, (struct sockaddr *)&ping->recv_addr, &ping->addr_len);
	gettimeofday(&ping->time_now, NULL);
	if (bytes_received < 0)
	{
		perror("recvfrom");
		error(EXIT_FAILURE, ping);
	}

	// Extract heander IP et ICMP
	struct iphdr *ip_header	= (struct iphdr *)ping->recv_buffer;
	ping->recv_icmp			= (struct icmphdr *)(ping->recv_buffer + (ip_header->ihl * 4)); // ignore IP header

	// Verify checksum
    uint16_t received_checksum = ping->recv_icmp->checksum;
    ping->recv_icmp->checksum = 0;
    uint16_t calculated_checksum = checksum((uint16_t *)ping->recv_icmp, bytes_received - (ip_header->ihl * 4));
    if (received_checksum != calculated_checksum)
    {
        fprintf(stderr, "ping: received packet with bad checksum\n");
        ping->stats->nb_lost++;
        return;
    }

	// If ICMP_ECHO received -> try recvfrom again
	if (ping->recv_icmp->type == ICMP_ECHO)
	{
		bytes_received = recvfrom(ping->socketfd, ping->recv_buffer, ping->flags->s, MSG_DONTWAIT, (struct sockaddr *)&ping->recv_addr, &ping->addr_len);
		gettimeofday(&ping->time_now, NULL);
		if (bytes_received < 0)
		{
			perror("recvfrom");
			error(EXIT_FAILURE, ping);
		}

		ip_header	= (struct iphdr *)ping->recv_buffer;
		ping->recv_icmp			= (struct icmphdr *)(ping->recv_buffer + (ip_header->ihl * 4)); // ignore IP header
	}

	// Handle ICMP_ECHOREPLY
	if ((ping->recv_icmp->type == ICMP_ECHOREPLY && ping->recv_icmp->code == 0))
	{
		int		ttl	= ip_header->ttl;
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
	// Handle ttl Exceeded
	else if (ping->recv_icmp->type == ICMP_TIME_EXCEEDED)
	{
		if (ping->flags->n != NOTSET)
			printf("%d bytes from %s: Time to live exceeded\n", bytes_received, inet_ntoa(ping->recv_addr.sin_addr));
		else
		{
			struct in_addr	source_addr;
			source_addr.s_addr = ip_header->saddr;
		
			// DNS reverse lookup
			struct hostent *host_entry_source = gethostbyaddr(&source_addr, sizeof(source_addr), AF_INET);
			if (host_entry_source != NULL)
				printf("%d bytes from %s (%s): Time to live exceeded\n", bytes_received, host_entry_source->h_name, inet_ntoa(ping->recv_addr.sin_addr));
			else
				printf("%d bytes from %s: Time to live exceeded\n", bytes_received, inet_ntoa(ping->recv_addr.sin_addr));
		}
		if (ping->flags->v != NOTSET) // Verbose mode
		{
			// Extract the original IP header from the ICMP payload
			struct iphdr *original_ip_header = (struct iphdr *)((uint8_t *)ping->recv_icmp + sizeof(struct icmphdr));

			// Extract the original ICMP header from the original IP payload
			struct icmphdr *original_icmp_header = (struct icmphdr *)((uint8_t *)original_ip_header + (original_ip_header->ihl * 4));

			// Print the original IP header
			printf("IP Hdr Dump:\n");
			for (size_t i = 0; i < sizeof(struct iphdr); i += 2)
			{
				printf(" %02x%02x", ((uint8_t *)original_ip_header)[i] & 0xFF, ((uint8_t *)original_ip_header)[i + 1] & 0xFF);
			}
			printf("\n");

			// Print detailed IP header information
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

			// Print ICMP information
			printf("ICMP: type %d, code %d, size %lu, id 0x%04x, seq 0x%04x\n",
				original_icmp_header->type,
				original_icmp_header->code,
				bytes_received - (ip_header->ihl * 4) - sizeof(struct icmphdr), // Taille de l'ICMP original
				ntohs(original_icmp_header->un.echo.id),   // Convertir en ordre d'octets hôte
				ntohs(original_icmp_header->un.echo.sequence)); // Convertir en ordre d'octets hôte
		}
		ping->stats->nb_lost++;
	}
	// Handle Destination Unreachable (ICMP Type 3)
	else
	{
		printf("%d bytes from %s: Destination Host Unreachable\n", \
			bytes_received, \
			inet_ntoa(ping->recv_addr.sin_addr));
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