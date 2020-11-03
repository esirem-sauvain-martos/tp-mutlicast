#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
struct sockaddr_in6 sin6;

#define IPPORT 54322

void Perror(const char *c)
{
	perror(c);
	exit(1);
}

void Usage()
{
	fprintf(stderr, "%s\n", "Usage: in2multi6 [-i interface][-h hop][-l loop] addr");
	exit(1);
}

int main(int argc, char **argv)
{
	u_int hops = 1, /* as defined in rfc2553 */
		loop = 1,	/* as defined in rfc2553 */
		ifi = 0;
	int s, cc, ch;
	char buf[1024];
	struct in6_addr addr6;
	extern char *optarg;
	extern int optind;

	addr6 = in6addr_any;
	if ((s = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		Perror("socket");

	while ((ch = getopt(argc, argv, "h:t:l:i:")) != -1)
	{
		switch (ch)
		{
		case 'h':
		case 't':
			hops = atoi(optarg);
			break;
		case 'l':
			loop = atoi(optarg);
			break;
		case 'i':
			if (sscanf(optarg, "%u\0", &ifi) != 1)
			{
				ifi = if_nametoindex(optarg);
				if (ifi == 0)
					Usage();
			}
			break;
		default:
			Usage();
		}
	}

	argc -= optind;
	argv += optind;

	if (argc != 1 || inet_pton(AF_INET6, *argv, &addr6) <= 0)
		Usage();

	if (setsockopt(s, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &hops, sizeof(hops)) < 0)
		Perror("setsockopt IPV6_MULTICAST_HOPS");

	if (setsockopt(s, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &loop, sizeof(loop)) < 0)
		Perror("setsockopt IPV6_MULTICAST_LOOP");

	if (ifi && (setsockopt(s, IPPROTO_IPV6, IPV6_MULTICAST_IF, &ifi, sizeof(u_int)) < 0))
		Perror("setsockopt IPV6_MULTICAST_IF");

	printf("interface: %d\n", ifi);

#ifdef SIN6_LEN
	sin6.sin6_len = sizeof(sin6);
#endif

	sin6.sin6_family = AF_INET6;
	sin6.sin6_addr = addr6;
	sin6.sin6_port = htons(54321);

	for (;;)
	{
		cc = read(0, buf, 1024);

		if (cc < 0)
			Perror("read file");

		if (cc == 0)
		{
			fprintf(stderr, "%d.\n", cc);
			exit(0);
		}

		if (sendto(s, buf, cc, 0, (struct sockaddr *)&sin6, sizeof(sin6)) < 0)
			Perror("sendto");

		fprintf(stderr, "-%d->\n", cc);
	}
}