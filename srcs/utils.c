/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 00:12:42 by hubourge          #+#    #+#             */
/*   Updated: 2025/04/21 14:44:59 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void	free_all(int code, t_ping *ping)
{
	if (ping)
	{
		if (ping->socketfd > 0)
			close(ping->socketfd);
		if (ping->stats && ping->stats->print)
			print_stats(ping);
		if (ping->ip)
			free(ping->ip);
		if (ping->host)
			free(ping->host);
		if (ping->stats)
			free(ping->stats);
		if (ping->flags)
		{
			if (ping->flags->p)
				free(ping->flags->p);
			free(ping->flags);
		}
	}
	if (ping)
		free(ping);
	if (code == EXIT_FAILURE || code == EXIT_SUCCESS)
		exit(code);
}

void	handle_sigint(int sig)
{
	(void)sig;
	g_stop_code = STOP;
}

void	check_sigint(t_ping *ping)
{
	if (g_stop_code == STOP)
	{
		ping->stats->print = 1;
		free_all(EXIT_SUCCESS, ping);
	}
}

unsigned short	checksum(void *packet, int len)
{
	unsigned short	*buffer	= packet;
	unsigned int	sum		= 0;
	unsigned short	result;
	
	// Sum all 16 bits words 
	// ex: 0800 + 0000 + 0001 + FFFF
	for (sum = 0; len > 1; len -= 2)
		sum += *(buffer)++;
	
	// If there is a byte left, add it
	if (len == 1)
		sum += *(unsigned char*)buffer;

	// Add overflow bit
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	
	// one's complement
	result = ~sum;
	return (result);
}

void	is_valid_hex_pattern(char *pattern, t_ping *ping)
{
	int len = strlen(pattern);

	for (int i = 0; i < len; i++)
	{
		if (!isxdigit(pattern[i]))
		{
			fprintf(stderr, "./ft_ping: error in pattern near %s\n", &pattern[i]);
			free_all(EXIT_FAILURE, ping);
		}
	}
}

void	fill_pattern(unsigned char *packet, const char *pattern, size_t len)
{
	size_t	pattern_len = strlen(pattern);
	size_t	i = 0, j = 0;

	int is_odd = (pattern_len % 2 != 0);

	// Limit to 32 bytes
	if (pattern_len > 32)
		pattern_len = 32;

	while (i < len)
	{
		char	hex_str[3];

		// If odd, add a 0 at the beginning
		if (is_odd && j == pattern_len - 1)
		{
			hex_str[0] = '0';
			hex_str[1] = pattern[j];
		}
		else
		{
			hex_str[0] = pattern[j];
			hex_str[1] = pattern[j + 1];
			j++;
		}

		hex_str[2] = '\0';
		packet[i] = (unsigned char)strtol(hex_str, NULL, 16);

		j++;
		if (j >= pattern_len)
			j = 0;
		i++;
	}
}
