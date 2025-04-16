/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/24 14:34:54 by hubourge          #+#    #+#             */
/*   Updated: 2025/04/16 18:22:50 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PING_H
# define FT_PING_H

# include "../libft/libft.h"
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <sys/socket.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <arpa/inet.h>
# include <sys/time.h>
# include <netdb.h>
# include <errno.h>
# include <signal.h>
# include <math.h>
# include <getopt.h>
# include <ctype.h>

# define RECV_BUFFER_SIZE	65536
# define NOT_EXIT			-1
# define PROCESS			0
# define STOP				1
# define NOTSET				-1
# define SET_DEFAULT		1

# define FLAG_TTL			1
# define FLAG_USAGE			2
# define FLAG_HELP			3

# define PRINT_OPT(short, long, msg) printf("  %-4s %-21s %s\n", short, long, msg);
# define PRINT_OPT_S(short, msg)     PRINT_OPT(short, "", msg)
# define PRINT_OPT_L(long, msg)      PRINT_OPT("", long, msg)

extern int	g_stop_code;

typedef struct s_stats
{
	int					print;
	size_t				nb_sent;
	size_t				nb_received;
	size_t				nb_lost;
	double				min;
	double				max;
	double				avg;
	double				mdev;
}	t_stats;

typedef struct	s_flags
{
	int				v;
	int				f;
	long long		l;
	int				n;
	long long		w;
	long long		W;
	char*			p;
	int				r;
	long long		s;
	int				T;
	int				ttl;
}	t_flags;

typedef struct s_ping
{
	char				*host;
	char				*ip;
	int					socketfd;
	unsigned char		*packet;
	struct	sockaddr_in	dest_addr;
	struct	sockaddr_in recv_addr;
	struct	icmphdr		*dest_icmp;
	struct	icmphdr		*recv_icmp;
	struct	iphdr		*recv_header;
	socklen_t			addr_len;
	char				recv_buffer[RECV_BUFFER_SIZE];
	struct timeval		time_last;
	struct timeval		time_now;
	t_stats				*stats;
	t_flags				*flags;
}	t_ping;

// ping.c
void			process(t_ping *ping);
void			initialize_timeouts(t_ping *ping, struct timeval *time_limit);
int				check_total_timeout(t_ping *ping, struct timeval *time_limit);
void			set_select_timeout(t_ping *ping, struct timeval *timeout);
void			adjust_remaining_timeout(t_ping *ping, struct timeval *time_limit, struct timeval *timeout);
void			reset_timeout_if_needed(t_ping *ping, struct timeval *timeout);
void			handle_stats(t_ping *ping, double rtt);

// ping_send.c
void			handle_send(t_ping *ping);
void			update_icmp_checksum(t_ping *ping);

// ping_receive.c
void			handle_receive(t_ping *ping);
void			receive_packet(int *bytes_received, t_ping *ping);
int				check_checksum(int bytes_received, t_ping *ping);
void			retry_receive_packet(int *bytes_received, t_ping *ping);
void			handle_icmp_echo_reply(t_ping *ping, int bytes_received);
void			handle_icmp_time_exceeded(t_ping *ping, int bytes_received);

// parsing.c
void			setup_ip(char *ip, t_ping *ping);
void			parsing(int ac, char **av, t_ping *ping);
void			parse_l(char *optarg, t_ping *ping);
void			parse_w(char *optarg, t_ping *ping);
void			parse_W(char *optarg, t_ping *ping);
void			parse_p(char *optarg, t_ping *ping);
void			parse_s(char *optarg, t_ping *ping);
void			parse_T(char *optarg, t_ping *ping);
void			parse_V(t_ping *ping);
void			parse_ttl(char *optarg, t_ping *ping);
void			parse_usage(t_ping *ping);
void			parse_help(t_ping *ping);
void			parse_info(char **av, t_ping *ping);

// init.c
void			init_struct(t_ping **ping);
void			init_flags(t_ping *ping);
void			init_stats(t_ping *ping);
void			init_socket_dest(t_ping *ping);
void			init_icmp_packet(t_ping *ping);

// utils.c
void			free_all(int code, t_ping *ping);
void			handle_sigint(int sig);
unsigned short	checksum(void *b, int len);
void			check_sigint(t_ping *ping);
void			is_valid_hex_pattern(char *pattern, t_ping *ping);
void			fill_pattern(unsigned char *packet, const char *pattern, size_t len);

// print.c
void			printf_header(t_ping *ping);
void			print_stats(t_ping *ping);
void			print_help();
void			print_usage();
void			print_dump(struct iphdr *original_ip_header, struct icmphdr *original_icmp_header, int bytes_received, t_ping *ping);


#endif