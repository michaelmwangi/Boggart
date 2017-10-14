#ifndef NETDEF_H
#define NETDEF_H

#include <sys/types.h> // system data types
#include <sys/socket.h> // socket definitions
#include <sys/select.h>
#include <sys/time.h> // timeval used in select
#include <errno.h>
#include <stdio.h>
#include <fcntl.h> // non blocking
#include <netinet/in.h> // sockaddr_in
#include <string.h> // memset
#include <arpa/inet.h> // inet functions
#include <unistd.h> // close
#include <netdb.h> // getnameinfo

#define LISTEN_QUEUE 511 // make sure this value is in congruence with /proc/sys/net/core/somaxconn as linx kernel will silently truncate to that value
#define BUF_SIZE 1024 * 16 // READ and WRITE IO buf
#define MAXFDS 1000 // maximum number of file descriptors that we will support for now

#endif