#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <linux/route.h>


#define DEFAULT_ETH "eth0"
#define FALSE    0
#define TRUE     1

static int set_addr(unsigned char ip[16], int flag);
static int get_addr(unsigned char ip[16], int flag);

int get_ip(unsigned char ip[16])
{
    return get_addr(ip, SIOCGIFADDR);
}

int get_ip_netmask(unsigned char ip[16])
{
    return get_addr(ip, SIOCGIFNETMASK);
}

int get_mac(unsigned char addr[6])
{
    return get_addr(addr, SIOCGIFHWADDR);
}

static int get_addr(unsigned char *addr, int flag)
{
    int32_t sockfd = 0;
    struct sockaddr_in *sin;
    struct ifreq ifr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket error!\n");
        return FALSE;
    }

    memset(&ifr, 0, sizeof(ifr));
    snprintf(ifr.ifr_name, (sizeof(ifr.ifr_name) - 1), "%s", DEFAULT_ETH);

    if(ioctl(sockfd, flag, &ifr) < 0 )
    {
        perror("ioctl error!\n");
        close(sockfd);
        return FALSE;
    }
    close(sockfd);

    if (SIOCGIFHWADDR == flag){
        memcpy((void *)addr, (const void *)&ifr.ifr_ifru.ifru_hwaddr.sa_data, 6);
        /*printf("mac address: %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n", addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);*/
    }else{
        sin = (struct sockaddr_in *)&ifr.ifr_addr;
        snprintf((char *)addr, 16, "%s", inet_ntoa(sin->sin_addr));
    }

    return TRUE;
}

int is_valid_ip(unsigned char ipaddr[16])
{
    int ret = 0;
    struct in_addr inp;
    ret = inet_aton(ipaddr, &inp);
    if (0 == ret)
    {
        return FALSE;
    }
    else
    {
        printf("inet_aton:ip=%ud\n",ntohl(inp.s_addr));
    }

    return TRUE;
}

/*
 * 先验证是否为合法IP，然后将掩码转化成32无符号整型，取反为000...00111...1，
 * 然后再加1为00...01000...0，此时为2^n，如果满足就为合法掩码
 *
 * */
int is_valid_netmask(unsigned char netmask[16])
{
    if(is_valid_ip(netmask) > 0)
    {
        unsigned int b = 0, i, n[4];
        sscanf(netmask, "%u.%u.%u.%u", &n[3], &n[2], &n[1], &n[0]);
        for(i = 0; i < 4; ++i) //将子网掩码存入32位无符号整型
            b += n[i] << (i * 8);
        b = ~b + 1;
        if((b & (b - 1)) == 0) //判断是否为2^n
            return TRUE;
    }

    return FALSE;
}


int set_ip(unsigned char ip[16])
{
    return set_addr(ip, SIOCSIFADDR);
}
int set_ip_mask(unsigned char ip[16])
{
    return set_addr(ip,SIOCSIFNETMASK);
}
static int set_addr(unsigned char ip[16], int flag)
{
    struct ifreq ifr;
    struct sockaddr_in sin;
    int sockfd;

    if (is_valid_ip(ip) < 0)
    {
        printf("ip was invalid!\n");
        return FALSE;
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd == -1){
        fprintf(stderr, "Could not get socket.\n");
        perror("eth0\n");
        return FALSE;
    }

    snprintf(ifr.ifr_name, (sizeof(ifr.ifr_name) - 1), "%s", DEFAULT_ETH);

    /* Read interface flags */
    if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0) {
        fprintf(stderr, "ifdown: shutdown ");
        perror(ifr.ifr_name);
        return FALSE;
    }

    memset(&sin, 0, sizeof(struct sockaddr));
    sin.sin_family = AF_INET;

    inet_aton(ip, (struct in_addr *)(&sin.sin_addr.s_addr));
    memcpy(&ifr.ifr_addr, &sin, sizeof(struct sockaddr));
    if (ioctl(sockfd, flag, &ifr) < 0){
        fprintf(stderr, "Cannot set IP address. ");
        perror(ifr.ifr_name);
        return FALSE;
    }

    return TRUE;
}


int set_gateway(unsigned char ip[16])
{
    int sockFd;
    struct sockaddr_in sockaddr;
    struct rtentry rt;

    if (is_valid_ip(ip) < 0)
    {
        printf("gateway was invalid!\n");
        return FALSE;
    }

    sockFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockFd < 0)
    {
        perror("Socket create error.\n");
        return FALSE;
    }

    memset(&rt, 0, sizeof(struct rtentry));
    memset(&sockaddr, 0, sizeof(struct sockaddr_in));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = 0;
    if(inet_aton(ip, &sockaddr.sin_addr)<0)
    {
        perror("inet_aton error\n" );
        close(sockFd);
        return FALSE;
    }

    memcpy ( &rt.rt_gateway, &sockaddr, sizeof(struct sockaddr_in));
    ((struct sockaddr_in *)&rt.rt_dst)->sin_family=AF_INET;
    ((struct sockaddr_in *)&rt.rt_genmask)->sin_family=AF_INET;
    rt.rt_flags = RTF_GATEWAY;
    if (ioctl(sockFd, SIOCADDRT, &rt)<0)
    {
        perror("ioctl(SIOCADDRT) error in set_default_route\n");
        close(sockFd);
        return FALSE;
    }

    return TRUE;
}

int main()
{
        char ip[16];
        get_ip(ip);
        printf("getIP:%s\n",ip);
        
        get_ip_netmask(ip);
        printf("getMask:%s\n",ip);


        set_ip("192.168.0.162");

        set_ip_mask("255.255.255.0");

        //set_gateway("192.168.0.0");

        return 0;
}
