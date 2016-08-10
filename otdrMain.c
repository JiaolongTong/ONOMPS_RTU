#include "common.h"
#include "otdr.h"
#include "sql.h"
#include "uploadCycTestData.h" 
#include "process.h"



typedef struct otdrNode otdrNode;
struct otdrNode{
	int    SNo;
        int    CM ;
        int    type;                                //type = 1 --------------> named test  
	                                            //type = 2 --------------> alarm test
                                                    //type = 3 --------------> cycle test
        time_t creat_time;                          //the time of this node created
	struct otdrNode *next;
};

int n =0;
int sem_id=0;       
otdrNode *linkHead;

otdrNode *insert(otdrNode *head,otdrNode *newNode)
{

        otdrNode *current;
        otdrNode *pre;
        current = (otdrNode *) malloc (sizeof(otdrNode ));
        pre = (otdrNode *) malloc (sizeof(otdrNode ));
	pre     = NULL;
	current = head;
        while(current!=NULL&&current->type <= newNode-> type){                  //插入到第一个比newNode大的节点之前, if tpye is same ,order by insert time ,older is header. 
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

int isEmpty(otdrNode *head)
{
        return (head==NULL);
}

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

otdrNode * Init_CycleLink(void)
{
         otdrNode *head,*node;
         head = link_creat();
         head = delete(head,0);                                        //创建一个空链表
         return(head);
}

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
	 mysql = SQL_Create();
	 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
	 if( rc != SQLITE_OK ){
		      printf( "Lookup SQL error: %s\n", zErrMsg);
		      sqlite3_free(zErrMsg);
		   }
	 mysql->db = mydb;
	 mysql->tableName     = "CycleTestSegnemtTable";	
	 mysql->filedsName    = "rtuCM"; 
	 mysql->mainKeyValue  = SNo;
	 rc= SQL_lookup(mysql,&result);
         CM =atoi(result[0]);
	 printf("CM:%d\n",CM);
  
	 node=(otdrNode *)malloc(sizeof(otdrNode));
	 node->SNo =intSNo;
         node->type=type;
         node->CM  =CM;                
         head=insert(head,node);                                     // 将节点node插入链表

         free(SNo);
	 SQL_Destory(mysql);  
	 sqlite3_close(mydb); 

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
void main(void)
{
        otdrNode *p1;
        int SNo=0;
        int intCM=0;
        int type=0;
        otdr * testPar;
        struct sigaction act;
        int sig;
        p1 =( otdrNode *) malloc (sizeof(otdrNode));
        linkHead=Init_CycleLink();
        sig=SIGUSR1;  
        sigemptyset(&act.sa_mask);
        act.sa_sigaction=addNewtoLink;
        act.sa_flags=SA_SIGINFO|SA_RESTART;                         //(1)SA_SIGINFO，当设定了该标志位时，表示信号附带的参数可以传递到信号处理函数中。 
                                                                    //(2)SA_RESTART可以自动重启被中断的慢系统调用.
                                                                    //   可以避免正在执行网络访问（eg:接收OTDR数据过程中），接收到调度进程发来信号时保持与OTDR的正常通信,不至于发生OTDR数据接收错误.
       if(sigaction(sig,&act,NULL)<0){                              //   eg:   145:recv error --> Interrupted system call
          printf("install sigal error\n");
       }
        while(1){
	    p1=outFirstnode(linkHead);                              //获取待服务器节点（头节点）
                if(p1!=NULL){
                   SNo     = p1->SNo; 
                   intCM   = p1->CM;
                   type    = p1->type;
                   testPar = OTDR_Create();                     
		   testPar = lookupParm(SNo,type);                  //查询光路测试参数       
	           printf("NowTime:%ld\n"   ,getLocalTimestamp());
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
                      upload(SNo,intCM,testPar);                    //在此判断网络超时                   
                   printf("\n");
                   printf("-------OTDR--Test-------\n");
                   OTDR_Destory(testPar);                          
                   linkHead = delete(linkHead,SNo);                 //测试完毕，删除当前测试节点。
                   sleep(1);
                   outPutALL(linkHead);
                  
                }    	
        }
}

void addNewtoLink(int signum,siginfo_t *info,void *myact)
{
       int type=0,SNo=0;
       type=info->si_int/100;
       SNo =info->si_int%100;
       printf("type:%d,SNo:%d,",type,SNo);
       switch(type){
           case 1:{                                                 //加入一个点名测试节点
		    linkHead=insertTestNode(linkHead,type,SNo);
		    outPutALL(linkHead);
                    //sendMessageQueue_B("1-OK");
		    break;
                  }
           case 2:{                                                 //加入一个告警测试节点
                    linkHead=insertTestNode(linkHead,type,SNo);                   
                    outPutALL(linkHead);
                    sendMessageQueue_B("2-OK");
		    break;
                  }
           case 3:{                                                 //加入一个周期测试节点
                    linkHead=insertTestNode(linkHead,type,SNo);                   
                    outPutALL(linkHead);
                    sendMessageQueue_B("3-OK");

		    break;
                  }
          default:break;
            }
}




