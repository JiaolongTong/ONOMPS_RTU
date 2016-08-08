#include  "cycletest.h"
#include  "sql.h"
#include  "process.h"
#include   <signal.h>


cycletest * Cycle_Create()
{
	cycletest * me = (cycletest *) malloc(sizeof(cycletest));
        return me;	
}
void Cycle_Destory(cycletest *me)
{
	free(me);	
}
responed *getCycletestParameter(mxml_node_t *root,mxml_node_t *tree,cycletest *cycfpar)
{
    mxml_node_t *BX,*SNo,*CM,*CLP,*SN,*T1,*T2,*IP01,*IP02,*IP03,*IP04,*IP05,*IP06;
    uint32_t  uint_a;
    char srttmp[3]="Bx";
    char i=0;
    uint32_t  intSN = 0,rtuCM =0,rtuCLP=0;

    responed *resp;    
    resp = Responed_Create();
    resp->RespondCode=0;                                                         // default Sucessful 

    SN =mxmlFindElement(root, tree, "SN",NULL, NULL,MXML_DESCEND);
    intSN =  strtoul(SN->child->value.text.string, NULL, 0);
    if(intSN==0) 
      {
        resp->RespondCode = 1 ;                                                 // 通信参数错误（时间表、IP地址）
        resp->ErrorSN     = 1 ;
        resp->Group[0].Error_inform = "Error: SN";
        return resp;                                                            // SN 
      }
    //Responed_Destory(resp);

    cycfpar->SN = intSN;
    cycfpar->Action = -1;                                                                          //将状态设置为-1，等待插入周期测试链表
    CM =mxmlFindElement(root, tree, "CM",NULL, NULL,MXML_DESCEND);
    rtuCM =  strtoul(CM->child->value.text.string, NULL, 0);   
    CLP =mxmlFindElement(root, tree, "CLP",NULL, NULL,MXML_DESCEND);
    rtuCLP =  strtoul(CLP->child->value.text.string, NULL, 0);  
 

/*****************************解析周期测试XML指令文件*********************************/ 
    for (i=0;i<intSN;i++)
    { 
	      srttmp[1]=i+0x31;
	      BX  = mxmlFindElement(root, tree, srttmp ,NULL, NULL,MXML_DESCEND);     
 
	      SNo = mxmlFindElement(BX, tree, "SNo",NULL, NULL,MXML_DESCEND);
	      uint_a = strtoul (SNo->child->value.text.string, NULL, 0);          
	      cycfpar->Group[i].SNo = uint_a;

	      T1 = mxmlFindElement(BX, tree, "T1",NULL, NULL,MXML_DESCEND);
	      cycfpar->Group[i].beginTime = T1->child->value.text.string;  
  
	      T2 = mxmlFindElement(BX, tree, "T2",NULL, NULL,MXML_DESCEND);
	      cycfpar->Group[i].period = T2->child->value.text.string; 

              IP01 = mxmlFindElement(BX, tree, "IP01",NULL, NULL,MXML_DESCEND);
	      cycfpar->Group[i].IP01 = IP01->child->value.text.string; 

              IP02 = mxmlFindElement(BX, tree, "IP02",NULL, NULL,MXML_DESCEND);
	      cycfpar->Group[i].IP02 = IP02->child->value.text.string; 

              IP03 = mxmlFindElement(BX, tree, "IP03",NULL, NULL,MXML_DESCEND);
	      cycfpar->Group[i].IP03 = IP03->child->value.text.string; 

              IP04 = mxmlFindElement(BX, tree, "IP04",NULL, NULL,MXML_DESCEND);
	      cycfpar->Group[i].IP04 = IP04->child->value.text.string; 

               IP05 = mxmlFindElement(BX, tree, "IP05",NULL, NULL,MXML_DESCEND);
	      cycfpar->Group[i].IP05 = IP05->child->value.text.string; 

              IP06 = mxmlFindElement(BX, tree, "IP06",NULL, NULL,MXML_DESCEND);
	      cycfpar->Group[i].IP06 = IP06->child->value.text.string; 

              if(cycfpar->Group[i].SNo == 0) break; 
    }
/*****************************数据库同步检查*********************************/  
 cycletest *cycTemp;
 sqlite3 *mydb;
 char *zErrMsg = 0;
 int rc;
 sql *mysql;
 char **result = NULL;
 char *s;
 s = (char *) malloc(sizeof(char)*10);
 cycTemp = Cycle_Create();
 mysql = SQL_Create();
 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
 if( rc != SQLITE_OK ){
	      printf( "Open SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	   }
 mysql->db =mydb;
 mysql->tableName    = "DefaultTsetSegmentTable";
 int flag=-1;
 int j=0,ErrorSNo =0;
 intSN=i;
     for(i=0,j=0;i<intSN;i++)
          {                   
	   uint32tostring(cycfpar->Group[i].SNo,s);
	   mysql->mainKeyValue =  s;
           mysql->filedsName   = "SNo";
           flag = SQL_existIN_db(mysql);                                              // 检查SNo对应的测试参数是否与本地数据库同步
	   if(flag==0){
	       printf("This is NULL:SNo=%d",cycfpar->Group[i].SNo);                   // 若不同步 把不存在的光路号记录下来.ErrorSN++  , resp->Group[0].SNo=cycfpar->Group[i].SNo
               resp->RespondCode = 14 ;                                               // 通信参数错误（时间表、IP地址）
               resp->Group[ErrorSNo].SNo = cycfpar->Group[i].SNo;
               resp->Group[ErrorSNo].Error_inform = "Error: Sqlite don't match\n";
               ErrorSNo++;          
	    }else{                                                                    // 若同步i>=j


		   cycTemp->Group[j].SNo   = cycfpar->Group[i].SNo ;
                   cycTemp->Group[j].beginTime  = cycfpar->Group[i].beginTime;
                   cycTemp->Group[j].period =cycfpar->Group[i].period ;
                   cycTemp->Group[j].IP01 = cycfpar->Group[i].IP01;
                   cycTemp->Group[j].IP02 = cycfpar->Group[i].IP02;
                   cycTemp->Group[j].IP03 = cycfpar->Group[i].IP03;
                   cycTemp->Group[j].IP04 = cycfpar->Group[i].IP04;
                   cycTemp->Group[j].IP05 = cycfpar->Group[i].IP05;
                   cycTemp->Group[j].IP06 = cycfpar->Group[i].IP06;                  //(1)更新数据库中存在的光路时间表

		   mysql->filedsName   = "P01";                                      //(2)更新查询数据库中存在的测试参数
		   rc= SQL_lookup(mysql,&result);
		   if( rc != SQLITE_OK ){
		      printf( "Lookup SQL error: %s\n", zErrMsg);
		      sqlite3_free(zErrMsg);
		   }else{
		   cycTemp->Group[j].paramter.MeasureLength_m =atoi(result[0]);
                   printf(" %s",result[0]);
                   printf(" %d",cycTemp->Group[j].paramter.MeasureLength_m);
		   }
	 
		   mysql->filedsName   = "P02";
		   rc= SQL_lookup(mysql,&result);
		   if( rc != SQLITE_OK ){
		      printf( "Lookup SQL error: %s\n", zErrMsg);
		      sqlite3_free(zErrMsg);
		   }else{
		   cycTemp->Group[j].paramter.PulseWidth_ns=atoi(result[0]);
                   printf(" %s",result[0]);
                   printf(" %d",cycTemp->Group[j].paramter.PulseWidth_ns);
		   }

		   mysql->filedsName   = "P03";
		   rc= SQL_lookup(mysql,&result);
		   if( rc != SQLITE_OK ){
		      printf( "Lookup SQL error: %s\n", zErrMsg);
		      sqlite3_free(zErrMsg);
		   }else{
		   cycTemp->Group[j].paramter.Lambda_nm  = atoi(result[0]);
                   printf(" %s",result[0]);
                   printf(" %d",cycTemp->Group[j].paramter.Lambda_nm);
		   }

		   mysql->filedsName   = "P04";
		   rc= SQL_lookup(mysql,&result);
		   if( rc != SQLITE_OK ){
		      printf( "Lookup SQL error: %s\n", zErrMsg);
		      sqlite3_free(zErrMsg);
		   }else{
		   cycTemp->Group[j].paramter.MeasureTime_ms=atoi(result[0]);
                   printf(" %s",result[0]);
                   printf(" %d",cycTemp->Group[j].paramter.MeasureTime_ms);
		   }

		   mysql->filedsName   = "P05";
		   rc= SQL_lookup(mysql,&result);
		   if( rc != SQLITE_OK ){
		      printf( "Lookup SQL error: %s\n", zErrMsg);
		      sqlite3_free(zErrMsg);
		   }else{
		   cycTemp->Group[j].paramter.n=atof(result[0]);
                   printf(" %s",result[0]);
                   printf(" %f", cycTemp->Group[j].paramter.n);
		   }

		   mysql->filedsName   = "P06";
		   rc= SQL_lookup(mysql,&result);
		   if( rc != SQLITE_OK ){
		      printf( "Lookup SQL error: %s\n", zErrMsg);
		      sqlite3_free(zErrMsg);
		   }else{
		   cycTemp->Group[j].paramter.NonRelectThreshold =atof(result[0]);
                   printf(" %s",result[0]);
                   printf(" %f",cycTemp->Group[j].paramter.NonRelectThreshold);
		   }

		   mysql->filedsName   = "P07";
		   rc= SQL_lookup(mysql,&result);
		   if( rc != SQLITE_OK ){
		      printf( "Lookup SQL error: %s\n", zErrMsg);
		      sqlite3_free(zErrMsg);
		   }else{
		   cycTemp->Group[j].paramter.EndThreshold=atof(result[0]);
                   printf(" %s",result[0]);
                   printf(" %f",cycTemp->Group[j].paramter.EndThreshold);
		   }
                   printf("\n");
                   j++;
           }
	  
   }

/***************************存储已同步光路的时间表到数据库**************************/
//对测试参数已经存在的光路更新其时间表
    intSN=j;                            //正确的光路条数
    cycTemp->SN=  cycfpar->SN;  
    cycTemp->Action = cycfpar->Action;                                               
    char * str;
    int PID =100;
    str   = (char *) malloc(sizeof(char)*200);
    mysql->tableName   = "CycleTestSegnemtTable"; 
   
    for(i=0 ;i<intSN;i++)
            {                
                 
                           /*SNo CM CLP SN bg   pd  IP1  IP2  IP3  IP4  IP5  IP6  Ac PID*/
		 sprintf(str,"%d,%d,%d,%d,'%s','%s','%s','%s','%s','%s','%s','%s',%d,%d\n",cycTemp->Group[i].SNo
		                                                          ,rtuCM
		                                                          ,rtuCLP
		                                                          ,cycTemp->SN
		                                                          ,cycTemp->Group[i].beginTime
		                                                          ,cycTemp->Group[i].period
		                                                          ,cycTemp->Group[i].IP01
		                                                          ,cycTemp->Group[i].IP02
		                                                          ,cycTemp->Group[i].IP03
		                                                          ,cycTemp->Group[i].IP04
		                                                          ,cycTemp->Group[i].IP05
                                                                          ,cycTemp->Group[i].IP06
		                                                          ,cycTemp->Action
                                                                          ,PID);
		  mysql->filedsValue =  str;

                  if(!semaphore_p())  
                            exit(EXIT_FAILURE);                                //P
		  rc = SQL_add(mysql);                                         //更新或者插入新的纪录
		  if( rc != SQLITE_OK ){
		      printf( "Save SQL error\n");
		      sqlite3_free(zErrMsg);
		  }else
                  printf("%s",str);
                  if(!semaphore_v())                                           //V
                           exit(EXIT_FAILURE);

    }

    SQL_Destory(mysql);  
    sqlite3_close(mydb);
/********************************打印已同步的光路*******************************************/
    for(i=0 ;i<intSN;i++)
    { 
		printf("------B%c--------\n"       ,i+0x31);
		printf("B%c-Action-uint -[%d]\n"   ,i+0x31,cycTemp->Action);
		printf("B%c-SNo-uint -[%d]\n"      ,i+0x31,cycTemp->Group[i].SNo);
		printf("B%c-beginTime-uint -[%s]\n",i+0x31,cycTemp->Group[i].beginTime);
		printf("B%c-period-uint -[%s]\n"   ,i+0x31,cycTemp->Group[i].period);
		printf("B%c-IP01-uint -[%s]\n"     ,i+0x31,cycTemp->Group[i].IP01);
		printf("B%c-IP02-uint -[%s]\n"     ,i+0x31,cycTemp->Group[i].IP02);
		printf("B%c-IP03-uint -[%s]\n"     ,i+0x31,cycTemp->Group[i].IP03);
		printf("B%c-IP04-uint -[%s]\n"     ,i+0x31,cycTemp->Group[i].IP04);
		printf("B%c-IP05-uint -[%s]\n"     ,i+0x31,cycTemp->Group[i].IP05);
		printf("B%c-IP06-uint -[%s]\n"     ,i+0x31,cycTemp->Group[i].IP06);

		printf("B%c-P01-uint -[%d]\n"      ,i+0x31,cycTemp->Group[i].paramter.Lambda_nm);
		printf("B%c-P02-uint -[%d]\n"      ,i+0x31,cycTemp->Group[i].paramter.MeasureLength_m);
		printf("B%c-P03-uint -[%d]\n"      ,i+0x31,cycTemp->Group[i].paramter.PulseWidth_ns);
		printf("B%c-P04-uint -[%d]\n"      ,i+0x31,cycTemp->Group[i].paramter.MeasureTime_ms);
		printf("B%c-P05-float-[%f]\n"      ,i+0x31,cycTemp->Group[i].paramter.n);
		printf("B%c-P06-float-[%f]\n"      ,i+0x31,cycTemp->Group[i].paramter.EndThreshold);
		printf("B%c-P07-float-[%f]\n"      ,i+0x31,cycTemp->Group[i].paramter.NonRelectThreshold);

		printf("\n");
    }

    if(resp->RespondCode != 0 ){
         resp->ErrorSN     =  ErrorSNo;                  //错误光路总条数
         Cycle_Destory(cycTemp);   
         return  resp;                                   //保存所有正确的光路，一旦有错误就拒绝执行周期测试启动，并返回错误的光路号
        }

       Cycle_Destory(cycTemp);
 

/***************************向周期测试守护进程发送启动周期测试信号**********************************
(1)向周期测试守护进程发送加入新节点信号   
(2)注意一定要将发送程序和接收程序划到一个用户组，并且都具有root权限，否则信号发射会失败
(3)程可以通过sigqueue函数向包括它本身在内的其他进程发送一个信号,
   如果程序没有发送这个信号的权限，对sigqueue函数的调用就将失败,
   而失败的常见原因是目标进程由另一个用户所拥有.
(4)现阶段使用的是不可靠信号,不支持排队，信号可能丢失。
***********************************************************************************************/
       int signum;
       union sigval mysigval;
       char* process;  
       int ret = 0;  
       int n;  
       pid_t cycPID[MAX_PID_NUM];  
      
       process ="/web/cgi-bin/cycMain";                        
       ret = get_pid_by_name(process, cycPID, MAX_PID_NUM);  
       printf("process '%s' is existed? (%d): %c\n", process, ret, (ret > 0)?'y':'n');  
       signum=SIGUSR1;                                         //设置信号值:插入或修改周期测试链表节点值
       mysigval.sival_int = 120;                               //设置信号的附加信息 (启动周期测试)                               
       for(n=0;n<ret;n++){  
        printf("cycPID:%u\n", cycPID[n]);                      //获取周期测试守护进程PID
        if(sigqueue(cycPID[n],signum,mysigval)==-1)
               printf("send signal error\n");
      }  


/******************************等待启动周期测试成功信号**************************************/

    char * recvStr;  
    recvStr = (char *) malloc (sizeof (char)*10);

    recvStr = recvMessageQueue_C();
   if(strncmp(recvStr, "120-OK", 6) == 0)                 //遇"120-OK"结束
     printf("SetCycleSegment sucessful!\n");
   else
     printf("SetCycleSegment failed!\n");
     free(recvStr);

   return resp;
}




responed *setCycletestSegment(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)
{
      cycletest *cy_p;  
      responed *ret=NULL;
      cy_p = Cycle_Create();  

     mxml_node_t *node,*perCMDcode;
     perCMDcode = mxmlFindElement(cmd, tree, "CMDcode",NULL, NULL,MXML_DESCEND);
      if(atoi(perCMDcode->child->value.text.string) !=cmdCode) 
       {
            printf("<RespondCode>3</RespondCode>\n");
	    printf("<Data>CMDcode Error [ %s:%s]</Data>\n",perCMDcode->value.element.name,perCMDcode->child->value.text.string);
            return (responed *)-1;
            
       }
      else
      {
            ret=getCycletestParameter(cmd,tree,cy_p);
            Cycle_Destory(cy_p);
            return ret;
      }	
} 


responed *endCycletestSegment(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)
{
      cycletest *cy_p;  

      cy_p = Cycle_Create();  
      responed *ret=NULL;
      mxml_node_t *node,*perCMDcode;
      perCMDcode = mxmlFindElement(cmd, tree, "CMDcode",NULL, NULL,MXML_DESCEND);
      if(atoi(perCMDcode->child->value.text.string) !=cmdCode) 
       {
            printf("<RespondCode>3</RespondCode>\n");
	    printf("<Data>CMDcode Error [ %s:%s]</Data>\n",perCMDcode->value.element.name,perCMDcode->child->value.text.string);
            return (responed *)-1;
            
       }
      else
      {
            ret= endCycle(cmd,tree,cy_p);
            Cycle_Destory(cy_p);
            return ret;
      }	
         
} 


responed *endCycle(mxml_node_t *root,mxml_node_t *tree,cycletest *cycfpar)
{

    mxml_node_t *EX,*CM,*CLP,*SN;
    uint32_t  uint_a;
    char srttmp[3]="Ex";
    int i=0;
    uint32_t  intSN = 0,rtuCM =0,rtuCLP=0;

    responed *resp;    
    resp = Responed_Create();
    resp->RespondCode=0;                                                         // default Sucessful 

    SN =mxmlFindElement(root, tree, "SN",NULL, NULL,MXML_DESCEND);
    intSN =  strtoul(SN->child->value.text.string, NULL, 0);
    cycfpar->SN = intSN;
    cycfpar->Action = -2;                                                                          //将状态设置为-2，等待从周期测试链表中删除
    CM =mxmlFindElement(root, tree, "CM",NULL, NULL,MXML_DESCEND);
    rtuCM =  strtoul(CM->child->value.text.string, NULL, 0);   
    CLP =mxmlFindElement(root, tree, "CLP",NULL, NULL,MXML_DESCEND);
    rtuCLP =  strtoul(CLP->child->value.text.string, NULL, 0);    
    for (i=0;i<intSN;i++)
    { 
	      srttmp[1]=i+0x31;
	      EX  = mxmlFindElement(root, tree, srttmp ,NULL, NULL,MXML_DESCEND);     
	      uint_a = strtoul (EX->child->value.text.string, NULL, 0);          
	      cycfpar->Group[i].SNo = uint_a;
              if(cycfpar->Group[i].SNo == 0) break; 
    }
   intSN=i;

/*************************************************************************/

 sqlite3 *mydb;
 char *zErrMsg = 0;
 int rc;
 sql *mysql;
 char result[100];char *s;
 s = (char *) malloc(sizeof(char)*10);
 mysql = SQL_Create();
 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
 if( rc != SQLITE_OK ){
	      printf( "Lookup SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	   }
 mysql->db =mydb;
 mysql->tableName   = "CycleTestSegnemtTable"; 
 mysql->filedsValue  =  "-2";                                    // 更新光路状态为“-2” 待启动   itoa(cycfpar->Action)
 mysql->filedsName   =  "Status";

   for(i=0;i<intSN;i++)
   {
         uint32tostring(cycfpar->Group[i].SNo,s);
         mysql->mainKeyValue = s;   
         if(!semaphore_p())  
              exit(EXIT_FAILURE);                                //P
         rc=SQL_modify(mysql);
         if( rc != SQLITE_OK ){
	     printf( "Modify SQL error\n");
	     sqlite3_free(zErrMsg);
	  }
          if(!semaphore_v())                                     //V
               exit(EXIT_FAILURE);
	printf("The SNo = %d is canceled! Action:%d \n",cycfpar->Group[i].SNo,cycfpar->Action);

   }
    free(s);
    SQL_Destory(mysql);  
    sqlite3_close(mydb);
/*******Send Signal to cycMain*******
(1)向周期测试守护进程发送取消周期测试信号
(2)注意一定要将发送程序和接收程序划到一个用户组，并且都具有root权限，否则信号发射会失败(BoaCgi 4777)
(3)程可以通过sigqueue函数向包括它本身在内的其他进程发送一个信号，如果程序没有发送这个信号的权限，对sigqueue函数的调用就将失败，而失败的常见原因是目标进程由另一个用户所拥有
(4)现阶段使用的是不可靠信号,不支持排队，信号可能丢失。
************************************/
       int   signum;
       union sigval mysigval;
       char* process;  
       int ret = 0;  
       int n;  
       pid_t cycPID[MAX_PID_NUM];  
      
       process ="/web/cgi-bin/cycMain";                        
       ret = get_pid_by_name(process, cycPID, MAX_PID_NUM);  
       printf("process '%s' is existed? (%d): %c\n", process, ret, (ret > 0)?'y':'n');  
       signum=SIGUSR1;                                         //设置信号值:插入或修改或取消周期测试链表节点值
       mysigval.sival_int = 220;                               //设置信号的附加信息 (取消周期测试)                               
       for(n=0;n<ret;n++){  
        printf("cycPID:%u\n", cycPID[n]);                      //获取周期测试守护进程PID
        if(sigqueue(cycPID[n],signum,mysigval)==-1)
               printf("send signal error\n");
      }  

/*************************************************************/
       char * recvStr;
       recvStr = (char *) malloc (sizeof (char)*10);
       recvStr = recvMessageQueue_C();
       if(strncmp(recvStr, "220-OK", 6) == 0){                 //遇"220-OK"结束
           printf("Cancel cycletest sucessful!\n");
           }
           else{
                printf("Cancel cycletest failed!\n");
               }

       free(recvStr);

       return resp;

}

