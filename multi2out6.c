#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <net/if.h>

#ifndef SO_REUSEPORT
#define SO_REUSEPORT SO_REUSEADDR
#endif

struct sockaddr_in6 sin6;
#define IPPORT 54321

void Perror(const char *c)
{
	perror(c);
	exit(1);
}

void Usage()
{
	fprintf(stderr, "%s\n", "Usage: multi2out6 [-i interface] addr");
	exit(1);
}

void BrokenPipe(int Signal)
{
	signal(SIGPIPE, BrokenPipe);
	return;
}

int main(int argc, char **argv)
{
	struct ipv6_mreq mreq;
	int cc, ccb, ch, s;
	char buf[10240];
	u_int one = 1;
	u_int ifi = 0;

	signal(SIGPIPE, BrokenPipe);
	while ((ch = getopt(argc, argv, "i:")) != -1)
	{
		switch (ch)
		{
		case 'i':
			if (sscanf(optarg, "%u\0", &ifi) != 1 && (ifi = if_nametoindex(optarg)) == 0)
				Usage();
			break;
		default:
			Usage();
		}
	}

	argc -= optind;
	argv += optind;
	if (argc != 1)
		Usage();

	if ((s = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		Perror("socket");

	setsockopt(s, SOL_SOCKET, SO_REUSEPORT, &one, sizeof(one));

#ifdef SIN6_LEN
	sin6.sin6_len = sizeof(sin6);
#endif

	sin6.sin6_family = AF_INET6;
	sin6.sin6_port = htons(IPPORT);

	if (bind(s, (struct sockaddr *)&sin6, sizeof(sin6)) < 0)
		Perror("bind");

	if (inet_pton(AF_INET6, *argv, &mreq.ipv6mr_multiaddr) != 1)
		Usage();

	// struct group_source_req group_filter;
	// group_filter.gsr_interface = ifi;

	// // Address group
	// struct sockaddr_in6 addr_group;
	// inet_pton(AF_INET6, "ff02::1000", &addr_group.sin6_addr);
	// memcpy(&group_filter.gsr_group, &addr_group, sizeof(addr_group));

	// // Address source
	// struct sockaddr_in6 addr_src;
	// inet_pton(AF_INET6, "2800::1", &addr_src.sin6_addr);
	// memcpy(&group_filter.gsr_source, &addr_src, sizeof(addr_src));

	// if (setsockopt(s, IPPROTO_IPV6, MCAST_BLOCK_SOURCE, &group_filter, sizeof(group_filter)) < 0)
	// 	Perror("setsockopt MCAST_BLOCK_SOURCE");

	mreq.ipv6mr_interface = ifi;

	if (setsockopt(s, IPPROTO_IPV6, IPV6_JOIN_GROUP, &mreq, sizeof(mreq)) < 0)
		Perror("setsockopt IPV6_JOIN_GROUP");

	for (;;)
	{
		struct sockaddr_in6 src;
		src.sin6_family = AF_INET6;
		socklen_t src_len = sizeof(src);

		cc = recvfrom(s, buf, 10240, 0, (struct sockaddr *)&src, &src_len);

		char str_src[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET6, &src.sin6_addr, str_src, INET6_ADDRSTRLEN);
		printf("%s says : \n", str_src);

		if (cc < 0)
			Perror("read socket");

		if (cc == 0)
		{
			fprintf(stderr, "..\n");
			exit(0);
		}

		ccb = write(1, buf, cc);

		if (ccb != cc)
			Perror("write file");

		fprintf(stderr, "<-%d-\n", cc);
	}
}
