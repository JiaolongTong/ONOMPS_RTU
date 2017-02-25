#include "tcp-sender.h"

//code :7E7E05FFE40101E6   

int tcp_sender(char PNo,char SwitchPos)
{ 

	int sockfd,i;
 
	char buffer[1024];
 
	struct sockaddr_in server_addr;
 
	struct hostent *host;
 
        char check=0;

        buffer[0]=0x7E;
        buffer[1]=0x7E;
        buffer[2]=0x05;
        buffer[3]=0xFF;
        buffer[4]=0xE4;
        buffer[5]=PNo;
        buffer[6]=SwitchPos;

        for(i=0 ;i<7;i++)
           check=check + buffer[i];

        buffer[7]=check;

        /* 使用hostname查询host 名字 */

	if((host=gethostbyname(IP))==NULL) {
 
		fprintf(stderr,"Gethostname error\n");
 
		return -1;

	} 


	/* 客户程序开始建立 sockfd描述符 */ 

	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1) // AF_INET:Internet;SOCK_STREAM:TCP
	{
 
		fprintf(stderr,"Socket Error:%s\a\n",strerror(errno));
 
		return -1;
 
	}
 

	/* 客户程序填充服务端的资料 */ 

	bzero(&server_addr,sizeof(server_addr));                // 初始化,置0

	server_addr.sin_family=AF_INET;                         // IPV4

	server_addr.sin_port=htons(portnumber);                 // (将本机器上的short数据转化为网络上的short数据)端口号

	server_addr.sin_addr.s_addr=inet_addr(IP);                     //*((struct in_addr *)host->h_addr); // IP地址

	
	/* 客户程序发起连接请求 */ 

	if(connect(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr_in))==-1)
 
	{
 
		fprintf(stderr,"Connect Error:%s\a\n",strerror(errno)); 

		return -1;
 
	}
	/* 连接成功,发送数据 */
	write(sockfd,buffer,8); 

	/* 结束通讯 */ 
	close(sockfd); 

	return 0;
} 
/*
int main(int argc,char **argv){
    if(argc!=3){
        printf("Error input : [PNo] [SwitchSatrus]");
     }
    tcp_sender(atoi(argv[1]),atoi(argv[2]));
}
*/



