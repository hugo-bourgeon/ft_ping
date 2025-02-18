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

	printf("PING %s (%s): %d data bytes\n", ping->host, ping->ip, PACKET_SIZE);

	while (1)
	{
		handle_send(ping);
		handle_receive(ping);

		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		check_sigint(ping);
		int ret = select(0, NULL, NULL, NULL, &timeout);
		if (ret < 0)
			g_stop_code = STOP;
	}
}

void	handle_send(t_ping *ping)
{
	gettimeofday(&ping->time_last, NULL);
	
	check_sigint(ping);	
	if (sendto(ping->socketfd, ping->packet, PACKET_SIZE, 0, (struct sockaddr *)&ping->dest_addr, sizeof(ping->dest_addr)) < 0)
	{
        perror("sendto");
        error(EXIT_FAILURE, ping);
    }
}

void	handle_receive(t_ping *ping)
{
	ping->addr_len = sizeof(ping->recv_addr);

	check_sigint(ping);
	int bytes_received = recvfrom(ping->socketfd, ping->recv_buffer, PACKET_SIZE, 0, (struct sockaddr *)&ping->recv_addr, &ping->addr_len);
    if (bytes_received < 0) // EST CE QUE C'EST LA BONNE MANIERE ?
	{
        perror("recvfrom");
		error(EXIT_FAILURE, ping);
	}
	check_sigint(ping);
	gettimeofday(&ping->time_now, NULL);
	
	// Extract ICMP
    ping->recv_icmp = (struct icmphdr *)(ping->recv_buffer + 20); // +20 ignore IP header
    if (ping->recv_icmp->type == ICMP_ECHOREPLY && ping->recv_icmp->code == 0)
	{
		double rtt = (ping->time_now.tv_sec - ping->time_last.tv_sec) * 1000.0 + 
					(ping->time_now.tv_usec - ping->time_last.tv_usec) / 1000.0;

		printf("%d bytes from %s: icmp_seq=%zu ttl=%d time=%.3f ms\n", \
		PACKET_SIZE, ping->ip, ping->stats->nb_sent, ntohs(ping->recv_icmp->un.echo.sequence), rtt);
		
		handle_stats(ping, rtt);
		ping->stats->nb_received++;
	}
	else
	{
        printf("Paquet ICMP reçu mais non valide (type=%d, code=%d)\n", ping->recv_icmp->type, ping->recv_icmp->code); ///////////// REVOIR ERREUR
		ping->stats->nb_lost++; // Est ce que c'est vraiment un packet lost ?
	}
	
	ping->stats->nb_sent++;
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