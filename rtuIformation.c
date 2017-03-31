#include "rtuIformation.h"
#include "sql.h"
#include "process.h"
#include "checkip.h"
#include "myModbus.h"

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
   mxml_node_t    *CM,*CLP,*perCMDcode;
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
            int  resultTypeNum,ErrorSNo=0;


	    CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	    rtuMode->rtuCM = strtoul(CM->child->value.text.string, NULL, 0);     //RTU局站号

	    CLP = mxmlFindElement(cmd, tree, "CLP",NULL, NULL,MXML_DESCEND);
	    rtuMode->rtuCLP = strtoul(CLP->child->value.text.string, NULL, 0);     //RTU局站号

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
	    mysql->tableName   =  "SubModuleTypeTable";	
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

		     rtuMode->rtuMode[atoi(resultModNo[i])-1]=atoi(resultType[0]);//获取本地已有模块信息(需要查询数据库) 
		    
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
            printf("--------RTU局站号:%d CLP:%d-------\n",rtuMode->rtuCM,rtuMode->rtuCLP);
            for(i=0;i<MAXSUBMODULE;i++)
            {
                  printf("新增/修改 模块:%d 类型:%d  \n",i+1,rtuMode->rtuMode[i]);  
            }
/**************************************************************************/
    int UseFlag=1,setFlag=-1;
    modbus_t * mb=NULL ;
    char  str[200];
 
    for(i=0 ;i<MAXSUBMODULE;i++)
    {

             if(rtuMode->rtuMode[i] ==1 || rtuMode->rtuMode[i] ==2 || rtuMode->rtuMode[i] ==3 || rtuMode->rtuMode[i] ==4 || rtuMode->rtuMode[i] ==5){                
		 if(!setModbus_P())                                                  //P
		     exit(EXIT_FAILURE);
                 mb =newModbus(MODBUS_DEV,MODBUS_BUAD);
                 if(rtuMode->rtuMode[i] ==5 || rtuMode->rtuMode[i] == 4) setFlag = setSubDeviceMode(mb,i+1,1); //待单片机部分修改完，删除
                 else setFlag = setSubDeviceMode(mb,i+1,rtuMode->rtuMode[i]-1);
                 freeModbus(mb);
                 if(!setModbus_V())                                                 //V
		     exit(EXIT_FAILURE);
                 usleep(100000);
                 if(0 !=setFlag)
                 {
                  	  resp->RespondCode  = 15;
			  resp->ErrorSN      = 1;                     
			  resp->SNorPN       = TYPE_SNo;
			  resp->Group[ErrorSNo].SNo = i+1;
			  resp->Group[ErrorSNo].Main_inform  = "未找到模块硬件设备";
			  resp->Group[ErrorSNo].Error_inform = "Error:Can't Find Hardware Devive!\n";
                          ErrorSNo++;
                 }else{ 
		                 /*ModuleNo rtuCM rtuCLP ModuleType ComAddr UseFlag*/
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
     }

    SQL_Destory(mysql);  
    sqlite3_close(mydb);
    if(resp->RespondCode != 0 ){
             resp->ErrorSN =  ErrorSNo;                  //错误光路总条数    
             return resp; 
    }
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
       int signum;
       union sigval mysigval;

       int ret = 0;  
       int n;  
       pid_t cycPID[MAX_PID_NUM];  
       char * recvStr; 
       recvStr = (char *) malloc (sizeof (char)*10);
/*周期测试守护进程*/

                      
       ret = get_pid_by_name("/web/cgi-bin/cycMain", cycPID, MAX_PID_NUM);
       if(ret>0){  
	       printf("process '/web/cgi-bin/cycMain' is existed? (%d): %c\n", ret, (ret > 0)?'y':'n');  
	       signum=SIGRTMIN+1;                                      //设置信号值:插入或修改周期测试链表节点值
	       mysigval.sival_int = 260;                               //设置信号的附加信息 (取消RTU模式)                               
	       for(n=0;n<ret;n++){  
		printf("cycPID:%u\n", cycPID[n]);                      //获取周期测试守护进程PID
		if(sigqueue(cycPID[n],signum,mysigval)==-1)
		       printf("send signal error\n");
		} 
		       /*等待周期测试守护进程取消RTU模式成功信号*/

	      if(recvMessageQueue_Backstage("260-OK-C",CYCLE_MESSAGE_TYPE)== 0){                  //遇"260-OK"结束
		  printf("CancelRTUMode failed!\n");
		  printf("<RespondCode>3</RespondCode>\n");
		  resp->RespondCode=-1;
	          return resp;
	      }
       }

/*障碍告警测试守护进程*/
                      
       ret = get_pid_by_name("/web/cgi-bin/alarmMain", cycPID, MAX_PID_NUM); 
       if(ret>0){ 
	       printf("process '/web/cgi-bin/alarmMain' is existed? (%d): %c\n", ret, (ret > 0)?'y':'n');  
	       signum=SIGRTMIN+1;                                         //设置信号值:插入或修改周期测试链表节点值
	       mysigval.sival_int = 260;                               //设置信号的附加信息 (取消RTU模式)                               
	       for(n=0;n<ret;n++){  
		       printf("cycPID:%u\n", cycPID[n]);                      //获取周期测试守护进程PID
		       if(sigqueue(cycPID[n],signum,mysigval)==-1)
			       printf("send signal error\n");
	       } 

	       if(recvMessageQueue_Backstage("260-OK-A",ALARM_MESSAGE_TYPE)== 0){                 //遇"260-OK"结束
		       printf("CancelRTUMode failed!\n");
		       printf("<RespondCode>3</RespondCode>\n");
		       resp->RespondCode=-1;
	               return resp;
	       }
       }
/*向主端保护守护进程*/                         
       ret = get_pid_by_name("/web/cgi-bin/ProtectMasterMain", cycPID, MAX_PID_NUM); 
       if(ret>0){ 
	       printf("process '/web/cgi-bin/ProtectMasterMain' is existed? (%d): %c\n", ret, (ret > 0)?'y':'n');  
	       signum=SIGRTMIN+1;                                         //设置信号值:插入或修改周期测试链表节点值
	       mysigval.sival_int = 260;                               //设置信号的附加信息 (取消RTU模式)                               
	       for(n=0;n<ret;n++){  
		       printf("cycPID:%u\n", cycPID[n]);                      //获取周期测试守护进程PID
		       if(sigqueue(cycPID[n],signum,mysigval)==-1)
			       printf("send signal error\n");
	       } 

	       if(recvMessageQueue_Backstage("260-OK-P",PROTECT_MESSAGE_TYPE) == 0){                 //遇"260-OK"结束
		       printf("CancelRTUMode failed!\n");
		       printf("<RespondCode>3</RespondCode>\n");
		       resp->RespondCode=-1;
	               return resp;
	       }
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
	 int  intSNo=0,intPN=0,intType=0,setRTUPort=0,i=0;

         char strPX[10],strNum[10];

         for(i=0;i<MAXRTUPORT;i++) rtuMode->rtuPort[i] =0;

	 CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	 rtuMode->rtuCM = strtoul(CM->child->value.text.string, NULL, 0);     //RTU局站号

         PN =mxmlFindElement(cmd, tree, "PN",NULL, NULL,MXML_DESCEND);
         intPN =  strtoul(PN->child->value.text.string, NULL, 0);
         rtuMode->sumPort =intPN;                                            //RTU中Port数量

         for (i=0;i<intPN;i++)                                               
         { 
  
                strPX[0]='\0';
                strcat(strPX,"P");
                uint32tostring(i+1,strNum);
                strcat(strPX,strNum);

		PX     = mxmlFindElement(cmd, tree, strPX ,NULL, NULL,MXML_DESCEND); 
                SNo    = mxmlFindElement(PX,tree,"SNo",NULL, NULL,MXML_DESCEND); 
                intSNo =  strtoul(SNo->child->value.text.string, NULL, 0);  

                Type  = mxmlFindElement(PX,tree,"Type",NULL, NULL,MXML_DESCEND); 
                intType =  strtoul(Type->child->value.text.string, NULL, 0);  

                rtuMode->rtuPort[intSNo-1]=intType+1;                          
         }

         printf("--------RTU局站号:%d-------\n",rtuMode->rtuCM);
         for(i=0;i<MAXRTUPORT;i++){
               if(rtuMode->rtuPort[i] !=0)
                  printf("新增Port:%d   类型:%d\n",i+1,rtuMode->rtuPort[i]);  
         }
/**************************************************************************/
	 sqlite3 *mydb=NULL;
	 sql *mysql=NULL;
         responed *resp=NULL;
         char strSQL[200],strNo[5];
         char ** result=NULL;
         int rc=0,ModuleNo=0,rednum=0,UseFlag=0,ErrorSNo=0,existFlag=0;
	 resp   = Responed_Create();
	 resp->RespondCode=0;  
	 mysql = SQL_Create();
	 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
	 if( rc != SQLITE_OK )
	    printf( "Lookup SQL error!\n");		  
	 mysql->db = mydb;	 	
         for(i=0 ;i<MAXRTUPORT;i++){
              if(rtuMode->rtuPort[i] !=0){
		    mysql->tableName ="SubModuleTypeTable";                 //检查子单元模块是否存在
		    ModuleNo = (i/8)+1;
		    uint32tostring(ModuleNo,strNo);
		    mysql->mainKeyValue = strNo;
                    if(!SQL_existIN_db(mysql)){
			    resp->RespondCode  = 15;
			    resp->ErrorSN      = 1;                     
			    resp->SNorPN       = TYPE_SNo;
			    resp->Group[ErrorSNo].SNo = i+1;
			    resp->Group[ErrorSNo].Main_inform  = "模块未激活";
			    resp->Group[ErrorSNo].Error_inform = "Error: Don't have such Module\n";
                            ErrorSNo++;
                    }else{
			    mysql->filedsName   = "UseFlag";
			    SQL_lookupPar(mysql,&result,&rednum);
			    UseFlag=atoi(result[0]);
			    SQL_freeResult(&result,&rednum);
			    if(UseFlag!=1){
				  resp->RespondCode  = 15;
				  resp->ErrorSN      = 1;                     
				  resp->SNorPN       = TYPE_SNo;
				  resp->Group[ErrorSNo].SNo = i+1;
				  resp->Group[ErrorSNo].Main_inform  = "模块未激活";
				  resp->Group[ErrorSNo].Error_inform = "Error: Don't have such Module\n";
		                  ErrorSNo++;
			    }else{            
				    mysql->tableName   = "PortOccopyTable";      
				    if(rtuMode->rtuPort[i] !=0){       
						   /*SNo ModleNo SubPort FiberType*/
					  sprintf(strSQL,"%d,%d,%d,%d\n",i+1                        // 物理编号(1-64)
								        ,(i/8)+1                    // 模块编号(1-8)
								        ,(i%8)+1                    // 子端口编号(1-8)
								        ,rtuMode->rtuPort[i]        // 端口类型
					  );
					  mysql->filedsValue =  strSQL;
					  if(!semaphore_p())  
					       exit(EXIT_FAILURE);                                  //P
					  rc = SQL_add(mysql);                                      //更新或者插入新的纪录
					  if( rc != SQLITE_OK ){
					      printf( "Save SQL error\n");
					  }else
					      printf("%s",strSQL);
					  if(!semaphore_v())                                        //V
						exit(EXIT_FAILURE);
				  }
			  }
                    }
               }
         }    
         SQL_Destory(mysql);  
         sqlite3_close(mydb);

         printf("Code-Res:%d",resp->RespondCode);
         if(resp->RespondCode != 0 ){
             resp->ErrorSN =  ErrorSNo;                  //错误光路总条数    
             return resp; 
         }
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
         char strPX[3],strNum[10];

         for(i=0;i<MAXRTUPORT;i++) rtuMode->rtuPort[i] =0;

	 CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	 rtuMode->rtuCM = strtoul(CM->child->value.text.string, NULL, 0);     //RTU局站号

         PN =mxmlFindElement(cmd, tree, "PN",NULL, NULL,MXML_DESCEND);
         intPN =  strtoul(PN->child->value.text.string, NULL, 0);
         rtuMode->sumPort =intPN;                                            //RTU中Port数量


         for (i=0;i<intPN;i++)                                               
         { 
  
                strPX[0]='\0';
                strcat(strPX,"P");
                uint32tostring(i+1,strNum);
                strcat(strPX,strNum);

		PX     = mxmlFindElement(cmd, tree, strPX ,NULL, NULL,MXML_DESCEND); 
                SNo    = mxmlFindElement(PX,tree,"SNo",NULL, NULL,MXML_DESCEND); 
                intSNo =  strtoul(SNo->child->value.text.string, NULL, 0);  

                rtuMode->rtuPort[intSNo-1]=1;    
                printf("delete SNo=%d \n",intSNo);                  
         }

            printf("--------RTU局站号:%d-------\n",rtuMode->rtuCM);
            for(i=0;i<MAXRTUPORT;i++)
            {
                if(rtuMode->rtuPort[i] == 1)
                    printf("删除Port:%d Type:%d\n",i+1,rtuMode->rtuPort[i]);  
            }
/***************************删除测试节点***************************************/
/*向后台进程发送相应节点信息，删除相应的测试节点*/


       int signum;
       union sigval mysigval;
       int ret = 0;  
       int n;  
       pid_t cycPID[MAX_PID_NUM];  
       char * recvStr; 
       recvStr = (char *) malloc (sizeof (char)*10);

       for(i=0;i<MAXRTUPORT;i++)
       {
           if(rtuMode->rtuPort[i]==1){
/*周期测试*/
	       ret = get_pid_by_name("/web/cgi-bin/cycMain", cycPID, MAX_PID_NUM); 
       
               if( ret >0 ){ 
		       printf("cycMain:process '/web/cgi-bin/cycMain' is existed? (%d): %c\n", ret, (ret > 0)?'y':'n');  
		       signum=SIGRTMIN+1;                                       //设置信号值:插入或修改周期测试链表节点值
		       mysigval.sival_int = 270+i+1;                           //设置信号的附加信息 (取消RTU模式)                               
		       for(n=0;n<ret;n++){  
			printf("cycPID:%u\n", cycPID[n]);                      //获取周期测试守护进程PID
			if(sigqueue(cycPID[n],signum,mysigval)==-1)
			       printf("cycMain: send signal error\n");
		       } 

                       if( recvMessageQueue_Backstage("270-OK-C",CYCLE_MESSAGE_TYPE) == 0){                 
			  printf("cycMain: CancelRTUPort failed!\n");
			  printf("<RespondCode>3</RespondCode>\n");
			  resp->RespondCode=-1;
		          return resp;
		       }		     

               }
/*障碍告警测试 */
   
	       ret = get_pid_by_name("/web/cgi-bin/alarmMain", cycPID, MAX_PID_NUM);  
               if(ret>0){
		       printf("alarmMain:process '/web/cgi-bin/alarmMain' is existed? (%d): %c\n", ret, (ret > 0)?'y':'n');  
		       signum=SIGRTMIN+1;                                      //设置信号值:插入或修改周期测试链表节点值
		       mysigval.sival_int = 270+i+1;                           //设置信号的附加信息 (取消RTU模式)                               
		       for(n=0;n<ret;n++){  
			printf("cycPID:%u\n", cycPID[n]);                      //获取周期测试守护进程PID
			if(sigqueue(cycPID[n],signum,mysigval)==-1)
			       printf("alarmMain:send signal error\n");
		       } 

                       if( recvMessageQueue_Backstage("270-OK-A",ALARM_MESSAGE_TYPE) == 0){                 
			  printf("alarmMain: CancelRTUPort failed!\n");
			  printf("<RespondCode>3</RespondCode>\n");
			  resp->RespondCode=-1;
		          return resp;
		       }
               }

/*向主端保护守护进程*/  
	       ret = get_pid_by_name("/web/cgi-bin/ProtectMasterMain", cycPID, MAX_PID_NUM); 
               if( ret >0 ){ 
		       printf("cycMain:process '/web/cgi-bin/ProtectMasterMain' is existed? (%d): %c\n", ret, (ret > 0)?'y':'n');  
		       signum=SIGRTMIN+1;                                      //设置信号值:插入或修改周期测试链表节点值
		       mysigval.sival_int = 270+i+1;                           //设置信号的附加信息 (取消RTU模式)                               
		       for(n=0;n<ret;n++){  
			printf("cycPID:%u\n", cycPID[n]);                      //获取周期测试守护进程PID
			if(sigqueue(cycPID[n],signum,mysigval)==-1)
			       printf("cycMain: send signal error\n");
		       } 

                       if( recvMessageQueue_Backstage("270-OK-P",PROTECT_MESSAGE_TYPE) == 0){                 
			  printf("ProtectMasterMain: CancelRTUPort failed!\n");
			  printf("<RespondCode>3</RespondCode>\n");
			  resp->RespondCode=-1;
		          return resp;
		       }
               }

           }
           //usleep(1000);
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

responed * requestRebootRTU(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)
{
   mxml_node_t    *CM,*CLP,*Time,*perCMDcode;
   responed *resp; 
   pid_t pid;
 
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
	    CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);	
	    CLP = mxmlFindElement(cmd, tree, "CLP",NULL, NULL,MXML_DESCEND);
	    Time = mxmlFindElement(cmd, tree, "Time",NULL, NULL,MXML_DESCEND);
     
/******************后台进程发送重启命令，并等待回复*****************/
        int signum=0,retProcess = 0,n=0;
        char* process=NULL,*recvStr=NULL;  
        pid_t otdrPID[MAX_PID_NUM];
        union sigval mysigval;  
                                     /*向otdrMain发送启动信号*/
        process ="/web/cgi-bin/otdrMain";                        
        retProcess = get_pid_by_name(process, otdrPID, MAX_PID_NUM);  
	printf("process '%s' is existed? (%d): %c ", process, retProcess, (retProcess > 0)?'y':'n');  
        if(retProcess>0){
		signum=SIGRTMIN;                                         
		mysigval.sival_int = 38;                                   //发送需要启动测试的光路号  及测试类型：百位表示类型+十位个位表示SNo   (eg:101,type=1,SNo=1)                         
		for(n=0;n<retProcess;n++){                                      
		     printf("otdrMain PID:%u\n", otdrPID[n]);             //获取OTDR测试进程PID
		     if(sigqueue(otdrPID[n],signum,mysigval)==-1)
			 printf("send signal error\n");
		}	 
		                    /*等待信号的成功处理消息*/		    
		recvStr = (char *) malloc (sizeof (char)*20);                                              
		recvStr = recvMessageQueue_Block("0-OK-38",38);                //阻塞等待测试执行完毕      
		if(strncmp(recvStr, "0-OK-38",7) == 0){                    //遇"0-OK-38"结束		      
		      printf("ReBoot RTU  sucessful!\n");
		}
		else{
		      printf("ReBoot RTU Faild\n");
		      resp->RespondCode=3;
		      resp->Group[0].Main_inform  = "重启失败-->未收到回复消息";
		      resp->Group[0].Error_inform = "Error:Don't get back massage![重启失败-->未收到回复消息]";
	              free(recvStr);
		      return resp;  
		}
                free(recvStr);
        }else{
		resp->RespondCode=3;
		resp->Group[0].Main_inform  = "重启失败-->未找到后台进程";
		resp->Group[0].Error_inform = "Error:Don't get back massage![重启失败-->未找到后台进程]";
	        free(recvStr);
		return resp;  
       }
	
   }  

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
            int canUSE=-1,fd=0;
            char *interface = "eth0";
            char str[100];
            char oldIP[16];
            char oldNetmask[16];

/**************************解析XML消息***************************************/
	    CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	    rtuReferenceTime->rtuCM = strtoul(CM->child->value.text.string, NULL, 0); 	
	    CLP = mxmlFindElement(cmd, tree, "CLP",NULL, NULL,MXML_DESCEND);
            rtuReferenceTime->rtuCLP= strtoul(CLP->child->value.text.string, NULL, 0);
	    IP = mxmlFindElement(cmd, tree, "IP",NULL, NULL,MXML_DESCEND);
	    Netmask = mxmlFindElement(cmd, tree,"Netmask",NULL, NULL,MXML_DESCEND);
            get_ip(oldIP);
            get_ip_netmask(oldNetmask);

            canUSE=PM_Check_IP(interface,IP->child->value.text.string);
            if(!canUSE){
		set_ip(IP->child->value.text.string);
		set_ip_netmask(Netmask->child->value.text.string);
/******************后台进程发送网络配置命令*****************/
		int signum=0,retProcess = 0,n=0;
		char* process=NULL,*recvStr=NULL;  
		pid_t otdrPID[MAX_PID_NUM];
		union sigval mysigval;  
		                             /*向otdrMain发送启动信号*/
		process ="/web/cgi-bin/otdrMain";                        
		retProcess = get_pid_by_name(process, otdrPID, MAX_PID_NUM);  
		printf("process '%s' is existed? (%d): %c ", process, retProcess, (retProcess > 0)?'y':'n');  
		if(retProcess>0){
			signum=SIGRTMIN;                                         
			mysigval.sival_int = 16;                                   //发送需要启动测试的光路号  及测试类型：百位表示类型+十位个位表示SNo   (eg:101,type=1,SNo=1)                         
			for(n=0;n<retProcess;n++){  
			     fd = open ("/web/cgi-bin/fiberMointor.tmp" , O_RDWR);
			     if (fd < 0)
				 perror ("Error opening file");
			     else {
		                sprintf(str,"%s\n%s",oldIP,oldNetmask);
				lseek(fd,0,SEEK_SET);
				if(write(fd,str,strlen(str))<0){
				     set_ip(oldIP);
				     set_ip_netmask(oldNetmask);
		                     printf("<RespondCode>3</RespondCode>\n");
		                     printf("<Data>Set IP Failed [%s]!\n</Data>\n",IP->child->value.text.string);
		                     resp->RespondCode=-1;
				     RTU_Destory(rtuReferenceTime);
                                     close (fd);
		                     return resp; 
				}    
			     }
			     close (fd);                                    
			     if(sigqueue(otdrPID[n],signum,mysigval)==-1)
				 printf("send signal error\n");
			}	 
		}else{
			set_ip(oldIP);
			set_ip_netmask(oldNetmask);
			resp->RespondCode=3;
			resp->Group[0].Main_inform  = "网络配置-->未找到后台进程";
			resp->Group[0].Error_inform = "Error:Don't get back massage![网络配置-->未找到后台进程]";
			free(recvStr);
			return resp;  
	       }

          }else{
               printf("<RespondCode>3</RespondCode>\n");
	       printf("<Data>Set IP Failed [%s]! Check NetWork\n</Data>\n",IP->child->value.text.string);
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


