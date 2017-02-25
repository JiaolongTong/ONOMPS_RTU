#include "Communicate.h"
const char base[]  =  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
int saveRecvXML(char *data,int len)
{
         int fd;
         fd=open(RCV_FILE,O_RDWR|O_CREAT|O_TRUNC|O_APPEND,S_IRWXU|S_IRGRP|S_IROTH);
         if(fd<0)
         { 
           return -1;
         }
         else
         {
           write(fd,data,len);   
           close(fd);
           return 0;
         }
}
char *  getCommendString(int code)
{
     char * str ;
	if(code == 100)
          return str = "SetDefaultTestSegment";
	if(code == 110)
          return str = "SetNamedTestSegment";
	if(code == 120)
          return str = "SetCycleTestSegment";
	if(code == 130)
          return str = "SetAlarmTestSegment";
	if(code == 140)
          return str = "SetAlarmSegment";
	if(code == 150)
          return str = "SetReferenceTime";
	if(code == 160)
          return str = "SetNetwork";
	if(code == 170)
          return str = "SetProtectGroup";
        if(code == 180)
          return str = "SetRTUMode";
        if(code == 190)
          return str = "SetRTUPort";
	if(code == 220)
          return str = "CancelCycleTest";
	if(code == 230)
          return str = "CancelAlarmTest";
	if(code == 240)
          return str = "CancelAlarm";
	if(code == 250)
          return str = "CancelProtectGroup";
	if(code == 260)
          return str = "CancelRTUMode";
	if(code == 270)
          return str = "CancelRTUPort";
	if(code == 300)
          return str = "RequestTestData";
        if(code == 320)
          return str = "RequestReferenceTime";
	if(code == 330)
          return str = "RequestNetwork";
	if(code == 360)
          return str = "RequestOpticalPower";
	if(code == 370)
          return str = "RequestCableProtect";
	if(code == 380)
          return str = "RequestRebootRtu";
	if(code == 390)
          return str = "RequestRTUStatus";
}
int getCommendCode(mxml_node_t * root,mxml_node_t * tree)
{
     char *commendtype;
     mxml_node_t *search;
     if ((search = mxmlFindElement(root, tree,"SetDefaultTestSegment",NULL, NULL,MXML_DESCEND))!=NULL)
        {
            if (strcmp(search->value.element.name,"SetDefaultTestSegment")==0)
		{
		   return 100;
		}    
        }

     if ((search = mxmlFindElement(root, tree, "SetNamedTestSegment",NULL, NULL,MXML_DESCEND))!=NULL)
        {
            if (strcmp(search->value.element.name,"SetNamedTestSegment")==0)
		{
		   return 110;
		}    
        }
     if ((search = mxmlFindElement(root, tree, "SetCycleTestSegment",NULL, NULL,MXML_DESCEND))!=NULL)
        {
            if (strcmp(search->value.element.name,"SetCycleTestSegment")==0)
		{
		   return 120;
		}    
        }
     if ((search = mxmlFindElement(root, tree, "SetAlarmTestSegment",NULL, NULL,MXML_DESCEND))!=NULL)
        {
            if (strcmp(search->value.element.name,"SetAlarmTestSegment")==0)
		{
		   return 130;
		}    
        }
     if ((search = mxmlFindElement(root, tree, "SetAlarmSegment",NULL, NULL,MXML_DESCEND))!=NULL)
        {
            if (strcmp(search->value.element.name,"SetAlarmSegment")==0)
		{
		   return 140;
		}    
        }
     if ((search = mxmlFindElement(root, tree, "SetReferenceTime",NULL, NULL,MXML_DESCEND))!=NULL)
        {
            if (strcmp(search->value.element.name,"SetReferenceTime")==0)
		{
		   return 150;
		}    
        }
     if ((search = mxmlFindElement(root, tree, "SetNetwork",NULL, NULL,MXML_DESCEND))!=NULL)
        {
            if (strcmp(search->value.element.name,"SetNetwork")==0)
		{
		   return 160;
		}    
        }

     if ((search = mxmlFindElement(root, tree, "SetProtectGroup",NULL, NULL,MXML_DESCEND))!=NULL)
        {
            if (strcmp(search->value.element.name,"SetProtectGroup")==0)
		{
		   return 170;
		}    
        }

     if ((search = mxmlFindElement(root, tree, "SetRTUMode",NULL, NULL,MXML_DESCEND))!=NULL)
        {
            if (strcmp(search->value.element.name,"SetRTUMode")==0)
		{
		   return 180;
		}    
        }

     if ((search = mxmlFindElement(root, tree, "SetRTUPort",NULL, NULL,MXML_DESCEND))!=NULL)
        {
            if (strcmp(search->value.element.name,"SetRTUPort")==0)
		{
		   return 190;
		}    
        }

     if ((search = mxmlFindElement(root, tree, "CancelCycleTest",NULL, NULL,MXML_DESCEND))!=NULL)
        {
            if (strcmp(search->value.element.name,"CancelCycleTest")==0)
		{
		   return 220;
		}    
        }

     if ((search = mxmlFindElement(root, tree, "CancelAlarmTest",NULL, NULL,MXML_DESCEND))!=NULL)
        {
            if (strcmp(search->value.element.name,"CancelAlarmTest")==0)
		{
		   return 230;
		}    
        }
     if ((search = mxmlFindElement(root, tree, "CancelAlarm",NULL, NULL,MXML_DESCEND))!=NULL)
        {
            if (strcmp(search->value.element.name,"CancelAlarm")==0)
		{
		   return 240;
		}    
        }
     if ((search = mxmlFindElement(root, tree, "CancelProtectGroup",NULL, NULL,MXML_DESCEND))!=NULL)
        {
            if (strcmp(search->value.element.name,"CancelProtectGroup")==0)
		{
		   return 250;
		}    
        }

     if ((search = mxmlFindElement(root, tree, "CancelRTUMode",NULL, NULL,MXML_DESCEND))!=NULL)
        {
            if (strcmp(search->value.element.name,"CancelRTUMode")==0)
		{
		   return 260;
		}    
        }


     if ((search = mxmlFindElement(root, tree, "CancelRTUPort",NULL, NULL,MXML_DESCEND))!=NULL)
        {
            if (strcmp(search->value.element.name,"CancelRTUPort")==0)
		{
		   return 270;
		}    
        }

     if ((search = mxmlFindElement(root, tree, "RequestTestData",NULL, NULL,MXML_DESCEND))!=NULL)
        {
            if (strcmp(search->value.element.name,"RequestTestData")==0)
		{
		   return 300;
		}    
        }
     if ((search = mxmlFindElement(root, tree, "RequestReferenceTime",NULL, NULL,MXML_DESCEND))!=NULL)
        {
            if (strcmp(search->value.element.name,"RequestReferenceTime")==0)
		{
		   return 320;
		}    
        }
     if ((search = mxmlFindElement(root, tree, "RequestNetwork",NULL, NULL,MXML_DESCEND))!=NULL)
        {
            if (strcmp(search->value.element.name,"RequestNetwork")==0)
		{
		   return 330;
		}    
        }
     if ((search = mxmlFindElement(root, tree, "RequestOpticalPower",NULL, NULL,MXML_DESCEND))!=NULL)
        {
            if (strcmp(search->value.element.name,"RequestOpticalPower")==0)
		{
		   return 360;
		}    
        }
     if ((search = mxmlFindElement(root, tree, "RequestCableProtect",NULL, NULL,MXML_DESCEND))!=NULL)
        {
            if (strcmp(search->value.element.name,"RequestCableProtect")==0)
		{
		   return 370;
		}    
        }
     if ((search = mxmlFindElement(root, tree, "RequestRebootRtu",NULL, NULL,MXML_DESCEND))!=NULL)
        {
            if (strcmp(search->value.element.name,"RequestRebootRtu")==0)
		{
		   return 380;

		}    
        }
     if ((search = mxmlFindElement(root, tree, "RequestRTUStatus",NULL, NULL,MXML_DESCEND))!=NULL)
        {
            if (strcmp(search->value.element.name,"RequestRTUStatus")==0)
		{
		   return 390;
		}    
        }
}

int RespondMessage_TestData(otdr *PXX,int PS,int SNo,int CM, int mode)
{
    	char   *pdata = NULL;
        char   str[20];
    
    	int    en_size=0,nret,ContentLength=0;
        time_t rawtime;
	struct tm * timeinfo;
	time (&rawtime);
        char * timE =ctime(&rawtime);		
    	if(nret = read_file( en_DATA, &en_size, &pdata )!=0)
      	{
		fprintf(stderr,"Open Error\n");
		printf("<RespondCode>1</RespondCode>\n");
		return -1;
      	}
    	if(en_size>en_MAXSIZE-1)
      	{    
		 ContentLength      =   en_MAXSIZE-1;   
		 pdata[en_MAXSIZE]  =  '\0';  
     	 } 
   	 else
      	{
		ContentLength   =  en_size;
        	pdata[ContentLength]  =  '\0';    
     	 } 
    	char *en_tetDat = base64_encode(pdata, ContentLength); 

	printf("<RespondCode>10</RespondCode>\n"     );
        printf("<Data>\n"                                );
        printf("    <TestData>\n"                        );
        printf("	<CMDcode>500</CMDcode>\n"        );
        printf("	<R>*</R>\n"                      );             
        uint32tostring(SNo, str);			printf("	<SNo>%s</SNo>\n"         ,str); 
        uint32tostring(CM, str);       			printf("	<CM>%s</CM>\n"           ,str);  
        uint32tostring(PS, str);       			printf("	<PS>%s</PS>\n"           ,str);	    
        uint32tostring(PXX->MeasureLength_m, str);	printf("	<P11>%s</P11>\n"         ,str);
        uint32tostring(PXX->PulseWidth_ns, str); 	printf("	<P12>%s</P12>\n"         ,str);
        uint32tostring(PXX->Lambda_nm, str); 		printf("	<P13>%s</P13>\n"         ,str);
        uint32tostring(PXX->MeasureTime_ms, str); 	printf("	<P14>%s</P14>\n"         ,str);	
        printf("	<P15>%f</P15>\n"        ,PXX->n);
	printf("	<P16>%f</P16>\n"        ,PXX->NonRelectThreshold);
	printf("	<P17>%f</P17>\n"        ,PXX->EndThreshold);
	printf("	<T9>%d</T9>\n"          ,rawtime);
    	printf("	<TstDat>%s\n</TstDat>\n",en_tetDat);
        printf("    </TestData>\n");		
        printf("</Data>\n");	  
    	free(en_tetDat);
    	if ( pdata != NULL )
    	{
		free( pdata);
		pdata =NULL;
    	}  
        return 0;
}

int RespondMessage_OpticPowerData(opticalpower *optPwr,int mode)
{
        time_t rawtime;
	struct tm * timeinfo;
	time (&rawtime);
        char * timE =ctime(&rawtime);
	printf("<RespondCode>10</RespondCode>\n");
        printf("<Data>\n"                                );
        printf("<OpticalPowerData>\n");
        printf("	<CMDcode>511</CMDcode>\n");
        printf("	<R>*</R>\n");
        printf("	<CM>%d</CM>\n",optPwr->CM);
        printf("	<CLP>2</CLP>\n");
        printf("	<SNo>%d</SNo>\n",optPwr->SNo);     
        printf("	<T9>%d</T9>\n"          ,rawtime);  
        printf("	<PowerValue>%f</PowerValue>\n",optPwr->value);     
        printf("</OpticalPowerData>\n");	  
        printf("</Data>\n");   
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
