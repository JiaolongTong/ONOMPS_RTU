#include "uploadCycTestData.h"

const char base[]  = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";


//////////////////////   for test
/*
  if(SwitchPos==PARALLEL)  Status = 1;
    if(SWitchPos==ACROSS)    Status = 0;    


int otdr_sem_id =0; 
void main(void)
{
      int SNo =1,CM=1;
      otdr *testPar;
      backData * bData=NULL;
      bData=backData_Create();
      bData->SwitchPos =96;
      strcpy(bData->backIP ,"192.168.0.140");
//    testPar = OTDR_Create();
//    testPar->MeasureLength_m=1000;
//    testPar->PulseWidth_ns=100;
//    testPar->Lambda_nm=10;
//    testPar->MeasureTime_ms=1;  
//    testPar->n=2.1;
//    testPar->NonRelectThreshold=2.2;
//    testPar->EndThreshold=2.3;
      upload(bData,SNo,CM,4);
}
**************************/
backData * backData_Create()
{
	backData * me = (backData *) malloc(sizeof(backData));
    return me;
}
void backData_Destory(backData *me)
{
	free(me);	
}


int upload(backData *bData,int SNo,int CM,int type)
{  
   CURL *curl=NULL;  
   CURLM *multi_handle=NULL; 
   FILE* fptr=NULL; 
   int still_running;    
   struct curl_httppost *formpost=NULL;  
   struct curl_httppost *lastptr=NULL;  
   struct curl_slist    *headerlist=NULL; 
   char *backIP=NULL;
   switch(type)
     {
	   case 1: 
                 {
                   XMLgenerOpticPowerWarming(RTUSENDFILE,bData->powerValue,bData->powerGate,bData->level,SNo,CM,type);break;
                 }
	   case 2: 
           case 3:
                 {   
                   XMLgenerNewOTDRData(RTUSENDFILE,bData->otdrPar,SNo,CM,type);break;
                 }
           case 4:{
                   XMLsendProtectSwitchSwitch(RTUSENDFILE,bData->SwitchStatusRecv,bData->SwitchStatusSend,bData->SNoOnlineSend,bData->SNoOnlineRecv,CM,type);break;
                 }

           case 5:{
                   XMLsendLineFaultWarming(RTUSENDFILE,bData->SNoOnlineSend,bData->SNoOnlineRecv,CM,type);break;
                 }
	   default: {printf("Input segment fault!\n");return -1;}break;
     }

   backIP=setBackAddr(bData->backIP);

   curl_formadd(&formpost,  
                &lastptr,  
                CURLFORM_COPYNAME,"sendfile",  
                CURLFORM_FILE,"send.xml",  
                CURLFORM_END);   

    if ((fptr = fopen(BACKFILE,"w")) == NULL)  
    {  
        printf("fopen BACKFILE file error\n");  
        return -1;  
    } 
    curl = curl_easy_init();  
    multi_handle = curl_multi_init();  
    //headerlist = curl_slist_append(headerlist, "Content-type:text/xml"); 
    headerlist = curl_slist_append(headerlist, "Expect:");
    if(curl && multi_handle) {     
    curl_easy_setopt(curl, CURLOPT_URL, backIP);                         // what URL that receives this POST
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,write_data); 
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER,   headerlist);  
    curl_easy_setopt(curl, CURLOPT_HTTPPOST,     formpost);  
    curl_easy_setopt(curl, CURLOPT_WRITEDATA,    fptr);                  //callback file ptr
    curl_multi_add_handle(multi_handle, curl);  
    curl_multi_perform(multi_handle, &still_running);  
    do {  
	      struct timeval timeout;  
	      int rc;                                                
	      fd_set fdread;  
	      fd_set fdwrite;  
	      fd_set fdexcep;  
	      int maxfd = -1;  
	      long curl_timeo = -1;  
	      FD_ZERO(&fdread);  
	      FD_ZERO(&fdwrite);  
	      FD_ZERO(&fdexcep);  
	      timeout.tv_sec = 1;                                                      // set a suitable timeout to play around with
	      timeout.tv_usec = 0;  	  
	      curl_multi_timeout(multi_handle, &curl_timeo);  
	      if(curl_timeo >= 0) {  
		timeout.tv_sec = curl_timeo / 1000;  
		if(timeout.tv_sec > 1)  
		   timeout.tv_sec = 1;  
		else  
		   timeout.tv_usec = (curl_timeo % 1000) * 1000;  
	      }  	       
	      curl_multi_fdset(multi_handle, &fdread, &fdwrite, &fdexcep, &maxfd);     // get file descriptors from the transfers
	      rc = select(maxfd+1, &fdread, &fdwrite, &fdexcep, &timeout);    
	      switch(rc) {  
		      case -1:break;                                                               // select error        
		      case  0:  
		      default:curl_multi_perform(multi_handle, &still_running);  break;  
	      }  
	} while(still_running);  
	curl_multi_cleanup(multi_handle);  
	curl_easy_cleanup(curl);                                                   //always cleanup 
	curl_formfree(formpost);                                                   // then cleanup the formpost chain  
	curl_slist_free_all (headerlist);                                          // free slist 
        fclose(fptr);
        fptr=NULL;
        formpost=NULL;
        headerlist=NULL;
        lastptr  =NULL;
        multi_handle=NULL;
        curl=NULL;
  }  
  printf("XML_Send Sucessful URL:%s\n",backIP);
  free(backIP);
  return 0;  

} 

size_t write_data(void* buffer,size_t size,size_t nmemb,void *stream)  
	{  
	    FILE *fptr = (FILE*)stream;  
	    fwrite(buffer,size,nmemb,fptr);  
	    return size*nmemb;  
	} 

char * setBackAddr(char * backIP)
{
     char *backAddr=NULL;
     backAddr=(char *)malloc(sizeof(char)* 100);
     *backAddr ='\0';
     strcat(backAddr,"http://");
     strcat(backAddr,backIP);
     strcat(backAddr,":8080/fiberMonitor/TomCat");
     return backAddr;
}

int XMLgenerOpticPowerWarming (char * filename,float powerValue,float powerGate,int level,int SNo,int CM,int type)
{
    FILE * fd;
    char str[100];
    char * sumStr;
    sumStr  =(char *)malloc(sizeof(char)*1024*8);
    *sumStr ='\0';
    strcat(sumStr,"<?xml version='1.0' encoding='UTF-8'?>\n");
    strcat(sumStr,"<AsynPush>\n");
    strcat(sumStr,"<OpticPowerWarming >\n");
    strcat(sumStr,"	<CMDcode>522</CMDcode>\n");
    strcat(sumStr,"	<R>*</R >\n");
    sprintf(str,"%d",CM);
    strcat(sumStr,"	<CM>"); strcat(sumStr,str);strcat(sumStr,"</CM>\n");
    strcat(sumStr,"	<CLP>*</CLP>\n");
    sprintf(str,"%d",SNo);
    strcat(sumStr,"	<SNo>"); strcat(sumStr,str);strcat(sumStr,"</SNo>\n");
    sprintf(str,"%d",level);
    strcat(sumStr,"	<WarmingLevel>"); strcat(sumStr,str);strcat(sumStr,"</WarmingLevel>\n");
    sprintf(str,"%f",powerGate);
    strcat(sumStr,"	<Gate>"); strcat(sumStr,str);strcat(sumStr,"</Gate>\n");
    sprintf(str,"%f",powerValue);
    strcat(sumStr,"	<PowerValue>"); strcat(sumStr,str);strcat(sumStr,"</PowerValue>\n");
    strcat(sumStr,"</OpticPowerWarming>\n");
    strcat(sumStr,"</AsynPush>\n");    
    if ((fd = fopen(filename,"w")) == NULL)  
    {  
        fprintf(stderr,"fopen file error:%s\n",filename);  
        return -1;  
    } 
    fwrite(sumStr,strlen(sumStr),1,fd);    
    fclose(fd);
    free(sumStr);
    return 0; 
}
//XMLsendProtectSwitchSwitch(RTYSENDFILE,bData->SwitchPos,ModNum,rPNo,sPNo,CM,type);break;
/*
CMDcode	523（固定）	INT(1Bytes)	每个指令或数据的唯一标识
R		        INT(3Bytes)	预留
CM		        String(10Bytes)	RTU代码
CLP		        String(10Bytes)	局站代码
ModNo		        INT(3Bytes)	本RTU的模块号
RSNo		        INT(3Bytes)	当前下行在纤端口
SSNo		        INT(3Bytes)	当前上行在纤端口
StatusRecv		INT(3Bytes)	当前上行1*2光开关状态（1->2:96 1->3:16）
StatusSend		INT(3Bytes)	当前下行1*2光开关状态（1->2:96 1->3:16）

*/
int XMLsendProtectSwitchSwitch(char * filename,int SwitchPosMaster,int SwitchPosSlaver,int SNoSend,int SNoRecv,int CM,int type)
{
    FILE * fd;
    char str[100];
    char * sumStr;
    
    int   PNo;
    int   ModNum  = (SNoSend-1)/8+1 ;

    sumStr  =(char *)malloc(sizeof(char)*1024*8);
    *sumStr ='\0';                                 
    strcat(sumStr,"<?xml version='1.0' encoding='UTF-8'?>\n");
    strcat(sumStr,"<AsynPush>\n");
    strcat(sumStr,"<SwitchReport>\n");
    strcat(sumStr,"	<CMDcode>523</CMDcode>\n");
    strcat(sumStr,"	<R>*</R >\n");
    sprintf(str,"%d",CM);
    strcat(sumStr,"	<CM>"); strcat(sumStr,str);strcat(sumStr,"</CM>\n");
    strcat(sumStr,"	<CLP>*</CLP>\n");
    sprintf(str,"%d",ModNum);
    strcat(sumStr,"	<ModNo>"); strcat(sumStr,str);strcat(sumStr,"</ModNo>\n");
    sprintf(str,"%d",SNoRecv);
    strcat(sumStr,"	<RSNo>"); strcat(sumStr,str);strcat(sumStr,"</RSNo>\n");
    sprintf(str,"%d",SNoSend);
    strcat(sumStr,"	<SSNo>"); strcat(sumStr,str);strcat(sumStr,"</SSNo>\n");

    sprintf(str,"%d",SwitchPosMaster);
    strcat(sumStr,"	<StatusRecv>"); strcat(sumStr,str);strcat(sumStr,"</StatusRecv>\n");
    sprintf(str,"%d",SwitchPosSlaver);
    strcat(sumStr,"	<StatusSend>"); strcat(sumStr,str);strcat(sumStr,"</StatusSend>\n");
    strcat(sumStr,"</SwitchReport>\n");
    strcat(sumStr,"</AsynPush>\n");    
    if ((fd = fopen(filename,"w")) == NULL)  
    {  
        fprintf(stderr,"fopen file error:%s\n",filename);  
        return -1;  
    } 
    fwrite(sumStr,strlen(sumStr),1,fd);    
    fclose(fd);
    free(sumStr);
    return 0; 

}
/*
CMDcode    	521（固定）	INT(1Bytes)	每个指令或数据的唯一标识
R		INT	预留
CM		String(10Bytes)	RTU代码
CLP		String(10Bytes)	局站代码
ModNo		INT(3Bytes)	本RTU的模块号
RSNo		INT(3Bytes)	当前下行在纤端口
SSNO		INT(3Bytes)	当前上行在纤端口
*/
int XMLsendLineFaultWarming   (char * filename,int SNoSend,int SNoRecv,int CM,int type){

    FILE * fd;
    char str[100];
    char * sumStr;
    
    int   PNo;
    int   ModNum = (SNoSend-1)/8+1;

    sumStr  =(char *)malloc(sizeof(char)*1024*8);
    *sumStr ='\0';                                 
    strcat(sumStr,"<?xml version='1.0' encoding='UTF-8'?>\n");
    strcat(sumStr,"<AsynPush>\n");
    strcat(sumStr,"<LineFaultWarming>\n");
    strcat(sumStr,"	<CMDcode>521</CMDcode>\n");
    strcat(sumStr,"	<R></R >\n");
    sprintf(str,"%d",CM);
    strcat(sumStr,"	<CM>"); strcat(sumStr,str);strcat(sumStr,"</CM>\n");
    strcat(sumStr,"	<CLP>*</CLP>\n");
    sprintf(str,"%d",ModNum);
    strcat(sumStr,"	<ModNo>"); strcat(sumStr,str);strcat(sumStr,"</ModNo>\n");
    sprintf(str,"%d",SNoRecv);
    strcat(sumStr,"	<RSNo>"); strcat(sumStr,str);strcat(sumStr,"</RSNo>\n");
    sprintf(str,"%d",SNoSend);
    strcat(sumStr,"	<SSNo>"); strcat(sumStr,str);strcat(sumStr,"</SSNo>\n");
    strcat(sumStr,"</LineFaultWarming>\n");
    strcat(sumStr,"</AsynPush>\n");    
    if ((fd = fopen(filename,"w")) == NULL)  
    {  
        fprintf(stderr,"fopen file error:%s\n",filename);  
        return -1;  
    } 
    fwrite(sumStr,strlen(sumStr),1,fd);    
    fclose(fd);
    free(sumStr);

}
int XMLgenerNewOTDRData (char * filename,otdr *Par,int SNo,int CM,int type)
{
    char    *sumStr;
    sumStr  =(char *)malloc(sizeof(char)*1024*100);
    char *pdata =NULL;
    int  en_size=0,nret,ContentLength=0;
    if(nret = read_file( en_ORDRDATA, &en_size, &pdata) !=0)
      {
	fprintf(stderr,"Open Error\n");
        return -1;
      }
    if(en_size>en_MAXSIZE-1)
      {    
         ContentLength   =   en_MAXSIZE-1;   
         pdata[en_MAXSIZE]  =  '\0';  
      } 
    else
      {
	ContentLength   =  en_size;
        pdata[ContentLength]  =  '\0';    
      } 


    char *en_tetDat = base64_encode(pdata, ContentLength); 
    *sumStr ='\0';
    char str[100];
        time_t rawtime;
	struct tm * timeinfo=NULL;
	time (&rawtime);
        char * timE =ctime(&rawtime);
   if(type==3){  //周期测试数据
	    strcat(sumStr,"<?xml version='1.0' encoding='UTF-8'?>\n");
	    strcat(sumStr,"<AsynPush>\n");
	    strcat(sumStr,"<NewOTDRData>\n");
	    strcat(sumStr,"	<CMDcode>524</CMDcode>\n");
	    strcat(sumStr,"	<R>*</R>\n");
	    sprintf(str,"%d",CM);
	    strcat(sumStr,"	<CM>"); strcat(sumStr,str);strcat(sumStr,"</CM>\n");
	    strcat(sumStr,"	<CLP>*</CLP>\n");
	    sprintf(str,"%d",SNo);
	    strcat(sumStr,"	<SNo>"); strcat(sumStr,str);strcat(sumStr,"</SNo>\n");
	    strcat(sumStr,"	<Type>1</Type>\n");
	    strcat(sumStr,"	<Data>\n");
	    sprintf(str,"%d",Par->Lambda_nm);
	    strcat(sumStr,"		<P11>");strcat(sumStr,str);strcat(sumStr,"</P11>\n");
	    sprintf(str,"%d",Par->MeasureLength_m);
	    strcat(sumStr,"		<P12>");strcat(sumStr,str);strcat(sumStr,"</P12>\n");
	    sprintf(str,"%d",Par->PulseWidth_ns);
	    strcat(sumStr,"		<P13>");strcat(sumStr,str);strcat(sumStr,"</P13>\n");
	    sprintf(str,"%d",Par->MeasureTime_ms);
	    strcat(sumStr,"		<P14>");strcat(sumStr,str);strcat(sumStr,"</P14>\n");
	    sprintf(str,"%f",Par->n);
	    strcat(sumStr,"		<P15>");strcat(sumStr,str);strcat(sumStr,"</P15>\n");
	    sprintf(str,"%f",Par->EndThreshold);
	    strcat(sumStr,"		<P16>");strcat(sumStr,str);strcat(sumStr,"</P16>\n");
	    sprintf(str,"%f",Par->NonRelectThreshold);
	    strcat(sumStr,"		<P17>");strcat(sumStr,str);strcat(sumStr,"</P17>\n");
	    sprintf(str,"%d",rawtime);
	    strcat(sumStr,"	        <T9>"); strcat(sumStr,str);strcat(sumStr,"</T9>\n");
	    strcat(sumStr,"		<ExtraInformation>Information</ExtraInformation>\n");
	    strcat(sumStr,"		<TstDat>\n");
	    strcat(sumStr,              en_tetDat);            //
	    strcat(sumStr,"\n</TstDat>\n");
	    strcat(sumStr,"	</Data>\n");
	    strcat(sumStr,"</NewOTDRData>\n");
	    strcat(sumStr,"</AsynPush>\n"); 
   }
   if(type==2){  //障碍告警测试数据
	    strcat(sumStr,"<?xml version='1.0' encoding='UTF-8'?>\n");
	    strcat(sumStr,"<AsynPush>\n");
	    strcat(sumStr,"<NewOTDRData>\n");
	    strcat(sumStr,"	<CMDcode>524</CMDcode>\n");
	    strcat(sumStr,"	<R>*</R>\n");
	    sprintf(str,"%d",CM);
	    strcat(sumStr,"	<CM>"); strcat(sumStr,str);strcat(sumStr,"</CM>\n");
	    strcat(sumStr,"	<CLP>*</CLP>\n");
	    sprintf(str,"%d",SNo);
	    strcat(sumStr,"	<SNo>"); strcat(sumStr,str);strcat(sumStr,"</SNo>\n");
	    strcat(sumStr,"	<Type>2</Type>\n");
	    strcat(sumStr,"	<Data>\n");
	    sprintf(str,"%d",Par->PS);  
	    strcat(sumStr,"		<PS>");strcat(sumStr,str);strcat(sumStr,"</PS>\n");
	    sprintf(str,"%d",Par->Lambda_nm);          
	    strcat(sumStr,"		<P11>");strcat(sumStr,str);strcat(sumStr,"</P11>\n");
	    sprintf(str,"%d",Par->MeasureLength_m);
	    strcat(sumStr,"		<P12>");strcat(sumStr,str);strcat(sumStr,"</P12>\n");
	    sprintf(str,"%d",Par->PulseWidth_ns);
	    strcat(sumStr,"		<P13>");strcat(sumStr,str);strcat(sumStr,"</P13>\n");
	    sprintf(str,"%d",Par->MeasureTime_ms);
	    strcat(sumStr,"		<P14>");strcat(sumStr,str);strcat(sumStr,"</P14>\n");
	    sprintf(str,"%f",Par->n);
	    strcat(sumStr,"		<P15>");strcat(sumStr,str);strcat(sumStr,"</P15>\n");
	    sprintf(str,"%f",Par->EndThreshold);
	    strcat(sumStr,"		<P16>");strcat(sumStr,str);strcat(sumStr,"</P16>\n");
	    sprintf(str,"%f",Par->NonRelectThreshold);
	    strcat(sumStr,"		<P17>");strcat(sumStr,str);strcat(sumStr,"</P17>\n");
	    sprintf(str,"%f",Par->AT01);
	    strcat(sumStr,"		<AT01>");strcat(sumStr,str);strcat(sumStr,"</AT01>\n");
	    sprintf(str,"%f",Par->AT02);
	    strcat(sumStr,"		<AT02>");strcat(sumStr,str);strcat(sumStr,"</AT02>\n");
	    sprintf(str,"%f",Par->AT03);
	    strcat(sumStr,"		<AT03>");strcat(sumStr,str);strcat(sumStr,"</AT03>\n");
	    sprintf(str,"%f",Par->AT04);
	    strcat(sumStr,"		<AT04>");strcat(sumStr,str);strcat(sumStr,"</AT04>\n");
	    sprintf(str,"%f",Par->AT05);
	    strcat(sumStr,"		<AT05>");strcat(sumStr,str);strcat(sumStr,"</AT05>\n");
	    sprintf(str,"%f",Par->AT06);
	    strcat(sumStr,"		<AT06>");strcat(sumStr,str);strcat(sumStr,"</AT06>\n");
	    sprintf(str,"%d",rawtime);
	    strcat(sumStr,"	        <T9>"); strcat(sumStr,str);strcat(sumStr,"</T9>\n");
	    strcat(sumStr,"		<ExtraInformation>Information</ExtraInformation>\n");
	    strcat(sumStr,"		<TstDat>\n");
	    strcat(sumStr,              en_tetDat);            //
	    strcat(sumStr,"\n</TstDat>\n");
	    strcat(sumStr,"	</Data>\n");
	    strcat(sumStr,"</NewOTDRData>\n");
	    strcat(sumStr,"</AsynPush>\n"); 
    }
    
    FILE * fd;   
    if ((fd = fopen(filename,"w")) == NULL)  
    {  
        fprintf(stderr,"fopen file error:%s\n",filename);  
        return -1;  
    } 
    fwrite(sumStr,strlen(sumStr),1,fd);    
    fclose(fd);

        free(sumStr);
    	free(en_tetDat);
    	if ( pdata != NULL )
    	{
		free( pdata);
		pdata =NULL;
    	}
    return 0; 
}

/* */ 
char *base64_encode(const char* data, int data_len) 
{ 
    int prepare = 0; 
    int ret_len; 
    int temp = 0; 
    char *ret = NULL; 
    char *f = NULL; 
    int tmp = 0; 
    char changed[4]; 
    int i = 0; 
    ret_len = data_len / 3; 
    temp = data_len % 3; 
    if (temp > 0) 
    { 
        ret_len += 1; 
    } 
    ret_len = ret_len*4 + 1; 
    ret = (char *)malloc(ret_len); 
      
    if ( ret == NULL) 
    { 
        printf("No enough memory.\n"); 
        exit(0); 
    } 
    memset(ret, 0, ret_len); 
    f = ret; 
    while (tmp < data_len) 
    { 
        temp = 0; 
        prepare = 0; 
        memset(changed, '\0', 4); 
        while (temp < 3) 
        { 
            if (tmp >= data_len) 
            { 
                break; 
            } 
            prepare = ((prepare << 8) | (data[tmp] & 0xFF)); 
            tmp++; 
            temp++; 
        } 
        prepare = (prepare<<((3-temp)*8)); 
        for (i = 0; i < 4 ;i++ ) 
        { 
            if (temp < i) 
            { 
                changed[i] = 0x40; 
            } 
            else 
            { 
                changed[i] = (prepare>>((3-i)*6)) & 0x3F; 
            } 
            *f = base[changed[i]]; 
            f++; 
        } 
    } 
    *f = '\0'; 
      
    return ret; 
      
} 
static char find_pos(char ch)   
{ 
    char *ptr = (char*)strrchr(base, ch);
    return (ptr - base); 
} 
/* */ 
char *base64_decode(const char *data, int data_len) 
{ 
    int ret_len = (data_len / 4) * 3; 
    int equal_count = 0; 
    char *ret = NULL; 
    char *f = NULL; 
    int tmp = 0; 
    int temp = 0; 
    char need[3]; 
    int prepare = 0; 
    int i = 0; 
    if (*(data + data_len - 1) == '=') 
    { 
        equal_count += 1; 
    } 
    if (*(data + data_len - 2) == '=') 
    { 
        equal_count += 1; 
    } 
    if (*(data + data_len - 3) == '=') 
    {
        equal_count += 1; 
    } 
    switch (equal_count) 
    { 
    case 0: 
        ret_len += 4;
        break; 
    case 1: 
        ret_len += 4;
        break; 
    case 2: 
        ret_len += 3;
        break; 
    case 3: 
        ret_len += 2;
        break; 
    } 
    ret = (char *)malloc(ret_len); 
    if (ret == NULL) 
    { 
        printf("No enough memory.\n"); 
        exit(0); 
    } 
    memset(ret, 0, ret_len); 
    f = ret; 
    while (tmp < (data_len - equal_count)) 
    { 
        temp = 0; 
        prepare = 0; 
        memset(need, 0, 4); 
        while (temp < 4) 
        { 
            if (tmp >= (data_len - equal_count)) 
            { 
                break; 
            } 
            prepare = (prepare << 6) | (find_pos(data[tmp])); 
            temp++; 
            tmp++; 
        } 
        prepare = prepare << ((4-temp) * 6); 
        for (i=0; i<3 ;i++ ) 
        { 
            if (i == temp) 
            { 
                break; 
            } 
            *f = (char)((prepare>>((2-i)*8)) & 0xFF); 
            f++; 
        } 
    } 
    *f = '\0'; 
    return ret; 
}

int read_file( char *filename, int *len, char **data )
{
    int step = 1024*10;
    int file = open( filename, O_RDONLY );
    if ( file == -1 )
        return -1;
    int i = 0;
    while ( 1 )
    {
        *data = realloc( *data, step*(i+1));
        if ( data == NULL )
        {
            close( file );
            return -1;
        }

        int cur_len = read( file, *data+(step*i), step );
        if ( cur_len == 0 )
            break;
        else
            *len += cur_len;
        i++;
    }
    close( file );
    return 0;
}

