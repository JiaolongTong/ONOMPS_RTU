#include "opticalprotect.h"
#include "process.h"
#include "sql.h"
opticalprotect * OpticalProtect_Create(){
    opticalprotect * me = (opticalprotect *) malloc(sizeof(opticalprotect));
    return me;
}
void OpticalProtect_Destory(opticalprotect *me){
    free(me);
}


void getOpticalProtectParameter(mxml_node_t *root,mxml_node_t *tree,opticalprotect *protectmpar)
{
	    mxml_node_t *PX,*PN,*PNo,*SNoA,*SNoB,*SwitchPos;
	    uint32_t  uint_a;
            int       intPN,i;
            char      strPX[3]="Px";
            PN =mxmlFindElement(root, tree, "PN",NULL, NULL,MXML_DESCEND);
            intPN =  strtoul(PN->child->value.text.string, NULL, 0);
            protectmpar->PN =intPN;
            protectmpar->Action = 1;
            for (i=0;i<intPN;i++)
            { 
		      strPX[1]=i+0x31;
		      PX     = mxmlFindElement(root, tree, strPX ,NULL, NULL,MXML_DESCEND); 
                      PNo    = mxmlFindElement(PX,tree,"PNo",NULL, NULL,MXML_DESCEND); 
                      uint_a =  strtoul(PNo->child->value.text.string, NULL, 0);  
                      protectmpar->Group[i].PNo=uint_a;
                      SNoA  = mxmlFindElement(PX,tree,"SNoA",NULL, NULL,MXML_DESCEND); 
                      uint_a =  strtoul(SNoA->child->value.text.string, NULL, 0);  
                      protectmpar->Group[i].SNoA=uint_a;
                      SNoB  = mxmlFindElement(PX,tree, "SNoB",NULL,NULL,MXML_DESCEND); 
                      uint_a =  strtoul(SNoB->child->value.text.string, NULL, 0);  
                      protectmpar->Group[i].SNoB=uint_a;   
                      SwitchPos  = mxmlFindElement(PX,tree, "SwitchPos",NULL,NULL,MXML_DESCEND); 
                      uint_a =  strtoul(SwitchPos->child->value.text.string, NULL, 0);  
                      protectmpar->Group[i].SwitchPos=uint_a;   
                            
            }
}

responed *  setOpticalProtectSegment(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)     //170
{	
   mxml_node_t *CM,*CLP,*perCMDcode;
   opticalprotect *protectmpar,*protecTemp;
   responed *resp; 
   char * strRes;
   int intCM,intCLP;
   int PS,SNo; 
   protectmpar = OpticalProtect_Create();   
   protecTemp  = OpticalProtect_Create();
   resp   = Responed_Create();
   resp->RespondCode=0;      
   perCMDcode = mxmlFindElement(cmd, tree, "CMDcode",NULL, NULL,MXML_DESCEND);
      if(atoi(perCMDcode->child->value.text.string) !=cmdCode) {
            printf("<RespondCode>3</RespondCode>\n");
	    printf("<Data>CMDcode Error [ %s:%s]</Data>\n",perCMDcode->value.element.name,perCMDcode->child->value.text.string);
            return (responed *)-1;    
       }
      else{
/**************************解析XML消息***************************************/
        getOpticalProtectParameter(cmd,tree,protectmpar);
	CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	intCM = strtoul (CM->child->value.text.string, NULL, 0); 
	CLP = mxmlFindElement(cmd, tree, "CLP",NULL, NULL,MXML_DESCEND);
	intCLP = strtoul (CLP->child->value.text.string, NULL, 0); 	
        int i;
        int PN;
        PN=protectmpar->PN;
        for (i=0;i<PN;i++){
             printf("----------Px:%d------------\n",protectmpar->Group[i].PNo);              
             printf("    SNoA:%d------SNoB:%d \n",protectmpar->Group[i].SNoB,protectmpar->Group[i].SNoB);      
        } 
	sqlite3 *mydb;
	int rc;
	sql *mysql;
        char * strSQL;
        strSQL   = (char *) malloc(sizeof(char)*400);
	mysql = SQL_Create();
	rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
        if( rc != SQLITE_OK ){
	   printf( "Open SQL error\n");
	}
        mysql->db =mydb;
/**************************数据库唯一性检查********************************/                    
        char *valueA,*valueB;
        strRes =(char *) malloc(sizeof(char)*100);
        int flagA=-1,flagB=-1,flagC=-1;
        int j=0,ErrorSNo =0;
        valueA = (char *) malloc(sizeof(char)*10);
        valueB = (char *) malloc(sizeof(char)*10);
        char resultPNo[64][5];                                                     // 注意考虑光路扩展
        int  resPN;
        mysql->tableName   = "ProtectGroupTable"; 
        for(i=0;i<PN;i++)
        {                   
		   uint32tostring(protectmpar->Group[i].SNoA,valueA);
		   mysql->filedsName   = "SNoA";
		   mysql->filedsValue  =  valueA;
		   flagA = SQL_Unique(mysql);                                              // 检查SNoA在配对表中是否已经存在
		   if(flagA){
		       resPN=SQL_findPNo(mysql,resultPNo);
		       for(j=0;j<resPN;j++){
		          mysql->mainKeyValue = resultPNo[j];
		          SQL_delete(mysql);
		       }
		       
		   }

		uint32tostring(protectmpar->Group[i].SNoB,valueB);
		mysql->filedsName   = "SNoB";
		mysql->filedsValue  =  valueB;
		flagB = SQL_Unique(mysql);                                                 // 检查SNoB在配对表中是否已经存在
		if(flagB){
		    resPN=SQL_findPNo(mysql,resultPNo);
		    for(j=0;j<resPN;j++){
		       mysql->mainKeyValue = resultPNo[j];
		       SQL_delete(mysql);
		    }      
		}   
      }
      free(valueA);
      free(valueB);
/************************数据库存储***************************************/	
        for(i=0;i<PN;i++){                                                   
                     /*PNo,rtuCM,rtuCLP,SNoA,SNoB,Status*/
	    sprintf(strSQL,"%d,%d,%d,%d,%d,%d,%d\n"
                          ,protectmpar->Group[i].PNo                      //PNo
		          ,intCM                                          //rtuCM
		          ,intCLP                                         //rtuCLP
		          ,protectmpar->Group[i].SNoA                     //SNoA
		          ,protectmpar->Group[i].SNoB                     //SNoB
                          ,protectmpar->Action                            //Status
                          ,protectmpar->Group[i].SwitchPos                //SwitchPos

                          );
	    mysql->filedsValue = strSQL;
            if(!semaphore_p())  
                exit(EXIT_FAILURE);                                       //P
            rc = SQL_add(mysql);                                          //更新或者插入新的纪录
            if( rc != SQLITE_OK ){
	        printf( "Save SQL error\n");
            }else
                printf("%s",strSQL);
            if(!semaphore_v())                                            //V
                exit(EXIT_FAILURE);
       }
       free(strSQL);
       OpticalProtect_Destory(protectmpar);
       SQL_Destory(mysql);  
       sqlite3_close(mydb);
   }  
   return resp;
}

void getProtectSwitchParameter(mxml_node_t *root,mxml_node_t *tree,opticalprotect *protectmpar){
	    mxml_node_t *GX,*PN,*PNo,*SNoA,*SNoB,*SwitchPos;

	    uint32_t  uint_a;
            int       intPN,i;
            char      strGX[3]="Gx";
            PN =mxmlFindElement(root, tree, "PN",NULL, NULL,MXML_DESCEND);
            intPN =  strtoul(PN->child->value.text.string, NULL, 0);
            protectmpar->PN =intPN;
            protectmpar->Action = -1;
            for (i=0;i<intPN;i++)
            { 
		 strGX[1]=i+0x31;
		 GX     = mxmlFindElement(root, tree, strGX ,NULL,NULL,MXML_DESCEND); 
                 PNo    = mxmlFindElement(GX, tree,"PNo" ,NULL, NULL,MXML_DESCEND); 
                 uint_a =  strtoul(PNo->child->value.text.string,NULL,0);  
                 protectmpar->Group[i].PNo=uint_a;
                 SNoA   = mxmlFindElement(GX, tree,"SNoA" ,NULL, NULL,MXML_DESCEND); 
                 uint_a =  strtoul(SNoA->child->value.text.string,NULL,0);  
                 protectmpar->Group[i].SNoA=uint_a;
                 SNoB   = mxmlFindElement(GX, tree,"SNoB" ,NULL, NULL,MXML_DESCEND); 
                 uint_a =  strtoul(SNoB->child->value.text.string,NULL,0);  
                 protectmpar->Group[i].SNoB=uint_a;  
                 SwitchPos   = mxmlFindElement(GX, tree,"SwitchPos" ,NULL, NULL,MXML_DESCEND); 
                 uint_a =  strtoul(SwitchPos->child->value.text.string,NULL,0);  
                 protectmpar->Group[i].SwitchPos=uint_a;

                          
            }

}

int ifProtectMatch(int SNoA,int SNoB,sql *mysql)
{
           char *s;
           char **result = NULL;
           int flagA=-1,flagB=-1;
           int rc,dbSNoA,dbSNoB;
	   mysql->filedsName   =  "SNoA";
	   rc= SQL_lookup(mysql,&result);
	   if( rc != SQLITE_OK ){
		      printf( "Lookup SQL error: SNoA\n");
           }else{
		   dbSNoA=atoi(result[0]);
                   printf("PNo=%s,SNoA=%d\n",mysql->mainKeyValue,dbSNoA);
	   }

	   mysql->filedsName   =  "SNoB";
	   rc= SQL_lookup(mysql,&result);
	   if( rc != SQLITE_OK ){
		      printf( "Lookup SQL error: SNoB\n");
           }else{
		   dbSNoB=atoi(result[0]);
                   printf("PNo=%s,SNoB=%d\n",mysql->mainKeyValue,dbSNoB);
	   }

           if(dbSNoA==SNoA && dbSNoB==SNoB)
                 return 1;                           //match
           else if (dbSNoB==SNoA && dbSNoA==SNoB)
                 return 1;                           //match
           else
                 return 0;                           //don't match          
}

responed *  requestProtectSwitch(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)   //370
{              
   mxml_node_t    *CM,*perCMDcode;
   opticalprotect *protectmpar;
   responed *resp; 
   int intCM;
   int ErrorSNo=0;     
   protectmpar = OpticalProtect_Create();   
   resp   = Responed_Create();
   resp->RespondCode=0;  
  
   perCMDcode = mxmlFindElement(cmd, tree, "CMDcode",NULL, NULL,MXML_DESCEND);
      if(atoi(perCMDcode->child->value.text.string) !=cmdCode) {
	      resp->RespondCode=3;
	      resp->Group[0].Main_inform  = "[指令号错误]";
	      resp->Group[0].Error_inform = "Error:CMDcode Error[指令号错误]";
	      return resp;  
      }else{
/**************************解析XML消息***************************************/
            getProtectSwitchParameter(cmd,tree,protectmpar);
	    CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	    intCM = strtoul (CM->child->value.text.string, NULL, 0); 	
            int i;
            int PN;
            PN=protectmpar->PN;
            printf("PN=%d\n",protectmpar->PN); 
            for (i=0;i<PN;i++){
                    printf("----------requstPx:%d------------\n",protectmpar->Group[i].PNo);              
                    printf("    SNoA:%d------SNoB:%d -------SwitchPos:%d\n",protectmpar->Group[i].SNoA,protectmpar->Group[i].SNoB,protectmpar->Group[i].SwitchPos);      
            }  
/**************************数据库与指令一致性检查***************************/
	 sqlite3 *mydb;
	 int rc,flagMatch=-1;
         int ErrorSNo=0,SwitchStatus=0;
	 sql *mysql;
         char *value; 
         char **result;
	 value = (char *) malloc(sizeof(char)*10);
	 mysql = SQL_Create();
	 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
	 if( rc != SQLITE_OK ){
	     printf( "Lookup SQL error\n");
	 }
	 mysql->db =mydb;
	 mysql->tableName    =  "ProtectGroupTable"; 
         for (i=0;i<PN;i++){
            uint32tostring(protectmpar->Group[i].PNo,value);
            mysql->mainKeyValue= value;
            flagMatch=ifProtectMatch(protectmpar->Group[i].SNoA,protectmpar->Group[i].SNoB,mysql); 
            mysql->filedsName   =  "Status";
            rc= SQL_lookup(mysql,&result);
	    if( rc != SQLITE_OK ){
		printf( "Lookup SQL error.\n");
	    }else{
		SwitchStatus =atoi(result[0]);
	    } 
            if((flagMatch==1) && (SwitchStatus==1)){
                 mysql->filedsValue  =  "0";                           // 更新光路状态为“0” 待启动   
		 if(!semaphore_p())  
		    exit(EXIT_FAILURE);                                //P
		 rc=SQL_modify(mysql);
	         if( rc != SQLITE_OK ){
	            printf( "Modify SQL error\n");
		  }
		 if(!semaphore_v())                                    //V
	            exit(EXIT_FAILURE); 
            }else{

                  resp->RespondCode = 14 ;                             // 参数错误
                  resp->SNorPN      = TYPE_PNo;
                  resp->Group[ErrorSNo].PNo = protectmpar->Group[i].PNo;
                  if(SwitchStatus==2){
                     resp->Group[ErrorSNo].Main_inform ="[光保护切换：保护组不存在]";
                     resp->Group[ErrorSNo].Error_inform = "Error: Sqlite don't match -->PNo don't exist [Status=2][光保护切换：保护组不存在].\n";
                     ErrorSNo++;
                  }
                  if(flagMatch!=1){
                     resp->Group[ErrorSNo].Main_inform ="[光保护切换：光路号指令与数据库不匹配]";
                     resp->Group[ErrorSNo].Error_inform = "Error: Sqlite don't match -->command and database not same[光保护切换：光路号指令与数据库不匹配].\n";
                     ErrorSNo++;
                  }
            }
               
        }
	free(value);    
	SQL_Destory(mysql);  
	sqlite3_close(mydb);
	if(resp->RespondCode != 0 ){
	   resp->ErrorSN     =  ErrorSNo;                  //错误光路总条数
	}
        OpticalProtect_Destory(protectmpar);
/***************************障碍告警测试进程发送信号**********************
(1)向障碍告警测试守护进程发送取消障碍告警测试信号
(2)注意一定要将发送程序和接收程序划到一个用户组，并且都具有root权限，否则信号发射会失败(BoaCgi 4777)
(3)程可以通过sigqueue函数向包括它本身在内的其他进程发送一个信号，如果程序没有发送这个信号的权限，对sigqueue函数的调用就将失败，而失败的常见原因是目标进程由另一个用户所拥有
(4)现阶段使用的是不可靠信号,不支持排队，信号可能丢失。
***********************************************************************/
       int   signum;
       union sigval mysigval;
       char* process;  
       int ret = 0;  
       int n;  
       pid_t cycPID[MAX_PID_NUM];  
      
       process ="/web/cgi-bin/alarmMain";                        
       ret = get_pid_by_name(process, cycPID, MAX_PID_NUM);  
       printf("process '%s' is existed? (%d): %c\n", process, ret, (ret > 0)?'y':'n');  
       signum=SIGUSR1;                                         //设置信号值:插入或修改或取消周期测试链表节点值
       mysigval.sival_int = 370;                               //设置信号的附加信息 (光保护配对)                               
       for(n=0;n<ret;n++){  
        printf("alarmPID:%u\n", cycPID[n]);                    //获取障碍告警测试进程PID
        if(sigqueue(cycPID[n],signum,mysigval)==-1)
               printf("send signal error\n");
      }  
/*************************等待取消成功********************************/
       char * recvStr;
       recvStr = (char *) malloc (sizeof (char)*10);
       recvStr = recvMessageQueue_C();
       if(strncmp(recvStr, "370-OK", 6) == 0){                 //遇"230-OK"结束
           printf("Cancel alarmtest sucessful!\n");
           }
           else{
                printf("Cancel alarmtest failed!\n");
               }
         free(recvStr);
       }  
   return resp;
}

responed *  endOpticalProtectSegment(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)          //250
{
   mxml_node_t  *PN,*CM,*CLP,*QX,*perCMDcode;
   responed *resp; 
   int rtuCM,rtuCLP,intPN,i,uint_a;
   char strQX[3]="QX";

   opticalprotect * optGropEndpar;
   optGropEndpar=OpticalProtect_Create();
   resp   = Responed_Create();
   resp->RespondCode=0;  
      
   perCMDcode = mxmlFindElement(cmd, tree, "CMDcode",NULL, NULL,MXML_DESCEND);
   if(atoi(perCMDcode->child->value.text.string) !=cmdCode) {
            printf("<RespondCode>3</RespondCode>\n");
	    printf("<Data>CMDcode Error [ %s:%s]</Data>\n",perCMDcode->value.element.name,perCMDcode->child->value.text.string);
            return (responed *)-1;    
   }
   else{
/**************************解析XML消息***************************************/
	    PN =mxmlFindElement(cmd, tree, "PN",NULL, NULL,MXML_DESCEND);
	    intPN =  strtoul(PN->child->value.text.string, NULL, 0);
	    optGropEndpar->PN = intPN;
	    optGropEndpar->Action = -2;                                                                          //将状态设置为-2，等待从取障碍告警测试中删除
	    CM =mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	    rtuCM =  strtoul(CM->child->value.text.string, NULL, 0);   
	    CLP =mxmlFindElement(cmd, tree, "CLP",NULL, NULL,MXML_DESCEND);
	    rtuCLP =  strtoul(CLP->child->value.text.string, NULL, 0);    
	    for (i=0;i<intPN;i++)
	    { 
		strQX[1]=i+0x31;
		QX  = mxmlFindElement(cmd, tree, strQX ,NULL, NULL,MXML_DESCEND);     
		uint_a = strtoul (QX->child->value.text.string, NULL, 0);          
		optGropEndpar->Group[i].PNo = uint_a;
		if(optGropEndpar->Group[i].PNo == 0) break; 
	    }
/*****************************修改光路状态**********************************/
	 sqlite3 *mydb;
	 char *zErrMsg = 0;
	 int rc,flag=-1,intStatus;
         int ErrorSNo=0;
	 sql *mysql;
	 char **result;char *value;
	 value = (char *) malloc(sizeof(char)*10);
	 mysql = SQL_Create();
	 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
	 if( rc != SQLITE_OK ){
		      printf( "Lookup SQL error\n");
	 }
	 mysql->db =mydb;
	 mysql->tableName    =  "ProtectGroupTable"; 
	 mysql->filedsName   =  "Status";
	 for(i=0;i<intPN;i++)
	 {     
		 uint32tostring(optGropEndpar->Group[i].PNo,value);
		 mysql->mainKeyValue = value;  
                 flag=SQL_existIN_db(mysql);
                 rc= SQL_lookup(mysql,&result);
		 if( rc != SQLITE_OK ){
		      printf( "Lookup SQL error.\n");
		 }else{
		   intStatus =atoi(result[0]);
		 }

                 if((flag==1) && (intStatus==1)){
	                 mysql->filedsValue  =  "-2";                            // 更新光路状态为“-2” 待启动   
			 if(!semaphore_p())  
			      exit(EXIT_FAILURE);                                //P
			 rc=SQL_modify(mysql);
			 if( rc != SQLITE_OK ){
			     printf( "Modify SQL error\n");
			     sqlite3_free(zErrMsg);
			  }
			  if(!semaphore_v())                                     //V
			       exit(EXIT_FAILURE);
			
                 }else{
                                                                                   // 若不同步 把不存在的号记录下来
                         resp->RespondCode = 14 ;                                                         // 参数错误
                         resp->SNorPN      = TYPE_PNo;
                         resp->Group[ErrorSNo].PNo = optGropEndpar->Group[i].PNo;
                         if(intStatus!=1)
                           resp->Group[ErrorSNo].Error_inform = "Error: Sqlite don't match -->ready Cancel [Status!=1]\n";
                         if(flag!=1)
                           resp->Group[ErrorSNo].Error_inform = "Error: Sqlite don't match -->lack of Group\n";
                         ErrorSNo++;  
                 }
	   }
	    free(value);    
	    SQL_Destory(mysql);  
	    sqlite3_close(mydb);
	    if(resp->RespondCode != 0 ){
		 resp->ErrorSN     =  ErrorSNo;                  //错误光路总条数
	    }
            OpticalProtect_Destory(optGropEndpar);
/***************************障碍告警测试进程发送信号**********************
	(1)向障碍告警测试守护进程发送取消障碍告警测试信号
	(2)注意一定要将发送程序和接收程序划到一个用户组，并且都具有root权限，否则信号发射会失败(BoaCgi 4777)
	(3)程可以通过sigqueue函数向包括它本身在内的其他进程发送一个信号，如果程序没有发送这个信号的权限，对sigqueue函数的调用就将失败，而失败的常见原因是目标进程由另一个用户所拥有
	(4)现阶段使用的是不可靠信号,不支持排队，信号可能丢失。
***********************************************************************/
	       int   signum;
	       union sigval mysigval;
	       char* process;  
	       int ret = 0;  
	       int n;  
	       pid_t cycPID[MAX_PID_NUM];       
	       process ="/web/cgi-bin/alarmMain";                        
	       ret = get_pid_by_name(process, cycPID, MAX_PID_NUM);  
	       printf("process '%s' is existed? (%d): %c\n", process, ret, (ret > 0)?'y':'n');  
	       signum=SIGUSR1;                                         //设置信号值:插入或修改或取消周期测试链表节点值
	       mysigval.sival_int = 250;                               //设置信号的附加信息 (光保护配对)                               
	       for(n=0;n<ret;n++){  
		printf("alarmPID:%u\n", cycPID[n]);                    //获取障碍告警测试进程PID
		if(sigqueue(cycPID[n],signum,mysigval)==-1)
		       printf("send signal error\n");
	      }  
/*************************等待取消成功********************************/
	       char * recvStr;
	       recvStr = (char *) malloc (sizeof (char)*10);
	       recvStr = recvMessageQueue_C();
	       if(strncmp(recvStr, "250-OK", 6) == 0){                 //遇"250-OK"结束
		   printf("Cancel alarmtest sucessful!\n");
		   }
		   else{
		        printf("Cancel alarmtest failed!\n");
		       }
	       free(recvStr);
     }
   return resp;
}

