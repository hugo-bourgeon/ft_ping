/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 19:33:55 by hubourge          #+#    #+#             */
/*   Updated: 2025/04/21 15:14:22 by hubourge         ###   ########.fr       */
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

	initialize_timeouts(ping, &time_limit);

	while (1)
	{
		check_sigint(ping);
		handle_send(ping);

		if (check_total_timeout(ping, &time_limit))
		{
			g_stop_code = STOP;
			break;
		}

		FD_ZERO(&read_fds);
		FD_SET(ping->socketfd, &read_fds);
		set_select_timeout(ping, &timeout);
		adjust_remaining_timeout(ping, &time_limit, &timeout);
	
		// Wait for packet
		bytes_recv = select(ping->socketfd + 1, &read_fds, NULL, NULL, &timeout);
		if (bytes_recv > 0)			// Packet received
			handle_receive(ping);
		else if (bytes_recv == 0)	// Packet Timeout
			ping->stats->nb_lost++;
		else if (bytes_recv < 0)	// Ctrl+C
			g_stop_code = STOP;

		reset_timeout_if_needed(ping, &timeout);
		check_sigint(ping);

		// Wait remaining time
		bytes_recv = select(0, NULL, NULL, NULL, &timeout);
		if (bytes_recv < 0)		// Ctrl+C
			g_stop_code = STOP;
	}
	check_sigint(ping);
}

void	initialize_timeouts(t_ping *ping, struct timeval *time_limit)
{
	if (ping->flags->w != NOT_SET)
	{
		gettimeofday(time_limit, NULL);
		time_limit->tv_sec += ping->flags->w;
	}
}

int	check_total_timeout(t_ping *ping, struct timeval *time_limit)
{
	gettimeofday(&ping->time_now, NULL);
	if (ping->flags->w != NOT_SET &&
		(ping->time_now.tv_sec > time_limit->tv_sec ||
		(ping->time_now.tv_sec == time_limit->tv_sec && ping->time_now.tv_usec >= time_limit->tv_usec)))
	{
		return (1); // Timeout reached
	}
	return (0);
}

void	set_select_timeout(t_ping *ping, struct timeval *timeout)
{
	timeout->tv_sec = ping->flags->W;
	timeout->tv_usec = 0;

	if (ping->flags->f != NOT_SET) // Flood mode
	{
		timeout->tv_sec = 0;
		timeout->tv_usec = 25000;
	}
}

void	adjust_remaining_timeout(t_ping *ping, struct timeval *time_limit, struct timeval *timeout)
{
	if (ping->flags->w != NOT_SET)
	{
		long remaining_sec = time_limit->tv_sec - ping->time_now.tv_sec;
		long remaining_usec = time_limit->tv_usec - ping->time_now.tv_usec;

		if (remaining_usec < 0)
		{
			remaining_sec--;
			remaining_usec += 1000000;
		}
		if (remaining_sec < 1)
		{
			timeout->tv_sec--;
			timeout->tv_usec = remaining_usec;
		}
	}
}

void	reset_timeout_if_needed(t_ping *ping, struct timeval *timeout)
{
    if (ping->flags->l != NOT_SET)
    {
        timeout->tv_sec = 0;
        timeout->tv_usec = 0;
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
