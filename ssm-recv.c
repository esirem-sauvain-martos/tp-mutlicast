//Pass the IP address of the sender as CommandLine argument: ./ssm-rec ip.address.of.sender
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

/* Not everyone has the headers for this, so improvise */
#ifndef MCAST_JOIN_SOURCE_GROUP
#define MCAST_JOIN_SOURCE_GROUP 46

struct group_source_req
{
        /* Interface index.  */
        uint32_t gsr_interface;

        /* Group address.  */
        struct sockaddr_storage gsr_group;

        /* Source address.  */
        struct sockaddr_storage gsr_source;
};
#endif

int main(int argc, char *argv[])
{
        struct group_source_req group_source_req;
        struct sockaddr_in *group;
        struct sockaddr_in *source;
        int fd = socket(AF_INET,SOCK_DGRAM,getprotobyname("udp")->p_proto);
        struct sockaddr_in bindaddr;

        /* Setup the socket to listen on */
        bindaddr.sin_family = AF_INET;
        bindaddr.sin_port = htons(9991);
        bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        bind(fd,(struct sockaddr*)&bindaddr,sizeof(bindaddr));

        /* Set up the connection to the group */
        group_source_req.gsr_interface = 0;
        group=(struct sockaddr_in*)&group_source_req.gsr_group;
        source=(struct sockaddr_in*)&group_source_req.gsr_source;
        /* Group is 232.1.1.1 */
        group->sin_family = AF_INET;
        inet_aton("232.1.1.1",&group->sin_addr);
        group->sin_port = 0;

        /* Source is 10.1.20.9 */
        source->sin_family = AF_INET;
        inet_aton(argv[1],&source->sin_addr);
        source->sin_port = 0;

        setsockopt(fd,SOL_IP,MCAST_JOIN_SOURCE_GROUP, &group_source_req,
                                sizeof(group_source_req));

        while(1) {
                char buffer[65536];
                int ret=recv(fd,(char*)buffer,sizeof(buffer),0);
                write(1,buffer,ret);
        }
        return 0;
}