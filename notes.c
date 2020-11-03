#include <stdio.h>
#include <stdlib.h>

int main ()
{
	printf("ping6 FF02::1, ping6 FF02::2 (spécifier l'interface avec -I)\n");
	printf("in2multi6.c -> send ipv6 datagram\n");
	printf("multi2out.c -> receive ipv6 datagram\n");
	printf("FF02::16 -> all MLDv2 routers on the link\n");
	printf("options Hop-By-Hop\n");
	printf("MLD types: 3: include, 4: exclude\n");

	return 0;
}
