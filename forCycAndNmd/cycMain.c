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

/***节点结构***/
/*
    SNO:光路号
    CM :局站号
    fristTestTime:首次启动时间
    nextTestTime :下一次启动时间
    timePeriod   :时间间隔
*/
typedef struct testLinknode testLinknode;
struct testLinknode{
	int    SNo;
        int    CM;
	time_t    fristTestTime;
        time_t    nextTestTime;
        time_t    timePeriod;
	struct testLinknode *next;
};

/*全局变量*/
int sem_id=0;              //信号量ID（数据库互斥访问）
int otdr_sem_id=0;         //信号量ID（OTDR互斥访问）                             
int flagNew=0;             //新节点插入标志
testLinknode *linkHead;    //链表头
int n =0;                  //链表节点数


/***插入有序节点****/
/*
     (1) 根据下次启动时间,按照时间先后插入链表，安时间戳大小排序.
*/
testLinknode *insert(testLinknode *head,testLinknode *newNode)
{

        testLinknode *current;
        testLinknode *pre;
        current = (testLinknode *) malloc (sizeof(testLinknode ));
        pre = (testLinknode *) malloc (sizeof(testLinknode ));
        pre     = NULL;
	current = head;
        while(current!=NULL&&current->nextTestTime < newNode->nextTestTime){
            pre = current;
            current = current->next;
        }
        newNode->next = current;
        if(pre==NULL){  
            head = newNode; 
        }else{
            pre->next = newNode;
        }
        n++;
        return (head);
}

/***创建新链表***/
testLinknode *link_creat(){
	testLinknode *head,*p1;
        head = (testLinknode *) malloc (sizeof(testLinknode ));
        p1 =   (testLinknode *) malloc (sizeof(testLinknode ));
	head =NULL;
        p1->SNo =0;
        p1->fristTestTime =0;
        p1->nextTestTime =0;
        p1->timePeriod =0;
	head = insert(head,p1);
	return(head);
}

/***判断链表是否为空***/
int isEmpty(testLinknode *head){
        return (head==NULL);
}

/***删除头节点***/
/*
    (1)链表非空的前提下才能删除
*/
testLinknode *deleteFirst(testLinknode *head ){
        if (isEmpty(head)){
            return NULL;
        }
        testLinknode *temp;
        temp = (testLinknode *) malloc (sizeof(testLinknode ));
        temp = head;
        head = head->next;
        n--;
        return temp;
    }

/***输出头节点***/
/*
    (1)链表非空情况下才能输出
*/
testLinknode * outFirstnode(testLinknode *head)
{
        testLinknode *p0;
	if(head==NULL){
		return(head);                               
	}
        p0 = (testLinknode *) malloc (sizeof(testLinknode ));
        
        p0->SNo           = head->SNo;
        p0->CM            = head->CM;
        p0->fristTestTime = head->fristTestTime;

        p0->nextTestTime  = head->nextTestTime;
        p0->timePeriod    = head->timePeriod;

	return(p0);
}

/***删除节点***/
/*
   (1)以光路号SNo为索引
*/
testLinknode *delete(testLinknode *head,int SNo){
	testLinknode *p1,*p2;
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
                 }
		n--;
	}else
		printf("Sorry,the SNo you want delete is not fount!\n");
	return(head);
}

/***查找结点***/
/*
   (1)以光路号SNo为索引
*/
testLinknode *findNode(testLinknode *head,int SNo)
{
	testLinknode * current;
        current = head;
        while(current!=NULL){
            if(current->SNo == SNo)
                return current;
	     current = current->next;
        }
        return NULL;
}

/***遍历链表***/
void outPutALL(testLinknode *head){
	testLinknode *p;

	p= head;
	if(p==NULL){
		printf("Don't have node in cycle test link!\n");
		return ;
	}
	else
		printf("There are %d lines on cycle testing:\n",n);
	while(p!=NULL){
		printf("SNo:%d,rtuCM:%d,firstTime:%ld,nextTime%ld\n",p->SNo,p->CM,p->fristTestTime,p->nextTestTime);
		p=p->next;
	}
}
testLinknode * removeAllNode(testLinknode * head)
{
   testLinknode * p=NULL;
   p=head;
   if(p==NULL)
      return p=NULL;  
   else
     while(p!=NULL)
        p=delete(p,p->SNo);
   return p;
}

time_t getSetTimestamp(testLinknode *head)
{
	return (head->nextTestTime);
}
testLinknode *taskScheduler(testLinknode *head,testLinknode *p1){
        //testLinknode *p1; 
        time_t now; 
        now = getLocalTimestamp(); 

        
        p1->nextTestTime = p1->nextTestTime + p1->timePeriod;  // p1->nextTestTime +p1->timePeriod;

        head=delete(head,p1->SNo);
        head=insert(head,p1); 
        return(head);        
}

/***初始化链表****/
/*
    (1)创建一个空链表
    (2)将数据库时间表中具有启动标识（status=1）的光路，加入初始化链表。
          --->光路状态为1 表示正在进行周期测试，因此在初始化过程中需要将其加入调度链表.
          --->保证每次启动，需要进程测试的节点能加入链表（例如，停机重启之后.
*/
testLinknode * Init_CycleLink(void)
{
	 sqlite3 *mydb;
	 char *zErrMsg = 0,*SNo;
	 int rc,i,SN=0;
	 sql *mysql;
	 char resultSNo[64][5];

         char **result = NULL;
         int  rednum =0;

         testLinknode *head,*node;
         head = link_creat();
         head = delete(head,0);                                     
         time_t T1,T2;
         int    CM=0;
	 SNo = (char *) malloc(sizeof(char)*5);
	 mysql = SQL_Create();
	 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
	 if( rc != SQLITE_OK ){
		      printf( "Lookup SQL error: %s\n", zErrMsg);
		      sqlite3_free(zErrMsg);
		   }
	 mysql->db = mydb;
	 mysql->tableName   = "CycleTestSegnemtTable";	
         mysql->filedsValue =  "1";                                
         mysql->filedsName  =  "Status";
         SN=SQL_findSNo(mysql,resultSNo);
         if(SN>0){
		 for(i =0 ;i<SN;i++)
		 {
		       printf("SNo:%s",resultSNo[i]);
		       strcpy(SNo,resultSNo[i]);

		       mysql->filedsName    = "rtuCM"; 
		       mysql->mainKeyValue  = SNo;
                       SQL_lookupPar(mysql,&result,&rednum);
                       CM =atoi(result[0]);
                       SQL_freeResult(&result,&rednum);
		       printf("CM:%d\n",CM);

		       mysql->filedsName    = "T1"; 
		       mysql->mainKeyValue  = SNo;
		       SQL_lookupPar(mysql,&result,&rednum);
                       T1 =atoi(result[0]);
                       SQL_freeResult(&result,&rednum);
		       printf("T1:%ld\n",T1);

		       mysql->filedsName    = "T2"; 
		       SQL_lookupPar(mysql,&result,&rednum);
                       T2= computTime(result[0]);
                       SQL_freeResult(&result,&rednum);
		       printf("T2:%ld\n",T2);

		        node=(testLinknode *)malloc(sizeof(testLinknode));
			node->SNo =atoi(SNo);
                        node->CM  =CM;

		        node->fristTestTime=T1;
                        if(T1<=getLocalTimestamp()){
			     node->nextTestTime = getLocalTimestamp();
                        }
		        else{
                            node->nextTestTime = T1;  //   getLocalTimestamp() 
                        }             
		        node->timePeriod = T2;                   
		        head=insert(head,node);
		  }
         }
         free(SNo);
	 SQL_Destory(mysql);  
	 sqlite3_close(mydb);
         return(head);
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
testLinknode * insertWaitingNode(testLinknode *head)                    
{
	 sqlite3 *mydb;
	 char *zErrMsg =NULL;
	 int rc,i,SN=0;
	 sql *mysql;
	 char resultSNo[64][5];
         char SNo[5];
         char **result = NULL;
         int  rednum =0;
         time_t T1,T2;
         int    CM=0;
         testLinknode *node,*find;
	 
	 mysql = SQL_Create();
	 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
	 if( rc != SQLITE_OK ){
		      printf( "Lookup SQL error: %s\n", zErrMsg);
		      sqlite3_free(zErrMsg);
		   }
	 mysql->db = mydb;
	 mysql->tableName   = "CycleTestSegnemtTable";	
         mysql->filedsValue =  "-1";                                
         mysql->filedsName  =  "Status";
         SN=SQL_findSNo(mysql,resultSNo);                           //查找光路状态为待启动的记录  光路状态为“-1”
         if(SN>0){
		 for(i =0 ;i<SN;i++)
		    {
		       printf("SNo:%s",resultSNo[i]);
		       strcpy(SNo,resultSNo[i]);

		       mysql->filedsName    = "rtuCM"; 
		       mysql->mainKeyValue  = SNo;
		       SQL_lookupPar(mysql,&result,&rednum);
                       CM =atoi(result[0]);
                       SQL_freeResult(&result,&rednum);
		       printf("CM:%d\n",CM);

		       mysql->filedsName    = "T1"; 
		       mysql->mainKeyValue  = SNo;
		       SQL_lookupPar(mysql,&result,&rednum);                        
                       T1 =atoi(result[0]);
                       SQL_freeResult(&result,&rednum);
		       printf("T1:%ld\n",T1);

		       mysql->filedsName    = "T2"; 
		       SQL_lookupPar(mysql,&result,&rednum);
                       T2= computTime(result[0]);
                       SQL_freeResult(&result,&rednum);
		       printf("T2:%ld\n",T2);

		       node=(testLinknode *)malloc(sizeof(testLinknode));
		       node->SNo =atoi(SNo);
                       node->CM  =CM;
		       node->fristTestTime=T1;
                       if(T1<=getLocalTimestamp()){
			    node->nextTestTime = getLocalTimestamp();
                       }
		       else{
                            node->nextTestTime = T1;  //  =getLocalTimestamp()    for test
                       }
		       node->timePeriod =  T2;                       //for test                  
                       find=findNode(head,node->SNo);                           // 查看链表中是否已经存在SNo光路
                       if(find ==NULL)
                       {
                           head=insert(head,node);                
		       }else{
		           head = delete(head,node->SNo);         
                           head=  insert(head,node); 
                       }
                       mysql->filedsValue  =  "1";                
                       mysql->filedsName   =  "Status";
                       mysql->mainKeyValue =  SNo;
                       if(!semaphore_p())  
                              exit(EXIT_FAILURE);                                //P
                       rc=SQL_modify(mysql);
                       if( rc != SQLITE_OK ){
			      printf( "Modify SQL error\n");
			      sqlite3_free(zErrMsg);
		       }
                       if(!semaphore_v())                                         //V
                             exit(EXIT_FAILURE);

                     }		
         }

	 SQL_Destory(mysql);  
	 sqlite3_close(mydb); 
 
         return(head);
}


/***删除待取消测试节点***/
/*
      (1) 查询光路时间表状态
            --->BOA收到取消周期测试指令后，将待取消测试的光路状态修改为-2
            --->周期测试调度进程，查询数据库中状态为-2的节点.  
      (2) 删除待取消的测试节点
           --->先从链表中删除该SNo节点
           --->再更新光路状态为“2”(已取消)
*/
testLinknode * removeWaitingNode(testLinknode *head) 
{
	 sqlite3 *mydb;
	 char *zErrMsg = NULL;
	 int rc,i,SN=0,intSNo=0;
	 sql *mysql;
	 char resultSNo[64][5];
         char SNo[5];
         testLinknode *find;
	 mysql = SQL_Create();
	 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
	 if( rc != SQLITE_OK ){
		      printf( "Lookup SQL error: %s\n", zErrMsg);
		      sqlite3_free(zErrMsg);
		   }
	 mysql->db = mydb;
	 mysql->tableName   = "CycleTestSegnemtTable";	
         mysql->filedsValue =  "-2";                                
         mysql->filedsName  =  "Status";
         SN=SQL_findSNo(mysql,resultSNo);                           
         if(SN>0){
		 for(i =0 ;i<SN;i++)
		    {
		       printf("SNo:%s",resultSNo[i]);
		       strcpy(SNo,resultSNo[i]);		
		       intSNo =atoi(SNo);                                    
                       find=findNode(head,intSNo);                  
                        if(find ==NULL)
                        {
                          printf("Don't have SNo=%d node in this Link!\n",intSNo);                
		        }else{
                          head = delete(head,intSNo);                                             
                          mysql->filedsValue  =  "2";                                             
                          mysql->filedsName   =  "Status";
                          mysql->mainKeyValue =  SNo;
                          if(!semaphore_p())  
                              exit(EXIT_FAILURE);                                                 //P
                          rc=SQL_modify(mysql);
                          if( rc != SQLITE_OK ){
			      printf( "Modify SQL error\n");
			      sqlite3_free(zErrMsg);
		          }
                         if(!semaphore_v())                                                       //V
                             exit(EXIT_FAILURE);                  	
                        }
	            }
         }
	 SQL_Destory(mysql);  
	 sqlite3_close(mydb);   
         return(head);
}

/***刷新状态***/
/*
      (1)将数据库中状态为“-1”和“-2”的状态光路，修改为“1”和“2”。
          --->防止BOA单方面修改状态而并没有通过插入节点程序更新状态，导致后台周期测试调度程序没有发现，或由于意外情况发生，没有及时处理的光路。
*/
void flushWaitingSNo(void)
{
      	 sqlite3 *mydb;
	 char *zErrMsg = 0;
	 int rc,i,SN=0;
	 sql *mysql;
	 char resultSNo[64][5];

	 mysql = SQL_Create();
	 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
	 if( rc != SQLITE_OK ){
		      printf( "Lookup SQL error: %s\n", zErrMsg);
		      sqlite3_free(zErrMsg);
		   }
	 mysql->db = mydb;
	 mysql->tableName   =  "CycleTestSegnemtTable";	
         mysql->filedsName  =  "Status";
         mysql->filedsValue =  "-1";                                
         SN=SQL_findSNo(mysql,resultSNo);                          


         if(SN>0){
	     for(i =0 ;i<SN;i++)
	        {
		    printf("SNo:%s",resultSNo[i]);	                                  
                    mysql->filedsValue  =  "1";                                             
                    mysql->mainKeyValue =  resultSNo[i];                                    //需要修改状态的光路号
                    if(!semaphore_p())  
                         exit(EXIT_FAILURE);                                                //P
                    rc=SQL_modify(mysql);
                    if( rc != SQLITE_OK ){
			 printf( "Modify SQL error\n");
			 sqlite3_free(zErrMsg);
		    }
                     if(!semaphore_v())                                                     //V
                         exit(EXIT_FAILURE);                 	 
	       }
         }

         mysql->filedsValue =  "-2";                                
         SN=SQL_findSNo(mysql,resultSNo);                          
         if(SN>0){
		 for(i =0 ;i<SN;i++)
		    {
		       printf("SNo:%s",resultSNo[i]);	                                  
                       mysql->filedsValue  =  "2";                                             
                       mysql->mainKeyValue =  resultSNo[i];                                 //需要修改状态的光路号
                       if(!semaphore_p())  
                            exit(EXIT_FAILURE);                                             //P
                       rc=SQL_modify(mysql);
                       if( rc != SQLITE_OK ){
			    printf( "Modify SQL error\n");
			   sqlite3_free(zErrMsg);
		       }
                       if(!semaphore_v())                                                   //V
                             exit(EXIT_FAILURE);                  	
	            }
         }

	 SQL_Destory(mysql);  
	 sqlite3_close(mydb);   
         return ;
} 

void addNewtoLink(int,siginfo_t*,void*);

/***otdr测试调度主进程***/
/*
    (1)  初始化链表
    (2)  创建信号机制，与BOA进程通信,与OTDR测试进程通信
          -->SA_SIGINFO标志，表示信号附带的参数可以传递到信号处理函数中
          -->SA_RESTART标志自动重启被中断的慢系统调用.可以避免正在执行网络访问（eg:接收OTDR数据过程中,
             接收到调度进程发来信号时保持与OTDR的正常通信,不至于发生OTDR数据接收错误.
             eg:   145:recv error --> Interrupted system call
    (3)  信号安装
          -->用于接收BOA的信号，通过附加信息区分消息类型（启动测试：120，取消测试：220）
          -->信号处理程序（addNewtoLink）类似中断处理，可以异步接收信号，进程内部通过传递的参数执行不同的任务.
    (4)  信号发送
          --->用于向OTDR测试进程发送信号，以启动一次测试任务.
          --->通过附加信息传递测试参数（类型type =300和光路SNo）形式为:300+SNO.
    (5)   执行调度程序
          --->A.获取待服务器节点（头节点）
          --->B.判断测节点是否到时，若到时则执行周期进行下一步(C)，否则等待该节点到时
          --->C.向otdrMain发送启动信号
                 获取OTDR测试进程PID
                 发送需要启动测试的光路号及测试类型：百位表示类型+十位个位表示SNo   (此处为:3XX,type=3,SNo=XX)     
          --->D.等待信号的成功处理消息
                  遇"3-OK"结束,未收到则挂起程序的执行.
          --->E.更新节点参数，并调度下一个需要启动的节点
                  节点参数主要更新测试的下次启动时间，下次启动时间=本次启动时间+时间间隔
          --->F.循环调度下个节点(to A).
    (6)信号处理
          --->当有信号到来，并处理成功，则flagNEW标识置位，主进程检擦到后会调度一次测试节点，获取最新链表的头节点进行测试任务
          --->再将flagNew标志置为0
*/
void main(void)
{
        testLinknode *p1;
         
        p1 =( testLinknode *) malloc (sizeof(testLinknode));
        otdr * testPar;

        sem_id = semget((key_t)1234, 1, 4777 | IPC_CREAT);                                //创建信号量 :每一个需要用到信号量的进程,在第一次启动的时候需要初始化信号量
        if(!set_semvalue())                                                               //程序第一次被调用，初始化信号量
        {  
            fprintf(stderr, "Failed to initialize semaphore\n");  
            exit(EXIT_FAILURE);  
        } 
        linkHead=Init_CycleLink();                                                        //初始化有序链表      
                     
        if(linkHead !=NULL)
            outPutALL(linkHead);
        else
            printf("Head:NULL");

/***************************************************************/
       int nowTime=0;
       int setTime = 0;
       int SNo=0;
       int intCM=0;

       int signum;
       union sigval mysigval;
       char* process;  
       int ret = 0;  
       int n;  
       pid_t cycPID[MAX_PID_NUM],alarmPID[MAX_PID_NUM];  
       int recvInt; 

        /*初始化信号机制（与BOA通信）*/
       struct sigaction act;
       int sig;
       sig=SIGUSR1;  
       sigemptyset(&act.sa_mask);
       act.sa_sigaction=addNewtoLink;
       act.sa_flags=SA_SIGINFO|SA_RESTART;                                                                                                                                                 
       if(sigaction(sig,&act,NULL)<0)                          
       {
              printf("install sigal error\n");
       }
        /*创建信号量 */
       sem_id = semget((key_t)1234, 1, 4777 | IPC_CREAT);    
       otdr_sem_id = semget((key_t)2468,1,4777 |IPC_CREAT);     


       process ="/web/cgi-bin/alarmMain";                        
       ret = get_pid_by_name(process, alarmPID, MAX_PID_NUM);   
       if(ret<=0)
          if(!setOTDRPV()) {                                                                //first launch (for modfiy)        
            exit(0);
          }                      
       /*if(!set_semvalue())                                                               
         {  
            fprintf(stderr, "Failed to initialize semaphore\n");  
            exit(EXIT_FAILURE);  
          } */
        /*周期调度主进程*/
        while(1)
        {
		p1=outFirstnode(linkHead);                                          //获取待服务节点（头节点）
                if(p1!=NULL){
                   SNo     = p1->SNo; 
                   intCM   = p1->CM;
                   setTime = p1->nextTestTime;   
                   nowTime = getLocalTimestamp(); 
                    
                }                             
                while(p1!=NULL){                                 
                    nowTime= getLocalTimestamp();             
	            if((p1!=NULL && nowTime >= setTime) || flagNew ==1)             //守护查询是否到时
                    {  
                                  /*信号处理*/                                      
                           if( flagNew ){
                                p1=outFirstnode(linkHead);
                                if(p1!=NULL){    
		                   SNo     = p1->SNo;  
                                   intCM   = p1->CM;
		                   setTime = p1->nextTestTime; 
                                   flagNew =0;  
                                }else{
				   flagNew =0;
                                   break;
                                }
                            }
                          if(p1!=NULL){
				   printf("SNo=%d   rtuCM=%d   Proid:%ld on cycTest!   NowTime:%ld    setTime:%4d\n",SNo,intCM,p1->timePeriod,getLocalTimestamp(),setTime); 
		                                 /*向otdrMain发送启动信号*/
				   if(!setOTDR_P())                                                //P
		                         exit(EXIT_FAILURE);   
				   process ="/web/cgi-bin/otdrMain";                        
				   ret = get_pid_by_name(process, cycPID, MAX_PID_NUM);  
				   printf("cycMain:process '%s' is existed? (%d): %c ", process, ret, (ret > 0)?'y':'n');  
				   signum=SIGRTMIN;//SIGUSR1;                                         
				   mysigval.sival_int = SNo+300;                                                      
				   for(n=0;n<ret;n++){                                      
					printf("otdrMain PID:%u\n", cycPID[n]);                  
					if(sigqueue(cycPID[n],signum,mysigval)==-1)
					       printf("send signal error\n");
				   }  
		                                /*等待信号的成功处理消息*/			    
				   recvInt = recvMessageQueue_D("3-OK",3333);
		                   if(recvInt==0){
		                        printf("cycMain Recv back message from otdrMain  sucessful!");
		                   }else{
		                        printf("cycMain Recv back message from otdrMain  Faild:Time out!");
		                   } 
		                   usleep(10000);                                                 //确保信号被处理完 
				   if(!setOTDR_V()) 
                                     exit(EXIT_FAILURE);                                          //V
                          }                                   
                                              /*更新头节点参数，执行周期调度*/
			  p1=outFirstnode(linkHead);        
		          if(p1!=NULL){
				SNo     = p1->SNo;
		                intCM   = p1->CM; 
				setTime = p1->nextTestTime;   
				nowTime = getLocalTimestamp(); 
		                printf("\n");
				linkHead= taskScheduler(linkHead,p1);              
		                printf("\n");
		          } 
                          break;
                    }	
                }
        }  
}

void addNewtoLink(int signum,siginfo_t *info,void *myact)
{
       printf("cycMain(R)the int value is %d \n",info->si_int);
       int SNo =0;
       if(info->si_int>270 && info->si_int <370)              //最大一次删除99个节点
       {
           SNo = info->si_int%270;
           linkHead = delete(linkHead,SNo);                   //删除节点
           outPutALL(linkHead);
           sendMessageQueue("270-OK");
	   return;
       }
       
       switch(info->si_int){
           case 120:{                                        //启动周期测试
		    linkHead=insertWaitingNode(linkHead);
		    outPutALL(linkHead);
                    sendMessageQueue("120-OK");
		    flagNew = 1;
		    break;
                  }
           case 220:{                                        //终止周期测试
                    linkHead=removeWaitingNode(linkHead);                   
                    outPutALL(linkHead);
                    sendMessageQueue("220-OK");
                    //flagNew = 1; 
		    break;
                  }

           case 260:{                                        //清除RTU模式
                    linkHead = removeAllNode(linkHead);                   
                    outPutALL(linkHead);
                    sendMessageQueue("260-OK");
                    flagNew = 1;
		    break;    
           }
          default:break;
      }
}

