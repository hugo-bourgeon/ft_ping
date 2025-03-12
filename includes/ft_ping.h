/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/24 14:34:54 by hubourge          #+#    #+#             */
/*   Updated: 2025/03/12 13:20:30 by hubourge         ###   ########.fr       */
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

# define PACKET_SIZE 64
# define RECV_BUFFER_SIZE 128
# define PROCESS 0
# define STOP 1
# define NOSET -1
# define TSONLY 10
# define TSANDADDR 11
# define TSPRESPEC 12

extern int g_stop_code;

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
	int				l;
	int				n;
	int				w;
	int				W;
	int				p;
	int				r;
	int				s;
	int				T;
	int				ttl;
}	t_flags;

typedef struct s_ping
{
	char				*host;
	char				*ip;
	int					socketfd;
	char				packet[PACKET_SIZE];
	struct	sockaddr_in	dest_addr;
	struct	sockaddr_in recv_addr;
	struct	icmphdr		*dest_icmp;
	struct	icmphdr		*recv_icmp;
	socklen_t			addr_len;
	char				recv_buffer[RECV_BUFFER_SIZE];
	struct timeval		time_last;
	struct timeval		time_now;
	t_stats				*stats;
	t_flags				*flags;
}	t_ping;

// ping.c
void			process(t_ping *ping);
void			handle_send(t_ping *ping);
void			handle_receive(t_ping *ping);
void			handle_stats(t_ping *ping, double rtt);

// parsing.c
void			parsing(int ac, char **av, t_ping *ping);
void			setup_ip(char *ip, t_ping *ping);

// init.c
void			init_struct(t_ping *ping);
void			init_socket_dest(t_ping *ping);
void			init_icmp_packet(t_ping *ping);

// utils.c
void			error(int code, t_ping *ping);
void			handle_sigint(int sig);
unsigned short	checksum(void *b, int len);
void			check_sigint(t_ping *ping);

// print.c
void			printf_header(t_ping *ping);
void			print_stats(t_ping *ping);


#endif