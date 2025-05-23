/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 17:52:27 by hubourge          #+#    #+#             */
/*   Updated: 2025/04/16 18:21:30 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void	setup_ip(char *ip, t_ping *ping)
{
	struct addrinfo	hints, *res;
	int				status;

	// Resolve hostname
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; // IPv4
	hints.ai_socktype = SOCK_RAW;
	status = getaddrinfo(ip, NULL, &hints, &res);
	if (status)
	{
		ft_printf(STDERR_FILENO, "ping: unknow host\n");
		free_all(EXIT_FAILURE, ping);
	}

	// Get IP address
	struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
	ping->ip = strdup(inet_ntoa(addr->sin_addr));
	if (!ping->ip)
	{
		perror("malloc");
		freeaddrinfo(res);
		free_all(EXIT_FAILURE, ping);
	}

	// Get hostname
	ping->host = strdup(ip);
	if (!ping->host)
	{
		perror("malloc");
		freeaddrinfo(res);
		free_all(EXIT_FAILURE, ping);
	}

	freeaddrinfo(res);
}

void	parsing(int ac, char **av, t_ping *ping)
{
	if (ac < 2)
	{
		ft_putstr_fd("Usage: ft_ping [hostname]\n", STDERR_FILENO);
		free_all(EXIT_FAILURE, ping);
	}

	int						opt;
	static struct option	long_options[] = 
	{
		{"ttl",		required_argument,	0,	FLAG_TTL},
		{"usage",	no_argument,		0,	FLAG_USAGE},
		{"help",	no_argument,		0,	FLAG_HELP},
		{0,			0,					0,	0}
	};

	while ((opt = getopt_long(ac, av, "vrnf?V :l:w:W:p:s:T:", long_options, NULL)) != -1)
	{
		switch (opt)
		{
			case 'v': ping->flags->v = SET_DEFAULT; break;
			case 'f': ping->flags->f = SET_DEFAULT; break;
			case 'n': ping->flags->n = SET_DEFAULT; break;
			case 'r': ping->flags->r = SET_DEFAULT; break;

			case 'l': parse_l(optarg, ping); break;
			case 'w': parse_w(optarg, ping); break;
			case 'W': parse_W(optarg, ping); break;
			case 'p': parse_p(optarg, ping); break;
			case 's': parse_s(optarg, ping); break;
			case 'T': parse_T(optarg, ping); break;
			case 'V': parse_V(ping); break;

			case FLAG_TTL:		parse_ttl(optarg, ping); break;
			case FLAG_USAGE:	parse_usage(ping); break;
			case FLAG_HELP:		parse_help(ping); break;
			case '?':			parse_info(av, ping); break;
		}
	}

	if (optind >= ac)
	{
		fprintf(stderr, "Error: missing host operand\n");
		free_all(EXIT_FAILURE, ping);
	}

	setup_ip(av[optind], ping);
}

void	parse_l(char *optarg, t_ping *ping)
{
	for (int i = 0; optarg[i]; i++)
	{
		if (optarg[i] < '0' || optarg[i] > '9')
		{
			fprintf(stderr, "ping: invalid preload value (%s)\n", optarg);
			free_all(EXIT_FAILURE, ping);
		}
	}

	ping->flags->l = atoll(optarg);
	if (ping->flags->l < 0 \
		|| strlen(optarg) > strlen("2499"))
	{
		fprintf(stderr, "ping: invalid preload value (%s)\n", optarg);
		free_all(EXIT_FAILURE, ping);
	}
	if (ping->flags->l > 2499)
	{
		fprintf(stderr, "ping: sending packet: No buffer space available\n");
		free_all(EXIT_FAILURE, ping);
	}
}

void	parse_w(char *optarg, t_ping *ping)
{
	for (int i = 0; optarg[i]; i++)
	{
		if (optarg[i] < '0' || optarg[i] > '9')
		{
			fprintf(stderr, "ping: invalid value (`%s' near `%s')\n", optarg, &optarg[i]);
			free_all(EXIT_FAILURE, ping);
		}
	}

	ping->flags->w = atoll(optarg); 
	if ((ping->flags->w > 2147483647) \
		|| (strlen(optarg) > strlen("2147483647")))
	{
		fprintf(stderr, "ping: option value too big: %s\n", optarg);
		free_all(EXIT_FAILURE, ping);
	}
	if (ping->flags->w == 0)
	{
		fprintf(stderr, "ping: option value too small: %s\n", optarg);
		free_all(EXIT_FAILURE, ping);
	}
}

void	parse_W(char *optarg, t_ping *ping)
{
	for (int i = 0; optarg[i]; i++)
	{
		if (optarg[i] < '0' || optarg[i] > '9')
		{
			fprintf(stderr, "ping: invalid value (`%s' near `%s')\n", optarg, &optarg[i]);
			free_all(EXIT_FAILURE, ping);
		}
	}

	ping->flags->W = atoll(optarg); 
	if ((ping->flags->W > 2147483647) \
		|| (strlen(optarg) > strlen("2147483647")))
	{
		fprintf(stderr, "ping: option value too big: %s\n", optarg);
		free_all(EXIT_FAILURE, ping);
	}
	if (ping->flags->W == 0)
	{
		fprintf(stderr, "ping: option value too small: %s\n", optarg);
		free_all(EXIT_FAILURE, ping);
	}
}

void	parse_p(char *optarg, t_ping *ping)
{
	is_valid_hex_pattern(optarg, ping);
	ping->flags->p = malloc(strlen(optarg) + 1);
	if (!ping->flags->p)
	{
		perror("malloc");
		free_all(EXIT_FAILURE, ping);
	}
	strcpy(ping->flags->p, optarg);
}

void	parse_s(char *optarg, t_ping *ping)
{
	for (int i = 0; optarg[i]; i++)
	{
		if (optarg[i] < '0' || optarg[i] > '9')
		{
			fprintf(stderr, "ping: invalid value ('%s' near '%s')\n", optarg, &optarg[i]);
			free_all(EXIT_FAILURE, ping);
		}
	}

	ping->flags->s = atoll(optarg);
	if (ping->flags->s < 0 || ping->flags->s > 65399 \
		|| strlen(optarg) > 5)
	{
		fprintf(stderr, "ping: option value too big: %s\n", optarg);
		free_all(EXIT_FAILURE, ping);
	}
	ping->flags->s += sizeof(struct icmphdr);
}

void	parse_T(char *optarg, t_ping *ping)
{
	for (int i = 0; optarg[i]; i++)
	{
		if (!isdigit(optarg[i]) &&
			!(i == 1 && optarg[0] == '0' && (optarg[1] == 'x' || optarg[1] == 'X')) &&
			!(i > 1 && isxdigit(optarg[i]) && optarg[0] == '0' && (optarg[1] == 'x' || optarg[1] == 'X')))
		{
			fprintf(stderr, "ping: invalid value ('%s' near '%s')\n", optarg, &optarg[i]);
			free_all(EXIT_FAILURE, ping);
		}
	}
	
	char *endptr = NULL;
	long val = strtol(optarg, &endptr, 0); // base 0 -> auto (hex ou décimal)
	
	if (*endptr != '\0' || val < 0 || val > 255)
	{
		fprintf(stderr, "ping: option value too big: %s\n", optarg); 
		free_all(EXIT_FAILURE, ping);
	}
	ping->flags->T = (int)val;
}

void	parse_V(t_ping *ping)
{
	printf("ft_ping version 1.0 \n");
	printf("inspired by ping (GNU inetutils) 2.0\n");
	printf("Written by hubourge.\n");

	free_all(EXIT_SUCCESS, ping);
}

void	parse_ttl(char *optarg, t_ping *ping)
{
	ping->flags->ttl = atoi(optarg);
	if (ping->flags->ttl < 1)
	{
		fprintf(stderr, "ping: option value too small: %s\n", optarg);
		free_all(EXIT_FAILURE, ping);
	}
	if (ping->flags->ttl > 255 || strlen(optarg) > 3)
	{
		fprintf(stderr, "ping: option value too big: %s\n", optarg);
		free_all(EXIT_FAILURE, ping);
	}
}

void	parse_usage(t_ping *ping)
{
	print_usage();
	free_all(EXIT_SUCCESS, ping);
}

void	parse_help(t_ping *ping)
{
	print_help();
	free_all(EXIT_SUCCESS, ping);
}

void	parse_info(char **av, t_ping *ping)
{
	if (strncmp("-?", av[optind - 1], strlen(av[optind - 1]) + 1) == 0)
	{
		print_help();
		free_all(EXIT_SUCCESS, ping);
	}
	fprintf(stderr, "Try './ft_ping --help' or './ft_ping --usage' for more information.\n");
	free_all(EXIT_FAILURE, ping);
}
