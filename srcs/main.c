/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/11 21:43:13 by hubourge          #+#    #+#             */
/*   Updated: 2025/02/12 01:36:00 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

int	parsing (int ac, char **av, t_ping *ping);

int	main(int ac, char **av)
{
	t_ping	ping;
	
	if (parsing(ac, &av[1], &ping) == 0)
		return (1);

	struct	sockaddr_in dest_addr;
	char	packet[PACKET_SIZE];
	int		sockfd;

	// Create a raw socket
	if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
	{
		perror("socket");
		return (1);
	}
	
	// Fill in the destination information
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family		= AF_INET;
	dest_addr.sin_port			= 0;
    dest_addr.sin_addr.s_addr	= inet_addr(ping.ip);

	// struct icmphdr
	// {
	//   u_int8_t type;                /* message type */
	//   u_int8_t code;                /* type sub-code */
	//   u_int16_t checksum;
	//   union
	//   {
	//     struct
	//     {
	//       u_int16_t        id;
	//       u_int16_t        sequence;
	//     } echo;                        /* echo datagram */
	//     u_int32_t        gateway;        /* gateway address */
	//     struct
	//     {
	//       u_int16_t        __unused;
	//       u_int16_t        mtu;
	//     } frag;                        /* path mtu discovery */
	//   } un;
	// };
	
	// ICMP packet
	struct icmphdr *icmp = (struct icmphdr *)packet;	
	memset(packet, 0, PACKET_SIZE);
    icmp->type				= ICMP_ECHO;
    icmp->code				= 0;
    icmp->checksum			= 0;
    icmp->un.echo.id		= getpid();
    icmp->un.echo.sequence	= 1;
	
	return (0);
}

int	parsing (int ac, char **av, t_ping *ping)
{
	if (ac < 2)
	{
		ft_putstr_fd("Usage: ft_ping [hostname]\n", 2);
		return (1);
	}

	ping->host = av[0];
	ping->ip = av[0];

	return (1);
}