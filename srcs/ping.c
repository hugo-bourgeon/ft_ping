/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 19:33:55 by hubourge          #+#    #+#             */
/*   Updated: 2025/02/12 20:35:29 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void	process(t_ping *ping)
{
	// PING google.com (142.250.179.78): 56 data bytes
	printf("PING %s (%s): %d data bytes\n", ping->host, ping->ip, PACKET_SIZE);

	while (1)
	{
		handle_send(ping);

		handle_receive(ping);

		sleep(1);
	}
}

void	handle_send(t_ping *ping)
{
	gettimeofday(&ping->time_last, NULL);
	if (sendto(ping->socketfd, ping->packet, PACKET_SIZE, 0, (struct sockaddr *)&ping->dest_addr, sizeof(ping->dest_addr)) < 0)
	{
        perror("sendto");
        error(EXIT_FAILURE, ping);
    }
}

void	handle_receive(t_ping *ping)
{
	ping->addr_len = sizeof(ping->recv_addr);

	int bytes_received = recvfrom(ping->socketfd, ping->recv_buffer, PACKET_SIZE, 0, (struct sockaddr *)&ping->recv_addr, &ping->addr_len);
    if (bytes_received < 0)
	{
        perror("Erreur recvfrom");
		error(EXIT_FAILURE, ping);
	}
	gettimeofday(&ping->time_now, NULL);
	
	// Extract ICMP
    ping->recv_icmp = (struct icmphdr *)(ping->recv_buffer + 20); // +20 ignore IP header

    // Check for ECHO_REPLY
    if (ping->recv_icmp->type == ICMP_ECHOREPLY && ping->recv_icmp->code == 0)
	{
		double rtt = (ping->time_now.tv_sec - ping->time_last.tv_sec) * 1000.0 + 
					(ping->time_now.tv_usec - ping->time_last.tv_usec) / 1000.0;

		printf("%d bytes from %s: icmp_seq=%zu ttl=%d time=%.3f ms\n", \
			PACKET_SIZE, ping->ip, ping->nb_sequence, ntohs(ping->recv_icmp->un.echo.sequence), rtt);
	}
	else
        printf("Paquet ICMP reçu mais non valide (type=%d, code=%d)\n", ping->recv_icmp->type, ping->recv_icmp->code); ///////////// REVOIR ERREUR
	ping->nb_sequence++;
}