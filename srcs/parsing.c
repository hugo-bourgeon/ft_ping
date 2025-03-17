/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 17:52:27 by hubourge          #+#    #+#             */
/*   Updated: 2025/03/13 21:28:30 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void	parsing(int ac, char **av, t_ping *ping)
{
	if (ac < 2)
	{
		ft_putstr_fd("Usage: ft_ping [hostname]\n", STDERR_FILENO);
		error(EXIT_FAILURE, ping);
	}

	int						opt;
	static struct option	long_options[] = 
	{
		{"ttl",	required_argument,	0,	1},
		{0,		0,					0,	0}
	};

	opterr = 0;
	while ((opt = getopt_long(ac, av, "vrnf? :l:w:W:p:s:T:", long_options, NULL)) != -1)
	{
		switch (opt)
		{
			case 'v': ping->flags->v = 1; break;
			case 'f': ping->flags->f = 1; break;
			case 'n': ping->flags->n = 1; break;
			case 'r': ping->flags->r = 1; break;

			case 'l':
				if (!optarg) { fprintf(stderr, "ping: option requires an argument -- 'l'\n"); error(EXIT_FAILURE, ping); }
				 
				for (int i = 0; optarg[i]; i++)
				{
					if (optarg[i] < '0' || optarg[i] > '9')
					{
						fprintf(stderr, "ping: invalid preload value (%s)\n", optarg);
						error(EXIT_FAILURE, ping);
					}
				}
				if (strlen(optarg) > strlen("2147483647")) 
				{
					fprintf(stderr, "ping: invalid preload value (%s)\n", optarg); 
					error(EXIT_FAILURE, ping);
				}

				ping->flags->l = atoll(optarg);
				if (ping->flags->l < 0 || ping->flags->l > 2147483647)
				{
					fprintf(stderr, "ping: invalid preload value (%s)\n", optarg);
					error(EXIT_FAILURE, ping);
				}
				break;

			case 'w': 
				if (!optarg) { fprintf(stderr, "ping: option requires an argument -- 'w'\n"); error(EXIT_FAILURE, ping); }

				for (int i = 0; optarg[i]; i++)
				{
					if (optarg[i] < '0' || optarg[i] > '9')
					{
						fprintf(stderr, "ping: invalid preload value (%s)\n", optarg);
						error(EXIT_FAILURE, ping);
					}
				}
				if (strlen(optarg) > strlen("2147483647")) 
				{
					fprintf(stderr, "ping: invalid preload value (%s)\n", optarg); 
					error(EXIT_FAILURE, ping);
				}

				ping->flags->w = atoll(optarg); 
				if (ping->flags->w < 0 || ping->flags->l > 2147483647)
				{
					fprintf(stderr, "ping: invalid preload value (%s)\n", optarg);
					error(EXIT_FAILURE, ping);
				}
				if (ping->flags->w == 0)
				{
					fprintf(stderr, "ping: option value too small: (%s)\n", optarg);
					error(EXIT_FAILURE, ping);
				}
				break;

			case 'W': 
				if (!optarg) { fprintf(stderr, "ping: option requires an argument -- 'W'\n"); error(EXIT_FAILURE, ping); }

				for (int i = 0; optarg[i]; i++)
				{
					if (optarg[i] < '0' || optarg[i] > '9')
					{
						fprintf(stderr, "ping: invalid preload value (%s)\n", optarg);
						error(EXIT_FAILURE, ping);
					}
				}
				if (strlen(optarg) > strlen("2147483647")) 
				{
					fprintf(stderr, "ping: invalid preload value (%s)\n", optarg); 
					error(EXIT_FAILURE, ping);
				}

				ping->flags->W = atoll(optarg);
				if (ping->flags->W < 0 || ping->flags->W > 2147483647)
				{
					fprintf(stderr, "ping: invalid preload value (%s)\n", optarg);
					error(EXIT_FAILURE, ping);
				}
				if (ping->flags->W == 0)
				{
					fprintf(stderr, "ping: option value too small: (%s)\n", optarg);
					error(EXIT_FAILURE, ping);
				}
			
				break;

			case 'p': 
				if (!optarg) { fprintf(stderr, "Error: -p requires an argument\n"); error(EXIT_FAILURE, ping); }
				ping->flags->p = atoi(optarg); 
				break;

			case 's': 
				if (!optarg) { fprintf(stderr, "Error: -s requires an argument\n"); error(EXIT_FAILURE, ping); }
				ping->flags->s = atoi(optarg); 
				break;

			case 'T':
				if (!optarg) { 
					fprintf(stderr, "Error: -T requires an argument (tsonly, tsandaddr, tsprespec)\n"); 
					error(EXIT_FAILURE, ping); 
				}
				ping->flags->T = 0;  // imcomplet
				break;

			case 1:  // --ttl <value>
				if (!optarg) { fprintf(stderr, "Error: --ttl requires an argument\n"); error(EXIT_FAILURE, ping); }
				ping->flags->ttl = atoi(optarg);
				if (ping->flags->ttl < 1 || ping->flags->ttl > 255) {
					fprintf(stderr, "Error: TTL must be between 1 and 255.\n");
					error(EXIT_FAILURE, ping);
				}
				break;

			case '?':
				if (optopt == 0 && opt == '?')
				{
					print_help();
					error(EXIT_SUCCESS, ping);
				}
				else
				{
					fprintf(stderr, "./ft_ping: invalid option -- '%c'\n", optopt);
					fprintf(stderr, "Try './ft_ping --help' or './ft_ping --usage' for more information.\n");
					error(EXIT_FAILURE, ping);
				}
		}
	}

	if (optind >= ac) {
		fprintf(stderr, "Error: missing host operand\n");
		error(EXIT_FAILURE, ping);
	}

	setup_ip(av[optind], ping);
}

void	setup_ip(char *ip, t_ping *ping)
{
	// printf("ip: %s\n", ip);
	struct addrinfo	hints, *res;
	int				status;

	// Resolve hostname
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; // Force IPv4
	hints.ai_socktype = SOCK_RAW;
	status = getaddrinfo(ip, NULL, &hints, &res);
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
	ping->host = strdup(ip);
	if (!ping->host)
	{
		perror("malloc");
		freeaddrinfo(res);
		error(EXIT_FAILURE, ping);
	}

	freeaddrinfo(res);
}
