#include <sys/types.h>  
#include <string.h>  
#include <stdlib.h>  
#include <sys/ioctl.h>  
#include <stdio.h>  
#include <errno.h>  
#include <net/if.h>  
  
struct ethtool_value {  
        __uint32_t      cmd;  
        __uint32_t      data;  
};  
void main()  
{  
    struct ethtool_value edata;  
    int fd = -1, err = 0;  
    struct ifreq ifr;  
  
        memset(&ifr, 0, sizeof(ifr));  
        strcpy(ifr.ifr_name, "eth0");  
        fd = socket(AF_INET, SOCK_DGRAM, 0);  
        if (fd < 0) {  
                perror("Cannot get control socket");  
          
        }  
  
        edata.cmd = 0x0000000a;  
        ifr.ifr_data = (caddr_t)&edata;  
        err = ioctl(fd, 0x8946, &ifr);  
        if (err == 0) {  
                fprintf(stdout, "Link detected: %s\n",  
                        edata.data ? "yes":"no");  
        } else if (errno != EOPNOTSUPP) {  
                perror("Cannot get link status");  
        }  
 
}  
