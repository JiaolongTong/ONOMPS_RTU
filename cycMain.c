#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <signal.h>

#include "sql.h"
#include "common.h"
#include "uploadCycTestData.h"
#include "defaulttest.h"
#include "process.h"
#include "otdr.h"
 

typedef struct testLinknode testLinknode;
struct testLinknode{
	int    SNo;
	time_t    fristTestTime;
        time_t    nextTestTime;
        time_t    timePeriod;
	struct testLinknode *next;
};


int sem_id=0;                                //
int flagNew=0;
testLinknode *linkHead;
int n =0;

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
int isEmpty(testLinknode *head){
        return (head==NULL);
}

testLinknode *deleteFirst(testLinknode *head ){//O[1]
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
 testLinknode * outFirstnode(testLinknode *head)
{
        testLinknode *p0;
	if(head==NULL){
	//printf("This is a void execl");
		return(head);                                //NULL
	}
        p0 = (testLinknode *) malloc (sizeof(testLinknode ));
        
        p0->SNo           = head->SNo;

        p0->fristTestTime = head->fristTestTime;

        p0->nextTestTime  = head->nextTestTime;
        p0->timePeriod    = head->timePeriod;

	return(p0);
}


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

void outPutALL(testLinknode *head){
	testLinknode *p;

	p= head;
	if(p==NULL){
		printf("This is a void excel!\n");
		return ;
	}
	else
		printf("There are %d lines on testing:\n",n);
	while(p!=NULL){
		printf("SNo:%d,firstTime:%ld,nextTime%ld\n",p->SNo,p->fristTestTime,p->nextTestTime);
		p=p->next;
	}
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



testLinknode * Init_CycleLink(void)
{
	 sqlite3 *mydb;
	 char *zErrMsg = 0,*SNo;
	 int rc,i,SN=0;
	 sql *mysql;
	 char resultSNo[64][5];

         char **result = NULL;
         testLinknode *head,*node;
         head = link_creat();
         head = delete(head,0);                                     //创建一个空链表
         time_t T1,T2;
	 SNo = (char *) malloc(sizeof(char)*5);
	 mysql = SQL_Create();
	 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
	 if( rc != SQLITE_OK ){
		      printf( "Lookup SQL error: %s\n", zErrMsg);
		      sqlite3_free(zErrMsg);
		   }
	 mysql->db = mydb;
	 mysql->tableName   = "CycleTestSegnemtTable";	
         mysql->filedsValue =  "1";                                //光路状态为1 表示正在进行周期测试，因此在初始化过程中需要将其加入调度链表
         mysql->filedsName  =  "Status";
         SN=SQL_findSNo(mysql,resultSNo);
         if(SN>0){
		 for(i =0 ;i<SN;i++)
		    {
		       printf("SNo:%s",resultSNo[i]);
		       strcpy(SNo,resultSNo[i]);
		       mysql->filedsName    = "T1"; 
		       mysql->mainKeyValue  = SNo;
		       rc= SQL_lookup(mysql,&result);
                       T1 =str2Timestamp(result[0]);
		       printf("T1:%ld\n",T1);
		       mysql->filedsName    = "T2"; 
		       rc= SQL_lookup(mysql,&result);
                       printf("res:%s",result[0]);
                       T2= computTime(result[0]);
		       printf("T2:%ld\n",T2);

			node=(testLinknode *)malloc(sizeof(testLinknode));
			node->SNo =atoi(SNo);
			node->fristTestTime=T1;
			node->nextTestTime =getLocalTimestamp();
			node->timePeriod =  T2;  //5*(i+1); 
			head=insert(head,node);
		    }
         }

         free(SNo);
	 SQL_Destory(mysql);  
	 sqlite3_close(mydb);
  
	if(result != NULL)
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

testLinknode * insertWaitingNode(testLinknode *head)                    //插入数据库中状态为-1 的节点，并把状态修改为1
{
	 sqlite3 *mydb;
	 char *zErrMsg = 0,*SNo;
	 int rc,i,SN=0;
	 sql *mysql;
	 char resultSNo[64][5];

         char **result = NULL;
         time_t T1,T2;
         testLinknode *node,*find;
	 SNo = (char *) malloc(sizeof(char)*5);
	 mysql = SQL_Create();
	 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
	 if( rc != SQLITE_OK ){
		      printf( "Lookup SQL error: %s\n", zErrMsg);
		      sqlite3_free(zErrMsg);
		   }
	 mysql->db = mydb;
	 mysql->tableName   = "CycleTestSegnemtTable";	
         mysql->filedsValue =  "-1";                                //光路状态为1 表示正在进行周期测试，因此在初始化过程中需要将其加入调度链表
         mysql->filedsName  =  "Status";
         SN=SQL_findSNo(mysql,resultSNo);                           //查找光路状态为待启动的记录  光路状态为“-1”
         if(SN>0){
		 for(i =0 ;i<SN;i++)
		    {
		       printf("SNo:%s",resultSNo[i]);
		       strcpy(SNo,resultSNo[i]);
		       mysql->filedsName    = "T1"; 
		       mysql->mainKeyValue  = SNo;
		       rc= SQL_lookup(mysql,&result);
                       T1 =str2Timestamp(result[0]);
		       printf("T1:%ld\n",T1);
		       mysql->filedsName    = "T2"; 
		       rc= SQL_lookup(mysql,&result);
                       printf("res:%s",result[0]);
                       T2= computTime(result[0]);
		       printf("T2:%ld\n",T2);
		       node=(testLinknode *)malloc(sizeof(testLinknode));
		       node->SNo =atoi(SNo);
		       node->fristTestTime=T1;
                        if(T1<=getLocalTimestamp())
			    node->nextTestTime =getLocalTimestamp();
			else
                            node->nextTestTime =T1;
			node->timePeriod =  T2;                  
                        find=findNode(head,node->SNo);            // 查看链表中是否已经存在SNo光路
                        if(find ==NULL)
                        {
                           head=insert(head,node);                // 若不存在直接将节点node插入链表
		        }else{
		           head = delete(head,node->SNo);         // 若已经存在，则先将其删除在插入链表。这样可保证更新的节点任然有序
                           head=  insert(head,node); 
                        }

                       mysql->filedsValue  =  "1";                // 更新光路状态为“1” 已启动
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

         free(SNo);
	 SQL_Destory(mysql);  
	 sqlite3_close(mydb); 

	if(result != NULL)
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


testLinknode * removeWaitingNode(testLinknode *head) 
{
	 sqlite3 *mydb;
	 char *zErrMsg = 0,*SNo;
	 int rc,i,SN=0,intSNo=0;
	 sql *mysql;
	 char resultSNo[64][5];
         testLinknode *find;
	 SNo = (char *) malloc(sizeof(char)*5);
	 mysql = SQL_Create();
	 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
	 if( rc != SQLITE_OK ){
		      printf( "Lookup SQL error: %s\n", zErrMsg);
		      sqlite3_free(zErrMsg);
		   }
	 mysql->db = mydb;
	 mysql->tableName   = "CycleTestSegnemtTable";	
         mysql->filedsValue =  "-2";                                //光路状态为1 表示正在进行周期测试，因此在初始化过程中需要将其加入调度链表
         mysql->filedsName  =  "Status";
         SN=SQL_findSNo(mysql,resultSNo);                           //查找光路状态为待启动的记录  光路状态为“-1”
         if(SN>0){
		 for(i =0 ;i<SN;i++)
		    {
		       printf("SNo:%s",resultSNo[i]);
		       strcpy(SNo,resultSNo[i]);		
		       intSNo =atoi(SNo);                                    
                       find=findNode(head,intSNo);                  // 查看链表中是否已经存在SNo光路
                        if(find ==NULL)
                        {
                          printf("Don't have SNo=%d node in this Link!\n",intSNo);                // 若不存在不做任何处理，直接退出
		        }else{
                          head = delete(head,intSNo);                                             // 若存:(1)先从链表中删除该SNo节点
                          mysql->filedsValue  =  "2";                                             //      (2)再更新光路状态为“2”            已终止
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

         free(SNo);
	 SQL_Destory(mysql);  
	 sqlite3_close(mydb);   
         return(head);
}

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
         mysql->filedsValue =  "-1";                                //光路状态为-1 表示正在进行周期测试，因此在初始化过程中需要修改其状态"1"
         SN=SQL_findSNo(mysql,resultSNo);                           //数据库中查找所有状态为“-1”需要清理的光路


         if(SN>0){
	     for(i =0 ;i<SN;i++)
	        {
		    printf("SNo:%s",resultSNo[i]);	                                  
                    mysql->filedsValue  =  "1";                                             
                    mysql->mainKeyValue =  resultSNo[i];                                    //需要修改状态的光路号
         if(!semaphore_p())  
                 exit(EXIT_FAILURE);                                                        //P
                    rc=SQL_modify(mysql);
                    if( rc != SQLITE_OK ){
			 printf( "Modify SQL error\n");
			 sqlite3_free(zErrMsg);
		    }
                     if(!semaphore_v())                                                    //V
                         exit(EXIT_FAILURE);                 	
	       }
         }

         mysql->filedsValue =  "-2";                                //光路状态为-2 表示等待取消周期测试，因此在初始化过程中需要修改其状态"2"
         SN=SQL_findSNo(mysql,resultSNo);                           //数据库中查找所有状态为“-2”需要清理的光路
         if(SN>0){
		 for(i =0 ;i<SN;i++)
		    {
		       printf("SNo:%s",resultSNo[i]);	                                  
                       mysql->filedsValue  =  "2";                                             
                       mysql->mainKeyValue =  resultSNo[i];                                     //需要修改状态的光路号
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

	 SQL_Destory(mysql);  
	 sqlite3_close(mydb);   
         return ;
}
otdr *lookupParm(int SNo)
{
	 sqlite3 *mydb;
         otdr    *myotdr;
	 char *zErrMsg = 0;
	 int rc;
         uint32_t uint_a;
         float float_a;
	 sql  *mysql;
         char **result = NULL;
         char *sno;
         sno = (char *) malloc(sizeof(char)*10);
         mysql  =  SQL_Create();
         myotdr  = OTDR_Create();
	 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
         if( rc != SQLITE_OK ){
	      printf( "Lookup SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	 }

         mysql->db          =  mydb;
	 mysql->tableName   =  "DefaultTsetSegmentTable";	
	 uint32tostring(SNo,sno);
	 mysql->mainKeyValue  =  sno;


         mysql->filedsName    =  "P01";
         rc=SQL_lookup(mysql,&result);
         if( rc != SQLITE_OK ){
	      printf( "Lookup SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	 }else{
         uint_a = strtoul (result[0], NULL, 0);  
	 myotdr->MeasureLength_m = uint_a;    
         }



         mysql->filedsName    = "P02";
	 rc= SQL_lookup(mysql,&result);
         if( rc != SQLITE_OK ){
	      printf( "Lookup SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	 }else{
         uint_a = strtoul (result[0], NULL, 0);  
	 myotdr->PulseWidth_ns = uint_a;    
         }


         mysql->filedsName    = "P03";
	 rc= SQL_lookup(mysql,&result);
         if( rc != SQLITE_OK ){
	      printf( "Lookup SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	 }else{
         uint_a = strtoul (result[0], NULL, 0);  
	 myotdr->Lambda_nm = uint_a;    
         }


         mysql->filedsName    = "P04";
	 rc= SQL_lookup(mysql,&result);
         if( rc != SQLITE_OK ){
	      printf( "Lookup SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	 }else{
         uint_a = strtoul (result[0], NULL, 0);  
	 myotdr->MeasureTime_ms = uint_a;    
         }

         mysql->filedsName    = "P05";
	 rc= SQL_lookup(mysql,&result);
         if( rc != SQLITE_OK ){
	      printf( "Lookup SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	 }else{
         float_a = atof (result[0]);  
	 myotdr->n = float_a;    
         }

         mysql->filedsName    = "P06";
	 rc= SQL_lookup(mysql,&result);
         if( rc != SQLITE_OK ){
	      printf( "Lookup SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	 }else{
         float_a = atof (result[0]);  
	 myotdr->NonRelectThreshold = float_a;    
         }


         mysql->filedsName    = "P07";
	 rc= SQL_lookup(mysql,&result);
         if( rc != SQLITE_OK ){
	      printf( "Lookup SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	 }else{
         float_a = atof (result[0]);  
	 myotdr->EndThreshold = float_a;    
         }



	free(sno);
	SQL_Destory(mysql);  
	sqlite3_close(mydb);
	
	if(result != NULL)
	{
	     if(result[0] != NULL)
		{
			free(result[0]);
			result[0] = NULL;
		}

		free(result);
		result = NULL;
	}

        return(myotdr);
}


void addNewtoLink(int,siginfo_t*,void*);
int main()
{
        testLinknode *p1;
        p1 =( testLinknode *) malloc (sizeof(testLinknode));
       // int ifNULL=-1;
        otdr * testPar;

        sem_id = semget((key_t)1234, 1, 4777 | IPC_CREAT);                                //创建信号量 :每一个需要用到信号量的进程,在第一次启动的时候需要初始化信号量
        if(!set_semvalue())                                                               //程序第一次被调用，初始化信号量
        {  
            fprintf(stderr, "Failed to initialize semaphore\n");  
            exit(EXIT_FAILURE);  
        } 


        flushWaitingSNo();                                      //清理数据库中的等待状态（Status为"-1"，"-2"）

        linkHead=Init_CycleLink();                              //初始化有序链表
                                      
        if(linkHead !=NULL)
            outPutALL(linkHead);
        else
            printf("Head:NULL");

        testPar =OTDR_Create();                                 //新建一个OTDR测试对象 

/******周期测试调度主进程********/

       int nowTime=0;
       int setTime = 0;
       int SNo=0;
       struct sigaction act;
       int sig;
       pid_t pid;
       
       pid=getpid();
       printf("PID:%d",pid);
       sig=SIGUSR1;  
       sigemptyset(&act.sa_mask);
       act.sa_sigaction=addNewtoLink;
       act.sa_flags=SA_SIGINFO|SA_RESTART;                     //(1)SA_SIGINFO，当设定了该标志位时，表示信号附带的参数可以传递到信号处理函数中。 
                                                               //(2)SA_RESTART可以自动重启被中断的慢系统调用.
                                                               //   可以避免正在执行网络访问（eg:接收OTDR数据过程中），接收到BOA发来信号时不至于发生OTDR数据接收错误.
       if(sigaction(sig,&act,NULL)<0)                          //   eg:   145:recv error --> Interrupted system call
       {
              printf("install sigal error\n");
       }

       sem_id = semget((key_t)1234, 1, 4777 | IPC_CREAT);                                //创建信号量 
       if(!set_semvalue())                                                               //程序第一次被调用，初始化信号量
         {  
            fprintf(stderr, "Failed to initialize semaphore\n");  
            exit(EXIT_FAILURE);  
          } 

        while(1)
        {
		p1=outFirstnode(linkHead);                       //获取待服务器节点（头节点）
                if(p1!=NULL){
                   SNo     = p1->SNo;   
                   setTime = p1->nextTestTime;   
                   nowTime = getLocalTimestamp(); 
                    
                }                             
                while(p1!=NULL){                                 //守护查询是否到时
 
                    nowTime= getLocalTimestamp();
                     
	            if((p1!=NULL && nowTime >= setTime) || flagNew ==1) 
                    {                                        
                           if(flagNew ){
                                p1=outFirstnode(linkHead);
                                if(p1!=NULL){    
		                   SNo     = p1->SNo;   
		                   setTime = p1->nextTestTime; 
                                   flagNew =0;  
                                }else{
				   flagNew =0;
                                   break;
                                }
                            }
		           printf("*SNo =%4d *** Proid:%ld *** ON OTDRTest! NowTime:%ld *** setTime:%4d\n ",SNo,p1->timePeriod,getLocalTimestamp(),setTime);  
		           testPar = lookupParm(SNo);
			   printf("------Time:%ld--------\n"  ,getLocalTimestamp());
			   printf("SNo-uint -[%d]\n",SNo);
			   printf("P01-uint -[%d]\n",testPar->MeasureLength_m);
			   printf("P02-uint -[%d]\n",testPar->PulseWidth_ns);
			   printf("P03-uint -[%d]\n",testPar->Lambda_nm);
			   printf("P04-uint -[%d]\n",testPar->MeasureTime_ms);
			   printf("P05-float-[%f]\n",testPar->n);
			   printf("P06-float-[%f]\n",testPar->NonRelectThreshold);
			   printf("P07-float-[%f]\n",testPar->EndThreshold);
			   printf("\n");
		
		           OtdrTest(testPar);
		           sleep(1);                                       //等待OTDR测试结束（此处获取测试参数并执行OTDR测试）
		           upload(SNo,testPar);                            //在此判断网络超时
                          // p1=NULL;
//printf("SNo=%d,fristTestTime=%ld,nextTestTime=%ld,timePeriod=%ld",p1->SNo,p1->fristTestTime,p1->nextTestTime,p1->timePeriod);  
			   p1=outFirstnode(linkHead); 
         
                           if(p1!=NULL){
				   SNo     = p1->SNo;   
				   setTime = p1->nextTestTime;   
				   nowTime = getLocalTimestamp(); 
                                   printf("1\n");
			           linkHead= taskScheduler(linkHead,p1);           //运行调度器，切换测试节点  
                                   printf("2\n");
                           } 

                           break;
                    }	
                }
        } 
 
/******调度主进程end********/      
        return 0;
}

void addNewtoLink(int signum,siginfo_t *info,void *myact)
{
       printf("the int value is %d \n",info->si_int);
       switch(info->si_int){
           case 120:{                                         //启动周期测试
		    linkHead=insertWaitingNode(linkHead);
		    outPutALL(linkHead);
                    sendMessageQueue("120-OK");
		    flagNew = 1;
		    break;
                  }
           case 220:{                                         //终止周期测试
                    linkHead=removeWaitingNode(linkHead);                   
                    outPutALL(linkHead);
                    sendMessageQueue("220-OK");
                    flagNew = 1;
		    break;
                  }
          default:break;
            }
}

