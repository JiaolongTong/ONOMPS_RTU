#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <signal.h>
#include "common.h"
#include "defaulttest.h"
#include "process.h"
#include "sql.h"
#include "otdr.h"
#include "uploadCycTestData.h" 
#include "myModbus.h"

/***测试链表节点结构***/ //LinkA
/*
    SNO:光路号
    CM :局站号
    ANo:告警组号（优先级）
    PowerGate     :光功率门限，低于此门限产生告警

    fristAlarmFlag:首次告警标识：首次产生告警，置位为1，告警消失：置位为0。
    nextAlarmTime :告警未消失时，下次向服务器上传告警信息的时间
    alarmClick    :上传告警的周期
*/

typedef struct checkNode checkNode;
struct checkNode{
	int    SNo;
        int    CM;
	int    ANo;
        float  PowerGate;

        int    fristAlarmFlag;
        time_t nextAlarmTime;
        time_t alarmClick;

	struct checkNode *next;
};

/***告警链表节点***/   //LinkB
/*
    SNO:光路号
    ANo:告警组号（优先级）
    Order:节点序列号，按优先级排序依据 Order = ANo*100 + SNo
*/
typedef struct alarmNode alarmNode;
struct alarmNode{
	int    SNo;
        int    CM;
	int    ANo;
        int    Order;
	struct alarmNode *next;
};


/*全局变量*/
int sem_id=0;                      //信号量ID（数据库互斥访问）      
int modbus_sem_id=0;               //信号量ID（ModBus互斥访问）  
int otdr_sem_id=0;                 //信号量ID（OTDR互斥访问）                    
int flagNewA=0;                    //有新的测试节点加入标识
int flagNewB=0;                    //有新的异常节点加入标识
checkNode *linkHead_check_A;       //测试节点链表头
alarmNode *linkHead_alarm_B;       //异常节点链表头
int num_A =0;                      //测试链表节点数
int num_B =0;                      //异常链表节点数
int flagNew=0;                     //节点更新/插入标志

/*****************************LinkA-checkLink************************************/
/***插入测试节点****/
/*
     (1) 尾插法.uint32tostring
*/
checkNode *insert_A(checkNode *head,checkNode *newNode)
{
        checkNode *current=NULL;
        checkNode *pre=NULL;
        //current = (checkNode *) malloc (sizeof(checkNode ));
        //pre = (checkNode *) malloc (sizeof(checkNode ));
	//pre     = NULL;
	current = head;
        while(current!=NULL){
            pre = current;
            current = current->next;
        }
        newNode->next = current;
        if(pre==NULL){  
            head = newNode; 
        }else{
            pre->next = newNode;
        }
        num_A++;
        return (head);
}
/***创建新链表***/
checkNode *link_creat_A(){
	checkNode *head=NULL,*p1=NULL;
        head = (checkNode *) malloc (sizeof(checkNode ));
        p1 =   (checkNode *) malloc (sizeof(checkNode ));
        head=NULL;
        p1->SNo           =0;
        p1->CM            =0;
        p1->ANo           =0;
        p1->PowerGate     =-30;
        p1->fristAlarmFlag=0;
        p1->nextAlarmTime =0;
        p1->alarmClick    =0;
 
	head = insert_A(head,p1);
	return(head);
}

/***删除节点***/
/*
   (1)以光路号SNo为索引
*/
checkNode *delete_A(checkNode *head,int SNo){
	checkNode *p1=NULL,*p2=NULL;
	if(head==NULL){
		printf("This is a void execl");
		return(head);
	}
	p1= head;
	while(p1->SNo!=SNo && p1->next !=NULL){
		p2=p1;
		p1 =p1->next;
	}
	if(p1->SNo==SNo)
	{
		if(p1==head){
			head =p1->next;
		        free(p1);
                        p1=NULL;
                 }
		else
                 {
			p2->next =p1->next;
		        free(p1);
                        p1=NULL;
                 }
		num_A--;
	}else
		printf("Sorry,the SNo you want to delete is not fount!\n");
	return(head);
}

/***查找结点***/
/*
   (1)以光路号SNo为索引
*/
checkNode *findNode_A(checkNode *head,int SNo)
{
	checkNode * current=NULL;
        current = head;
        while(current!=NULL){
            if(current->SNo == SNo)
                return current;
	     current = current->next;
        }
        return NULL;
}

/***遍历链表***/
void outPutALL_A(checkNode *head){
	checkNode *p=NULL;
	p= head;
	if(p==NULL){
		printf("Don't have node in alarm tese link!\n");
		return ;
	}
	else
		printf("There are %d lines on alarm testing linkA:\n",num_A);
	while(p!=NULL){
		printf("SNo:%d,rtuCM:%d,ANo:%d,PowerGate:%ffristAlarmFlag:%d,nextAlarmTime:%ld,alarmClick:%ld\n"
                       ,p->SNo,p->CM,p->ANo,p->PowerGate,p->fristAlarmFlag,p->nextAlarmTime, p->alarmClick);
                p=p->next;
	}
}




/***************************LinkB-alarmLink************************************/
/***插入异常有序节点****/
/*
     (1) 根据故障优先级大小排序.
*/
alarmNode *insert_B(alarmNode *head,alarmNode *newNode)
{

        alarmNode *current=NULL;
        alarmNode *pre=NULL;
       // current = (alarmNode *) malloc (sizeof(alarmNode ));
       // pre = (alarmNode *) malloc (sizeof(alarmNode ));
       // pre     = NULL;
	current = head;
        while(current!=NULL&&current->Order < newNode->Order){
            pre = current;
            current = current->next;
        }
        newNode->next = current;
        if(pre==NULL){  
            head = newNode; 
        }else{
            pre->next = newNode;
        }
        num_B++;
        return (head);
}

/***创建新链表***/
alarmNode *link_creat_B(){
	alarmNode *head=NULL,*p1=NULL;
        head = (alarmNode *) malloc (sizeof(alarmNode ));
        p1   = (alarmNode *) malloc (sizeof(alarmNode ));
	head =NULL;
        p1->SNo =0;
        p1->CM =0;
        p1->ANo =0;
        p1->Order =0;
	head = insert_B(head,p1);
	return(head);
}

/***判断链表是否为空***/
int isEmpty_B(alarmNode *head){
        return (head==NULL);
}

/***删除头节点***/
/*
    (1)链表非空的前提下才能删除
*/
alarmNode *deleteFirst_B(alarmNode *head ){
        if (isEmpty_B(head)){
            return NULL;
        }
        alarmNode *temp=NULL;
        temp = (alarmNode *) malloc (sizeof(alarmNode ));
        temp = head;
        head = head->next;
        num_B--;
        return temp;
    }

/***输出头节点***/
/*
    (1)链表非空情况下才能输出
*/
alarmNode * outFirstnode_B(alarmNode *head)
{
        alarmNode *p0=NULL;
	if(head==NULL){
		return(head);                               
	}
        p0 = (alarmNode *) malloc (sizeof(alarmNode ));        
        p0->SNo           = head->SNo;
        p0->CM            = head->CM;
        p0->ANo           = head->ANo;
        p0->Order         = head->Order;
	return(p0);
}

/***删除节点***/
/*
   (1)以光路号SNo为索引
*/
alarmNode *delete_B(alarmNode *head,int SNo){
	alarmNode *p1=NULL,*p2=NULL;
	if(head==NULL){
		printf("This is a void execl");
		return(head);
	}
	p1= head;
	while(p1->SNo!=SNo && p1->next !=NULL){
		p2=p1;
		p1 =p1->next;
	}
	if(p1->SNo==SNo)
	{
		if(p1==head){
			head =p1->next;
		        free(p1);
                 }
		else
                 {
			p2->next =p1->next;
		        free(p1);
                        p1=NULL;
                 }
		num_B--;
	}else
		printf("Sorry,the SNo you want delete is not fount!\n");
	return(head);
}

/***查找结点***/
/*
   (1)以光路号SNo为索引
*/
alarmNode *findNode_B(alarmNode *head,int SNo)
{
	alarmNode * current=NULL;
        current = head;
        while(current!=NULL){
            if(current->SNo == SNo)
                return current;
	     current = current->next;
        }
        return NULL;
}

/***遍历链表***/
void outPutALL_B(alarmNode *head){
	alarmNode *p=NULL;
	p= head;
	if(p==NULL)
	   return ;
        else {
          printf("There are %d lines on testing_B.\n",num_B); 
	  while(p!=NULL){
		printf("SNo:%d,rtuCM:%d,Level:%d,Order:%d\n",p->SNo,p->CM,p->ANo,p->Order);
		p=p->next;
	  }
       }
}



/**************************************************************************/
/***初始化测试链表****/
/*
    (1)创建一个空链表
    (2)将数据库障碍告警测试表中具有启动标识（status=1）的光路，加入初始化链表。
          --->光路状态为1 表示正在进行障碍告警测试表，因此在初始化过程中需要将其加入调度链表.
          --->保证每次启动，需要进程测试的节点能加入链表（例如，停机重启之后).
*/
checkNode * InitA_CycleLink(void)
{
	 sqlite3 *mydb;
	 char *zErrMsg = 0,*SNo,*ModNo,*SNoOnline;
         char strPNo[10];
         int oldSwitchPos=0;
	 int rc,i,rednum=0,SN=0;
	 sql *mysql;
	 char resultSNo[64][5];
         char **result = NULL;
         checkNode *head=NULL,*node=NULL;
         uint32_t ANo,intModNo;
         int fiberType=-1,ModuleType=-1,protectFlag=0;
         float PowerGate,PowerGateOnline;
         int    CM=0;
         int    ret=-1;
         time_t T3,T4;

         head = link_creat_A();
         head = delete_A(head,0);                                     
         SNoOnline =(char *) malloc(sizeof(char)*5);
	 SNo = (char *) malloc(sizeof(char)*5);
	 ModNo = (char *) malloc(sizeof(char)*5);
	 mysql = SQL_Create();
	 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
	 if( rc != SQLITE_OK ){
	    printf( "Lookup SQL error\n");
	 }
	 mysql->db = mydb;
	 mysql->tableName   =  "AlarmTestSegmentTable";	
         mysql->filedsValue =  "1";                                
         mysql->filedsName  =  "Status";

         SN=SQL_findSNo(mysql,resultSNo);
         if(SN>0){
		for(i =0 ;i<SN;i++){
   		     printf("SNo:%s ",resultSNo[i]);                          //获取光纤类型
		     strcpy(SNo,resultSNo[i]);         
	             mysql->tableName     = "AlarmTestSegmentTable";          
		     mysql->filedsName    = "fiberType"; 
		     mysql->mainKeyValue  = SNo;
		     SQL_lookupPar(mysql,&result,&rednum); 
                     fiberType=atoi(result[0]);
		     printf("fiberType:%d ",fiberType);
                     SQL_freeResult(&result,&rednum);
                      
		     mysql->filedsName    = "protectFlag"; 
	             SQL_lookupPar(mysql,&result,&rednum); 
		     protectFlag=atoi(result[0]);
	             printf("protectFlag:%d ",protectFlag);
	             SQL_freeResult(&result,&rednum);
 
                     intModNo=((atoi(resultSNo[i])-1)/8+1);                  // 获取所属模块类型              
                     uint32tostring(intModNo,ModNo);
                     mysql->mainKeyValue  = ModNo;
                     mysql->tableName     =  "SubModuleTypeTable";
                     mysql->filedsName    =  "ModuleType";
                     SQL_lookupPar(mysql,&result,&rednum); 
                     ModuleType=atoi(result[0]); 
		     printf("moduleType:%d \n",ModuleType);
                     SQL_freeResult(&result,&rednum);



                     if(fiberType==0 || ModuleType==4  ){                     // 备纤 或 在纤（OPM）模块
                        if(protectFlag == 0 ){
			        mysql->tableName     = "AlarmTestSegmentTable";   
		                mysql->mainKeyValue  = SNo;

				mysql->filedsName    = "rtuCM"; 
				SQL_lookupPar(mysql,&result,&rednum); 
				CM =atoi(result[0]);
				printf("CM:%d ",CM);
				SQL_freeResult(&result,&rednum);
	 
				mysql->filedsName    = "Level"; 
				SQL_lookupPar(mysql,&result,&rednum); 
				ANo =atoi(result[0]);
				printf("ANo:%d ",ANo);
				SQL_freeResult(&result,&rednum);

				mysql->filedsName    = "AT06"; 
				SQL_lookupPar(mysql,&result,&rednum); 
				PowerGate=atof(result[0]);
				printf("PowerGate:%f ",PowerGate);
				SQL_freeResult(&result,&rednum);

				mysql->filedsName    = "T3"; 
				SQL_lookupPar(mysql,&result,&rednum); 
				T3 =computTime(result[0]);
				printf("T3:%ld ",T3);
				SQL_freeResult(&result,&rednum);

				mysql->filedsName    = "T4"; 
				SQL_lookupPar(mysql,&result,&rednum); 
				T4= computTime(result[0]);
				printf("T4:%ld \n",T4);
				SQL_freeResult(&result,&rednum);
		                                     
	    	                if(!setModbus_P())                                                //P
		                   exit(EXIT_FAILURE);  
		                usleep(50000);                         
		                modbus_t * mb=newModbus(MODBUS_DEV,MODBUS_BUAD);
		                ret = setOneOpticalThreshold(mb,atoi(SNo),PowerGate); 
		                freeModbus(mb);     
		                usleep(50000);                  
		                if(!setModbus_V())                                                //V
		                   exit(EXIT_FAILURE);        
		                if(ret==0){ 	
		                    node=(checkNode *)malloc(sizeof(checkNode));		    
				    node->SNo           = atoi(SNo);
				    node->CM            = CM;
				    node->ANo           = ANo;
				    node->PowerGate     = PowerGate;
				    node->fristAlarmFlag= 0;
				    node->nextAlarmTime = getLocalTimestamp();                    
				    node->alarmClick    = T3;                                
				    head=insert_A(head,node); 
		                } 
                        }
                    } 
		}
         }
         free(SNo);
         free(ModNo);
	 SQL_Destory(mysql);  
	 sqlite3_close(mydb);
         return(head);
}

alarmNode * InitB_CycleLink(void)
{
         alarmNode *head,*node;
         head = link_creat_B();
         head = delete_B(head,0); 
         return(head);
}
/***遍历链表***/
alarmNode * deleteALL_B(alarmNode *head){
	alarmNode *p=NULL;
	union sigval mysigval;
	char* process;  
	int ret = 0,n,signum;  
        pid_t cycPID[MAX_PID_NUM];  
	int recvInt=-1; 
	p= head;
	if(p==NULL){
		return p=NULL;
	}
	else{
	    while(p!=NULL){
		           /*向otdrMain发送启动信号*/
		        if(!setOTDR_P())                                                //P
		           exit(EXIT_FAILURE);   
		        printf("Send message to otdrMain! SNo=%d\n",p->SNo);
			process ="/web/cgi-bin/otdrMain";                        
			ret = get_pid_by_name(process, cycPID, MAX_PID_NUM);  
			printf("alarmMain:process '%s' is existed? (%d): %c ", process, ret, (ret > 0)?'y':'n');  
			signum=SIGRTMIN;                                     
			mysigval.sival_int = p->SNo+200;                                                      
			for(n=0;n<ret;n++){                                      
				printf("otdrMain PID:%u\n", cycPID[n]);                  
				if(sigqueue(cycPID[n],signum,mysigval)==-1)
					printf("send signal error\n");
				}  
		           /*等待信号的成功处理消息*/			    
			recvInt = recvMessageQueue_D("2-OK",2222);
		        if(recvInt==0){
		              printf("alarmMain Recv back message from otdrMain  sucessful!");
		         }else{
		              printf("alarmMain Recv back message from otdrMain  Faild:Time out!");
		         } 		                                                               
		        usleep(10000);    //确保信号被处理完
		        if(!setOTDR_V())                                                //V
		           exit(EXIT_FAILURE); 

		        if(recvInt==0)  
			    p=delete_B(p,p->SNo);
	   }
           printf("\n\n");
        }    
       return p;
}

checkNode *removeAllNode_A(checkNode * head)
{
   checkNode * p=NULL;
   p=head;
   if(p==NULL)
      return p=NULL; 
   else
     while(p!=NULL)
        p=delete_A(p,p->SNo);
  return p;
}

alarmNode *removeAllNode_B(alarmNode * head)
{
   alarmNode * p=NULL;
   p=head;
   if(p==NULL)
      return p=NULL; 
   else
     while(p!=NULL)
        p=delete_B(p,p->SNo);
  return p;
}
/***插入待启动节点***/
/*
      (1) 查询光路时间表状态
            --->BOA收到周期测试指令后，将待测试的光路状态修改为-1
            --->周期测试调度进程，查询数据库中状态为-1的节点. 
      (2) 加入测试节点
            --->将刚刚修改的光路加入周期测试链表
            --->BOA检查刚刚待测试状态为-1的光路，是否全部修改为1状态，若成功，则向中心服务器报告周期测试指令成功执行。
      (3) 检擦测试节点对应的光路数否已经存在
            --->若链表中SNo指定的光路存在，则只需更新测试参数（起始时间，下次启动时间、时间间隔）
            --->若不存在，则插入新的周期测试节点(先将其删除在插入链表。这样可保证更新的节点任然有序)
      (4)修改数据库时间表状态
            --->在数据库中，将刚刚检查出状态为-1的状态修改为1，保证所有待测试光路都处于测试状态
            --->修改完成后，即可向BOA发送测试节点加入成功消息
            --->修改过程中需要信号量进行数据库互斥访问(PV操作)
*/
checkNode *insertWaitingNode(checkNode *head)                    
{
	 sqlite3 *mydb;
	 char *zErrMsg = 0,*SNo=NULL,*ModNo=NULL,*SNoOnline;
         char strPNo[10];
         int oldSwitchPos=0;
	 int rc,i,rednum=0;
	 sql *mysql;
	 char resultSNo[64][5];
         char **result = NULL;
         checkNode *node,*find;
         time_t T3,T4;
                                   
         uint32_t SN=0,ANo=0,CM=0,intModNo=0;
         int   fiberType=-1,ModuleType=-1,protectFlag=-1,ret =-1;
         float PowerGate,PowerGateOnline;

         SNoOnline =(char *) malloc(sizeof(char)*5);
	 SNo = (char *) malloc(sizeof(char)*5);
         ModNo=(char *) malloc(sizeof(char)*5);
	 mysql = SQL_Create();
	 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
	 if( rc != SQLITE_OK ){
	     printf( "Lookup SQL error: %s\n", zErrMsg);
		      sqlite3_free(zErrMsg);
		   }
	 mysql->db = mydb;
	 mysql->tableName   =  "AlarmTestSegmentTable";	
         mysql->filedsValue =  "-1";                                
         mysql->filedsName  =  "Status";
         SN=SQL_findSNo(mysql,resultSNo);                                       //查找光路状态为待启动的记录
         if(SN>0){
		for(i =0 ;i<SN;i++){

   		        printf("SNo:%s",resultSNo[i]);                          //获取光纤类型
		        strcpy(SNo,resultSNo[i]);         
	                mysql->tableName     = "AlarmTestSegmentTable";          
		        mysql->filedsName    = "fiberType"; 
			mysql->mainKeyValue  = SNo;
		        SQL_lookupPar(mysql,&result,&rednum); 
                        fiberType=atoi(result[0]);
		        printf("fiberType:%d\n",fiberType);
                        SQL_freeResult(&result,&rednum);
                 

			mysql->filedsName    = "protectFlag"; 
			SQL_lookupPar(mysql,&result,&rednum); 
		        protectFlag=atoi(result[0]);
			printf("protectFlag:%d\n",protectFlag);
                        SQL_freeResult(&result,&rednum);

      
                        intModNo=((atoi(resultSNo[i])-1)/8+1);                  // 获取所属模块类型              
                        uint32tostring(intModNo,ModNo);
                        mysql->mainKeyValue  = ModNo;
                        mysql->tableName     =  "SubModuleTypeTable";
                        mysql->filedsName    =  "ModuleType";
                        SQL_lookupPar(mysql,&result,&rednum); 
                        ModuleType=atoi(result[0]); 
                        SQL_freeResult(&result,&rednum);



                        if(fiberType==0 || ModuleType==4){                       //备纤 或 在纤（OPM）模块
                                if(protectFlag==0){
			                mysql->tableName     = "AlarmTestSegmentTable";   
					mysql->filedsName    = "rtuCM"; 
					mysql->mainKeyValue  = SNo;
					SQL_lookupPar(mysql,&result,&rednum); 
				        CM =atoi(result[0]);
					printf("CM:%d  ",CM);
		                        SQL_freeResult(&result,&rednum);

					mysql->filedsName    = "Level"; 
					mysql->mainKeyValue  = SNo;
					SQL_lookupPar(mysql,&result,&rednum); 
				        ANo =atoi(result[0]);
					printf("ANo:%d  ",ANo);
		                        SQL_freeResult(&result,&rednum);

					mysql->filedsName    = "AT06"; 
				        mysql->mainKeyValue  =  SNo;
					SQL_lookupPar(mysql,&result,&rednum); 
				        PowerGate=atof(result[0]);
					printf("PowerGate:%f  ",PowerGate);
		                        SQL_freeResult(&result,&rednum);

					mysql->filedsName    = "T3"; 
					SQL_lookupPar(mysql,&result,&rednum); 
				        T3 =computTime(result[0]);
					printf("T3:%ld  ",T3);
		                        SQL_freeResult(&result,&rednum);

					mysql->filedsName    = "T4"; 
					SQL_lookupPar(mysql,&result,&rednum); 
				        T4= computTime(result[0]);
					printf("T4:%ld  ",T4);
		                        SQL_freeResult(&result,&rednum);
  
                                        mysql->filedsName    = "Status"; 
                                        mysql->filedsValue   = "1";
				        if(!semaphore_p())  
				          exit(EXIT_FAILURE);                                 //P
				        rc=SQL_modify(mysql);
				        if( rc != SQLITE_OK )
					  printf( "Modify SQL error\n");
				        if(!semaphore_v())                                     //V
				          exit(EXIT_FAILURE); 
	
				        node=(checkNode *)malloc(sizeof(checkNode));		    
					node->SNo           = atoi(SNo);
					node->CM            = CM;
					node->ANo           = ANo;
					node->PowerGate     = PowerGate;
					node->fristAlarmFlag= 0;
					node->nextAlarmTime = getLocalTimestamp();                    
					node->alarmClick    = T3;                                
		                        find=findNode_A(head,node->SNo);                          // 查看链表中是否已经存在SNo光路
				        if(find ==NULL){
				               head = insert_A(head,node);                
					}else{
					       head = delete_A(head,find->SNo);         
				               head = insert_A(head,node); 
				        }
                                }//end if
                          }//endif
                       
		}  //end for 
         }
         free(SNo);
         free(ModNo);
	 SQL_Destory(mysql);  
	 sqlite3_close(mydb);  
         return(head);
}

/***删除待取消测试节点***/
/*
      (1) 查询障碍告警测试参数表状态
            --->BOA收到取消障碍告警测试参数指令后，将待取消测试的光路状态修改为-2
            --->障碍告警测试参数调度进程，查询数据库中状态为-2的节点.  
      (2) 删除待取消的测试节点
           --->先从链表中删除该SNo节点
           --->再更新光路状态为“2”(已取消)
*/
checkNode * removeWaitingNode(checkNode *head) 
{
	 sqlite3 *mydb;
	 char *SNo,*ModNo;
	 int rc,i,SN=0,intSNo=0,intModNo=0;
         int fiberType=-1,ModuleType=-1;
	 sql *mysql;
	 char resultSNo[64][5];
         char **result=NULL;
         int  rednum=0; 
         checkNode *find;
	 SNo = (char *) malloc(sizeof(char)*5);
         ModNo = (char *) malloc(sizeof(char)*5);
	 mysql = SQL_Create();
	 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
	 if( rc != SQLITE_OK )
	     printf("Lookup SQL error\n");
	 mysql->db = mydb;
	 mysql->tableName   = "AlarmTestSegmentTable";	
         mysql->filedsValue =  "-2";                                
         mysql->filedsName  =  "Status";
         SN=SQL_findSNo(mysql,resultSNo);                           
         if(SN>0){
		 for(i =0 ;i<SN;i++){
		       printf("SNo:%s",resultSNo[i]);                          //获取光纤类型
		       strcpy(SNo,resultSNo[i]);		
		       intSNo =atoi(SNo);     
                             
	               mysql->tableName     = "AlarmTestSegmentTable";          
		       mysql->filedsName    = "fiberType"; 
		       mysql->mainKeyValue  = SNo;
		       SQL_lookupPar(mysql,&result,&rednum); 
                       fiberType=atoi(result[0]);
		       printf("fiberType:%d\n",fiberType);
                       SQL_freeResult(&result,&rednum);
                    
                       intModNo=((intSNo-1)/8+1);                              // 获取所属模块类型              
                       uint32tostring(intModNo,ModNo);
                       mysql->mainKeyValue  =  ModNo;
                       mysql->tableName     =  "SubModuleTypeTable";
                       mysql->filedsName    =  "ModuleType";
                       SQL_lookupPar(mysql,&result,&rednum); 
                       ModuleType=atoi(result[0]); 
                       SQL_freeResult(&result,&rednum);

		       if(fiberType==0 || ModuleType == 4 ){		       //备纤 或 在纤（OPM）模块                        
		          find=findNode_A(head,intSNo);                  
		          if(find ==NULL)
		             printf("Don't have SNo=%d node in LinkA!\n",intSNo);                
			  else
		             head = delete_A(head,intSNo);    
                       }          
                       mysql->tableName    = "AlarmTestSegmentTable";                                    
                       mysql->mainKeyValue = SNo;
                       if(!semaphore_p())  
                          exit(EXIT_FAILURE);                                 //P
                       rc=SQL_delete(mysql);
                       if( rc != SQLITE_OK )
			  printf( "Delete SQL error\n");
                       if(!semaphore_v())                                     //V
                          exit(EXIT_FAILURE);    
                              	
                  }
         }
         free(SNo);
         free(ModNo);
	 SQL_Destory(mysql);  
	 sqlite3_close(mydb);
          
         return(head);
}

float realValue[8] ={4,4,4,4,4,4,4,4};                              //for test
alarmNode  *rollPolingAlarm(checkNode *headA,alarmNode *headB)
{
     	checkNode *p=NULL;
        alarmNode *q=NULL,*node=NULL;    
        float powerValue=-100.0,PowerGate=0,powerGateOnline=0; 
        backData *bData=NULL; 
        int i=0,j=0;
	sqlite3 *mydb=NULL;
	sql *mysql=NULL;
	char **result = NULL;
        modbus_t *mb=NULL;
        char strSNo[10],strPNo[10],strSNoOnline[10],strNewSwitchPos[10];
	int  rednum =0;
        int  oldSwitchPos=0,newSwitchPos=0,flagSW=-1;
	int rc=0,intSNo=0,intANo=0,intCM=0;
        int fristAlarmFlag=0;
        time_t nowTime=0,nextAlarmTime=0,alarmClick=0;
	p= headA;
        q= headB;
        mysql = SQL_Create();
	if(p==NULL){
		//printf("This is a void excel!\n");
		return q;
	} 
	else
	  while(p!=NULL){    
             if(p!=NULL){
                  if(flagNew == 0 ){                                                                   
		      if(p!=NULL)intSNo=p->SNo;
		      if(p!=NULL)intANo=p->ANo; 
		      if(p!=NULL)intCM =p->CM;    
		      if(p!=NULL)fristAlarmFlag=p->fristAlarmFlag;     
		      if(p!=NULL)PowerGate=p->PowerGate;   
		      if(p!=NULL)nextAlarmTime=p->nextAlarmTime; 
		      if(p!=NULL)alarmClick=p->alarmClick;

	              if(!setModbus_P())                                              //P  
		         exit(EXIT_FAILURE); 
                      usleep(50000);  		        
		      mb=newModbus(MODBUS_DEV,MODBUS_BUAD);
		      powerValue = getOneOpticalValue(mb,intSNo,2);                //获取当前光路光功率值                      
		      freeModbus(mb);  
                      usleep(50000);		                 
		      if(!setModbus_V())                                              //V
                         exit(EXIT_FAILURE);  
                      if(powerValue==0)powerValue=-70.0; 
  
                  }else{
                       if(p!=NULL){
		               if(p!=NULL)p=p->next; 
		               if(p!=NULL){
				    if(p!=NULL)intSNo=p->SNo;
				    if(p!=NULL)intANo=p->ANo; 
				    if(p!=NULL)intCM =p->CM;    
				    if(p!=NULL)fristAlarmFlag=p->fristAlarmFlag;     
				    if(p!=NULL)PowerGate=p->PowerGate;              
				    if(p!=NULL)nextAlarmTime=p->nextAlarmTime; 
				    if(p!=NULL)alarmClick=p->alarmClick; 

				    if(!setModbus_P())                                                //P  
					exit(EXIT_FAILURE);   
				    usleep(50000);				
				    mb=newModbus(MODBUS_DEV,MODBUS_BUAD);
				    powerValue = getOneOpticalValue(mb,intSNo,2);                        //获取当前光路光功率值							           
			            freeModbus(mb);  		       
                                    usleep(50000);				                     
			            if(!setModbus_V())                                                //V
				       exit(EXIT_FAILURE); 
                                    if(powerValue==0)powerValue=-70.0;  
                                    flagNew=0;
		                                        
		               }else{
		                    p=NULL;
                                    flagNew=0;
		                    break;
		               }
                       }else{
			    p=NULL;
                            flagNew=0;
			    break;                           
                       }
                  }
             }else{
		  p==NULL;
                  flagNew=0;
		  break;
             }              
             if( powerValue < PowerGate ){    //异常	 
                     nowTime = getLocalTimestamp();                   
                     if(fristAlarmFlag ==0){                                   //状态C: 首次出现异常 -->fristAlarmFlag=0   实际光功率值<阈值                 
                          printf("障碍告警(备纤、在纤OPM):StateC--->光路%d 首次出现异常...光功率值:%f <---> 参考门限:%f\n\n",intSNo ,powerValue,PowerGate);
                         
		          fristAlarmFlag = 1;
			  node=(alarmNode *)malloc(sizeof(alarmNode));
			  node->SNo = intSNo;
		          node->CM  = intCM;
		          node->ANo = intANo;     
		          node->Order = (intANo)*100 +intSNo; 
                          if(p!=NULL){
			       q=insert_B(q,node);
                          }else{
		               p=NULL;
		               break;
                          }
	                  nextAlarmTime  = getLocalTimestamp()+alarmClick;                  
                     }else if(nowTime >= nextAlarmTime){                       //状态D:长期处于异常 -->fristAlarmFlag=1   实际光功率值<阈值  
                          printf("障碍告警(备纤、在纤OPM):StateD--->光路%d 长期处于异常...光功率值:%f <---> 参考门限:%f\n\n",intSNo,powerValue,PowerGate);                                   
			  node=(alarmNode *)malloc(sizeof(alarmNode));
			  node->SNo = intSNo;
			  node->CM  = intCM;
			  node->ANo = intANo;     
			  node->Order = (intANo)*100 +intSNo; 
                          if(p!=NULL){
			     q=insert_B(q,node);
                          }else{
		             p=NULL;
		             break;
                          }                        
	                  nextAlarmTime  = getLocalTimestamp()+alarmClick;   
                     }  
                 }                               //正常             
                 else if(fristAlarmFlag!=0){                                 //状态A:从异常中首次恢复 --> fristAlarmFlag=1 实际光功率值>=阈值      
                          printf("障碍告警(备纤、在纤OPM):StateA--->光路%d 从异常中恢复...光功率值:%f <---> 参考门限:%f\n\n",intSNo,powerValue,PowerGate);  
                          bData=backData_Create();
                          bData->powerValue =powerValue;
                          bData->powerGate  =PowerGate;
                          bData->level      =0;
                          if(p!=NULL){
		                  mysql = SQL_Create();
				  rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
				  if( rc != SQLITE_OK )
					printf("Lookup IP SQL error\n");			 
				  mysql->db = mydb;
				  mysql->tableName        = "AlarmTestSegmentTable";
				  mysql->filedsName       = "IP01";
				  uint32tostring(intSNo,strSNo);
				  mysql->mainKeyValue = strSNo;  
				  SQL_lookupPar(mysql,&result,&rednum);
				  strcpy(bData->backIP,result[0]);
				  SQL_freeResult(&result,&rednum); 
			     	  SQL_Destory(mysql);  
				  sqlite3_close(mydb); 
		                  upload(bData,intSNo,intCM,1);
                          }else{
		                  p=NULL;
		                  break;
                          }
                          backData_Destory(bData);      
                          fristAlarmFlag= 0;
                      } 
                else ;                                                       //状态B:正常  -->  fristAlarmFlag=0 实际光功率值>=阈值            

                if(p!=NULL){
                      if(flagNew==0){
			      if(p!=NULL) p->fristAlarmFlag=fristAlarmFlag;
			      if(p!=NULL) p->nextAlarmTime=nextAlarmTime  ;
			      if(p!=NULL) p=p->next;
                      }else{
		              if(p!=NULL) p=p->next;
                              flagNew=0;   
		              break;
                      }
                }else{
                     p=NULL; 
                     flagNew=0; 
                     break;
                }                    
       }
       return q;
}
void addNewtoLink(int signum,siginfo_t *info,void *myact);
void main(void)
{
        checkNode *node_A;
        alarmNode *node_B;
        int i,flagCycle = 0,flagProtect=0 ; 
        pid_t cyclePID[MAX_PID_NUM],protectPID[MAX_PID_NUM];  
        /*初始化信号量*/
        sem_id        = semget((key_t)1234, 1, 4777 | IPC_CREAT);                         //创建数据库信号量 :每一个需要用到信号量的进程,在第一次启动的时候需要获取指定的信号量
        modbus_sem_id = semget((key_t)5678, 1, 4777 | IPC_CREAT);                         //创建ModBus信号量 :每一个需要用到信号量的进程,在第一次启动的时候需要获取指定的信号量
        otdr_sem_id   = semget((key_t)2468, 1, 4777 | IPC_CREAT);                         //创建OTDR信号量   :每一个需要用到信号量的进程,在第一次启动的时候需要获取指定的信号量 


       flagCycle = get_pid_by_name("/web/cgi-bin/cycMain", cyclePID, MAX_PID_NUM);  
       flagProtect = get_pid_by_name("/web/cgi-bin/ProtectMasterMain", protectPID, MAX_PID_NUM);  
       if(flagCycle <=0 && flagProtect<=0)
          if(!setOTDRPV()) {                                                                //first launch (for modfiy)        
            exit(0);
       }   
        /*初始化测试链表*/
        linkHead_check_A=InitA_CycleLink();
        linkHead_alarm_B=InitB_CycleLink();
        if(linkHead_check_A !=NULL)
            outPutALL_A(linkHead_check_A);
        else
            printf("linkA Head:NULL\n");

        if(linkHead_alarm_B !=NULL)
            outPutALL_B(linkHead_alarm_B);
        else
            printf("linkB Head:NULL\n");
        /*初始化信号机制（IPC）*/
        struct sigaction act;
        int sig;
        sig=SIGUSR1;  //for Boa
        sigemptyset(&act.sa_mask);
        act.sa_sigaction=addNewtoLink;
        act.sa_flags=SA_SIGINFO|SA_RESTART;                                                                                                                                                               
        if(sigaction(sig,&act,NULL)<0){                              
          printf("install sigal error\n");
        }
        /*执行调度程序*/
        while(1){
             
            linkHead_alarm_B=rollPolingAlarm(linkHead_check_A,linkHead_alarm_B); 
            
            outPutALL_B(linkHead_alarm_B);     

            linkHead_alarm_B=deleteALL_B(linkHead_alarm_B);
 
            usleep(100000);
           
  
        }   	
}
void addNewtoLink(int signum,siginfo_t *info,void *myact)
{
       printf("alarmMain(R): the int value is %d \n",info->si_int);
       int SNo =0;
       if(info->si_int>270 && info->si_int <370)                                //最大一次删除99个节点
       {
           SNo = info->si_int%270;
           linkHead_check_A = delete_A(linkHead_check_A,SNo);                   //删除节点
           linkHead_alarm_B = delete_B(linkHead_alarm_B,SNo);
           outPutALL_A(linkHead_check_A);
           outPutALL_B(linkHead_alarm_B); 
           sendMessageQueue("270-OK");
	   return;
       }
       switch(info->si_int){
           case 130:{ 
                    flagNew = 1;  
                    linkHead_check_A = insertWaitingNode(linkHead_check_A);      //启动告警测试
                    outPutALL_A(linkHead_check_A);
                    sendMessageQueue("130-OK");
		    break;
                  }
           case 230:{                                                            //终止告警测试
                    flagNew = 1;
                    linkHead_check_A = removeWaitingNode(linkHead_check_A);                   
                    outPutALL_A(linkHead_check_A);
                    sendMessageQueue("230-OK");
		    break;
                  }          
           case 260:{                                                           //清除RTU模式
                    flagNew = 1;
                    linkHead_check_A = removeAllNode_A(linkHead_check_A); 
                    linkHead_alarm_B = removeAllNode_B(linkHead_alarm_B);                  
                    outPutALL_A(linkHead_check_A);
                    outPutALL_B(linkHead_alarm_B); 
                    sendMessageQueue("260-OK");
		    break;    
                   }         
          default:{                                                             //异步接收光功率异常(测试)
                    break;
                  }
      }
}

