/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_send.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 15:17:20 by hubourge          #+#    #+#             */
/*   Updated: 2025/04/21 15:08:57 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void	update_icmp_checksum(t_ping *ping)
{
	ping->dest_icmp->un.echo.sequence = htons(ntohs(ping->dest_icmp->un.echo.sequence) + 1);
	ping->dest_icmp->checksum = 0;
	ping->dest_icmp->checksum = checksum(ping->packet, sizeof(ping->packet));
}

void	handle_send(t_ping *ping)
{
	update_icmp_checksum(ping);
	gettimeofday(&ping->time_last, NULL);
	
	if (sendto(ping->socketfd, ping->packet, ping->flags->s, 0, (struct sockaddr *)&ping->dest_addr, sizeof(ping->dest_addr)) < 0)
	{
		perror("sendto");
		free_all(EXIT_FAILURE, ping);
	}

	if (ping->flags->l != NOT_SET)
		ping->flags->l--;

	ping->stats->nb_sent++;
}
