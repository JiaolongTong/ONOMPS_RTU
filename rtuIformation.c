#include "rtuIformation.h"
#include "sql.h"
#include "process.h"
#include "checkip.h"

rtuInform * RTU_Create()
{
    rtuInform * me = (rtuInform *) malloc(sizeof(rtuInform));
    return me;
}
void RTU_Destory(rtuInform *me)
{
    free(me);
}



responed * setReferenceTime(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)
{
   mxml_node_t    *CM,*T8,*perCMDcode;
   rtuInform *rtuReferenceTime;
   responed *resp; 
   int intCM;
  
   rtuReferenceTime = RTU_Create();   
   resp   = Responed_Create();
   resp  -> RespondCode=0;  
      
   perCMDcode = mxmlFindElement(cmd, tree, "CMDcode",NULL, NULL,MXML_DESCEND);
      if(atoi(perCMDcode->child->value.text.string) !=cmdCode) {
            printf("<RespondCode>3</RespondCode>\n");
	    printf("<Data>CMDcode Error [ %s:%s]</Data>\n",perCMDcode->value.element.name,perCMDcode->child->value.text.string);
            resp->RespondCode=-1;
            return resp;   
       }
      else{
/**************************解析XML消息***************************************/
	    CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	    rtuReferenceTime->rtuCM = strtoul(CM->child->value.text.string, NULL, 0); 	
	    T8 = mxmlFindElement(cmd, tree, "T8",NULL, NULL,MXML_DESCEND);
	    rtuReferenceTime->rtuRefenceTime= strtoul(T8->child->value.text.string, NULL, 0); 	  
            printf("------setAlarm------\n");    
            printf("rtuCM=%d\n",rtuReferenceTime->rtuCM); 
            printf("StampTime=%d\n",rtuReferenceTime->rtuRefenceTime);
            time_t rawtime =(time_t)rtuReferenceTime->rtuRefenceTime;
            rtc_set_time("/dev/rtc0",rawtime);
            char * timE =ctime(&rawtime);
            printf("System Time:%s\n",timE);
       }  
   RTU_Destory(rtuReferenceTime);
   return resp;
}

responed * requestReferenceTime(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)
{
   mxml_node_t    *CM,*T8,*perCMDcode;
   rtuInform *rtuReferenceTime;
   responed *resp; 
   int intCM;
  
   rtuReferenceTime = RTU_Create();   
   resp   = Responed_Create();
   resp  -> RespondCode=0;  
      
   perCMDcode = mxmlFindElement(cmd, tree, "CMDcode",NULL, NULL,MXML_DESCEND);
      if(atoi(perCMDcode->child->value.text.string) !=cmdCode) {
            printf("<RespondCode>3</RespondCode>\n");
	    printf("<Data>CMDcode Error [ %s:%s]</Data>\n",perCMDcode->value.element.name,perCMDcode->child->value.text.string);
            resp->RespondCode=-1;
            return resp;   
       }
      else{
/**************************解析XML消息***************************************/
	    CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	    rtuReferenceTime->rtuCM = strtoul(CM->child->value.text.string, NULL, 0); 	
	    T8 = mxmlFindElement(cmd, tree, "T8",NULL, NULL,MXML_DESCEND);
	    rtuReferenceTime->rtuRefenceTime= strtoul(T8->child->value.text.string, NULL, 0); 	  
            time_t rawtime = getLocalTimestamp() ;
            char * timE =ctime(&rawtime);
 	    printf("<RespondCode>11</RespondCode>\n");
            printf("<Data>\n");
            printf("<ReferenceTime>\n");
            printf("<CMDcode>514</CMDcode>\n");
            printf("	<R>*</R>\n");
            printf("	<CM>%d</CM>\n",rtuReferenceTime->rtuCM);
            printf("	<T8>%ld</T8>\n" ,rawtime);
            printf("</ReferenceTime>\n");
            printf("</Data>\n");
            printf("System Time:%s\n",timE);
       }  
   RTU_Destory(rtuReferenceTime);
   return resp;   
}

responed * setRTUMode(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)
{
   mxml_node_t    *CM,*perCMDcode;
   rtuInform *rtuMode;
   responed *resp; 
   int intCM;
  
   rtuMode = RTU_Create();   
   resp   = Responed_Create();
   resp  -> RespondCode=0;  
      
   perCMDcode = mxmlFindElement(cmd, tree, "CMDcode",NULL, NULL,MXML_DESCEND);
      if(atoi(perCMDcode->child->value.text.string) !=cmdCode) {
            printf("<RespondCode>3</RespondCode>\n");
	    printf("<Data>CMDcode Error [ %s:%s]</Data>\n",perCMDcode->value.element.name,perCMDcode->child->value.text.string);
            resp->RespondCode=-1;
            return resp;   
       }
      else{
/**************************解析XML消息***************************************/
	    mxml_node_t *KX,*MN,*KNo,*Type;
	    int  intKNo,intType,intMN,i;
            char strKX[3]="Kx";

	 sqlite3 *mydb;
	 char *zErrMsg = 0,*SNo;
	 int rc;
	 sql *mysql;
         char resultModNo[64][5];
         int  resultModNum=0;
         int   ModNo;
         char **resultType = NULL;
         int  resultTypeNum;


	 CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	 rtuMode->rtuCM = strtoul(CM->child->value.text.string, NULL, 0);     //RTU局站号

         MN =mxmlFindElement(cmd, tree, "MN",NULL, NULL,MXML_DESCEND);
         intMN =  strtoul(MN->child->value.text.string, NULL, 0);
         rtuMode->sumModule =intMN;                                            //RTU中子单元数量


 
 
	 mysql = SQL_Create();
	 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
	 if( rc != SQLITE_OK ){
		      printf( "Lookup SQL error: %s\n", zErrMsg);
		      sqlite3_free(zErrMsg);
		   }
	 mysql->db = mydb;
	 mysql->tableName   = "SubModuleTypeTable";	
         mysql->filedsValue =  "1";                                
         mysql->filedsName  =  "UseFlag";
        
         resultModNum=SQL_findModNo(mysql,resultModNo);
  
         for(i=0;i<MAXSUBMODULE;i++) rtuMode->rtuMode[i] =0;

         for(i=0;i<resultModNum;i++)
         {
             mysql->mainKeyValue = resultModNo[i];
             mysql->filedsValue =  "1";                                
             mysql->filedsName  =  "ModuleType";
             SQL_lookupPar(mysql,&resultType,&resultTypeNum);

             rtuMode->rtuMode[atoi(resultModNo[i])-1]=atoi(resultType[0]);                                         //获取本地已有模块信息(需要查询数据库) 
            
             SQL_freeResult(&resultType,&resultTypeNum);          
         }
       
        for(i=0;i<MAXSUBMODULE;i++)
             printf("已有模块:%d 类型:%d\n",i+1,rtuMode->rtuMode[i]);

            for (i=0;i<intMN;i++)                                                //
            { 
		strKX[1]=i+0x31;
		KX     = mxmlFindElement(cmd, tree, strKX ,NULL, NULL,MXML_DESCEND); 
                KNo    = mxmlFindElement(KX,tree,"KNo",NULL, NULL,MXML_DESCEND); 
                intKNo =  strtoul(KNo->child->value.text.string, NULL, 0);  

                Type  = mxmlFindElement(KX,tree,"Type",NULL, NULL,MXML_DESCEND); 
                intType =  strtoul(Type->child->value.text.string, NULL, 0);  

                 rtuMode->rtuMode[intKNo-1]=intType;
                            
            }


            printf("--------RTU局站号:%d-------\n",rtuMode->rtuCM);
            for(i=0;i<MAXSUBMODULE;i++)
            {
                  printf("新增/修改 模块:%d   类型:%d\n",i+1,rtuMode->rtuMode[i]);  
            }
/**************************************************************************/
    int UseFlag=1;
    char * str;
    str   = (char *) malloc(sizeof(char)*200);
    for(i=0 ;i<MAXSUBMODULE;i++)
    {                
          if(rtuMode->rtuMode[i] !=0){       
                           /*SNo CM CLP SN bg   pd  IP1  IP2  IP3  IP4  IP5  IP6  Ac PID*/
		 sprintf(str,"%d,%d,%d,%d,'%d','%d'\n",i+1                           //模块物理编号
		                                      ,rtuMode->rtuCM
		                                      ,rtuMode->rtuCLP
		                                      ,rtuMode->rtuMode[i]           //模块类型
		                                      ,i+1                           //模块通信地址
		                                      ,UseFlag
                                                       );
		  mysql->filedsValue =  str;

                  if(!semaphore_p())  
                            exit(EXIT_FAILURE);                                //P
		  rc = SQL_add(mysql);                                         //更新或者插入新的纪录
		  if( rc != SQLITE_OK ){
		      printf( "Save SQL error\n");
		  }else
                  printf("%s",str);
                  if(!semaphore_v())                                           //V
                           exit(EXIT_FAILURE);
          }
     }

    SQL_Destory(mysql);  
    sqlite3_close(mydb);
 }  
 RTU_Destory(rtuMode);
 
 return resp; 
}


responed * cancelRTUMode(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)
{
   mxml_node_t    *CM,*T8,*perCMDcode;
   rtuInform *rtuReferenceTime;
   responed *resp; 
   int intCM;
  
   rtuReferenceTime = RTU_Create();   
   resp   = Responed_Create();
   resp  -> RespondCode=0;  
      
   perCMDcode = mxmlFindElement(cmd, tree, "CMDcode",NULL, NULL,MXML_DESCEND);
   if(atoi(perCMDcode->child->value.text.string) !=cmdCode) {
            printf("<RespondCode>3</RespondCode>\n");
	    printf("<Data>CMDcode Error [ %s:%s]</Data>\n",perCMDcode->value.element.name,perCMDcode->child->value.text.string);
            resp->RespondCode=-1;
            return resp;   
   }
   else{
/**************************解析XML消息***************************************/
	    CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	    intCM = strtoul(CM->child->value.text.string, NULL, 0); 	

/***************************向后台发送取消请求*******************************/

/*周期测试守护进程*/
       int signum;
       union sigval mysigval;
       char* process;  
       int ret = 0;  
       int n;  
       pid_t cycPID[MAX_PID_NUM];  
       char * recvStr; 
       recvStr = (char *) malloc (sizeof (char)*10);

       process ="/web/cgi-bin/cycMain";                        
       ret = get_pid_by_name(process, cycPID, MAX_PID_NUM);  
       printf("process '%s' is existed? (%d): %c\n", process, ret, (ret > 0)?'y':'n');  
       signum=SIGUSR1;                                         //设置信号值:插入或修改周期测试链表节点值
       mysigval.sival_int = 260;                               //设置信号的附加信息 (取消RTU模式)                               
       for(n=0;n<ret;n++){  
        printf("cycPID:%u\n", cycPID[n]);                      //获取周期测试守护进程PID
        if(sigqueue(cycPID[n],signum,mysigval)==-1)
               printf("send signal error\n");
        } 
               /*等待周期测试守护进程取消RTU模式成功信号*/
 

      recvStr = recvMessageQueue_C();
      if(strncmp(recvStr, "260-OK", 6) != 0){                 //遇"260-OK"结束
          printf("CancelRTUMode failed!\n");
          printf("<RespondCode>3</RespondCode>\n");
          resp->RespondCode=-1;
       return resp;
       }


/*障碍告警测试守护进程*/
       process ="/web/cgi-bin/alarmMain";                        
       ret = get_pid_by_name(process, cycPID, MAX_PID_NUM);  
       printf("process '%s' is existed? (%d): %c\n", process, ret, (ret > 0)?'y':'n');  
       signum=SIGUSR1;                                         //设置信号值:插入或修改周期测试链表节点值
       mysigval.sival_int = 260;                               //设置信号的附加信息 (取消RTU模式)                               
       for(n=0;n<ret;n++){  
        printf("cycPID:%u\n", cycPID[n]);                      //获取周期测试守护进程PID
        if(sigqueue(cycPID[n],signum,mysigval)==-1)
               printf("send signal error\n");
       } 

       recvStr = recvMessageQueue_C();
       if(strncmp(recvStr, "260-OK", 6) != 0){                 //遇"260-OK"结束
          printf("CancelRTUMode failed!\n");
          printf("<RespondCode>3</RespondCode>\n");
          resp->RespondCode=-1;
       return resp;
       }


      free(recvStr);


/***********修改数据库标识/清空数据库*******************/

	 sqlite3 *mydb;
	 char *zErrMsg = 0,*SNo;
	 int rc,i=0;
	 sql *mysql;
         char resultModNo[64][5];
         int  resultModNum=0;

	 mysql = SQL_Create();
	 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
	 if( rc != SQLITE_OK )
            printf( "Lookup SQL error: %s\n");

	 mysql->db = mydb;
	 mysql->tableName   = "SubModuleTypeTable";	
         mysql->filedsValue =  "1";                                
         mysql->filedsName  =  "UseFlag";
        
         resultModNum=SQL_findModNo(mysql,resultModNo);
 
         mysql->filedsValue =  "0";
         if(!semaphore_p())
             exit(EXIT_FAILURE);                                //P


         for(i=0;i<resultModNum;i++){
            mysql->mainKeyValue  = resultModNo[i];
            SQL_modify(mysql);
         }


	mysql->tableName="DefaultTsetSegmentTable";
	SQL_clearTable(mysql);


	mysql->tableName="NamedTestSegmentTable";
	SQL_clearTable(mysql);


	mysql->tableName="CycleTestSegnemtTable";
	SQL_clearTable(mysql);


	mysql->tableName="AlarmTestSegmentTable";
	SQL_clearTable(mysql);


	mysql->tableName="PortOccopyTable";
	SQL_clearTable(mysql);


	mysql->tableName="ProtectGroupTable";
	SQL_clearTable(mysql);


         if(!semaphore_v())  
            exit(EXIT_FAILURE);                                //V

   }  

      printf("CancelRTUMode Sucessful!\n");

   return resp;   
}

responed * setRTUPort(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)
{
   mxml_node_t    *CM,*perCMDcode;
   rtuInform *rtuMode;
   responed *resp; 
   int intCM;
  
   rtuMode = RTU_Create();   
   resp   = Responed_Create();
   resp  -> RespondCode=0;  
      
   perCMDcode = mxmlFindElement(cmd, tree, "CMDcode",NULL, NULL,MXML_DESCEND);
      if(atoi(perCMDcode->child->value.text.string) !=cmdCode) {
            printf("<RespondCode>3</RespondCode>\n");
	    printf("<Data>CMDcode Error [ %s:%s]</Data>\n",perCMDcode->value.element.name,perCMDcode->child->value.text.string);
            resp->RespondCode=-1;
            return resp;   
       }
      else{
/**************************解析XML消息***************************************/

         mxml_node_t *PX,*PN,*SNo,*Type;
	 int  intSNo,intPN,intType,i=0;
         char strPX[3]="Px";

         for(i=0;i<MAXRTUPORT;i++) rtuMode->rtuPort[i] =0;

	 CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	 rtuMode->rtuCM = strtoul(CM->child->value.text.string, NULL, 0);     //RTU局站号

         PN =mxmlFindElement(cmd, tree, "PN",NULL, NULL,MXML_DESCEND);
         intPN =  strtoul(PN->child->value.text.string, NULL, 0);
         rtuMode->sumPort =intPN;                                            //RTU中Port数量

            printf("hello!\n");

         for (i=0;i<intPN;i++)                                               
         { 
  	        strPX[1]=i+0x31;
		PX     = mxmlFindElement(cmd, tree, strPX ,NULL, NULL,MXML_DESCEND); 
                SNo    = mxmlFindElement(PX,tree,"SNo",NULL, NULL,MXML_DESCEND); 
                intSNo =  strtoul(SNo->child->value.text.string, NULL, 0);  

                Type  = mxmlFindElement(PX,tree,"Type",NULL, NULL,MXML_DESCEND); 
                intType =  strtoul(Type->child->value.text.string, NULL, 0);  

                rtuMode->rtuPort[intSNo-1]=intType+1;                          
         }

            printf("--------RTU局站号:%d-------\n",rtuMode->rtuCM);
            for(i=0;i<MAXRTUPORT;i++)
            {
               if(rtuMode->rtuPort[i] !=0)
                  printf("新增Port:%d   类型:%d\n",i+1,rtuMode->rtuPort[i]);  
            }
/**************************************************************************/
	 sqlite3 *mydb;
	 int rc;
	 sql *mysql;
         char * str;
         str   = (char *) malloc(sizeof(char)*200);

	 mysql = SQL_Create();
	 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
	 if( rc != SQLITE_OK )
	    printf( "Lookup SQL error!\n");		  
	 mysql->db = mydb;
	 mysql->tableName   = "PortOccopyTable";	
         for(i=0 ;i<MAXRTUPORT;i++)
         {                
          if(rtuMode->rtuPort[i] !=0){       
                           /*SNo CM CLP SN bg   pd  IP1  IP2  IP3  IP4  IP5  IP6  Ac PID*/
		 sprintf(str,"%d,%d,%d,%d\n",i+1                           //Port物理编号
		                            ,(i/8)+1                       //Port 模块
		                            ,(i%8)+1                       //Port on模块
		                            ,rtuMode->rtuPort[i]           //模块类型
                                            );
		  mysql->filedsValue =  str;

                  if(!semaphore_p())  
                       exit(EXIT_FAILURE);                                  //P
		  rc = SQL_add(mysql);                                      //更新或者插入新的纪录
		  if( rc != SQLITE_OK ){
		      printf( "Save SQL error\n");
		  }else
                      printf("%s",str);
                  if(!semaphore_v())                                        //V
                        exit(EXIT_FAILURE);
          }
       }

    SQL_Destory(mysql);  
    sqlite3_close(mydb);
 }  
 RTU_Destory(rtuMode);
 
 return resp; 



}
responed * cancelRTUPort(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)
{
   mxml_node_t    *CM,*perCMDcode;
   rtuInform *rtuMode;
   responed *resp; 
   int intCM;
  
   rtuMode = RTU_Create();   
   resp   = Responed_Create();
   resp  -> RespondCode=0;  
      
   perCMDcode = mxmlFindElement(cmd, tree, "CMDcode",NULL, NULL,MXML_DESCEND);
      if(atoi(perCMDcode->child->value.text.string) !=cmdCode) {
            printf("<RespondCode>3</RespondCode>\n");
	    printf("<Data>CMDcode Error [ %s:%s]</Data>\n",perCMDcode->value.element.name,perCMDcode->child->value.text.string);
            resp->RespondCode=-1;
            return resp;   
       }
      else{
/**************************解析XML消息***************************************/

         mxml_node_t *PX,*PN,*SNo,*Type;
	 int  intSNo,intPN,intType,i=0;
         char strPX[3]="Px";

         for(i=0;i<MAXRTUPORT;i++) rtuMode->rtuPort[i] =0;

	 CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	 rtuMode->rtuCM = strtoul(CM->child->value.text.string, NULL, 0);     //RTU局站号

         PN =mxmlFindElement(cmd, tree, "PN",NULL, NULL,MXML_DESCEND);
         intPN =  strtoul(PN->child->value.text.string, NULL, 0);
         rtuMode->sumPort =intPN;                                            //RTU中Port数量

            printf("hello!\n");

         for (i=0;i<intPN;i++)                                               
         { 
  	        strPX[1]=i+0x31;
		PX     = mxmlFindElement(cmd, tree, strPX ,NULL, NULL,MXML_DESCEND); 
                SNo    = mxmlFindElement(PX,tree,"SNo",NULL, NULL,MXML_DESCEND); 
                intSNo =  strtoul(SNo->child->value.text.string, NULL, 0);  

                rtuMode->rtuPort[intSNo-1]=1;                          
         }

            printf("--------RTU局站号:%d-------\n",rtuMode->rtuCM);
            for(i=0;i<MAXRTUPORT;i++)
            {
                if(rtuMode->rtuPort[i] ==1)
                    printf("删除Port:%d\n",i+1,rtuMode->rtuPort[i]);  
            }
/***************************删除测试节点***************************************/
/*向后台进程发送相应节点信息，删除相应的测试节点*/


       int signum;
       union sigval mysigval;
       char* process;  
       int ret = 0;  
       int n;  
       pid_t cycPID[MAX_PID_NUM];  
       char * recvStr; 
       recvStr = (char *) malloc (sizeof (char)*10);


       for(i=0;i<MAXRTUPORT;i++)
       {
           if(rtuMode->rtuPort[i]==1){
/*障碍告警测试 */
	       process ="/web/cgi-bin/alarmMain";                        
	       ret = get_pid_by_name(process, cycPID, MAX_PID_NUM);  
	       printf("alarmMain:process '%s' is existed? (%d): %c\n", process, ret, (ret > 0)?'y':'n');  
	       signum=SIGUSR1;                                         //设置信号值:插入或修改周期测试链表节点值
	       mysigval.sival_int = 270+i+1;                           //设置信号的附加信息 (取消RTU模式)                               
	       for(n=0;n<ret;n++){  
		printf("cycPID:%u\n", cycPID[n]);                      //获取周期测试守护进程PID
		if(sigqueue(cycPID[n],signum,mysigval)==-1)
		       printf("alarmMain:send signal error\n");
	       } 

	       recvStr = recvMessageQueue_C();
	       if(strncmp(recvStr, "270-OK", 6) != 0){                 //遇"270-OK"结束
		  printf("alarmMain: CancelRTUPort failed!:%s\n",recvStr);
		  printf("<RespondCode>3</RespondCode>\n");
		  resp->RespondCode=-1;
	       return resp;
	       }
/*周期测试*/
	       process ="/web/cgi-bin/cycMain";  
	       ret = get_pid_by_name(process, cycPID, MAX_PID_NUM);  
	       printf("cycMain:process '%s' is existed? (%d): %c\n", process, ret, (ret > 0)?'y':'n');  
	       signum=SIGUSR1;                                         //设置信号值:插入或修改周期测试链表节点值
	       mysigval.sival_int = 270+i+1;                           //设置信号的附加信息 (取消RTU模式)                               
	       for(n=0;n<ret;n++){  
		printf("cycPID:%u\n", cycPID[n]);                      //获取周期测试守护进程PID
		if(sigqueue(cycPID[n],signum,mysigval)==-1)
		       printf("cycMain: send signal error\n");
	       } 

	       recvStr = recvMessageQueue_C();
	       if(strncmp(recvStr, "270-OK", 6) != 0){                 //遇"270-OK"结束
		  printf("cycMain: CancelRTUPort failed!\n");
		  printf("<RespondCode>3</RespondCode>\n");
		  resp->RespondCode=-1;
	       return resp;
	       }


           }
           usleep(1000);
       }



/****************************删除数据库记录***************************************/
	 sqlite3 *mydb;
	 int rc;
	 sql *mysql;
         char * str;
         str   = (char *) malloc(sizeof(char)*10);

	 mysql = SQL_Create();
	 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
	 if( rc != SQLITE_OK )
	    printf( "Lookup SQL error!\n");		  
	 mysql->db = mydb;
	
         for(i=0 ;i<MAXRTUPORT;i++)
         {                
          if(rtuMode->rtuPort[i] ==1){  
               if(!semaphore_p())  
                    exit(EXIT_FAILURE);                                  //P
               uint32tostring(i+1,str);   
               mysql->mainKeyValue =  str;

  	       mysql->tableName   = "PortOccopyTable";
               rc = SQL_delete(mysql);                                   //删除该端口对应的端口占用表中相应的记录

  	       mysql->tableName   = "DefaultTsetSegmentTable";
               rc = SQL_delete(mysql);                                   //删除该端口对应的优化参数表的记录

  	       mysql->tableName   = "NamedTestSegmentTable";
               rc = SQL_delete(mysql);                                   //删除该端口对应的点名测试表的记录

  	       mysql->tableName   = "AlarmTestSegmentTable";
               rc = SQL_delete(mysql);                                   //删除该端口对应的障碍告警测试表的记录

  	       mysql->tableName   = "CycleTestSegnemtTable";             //删除该端口对应的周期测试表的记录   
               rc = SQL_delete(mysql);   

  	       mysql->tableName   = "ProtectGroupTable";                 //删除该端口对应的配对组表的记录
               if((i+1)%2==0)
                  uint32tostring((i+1)/2,str);
               else
                  uint32tostring((i+1+1)/2,str);   
               mysql->mainKeyValue =  str;               
               rc = SQL_delete(mysql);   

	       if( rc != SQLITE_OK )
		     printf( "Delete SQL error\n");
	       else
                     printf("%s",str);
               if(!semaphore_v())                                        //V
                    exit(EXIT_FAILURE);
          }
       }

    SQL_Destory(mysql);  
    sqlite3_close(mydb);
 }  
 RTU_Destory(rtuMode);
 
 return resp; 

}

responed * setNetwork(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)
{
   mxml_node_t    *CM,*CLP,*IP,*Netmask,*Gateway,*perCMDcode;
   rtuInform *rtuReferenceTime;
   responed *resp; 
  
   rtuReferenceTime = RTU_Create();   
   resp   = Responed_Create();
   resp  -> RespondCode=0;  
      
   perCMDcode = mxmlFindElement(cmd, tree, "CMDcode",NULL, NULL,MXML_DESCEND);
      if(atoi(perCMDcode->child->value.text.string) !=cmdCode) {
            printf("<RespondCode>3</RespondCode>\n");
	    printf("<Data>CMDcode Error [ %s:%s]</Data>\n",perCMDcode->value.element.name,perCMDcode->child->value.text.string);
            resp->RespondCode=-1;
            RTU_Destory(rtuReferenceTime);
            return resp;   
       }
      else{
/**************************解析XML消息***************************************/
	    CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	    rtuReferenceTime->rtuCM = strtoul(CM->child->value.text.string, NULL, 0); 	
	    CLP = mxmlFindElement(cmd, tree, "CLP",NULL, NULL,MXML_DESCEND);
            rtuReferenceTime->rtuCLP= strtoul(CLP->child->value.text.string, NULL, 0);
	    IP = mxmlFindElement(cmd, tree, "IP",NULL, NULL,MXML_DESCEND);
	    Netmask = mxmlFindElement(cmd, tree,"Netmask",NULL, NULL,MXML_DESCEND);
	   // Gateway = mxmlFindElement(cmd, tree,"Gateway",NULL, NULL,MXML_DESCEND);

            int canUSE=-1;
            char *interface = "eth0";
            char * ipaddr =IP->child->value.text.string;
            canUSE=PM_Check_IP(interface,ipaddr);
            if(!canUSE){
               set_ip(IP->child->value.text.string);
               set_ip_netmask(Netmask->child->value.text.string);
               //set_gateway(Gateway->child->value.text.string);
            }
            else{

               printf("<RespondCode>3</RespondCode>\n");
	       printf("<Data> IP conflict [%s] can't use!\n</Data>\n",IP->child->value.text.string);
               resp->RespondCode=-1;
               RTU_Destory(rtuReferenceTime);
               return resp;  
             }
	
       }  
   RTU_Destory(rtuReferenceTime);
   return resp;   

}
responed * requestNetwork(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)
{
   mxml_node_t    *CM,*CLP,*perCMDcode;
   rtuInform *rtuReferenceTime;
   responed *resp; 
   char IP[16],Netmask[16];
  
   rtuReferenceTime = RTU_Create();   
   resp   = Responed_Create();
   resp  -> RespondCode=0;  
      
   perCMDcode = mxmlFindElement(cmd, tree, "CMDcode",NULL, NULL,MXML_DESCEND);
      if(atoi(perCMDcode->child->value.text.string) !=cmdCode) {
            printf("<RespondCode>3</RespondCode>\n");
	    printf("<Data>CMDcode Error [ %s:%s]</Data>\n",perCMDcode->value.element.name,perCMDcode->child->value.text.string);
            resp->RespondCode=-1;
            return resp;   
       }
      else{
/**************************解析XML消息***************************************/
	    CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	    rtuReferenceTime->rtuCM = strtoul(CM->child->value.text.string, NULL, 0); 	
	    CLP = mxmlFindElement(cmd, tree, "CLP",NULL, NULL,MXML_DESCEND);
            rtuReferenceTime->rtuCLP= strtoul(CLP->child->value.text.string, NULL, 0);

            get_ip(IP);
            get_ip_netmask(Netmask);

 	    printf("<RespondCode>12</RespondCode>\n");
            printf("<Data>\n");
            printf("<NetworkSegment>\n");
            printf("<CMDcode>513</CMDcode>\n");
            printf("	<R>*</R>\n");
            printf("	<CM>%d</CM>\n",rtuReferenceTime->rtuCM);
            printf("	<CLP>%d</CLP>\n",rtuReferenceTime->rtuCLP);
            printf("	<IP>%s</IP>\n" ,IP);
            printf("	<Netmask>%s</Netmask>\n",Netmask);
            printf("	<Gateway>192.168.0.1</Gateway>\n");
            printf("</NetworkSegment>\n");
            printf("</Data>\n");	
       }  
   RTU_Destory(rtuReferenceTime);
   return resp;   
}


