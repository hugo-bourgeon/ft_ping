/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/24 14:34:54 by hubourge          #+#    #+#             */
/*   Updated: 2025/02/12 01:38:47 by hubourge         ###   ########.fr       */
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

# define PACKET_SIZE 64

typedef struct s_ping
{
	char *host;
	char *ip;
}	t_ping;

#endif