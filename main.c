#include <stdio.h>

void main(){
   
  int   i,N=5;
  int   ping=0,gai=0,shengyu=0;
  int   Ben=0;
  for(i=1;i<=N;i++){
         ping++;
         gai++;
         printf("     瓶 %d  盖:%d 酒:%d\n",ping,gai,N);
       
         if(ping  == 2 && ping >0 ) {
              ping = ping -2;
              N=N+1;
              printf("兑换:瓶 %d  盖:%d 酒:%d\n",ping,gai,N);

         }

         if(gai  == 4 && gai >0) {
              gai =gai - 4;
              N=N+1;
              printf("兑换:瓶 %d  盖:%d 酒:%d\n",ping,gai,N);


         }

  }
  while (1) {
       if (ping > 0) { 
        N ++;
        ping ++;
        gai ++;
        printf("借瓶:瓶 %d  盖:%d 酒:%d\n",ping,gai,N);
        ping = ping-2;
        printf("兑换:瓶 %d  盖:%d 酒:%d\n",ping,gai,N);
       } else if (gai >= 2) { 
        N  = N + gai;
        gai = 4-gai; 
        printf("兑换:瓶 %d  盖:%d 酒:%d\n",ping,gai,N);
       } else {
        break;
  }
}
  printf("总共喝了 %d 瓶, 剩余 空瓶 %d  瓶盖 %d\n",N,ping,gai);
}
