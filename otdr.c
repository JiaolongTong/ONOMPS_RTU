#include "otdr.h"  
otdr * OTDR_Create()
{
	otdr * me = (otdr *) malloc(sizeof(otdr));
    return me;
}
void OTDR_Destory(otdr *me)
{
	free(me);	
}
                                         // OTDR 
int NetworkIdle(int s,char *buf)
	{
		frame_header_t  *header;
		start_measure_t *start;
		uint32_t tlen;
		tlen   = sizeof(frame_header_t) + 12;
		header = (frame_header_t *)buf;
		strcpy(header->FrameSync, FRAME_SYNC_STRING);
		header->TotalLength = tlen;
		start =  (start_measure_t *)(buf + sizeof(frame_header_t));
		start->cmd = CMD_HOST_NETWORK_IDLE;
		start->len = 0;
		return send(s, buf, tlen, 0);
	}

// 设置参数并启动OTDR测试
int HostStartMeasure(int sockt,otdr const * me,char * buf)
	{
		frame_header_t *header;
		start_measure_t *start;
		uint32_t tlen;
		memset(buf, 0, 1024);
		tlen = sizeof(frame_header_t) + sizeof(start_measure_t);
		header = (frame_header_t *)buf;
		strcpy(header->FrameSync, FRAME_SYNC_STRING);
		header->TotalLength = tlen;
		start = (start_measure_t *)(buf + sizeof(frame_header_t));
		start->cmd = CMD_HOST_START_MEASURE;
		start->len = sizeof(start_measure_t)-12;
		start->Ctrl.OtdrMode = 1;
		start->Ctrl.OtdrOptMode = 0;
		start->Ctrl.RSVD = 0;
		start->Ctrl.EnableRefresh = 1;
		start->Ctrl.RefreshPeriod_ms = 1000;                //var time
		start->State.Lambda_nm = me->Lambda_nm;
		start->State.MeasureLength_m = me->MeasureLength_m;
		start->State.PulseWidth_ns = me->PulseWidth_ns;
		start->State.MeasureTime_ms = me->MeasureTime_ms*1000;
		start->State.n =me->n;
		start->State.EndThreshold= me->EndThreshold;
		start->State.NonRelectThreshold= me->NonRelectThreshold;
		printf("Start Measure %dm-%dns\n", me->MeasureLength_m, me->PulseWidth_ns);
		return send(sockt, buf, tlen, 0);
	}

// process data
int ProcessData(char pbuf[], uint32_t len,int * flag)
	{
		otdr_state_t *state;
		uint32_t cmd;
		time_t rawtime;
		struct tm * timeinfo;
		char timE [80];
		time (&rawtime);
		timeinfo = localtime (&rawtime);
		strftime (timE,80,"%Y-%m-%d %I:%M:%S",timeinfo);
		cmd = *(uint32_t*)pbuf;
		switch(cmd)
		{
			case CMD_RESPONSE_STATE:
				{
					uint32_t code;
					state = (otdr_state_t*)pbuf;
					code  = state->StateCode;
					printf("\n%s : OTDR State code : %d\n", timE, code);
					memset(pbuf, 0, 100);
					return code;
				}
			case CMD_DSP_UPLOAD_ALL_DATA:
				{
					FILE *fp;
					printf("%s : Receive final all data : %d\n", timE, len);
					fp = fopen("OtdrAllData.hex", "w");
					if(NULL == fp)
						{
							printf("file can't open\n");
						}
					else
						{
							fwrite(pbuf, 1, len, fp);
							fclose(fp);
							printf("Successfully save data to file 'OtdrAllData.hex'\n");
						}
					memset(pbuf, 0, 100);
					*flag = 1;
					return len;
				}
			case CMD_DSP_UPLOAD_REF_DATA:
					{
						otdr_ref_t *ref;
						ref = (otdr_ref_t*)pbuf;
						printf("%s : receive refresh data, datanum = %d\n", timE, ref->OtdrData.DataNum);
						memset(pbuf, 0, 100);
						return len;
					}
			default :
					{
						printf("<Error>cmd : 0x%x len = %d<Error>\n", cmd, len);
						return 1;
					}
		}
	}
int OtdrTest(otdr const * me)
	{
		int  count,sock, rxlen, curlen, totalrxlen, size;
        int  get_final;
		struct sockaddr_in dst;                       
		frame_header_t *header;
        char buf[BUF_SIZE];                      //保存接收到的数据 
		size = sizeof(dst);
		bzero(&dst, size);
		dst.sin_family = AF_INET;
		dst.sin_port = htons(5000);
		dst.sin_addr.s_addr = inet_addr(OTDR_IP);   //配置模块IP地址
		sock = socket(PF_INET, SOCK_STREAM, 0);
		if(sock < 0)
			{
				perror("OTDR socket error\n");
				return -1;
			}
		count = 256*1024;                                                          //设定接收缓冲区为256K
		setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &count, sizeof(count));
		if(connect(sock, (struct sockaddr*)&dst, size) < 0)
			{
				printf("OTDR connect errno ");
				return -1;
			}
			HostStartMeasure(sock,me, buf);                               //设定测量参数，并开始测试      
			get_final = 0;
            while(!get_final){
				curlen = recv(sock, buf, sizeof(frame_header_t), 0);      //接收数据帧头
				if(curlen <= 0)
					{
						printf("%d : recv error --> %s\n", __LINE__, strerror(errno));
						return -1;
					}	
				if(strcmp(buf, FRAME_SYNC_STRING) != 0)                   //检查帧头是否正确
					{
						printf("frame sync string not found\n");
						return -1;
					}	
				header = (frame_header_t*)buf;  		          //检查长度是否正确
				totalrxlen = header->TotalLength;
				totalrxlen -= sizeof(frame_header_t);
				if(totalrxlen < 12)
					{
						printf("total rxlen error\n");
						return -1;
					}
				rxlen = 0;
				while(rxlen < totalrxlen)   	                          // rcv remainder
					{
						curlen = recv(sock, buf+rxlen, totalrxlen-rxlen, 0);
						if(curlen <= 0)
							{
								printf("%d : recv error --> %s\n", __LINE__, strerror(errno));
								return -1;
							}
						rxlen += curlen;
					}
				rxlen = ProcessData(buf, rxlen,&get_final);
				NetworkIdle(sock,buf);				                                                          //sleep(1);
            }
		close(sock);
		return 0;
	}
