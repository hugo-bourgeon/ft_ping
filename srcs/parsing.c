/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 17:52:27 by hubourge          #+#    #+#             */
/*   Updated: 2025/02/15 15:38:39 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void	parsing(int ac, char **av, t_ping *ping)
{
	struct addrinfo hints, *res;
	int status;

	if (ac < 2)
	{
		ft_putstr_fd("Usage: ft_ping [hostname]\n", STDERR_FILENO);
		error(EXIT_FAILURE, ping);
	}

	// Resolve hostname
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; // Force IPv4
	hints.ai_socktype = SOCK_RAW;
	status = getaddrinfo(av[1], NULL, &hints, &res);
	if (status)
	{
		ft_printf(STDERR_FILENO, "ping: unknow host\n");
		error(EXIT_FAILURE, ping);
	}

	// Get IP address
	struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
	ping->ip = strdup(inet_ntoa(addr->sin_addr));
	if (!ping->ip)
	{
		perror("malloc");
		freeaddrinfo(res);
		error(EXIT_FAILURE, ping);
	}

	// Get hostname
	ping->host = strdup(av[1]);
	if (!ping->host)
	{
		perror("malloc");
		freeaddrinfo(res);
		error(EXIT_FAILURE, ping);
	}
	
	freeaddrinfo(res);
}

// void	parsing(int ac, char **av, t_ping *ping)
// {
// 	if (ac < 2)
// 	{
// 		ft_putstr_fd("Usage: ft_ping [hostname]\n", STDERR_FILENO);
// 		error(EXIT_FAILURE, ping);
// 	}

// 	struct hostent *host_entity = gethostbyname(av[1]);
// 	if (!host_entity)
// 	{
// 		ft_printf(STDERR_FILENO, "ping: cannot resolve %s: Unknown host\n", av[1]);
// 		error(EXIT_FAILURE, ping);
// 	}
// 	ping->ip			= inet_ntoa(*(struct in_addr *)host_entity->h_addr);
// 	ping->host			= av[1];
// }
