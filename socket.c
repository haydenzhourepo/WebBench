/* $Id: socket.c 1.1 1995/01/01 07:11:14 cthuang Exp $
 *
 * This module has been modified by Radim Kolar for OS/2 emx
 */

/***********************************************************************
  module:       socket.c
  program:      popclient
  SCCS ID:      @(#)socket.c    1.5  4/1/94
  programmer:   Virginia Tech Computing Center
  compiler:     DEC RISC C compiler (Ultrix 4.1)
  environment:  DEC Ultrix 4.3 
  description:  UNIX sockets code.
 ***********************************************************************/

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <netdb.h>


// On success, returns a non-null pointer to dst.  NULL is returned if there was an
// error, with errno set to indicate the error
static const char* sock_ntop_host(char* dst, const struct sockaddr* sa)
{
    if (dst == NULL)
        return NULL;

    if (sa->sa_family == AF_INET)
    {
        struct sockaddr_in* sin = (struct sockaddr_in*)sa;
        return inet_ntop(AF_INET, &sin->sin_addr, dst, 128);
    }
#ifdef AF_INET6
    else if (sa->sa_family == AF_INET6)
    {
        struct sockaddr_in6* sin6 = (struct sockaddr_in6*)sa;
        return inet_ntop(AF_INET6, &sin6->sin6_addr, dst, 128);
    }
#endif
    else
    {
        fprintf(stderr, "address family not support %d", sa->sa_family);
        return NULL;
    }
}

int Socket(int ipv6, const char* host, int clientPort)
{
    int sock = -1;
    int error;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    char servip[128];

    bzero(&hints, sizeof(hints));
    if (ipv6)
        hints.ai_family = AF_INET6;
    else
        hints.ai_family = AF_INET;

    hints.ai_socktype = SOCK_STREAM;
    char strport[6];
    sprintf(strport, "%d", clientPort);
    error = getaddrinfo(host, strport, &hints, &result);
    if (error)
    {
        fprintf(stderr, "socket: getaddrinfo error %s\n", gai_strerror(error));
        return -1;
    }

    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        // bzero(&servip, sizeof(servip));
        // if (sock_ntop_host(servip, rp->ai_addr) == NULL)
        //     fprintf(stderr, "socket: host_inet_ntop error %s\n", strerror(errno));
        // else
        //     printf("server ip %s\n", servip);

        if ((sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1)
        {
            // fprintf(stderr, "socket: create socket error %s\n", rp->ai_addr);
            continue;
        }

        if (connect(sock, rp->ai_addr, rp->ai_addrlen) != -1)
            break;

        close(sock);
    }

    freeaddrinfo(result);

    if (rp == NULL)
        return -1;

    return sock;
}
