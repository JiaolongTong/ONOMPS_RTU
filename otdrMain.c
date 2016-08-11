#include "common.h"
#include "otdr.h"
#include "sql.h"
#include "uploadCycTestData.h" 
#include "process.h"

/***节点结构***/
/*
    SNO:光路号
    CM :局站号
    type:测试类型（具有优先级）
         --->type = 1 --------------> 点名测试（优先级最高）
         --->type = 2 --------------> 告警测试（优先级次高）
         --->type = 3 --------------> 周期测试（优先级最低）
    creat_time:节点创建的时间
*/
typedef struct otdrNode otdrNode;
struct otdrNode{
	int    SNo;                                 
        int    CM ;                                 
        int    type;                                	                                                                                       
        time_t creat_time;                          
	struct otdrNode *next;
};


/*全局变量*/
int sem_id=0;             //信号量ID（数据库互斥访问）
int n =0;                 //链表节点数
otdrNode *linkHead;       //链表头


/***插入有序节点****/
/*
     (1) 如果测试类型不同,插入到第一个类型比newNode大的节点之前.
     (2) 如果测试类型相同,按照插入时间的先后排序，先到先排，后到后排.
*/
otdrNode *insert(otdrNode *head,otdrNode *newNode)
{

        otdrNode *current;
        otdrNode *pre;
        current = (otdrNode *) malloc (sizeof(otdrNode ));
        pre = (otdrNode *) malloc (sizeof(otdrNode ));
	pre     = NULL;
	current = head;
        while(current!=NULL&&current->type <= newNode-> type){   
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
otdrNode *link_creat()
{
	otdrNode *head,*p1;
        head =   (otdrNode *) malloc (sizeof(otdrNode ));
        p1   =   (otdrNode *) malloc (sizeof(otdrNode ));
	head =   NULL;
        p1->SNo  =0;
        p1->CM   =0;
        p1->type =0;
        p1->creat_time =0;
	head = insert(head,p1);
	return(head);
}

/***删除节点***/
/*
   (1)以光路号SNo为索引
*/
otdrNode *delete(otdrNode *head,int SNo){
	otdrNode *p1,*p2;
	if(head==NULL){
		printf("There don't have node in OTDR testLink!\n");
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
otdrNode *findNode(otdrNode *head,int SNo)
{
	otdrNode * current;
        current = head;
        while(current!=NULL){
            if(current->SNo == SNo)  return current;

	     current = current->next;
        }
        return NULL;
}

/***判断链表是否为空***/
int isEmpty(otdrNode *head)
{
        return (head==NULL);
}

/***删除头节点***/
/*
    (1)链表非空的前提下才能删除
*/
otdrNode *deleteFirst(otdrNode *head )//O[1]
{
        if (isEmpty(head)){
            return NULL;
        }
        otdrNode *temp;
        temp = (otdrNode *) malloc (sizeof(otdrNode ));
        temp = head;
        head = head->next;
        n--;
        return temp;
    }


/***输出头节点***/
/*
    (1)链表非空情况下才能输出
*/
otdrNode * outFirstnode(otdrNode *head)
{
        otdrNode *p0;
	if(head==NULL)
	     return(head);                               
        p0 = (otdrNode *) malloc (sizeof(otdrNode )); 
        p0->SNo           = head->SNo;
        p0->CM            = head->CM ;
        p0->type          = head->type;
        p0->creat_time    = head->creat_time;
	return(p0);
}
/***遍历链表***/
void outPutALL(otdrNode *head){
	otdrNode *p;

	p= head;
	if(p==NULL){
		printf("There don't have node in OTDR testLink!\n");
		return ;
	}
	else
		printf("There are %d tests on working line.\n",n);
	while(p!=NULL){
		printf("SNo:%d,type:%d,CM:%d,creat_time:%ld\n",p->SNo,p->type,p->CM,p->creat_time);
		p=p->next;
	}
}

/***初始化链表****/
/*
    (1)创建一个空链表
    (2)删除头节点，形成空链表
*/
otdrNode * Init_CycleLink(void)
{
         otdrNode *head,*node;
         head = link_creat();
         head = delete(head,0);                                        
         return(head);
}

/***插入一个测试节点***/
/*
     (1)根据测试类型在数据库中查询光路（SNo）对应的局站号（rtuCM）
     (2)新建节点，设定好调度参数（测试类型，光路号，局站号，创建时间）
     (3)有序插入节点到调度链表
*/
otdrNode * insertTestNode(otdrNode *head,int type,int intSNo)          //插入数据库中状态为-1 的节点，并把状态修改为1
{
	 sqlite3 *mydb;
	 char *zErrMsg = 0,*SNo;
	 int rc;
	 sql *mysql;
         char **result = NULL;
         int    CM=0;
         otdrNode *node;
	 SNo = (char *) malloc(sizeof(char)*5);
         uint32tostring((uint32_t)intSNo,SNo);
         if(type!=1){
		 mysql = SQL_Create();
		 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
		 if( rc != SQLITE_OK ){
			      printf( "Lookup SQL error: %s\n", zErrMsg);
			      sqlite3_free(zErrMsg);
			   }
		 mysql->db = mydb;
		 if(type==3)
		    mysql->tableName     = "CycleTestSegnemtTable";
		 if(type==2)
		    mysql->tableName     = "DefaultTsetSegmentTable";	   
		 mysql->filedsName    = "rtuCM"; 
		 mysql->mainKeyValue  = SNo;
		 rc= SQL_lookup(mysql,&result);
		 CM =atoi(result[0]);
		 printf("CM:%d\n",CM); 
		 SQL_Destory(mysql);  
		 sqlite3_close(mydb); 
         }
	 node=(otdrNode *)malloc(sizeof(otdrNode));
	 node->SNo =intSNo;
         node->type=type;
         if(type!=1)
            node->CM  =CM;
         else
            node->CM  =0;                       
         head=insert(head,node);   
	 free(SNo);  
	 if(result!= NULL)
	   {
	       if(result[0] != NULL)
	          {
		      free(result[0]);
		      result[0] = NULL;
		  }

		free(result);
		result = NULL;
	   }                                  
         return(head);
}
void addNewtoLink(int signum,siginfo_t *info,void *myact);


/***otdr测试调度主进程***/
/*
    (1)  初始化链表
    (2)  创建信号机制，与BOA进程通信(SIG = SIGUSR1)
          -->SA_SIGINFO标志，表示信号附带的参数可以传递到信号处理函数中
          -->SA_RESTART标志自动重启被中断的慢系统调用.可以避免正在执行网络访问（eg:接收OTDR数据过程中,
             接收到调度进程发来信号时保持与OTDR的正常通信,不至于发生OTDR数据接收错误.
             eg:   145:recv error --> Interrupted system call
          -->信号安装，完成安装过程后，该进程就可以接收其他进程的消息，对方通过PID指定将信号发送到本进程
          -->信号处理程序（addNewtoLink）类似中断处理，可以异步接收信号，进程内部通过传递的参数执行不同的任务.
    (3)   执行调度程序
          --->A.获取待服务器节点（头节点）
          --->B.查询光路测试参数
          --->C.执行OTDR测试
          --->D.根据测试类型回传数据文件
                点名测试：向BOA发送消息队列，由BOA回传.
                周期、告警测试:调用HTTPClient，向指定服务器回传.
          --->E.测试完毕，删除当前测试节点.
          --->F.循环调度下个节点(to A).
*/

void main(void)
{
        otdrNode *p1;
        int SNo=0;
        int intCM=0;
        int type=0;
        otdr * testPar;
        
        
        /*初始化测试链表*/
        p1 =( otdrNode *) malloc (sizeof(otdrNode));
        linkHead=Init_CycleLink();
        /*初始化信号机制（IPC）*/
        struct sigaction act;
        int sig;
        sig=SIGUSR1;  
        sigemptyset(&act.sa_mask);
        act.sa_sigaction=addNewtoLink;
        act.sa_flags=SA_SIGINFO|SA_RESTART;                                                                                                                                                               
        if(sigaction(sig,&act,NULL)<0){                              
          printf("install sigal error\n");
        }
       /*执行调度程序*/
        while(1){
	    p1=outFirstnode(linkHead);                            
                if(p1!=NULL){
                   SNo     = p1->SNo; 
                   intCM   = p1->CM;
                   type    = p1->type;
                   testPar = OTDR_Create();  
                   
		   testPar = lookupParm(SNo,type);                      
	           printf("NowTime:%ld,Type:%d\n"   ,getLocalTimestamp(),type);
	           printf("SNo-uint -[%d]\n",SNo);
	           printf("P01-uint -[%d]\n",testPar->MeasureLength_m);
	           printf("P02-uint -[%d]\n",testPar->PulseWidth_ns);
		   printf("P03-uint -[%d]\n",testPar->Lambda_nm);
		   printf("P04-uint -[%d]\n",testPar->MeasureTime_ms);
		   printf("P05-float-[%f]\n",testPar->n);
		   printf("P06-float-[%f]\n",testPar->NonRelectThreshold);
		   printf("P07-float-[%f]\n",testPar->EndThreshold);	
		   OtdrTest(testPar);		  
                   if(type == 1)
                      sendMessageQueue_B("1-OK");  
                   else
                      upload(SNo,intCM,testPar);                                  
                   printf("\n");
                   printf("-------OTDR--Test-------\n");
                   OTDR_Destory(testPar);                          
                   linkHead = delete(linkHead,SNo);                
                   sleep(1);
                   outPutALL(linkHead);
                  
                }    	
        }
}

/***加入新的测试节点***/
/*
   (1)信号附加参数是：type*100+SNo形式
      --->type=1 :加入一个点名测试节点,测试执行完毕，由main向BOA回复“1-OK”消息队列
      --->type=2 :加入一个告警测试节点,节点插入成功，向告警测试进程(alarmMain)发送“2-OK”消息队列
      --->type=3 :加入一个告周期测试节点,节点插入成功，向周期测试进程(cycMain)发送“3-OK”消息队列
   (2)同一类型的消息，按照信号到来的时间，依次插入测试链表
      --->可以解决多个客户端（WEB）向同一个光路，几乎同时进行点名测试。
*/
void addNewtoLink(int signum,siginfo_t *info,void *myact)
{
       int type=0,SNo=0;
       type=info->si_int/100;
       SNo =info->si_int%100;
       printf("type:%d,SNo:%d,",type,SNo);
       switch(type){
           case 1:{                                               
		    linkHead=insertTestNode(linkHead,type,SNo);
		    outPutALL(linkHead);
                    //sendMessageQueue_B("1-OK");                   在主进程中回复
		    break;
                  }
           case 2:{                                                
                    linkHead=insertTestNode(linkHead,type,SNo);                   
                    outPutALL(linkHead);
                    sendMessageQueue_B("2-OK");
		    break;
                  }
           case 3:{                                                
                    linkHead=insertTestNode(linkHead,type,SNo);                   
                    outPutALL(linkHead);
                    sendMessageQueue_B("3-OK");

		    break;
                  }
          default:break;
            }
}




