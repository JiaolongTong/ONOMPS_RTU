#include <stdio.h>  
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>  
#include <sys/time.h>  
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <curl/curl.h>
//#include "/usr/local/curl_x86/include/curl/curl.h"
#define  TOMCAT_SERVER    "http://192.168.0.140:8080/fiberMonitor/TomCat"
#define  BOA_SERVER       "http://192.168.0.115:5000/cgi-bin/BoaCom.cgi?value=SetNamedTestSegment" 
#define  BACKFILE     "back.xml"  
#define  RTUSENDFILE  "send.xml"
#define  en_ORDRDATA  "OtdrAllData.hex"
#define  en_MAXSIZE    1024*100
const char base[]  =   "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
size_t write_data(void* buffer,size_t size,size_t nmemb,void *stream);
int XMLgenerLineFaultWarming(char * filename,char * types); 
int XMLgenerOpticPowerWarming (char * filename,char * types); 
int XMLgenerStartCableProtection (char * filename,char * types); 
int XMLgenerNewOTDRData (char * filename,char * types); 

char *base64_encode(const char* data, int data_len); 
char *base64_decode(const char* data, int data_len); 
static char find_pos(char ch); 
int read_file( char *filename, int *len, char **data );

int main(int argc, char **argv)  
{  
   CURL *curl;  
   CURLM *multi_handle; 
   FILE* fptr;  
   int still_running;    
   struct curl_httppost *formpost=NULL;  
   struct curl_httppost *lastptr=NULL;  
   struct curl_slist    *headerlist=NULL; 
   if (argc !=2)
   {
	fprintf(stderr,"usage:%s PossessNum(1|2|3|4)\n",*argv);
        return -1;
   } 
   int aa =atoi( argv[1]);
   switch(aa)
     {
	   case 1: XMLgenerLineFaultWarming(RTUSENDFILE,"LineFaultWarming")  ;break;
	   case 2: XMLgenerOpticPowerWarming(RTUSENDFILE,"OpticPowerWarming");break;
	   case 3: XMLgenerStartCableProtection(RTUSENDFILE,"StartCableProtection");break;
	   case 4: XMLgenerNewOTDRData(RTUSENDFILE,"NewOTDRData");break;
	   default: printf("Input segment fault!\n");return -1;
     }

   curl_formadd(&formpost,  
                &lastptr,  
                CURLFORM_COPYNAME,"sendfile",  
                CURLFORM_FILE,"send.xml",  
                CURLFORM_END);   

    if ((fptr = fopen(BACKFILE,"w")) == NULL)  
    {  
        fprintf(stderr,"fopen file error:%s\n",BACKFILE);  
        return -1;  
    } 
    curl = curl_easy_init();  
    multi_handle = curl_multi_init();  
    //headerlist = curl_slist_append(headerlist, "Content-type:text/xml"); 
    headerlist = curl_slist_append(headerlist, "Expect:");
    if(curl && multi_handle) {     
    curl_easy_setopt(curl, CURLOPT_URL, TOMCAT_SERVER);                         // what URL that receives this POST
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
  }  
  return 0;  
} 

size_t write_data(void* buffer,size_t size,size_t nmemb,void *stream)  
	{  
	    FILE *fptr = (FILE*)stream;  
	    fwrite(buffer,size,nmemb,fptr);  
	    return size*nmemb;  
	} 
int XMLgenerLineFaultWarming(char * filename,char *types)
{
    FILE * fd;
    char * sumStr;
    sumStr  =(char *)malloc(sizeof(char)*1024*8);
    strcat(sumStr,"<?xml version='1.0' encoding='UTF-8'?>\n");
    strcat(sumStr,"<AsynPush>\n");
    strcat(sumStr,"<LineFaultWarming>\n");
    strcat(sumStr,"	<CMDcode>521</CMDcode>\n");
    strcat(sumStr,"	<R></R >\n");
    strcat(sumStr,"	<CM>3</CM>\n");
    strcat(sumStr,"	<CLP>2</CLP>\n");
    strcat(sumStr,"	<WarmingLevel>1</WarmingLevel>\n");
    strcat(sumStr,"</LineFaultWarming >\n");
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

int XMLgenerOpticPowerWarming(char * filename,char *types)
{
    FILE * fd;
    char * sumStr;
    sumStr  =(char *)malloc(sizeof(char)*1024*8);
    strcat(sumStr,"<?xml version='1.0' encoding='UTF-8'?>\n");
    strcat(sumStr,"<AsynPush>\n");
    strcat(sumStr,"<OpticPowerWarming >\n");
    strcat(sumStr,"	<CMDcode>522</CMDcode>\n");
    strcat(sumStr,"	<R></R>\n");
    strcat(sumStr,"	<CM>3</CM>\n");
    strcat(sumStr,"	<CLP>2</CLP>\n");
    strcat(sumStr,"	<WarmingLevel>1</WarmingLevel>\n");
    strcat(sumStr,"	<PowerValue>1.23</PowerValue>\n");
    strcat(sumStr,"</OpticPowerWarming  >\n");
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


int XMLgenerStartCableProtection (char * filename,char *types)
{
    FILE * fd;
    char * sumStr;
    sumStr  =(char *)malloc(sizeof(char)*1024*8);
    strcat(sumStr,"<?xml version='1.0' encoding='UTF-8'?>\n");
    strcat(sumStr,"<AsynPush>\n");
    strcat(sumStr,"<StartCableProtection>\n");
    strcat(sumStr,"	<CMDcode>522</CMDcode>\n");
    strcat(sumStr,"	<R></R>\n");
    strcat(sumStr,"	<CM>3</CM>\n");
    strcat(sumStr,"	<CLP>2</CLP>\n");
    strcat(sumStr,"	<GSN>4</GSN>\n");
    strcat(sumStr,"	<G1>3</G1>\n");
    strcat(sumStr,"	<G2>5</G2>\n");
    strcat(sumStr,"	<G3>6</G3>\n");
    strcat(sumStr,"	<G4>7</G3>\n");
    strcat(sumStr,"</StartCableProtection>\n");
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

int XMLgenerNewOTDRData (char * filename,char * types)
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
    strcat(sumStr,"<?xml version='1.0' encoding='UTF-8'?>\n");
    strcat(sumStr,"<AsynPush>\n");
    strcat(sumStr,"<NewOTDRData>\n");
    strcat(sumStr,"	<CMDcode>524</CMDcode>\n");
    strcat(sumStr,"	<R>*</R>\n");
    strcat(sumStr,"	<CM>3</CM>\n");
    strcat(sumStr,"	<CLP>2</CLP>\n");
    strcat(sumStr,"	<SNo>4</SNo>\n");
    strcat(sumStr,"	<Type>1</Type>\n");
    strcat(sumStr,"	<Data>\n");
    strcat(sumStr,"		<P11>0</P11>\n");
    strcat(sumStr,"		<P12>7</P12>\n");
    strcat(sumStr,"		<P13>7</P13>\n");
    strcat(sumStr,"		<P14>7</P14>\n");
    strcat(sumStr,"		<P15>7</P15>\n");
    strcat(sumStr,"		<P16>7</P16>\n");
    strcat(sumStr,"		<P17>7</P17>\n");
    strcat(sumStr,"		<T9>7</T9>\n");
    strcat(sumStr,"		<ExtraInformation>test extar information!</ExtraInformation>\n");
    strcat(sumStr,"		<TstDat>\n");
    strcat(sumStr,              en_tetDat);            //
    strcat(sumStr,"	        \n</TstDat>\n");
    strcat(sumStr,"	</Data>\n");
    strcat(sumStr,"</NewOTDRData>\n");
    strcat(sumStr,"</AsynPush>\n"); 

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
