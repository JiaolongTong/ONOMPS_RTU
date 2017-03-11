#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <signal.h>
#include "sql.h"
#include "common.h"
#include "process.h"
#include "myModbus.h"


#define  State0   0      //空闲状态 等待切换到任意有效状态
#define  StateA   1      //状态A 光电二极管A有光，光开关A切换成1-2接通  光开关B切换成1-2接通   
#define  StateB   2      //状态B 光电二极管B有光，光开关A切换成1-3接通  光开关B切换成1-3接通
#define  StateC   3      //无效状态，光电二极管都有光或者都无光 光开关A B 均保持


/***测试链表节点结构***/ //LinkA
/*
    PNo:     暂时不使用   后期若一个保护从模块上存在多个保护组使用  (主键)
    ModuleNo:当前主键  一个模块上只存在一个保护组
    SNoA:    从站模块上光电二极管A对应主站的光路号(获取实时光功率时使用)
    SNoB:    从站模块上光电二极管B对应主站的光路号(获取实时光功率时使用)
    PowerGateA:  从站模块上光电二极管A对应的光功率阈值
    PowerGateB:  从站模块上光电二极管B对应的光功率阈值
    SwitchPos :  从站模块上光开关状态 (A B 同步)
    ConnectPos:
    beforeState:前一次监测时的状态
    nowState  :当前监测到的状态
*/

typedef struct checkProtectNode checkProtectNode;
struct checkProtectNode{
        int    PNo;                                        
        int    ModuleNo;
        int    SNoA;
        int    SNoB;           
        float  PowerGateA;
        float  PowerGateB;
        int    SwitchPos;
        int    ConnectPos;
        int    beforeState;
        int    nowState;
	struct checkProtectNode *next;
};

/*全局变量*/
int sem_id =0;                            //信号量ID（数据库互斥访问）      
int modbus_sem_id =0;                     //信号量ID（ModBus互斥访问） 
int link_sem_id=0;            
static int num_A =0;                             //测试链表节点数
static int flagNew=0;             //新节点插入标志
static checkProtectNode *linkHead_check_A;       //测试节点链表头

static modbus_mapping_t *modbus_mapping;  //

static modbus_t *modbus_tcp = NULL;

static netInfor serverInfor;



int initLinkPV()  
{  
    union semun sem_union;  
    sem_union.val = 1;  
    if(semctl(link_sem_id, 0, SETVAL, sem_union) == -1)  
        return 0;  
    return 1;  
}  
  
 void delLinkPV()  
{  
    union semun sem_union;  
  
    if(semctl(link_sem_id, 0, IPC_RMID, sem_union) == -1)  
        fprintf(stderr, "Failed to delete semaphore\n");  
}  
  
 int setLink_P()  
{  
    struct sembuf sem_b;  
    sem_b.sem_num = 0;  
    sem_b.sem_op = -1;//P()  
    sem_b.sem_flg = SEM_UNDO;  
    if(semop(link_sem_id, &sem_b, 1) == -1)  
    {  
        fprintf(stderr, "semaphore_p %s\n",strerror(errno));  
        return 0;  
    }  
    return 1;  
}  
  
 int setLink_V()  
{  
    struct sembuf sem_b;  
    sem_b.sem_num = 0;  
    sem_b.sem_op = 1;//V()  
    sem_b.sem_flg = SEM_UNDO;  
    if(semop(link_sem_id, &sem_b, 1) == -1)  
    {  
        fprintf(stderr, "semaphore_v failed %s\n",strerror(errno));  
        return 0;  
    }  
    return 1;  
}  

static void close_sigint(int dummy)
{
    if (serverInfor.server_socket != -1) {
	close(serverInfor.server_socket);
    }
    modbus_free(modbus_tcp);
    modbus_mapping_free(modbus_mapping);
    //delLinkPV();
    printf("exit normally!\n");
    exit(dummy);
}
/*****************************LinkA-checkLink************************************/
/***插入测试节点****/
/*
     (1) 尾插法.
*/
checkProtectNode *insert_protect_node(checkProtectNode *head,checkProtectNode *newNode)
{
        checkProtectNode *current=NULL;
        checkProtectNode *pre=NULL;

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
/*


*/
checkProtectNode *creat_protect_link(){
	checkProtectNode *head=NULL,*p1=NULL;
        head = (checkProtectNode *) malloc (sizeof(checkProtectNode ));
        p1 =   (checkProtectNode *) malloc (sizeof(checkProtectNode ));
        head=NULL;
        p1->PNo            =0;
        p1->ModuleNo       =0;

        p1->SNoA           =0;
        p1->SNoB           =0;

        p1->PowerGateA     =0;
        p1->PowerGateB     =0;

        p1->SwitchPos      =0;
        p1->ConnectPos     =0;

        p1->beforeState    =0;
        p1->nowState       =0;
 
	head = insert_protect_node(head,p1);
	return(head);
}

/***删除节点***/
/*
   (1)以光路号PNo为索引
*/
checkProtectNode *delete_protect_node(checkProtectNode *head,int ModuleNo){
	checkProtectNode *p1=NULL,*p2=NULL;
	if(head==NULL){
		printf("This is a void execl");
		return(head);
	}
	p1= head;
	while(p1->ModuleNo!=ModuleNo && p1->next !=NULL){
		p2=p1;
		p1 =p1->next;
	}
	if(p1->ModuleNo==ModuleNo)
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
		printf("Sorry,the PNo you want to delete is not fount!\n");
	return(head);
}

/***查找结点***/
/*
   (1)以光路号ModuleNo为索引
*/
checkProtectNode *find_protect_node(checkProtectNode *head,int ModuleNo)
{
	checkProtectNode * current=NULL;
        current = head;
        while(current!=NULL){
            if(current->ModuleNo == ModuleNo)
                return current;
	    current = current->next;
        }
        return NULL;
}

/***遍历链表***/
void output_all_protect_node(checkProtectNode *head){
	checkProtectNode *p=NULL;
        int i=0;
	p= head;
	if(p==NULL){
		printf("Don't have node in protect test link!\n");
		return ;
	}
	else
		printf("There are %d nodes on ProtectSlaver link:\n",num_A);
	while(p!=NULL){
                printf("---------------------Node:%d------------------------\n",i+1);
                printf("|PNo:%4d                    ModuleNo:%4d            |\n",p->PNo,p->ModuleNo);  
                printf("|SNoA:%4d                   SNoB:%4d                |\n",p->SNoA,p->SNoB); 
                printf("|PowerGateA:%2.2f           PowerGateB:%2.2f        |\n",p->PowerGateA,p->PowerGateB); 
                printf("|SwitchPos:%4d              ConnectPos:%4d          |\n",p->SwitchPos ,p->ConnectPos); 
                printf("|beforeState:%4d            nowState:%4d            |\n",p->beforeState,p->nowState);   
                printf("---------------------------------------------------\n\n",i+1);
                p=p->next;
                i++;

	}
}


checkProtectNode * frist_protect_node(checkProtectNode *head)
{
        checkProtectNode *node=NULL;
	if(head==NULL){
		return(head);                               
	}
        node = (checkProtectNode *) malloc (sizeof(checkProtectNode ));    
        node=head;
	return(node);                               
}
/**************************************************************************/
/***初始化测试链表****/
/*
    (1)创建一个空链表
    (2)将数据库障碍告警测试表中具有启动标识（status=1）的光路，加入初始化链表。
          --->光路状态为1 表示正在进行障碍告警测试表，因此在初始化过程中需要将其加入调度链表.
          --->保证每次启动，需要进程测试的节点能加入链表（例如，停机重启之后).

*/
checkProtectNode * InitA_CycleLink(void)
{
	 sqlite3 *mydb;
	 sql  *mysql;
	 char *zErrMsg = 0;
         char  ModNo[10];
         int   PNo=0,SNoA=0,SNoB=0,SwitchPos=0,ConnectPos=0,ModuleType=0;
         float GateA=0.0,GateB=0.0;
	 int   rc,i,ret=-1,rednum=0,PN=0;
	 char resultModNo[64][5];
         char **result = NULL;
         checkProtectNode *head=NULL,*node=NULL;
         enum SlaveProtectTable slt;
         head = creat_protect_link();
         head = delete_protect_node(head,0);   
                               
	 mysql = SQL_Create();
	 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
	 if( rc != SQLITE_OK ){
	    printf( "Lookup SQL error\n");
	 }
	 mysql->db = mydb;
	 mysql->tableName   =  "SlaveProtectTable";	
         mysql->filedsValue =  "3";                                
         mysql->filedsName  =  "Status";
         PN=SQL_findModNo(mysql,resultModNo);
         if(PN>0){
		for(i =0 ;i<PN;i++){ 
		     strcpy(ModNo,resultModNo[i]);   
                     mysql->mainKeyValue  =  ModNo;                   // 获取所属模块类型 
                     mysql->tableName     =  "SubModuleTypeTable";
                     mysql->filedsName    =  "ModuleType";
                     if(SQL_lookupPar(mysql,&result,&rednum)<0)break; 
                     ModuleType=atoi(result[0]);
                     SQL_freeResult(&result,&rednum);  
                                     
                     if(ModuleType == MODE5_PROTECT_SLAVER ){
			     mysql->tableName     =  "SlaveProtectTable";
			     mysql->filedsName    =  "*";
			     SQL_lookupPar(mysql,&result,&rednum); 
			     PNo       =atoi(result[eSPT_PNo]);
			     SNoA      =atoi(result[eSPT_SNoA]);
			     SNoB      =atoi(result[eSPT_SNoB]);
			     GateA     =atof(result[eSPT_GateA]);
			     GateB     =atof(result[eSPT_GateB]);
			     SwitchPos =atoi(result[eSPT_SwitchPos]);
			     ConnectPos=atoi(result[eSPT_ConnectPos]); 
                             //SQL_freeResult(&result,&rednum);
   
		             node=(checkProtectNode *)malloc(sizeof(checkProtectNode));	
                             node->ModuleNo       =atoi(ModNo);        
                             node->PNo            =PNo;       
			     node->SNoA           =SNoA;
			     node->SNoB           =SNoB;
			     node->PowerGateA     =GateA;
			     node->PowerGateB     =GateB;
			     node->SwitchPos      =SwitchPos;
			     node->ConnectPos     =ConnectPos;	
			     node->beforeState    =State0;
			     node->nowState       =State0;                            
			     head=insert_protect_node(head,node); 

		    } 
                        
                    
		}
         }

	 SQL_Destory(mysql);  
	 sqlite3_close(mydb);
         return(head);
}


checkProtectNode * insertNode_from_modbusTCP(checkProtectNode *head,uint16_t ModNo){
	 sql  *mysql;
         sqlite3 *mydb;
         char **result=NULL;
         char   strModNo[10];
         int    rc,rednum =0,ModuleNo=0,PNo=0,SNoA=0,SNoB=0,SwitchPos=0,ConnectPos=0;
         float  GateA=0,GateB=0;
         checkProtectNode *node=NULL,*find=NULL;
	 uint32tostring(ModNo,strModNo);
	 mysql = SQL_Create();
	 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
	 if( rc != SQLITE_OK ){
	    printf( "Lookup SQL error\n");
	 }
	 mysql->db = mydb;
	 mysql->tableName    = "SlaveProtectTable";	
         mysql->mainKeyValue =  strModNo;                                         
         if(1==SQL_existIN_db(mysql)){
               mysql->filedsName  =  "*";
               SQL_lookupPar(mysql,&result,&rednum); 
	       PNo       =atoi(result[0]);
               ModuleNo  =atoi(result[1]);
               SNoA      =atoi(result[2]);
               SNoB      =atoi(result[3]);
               GateA     =atof(result[4]);
               GateB     =atof(result[5]);
               SwitchPos =atoi(result[6]);
               ConnectPos=atoi(result[7]);  
               //SQL_freeResult(&result,&rednum);
 
         }
         SQL_Destory(mysql);
	 sqlite3_close(mydb);
                                     
         node=(checkProtectNode *)malloc(sizeof(checkProtectNode));	
         node->ModuleNo       =ModuleNo;      
         node->PNo            =PNo;       
         node->SNoA           =SNoA;
	 node->SNoB           =SNoB;
	 node->PowerGateA     =GateA;
	 node->PowerGateB     =GateB;
	 node->SwitchPos      =SwitchPos;
	 node->ConnectPos     =ConnectPos;	
	 node->beforeState    =State0;
	 node->nowState       =State0;  
         find=find_protect_node(head,node->ModuleNo);                           
         if(find ==NULL){
                head=insert_protect_node(head,node);                
         }else{
		head = delete_protect_node(head,node->ModuleNo);         
                head=  insert_protect_node(head,node); 
         }
         return head;
}

checkProtectNode * deleteNode_from_modbusTCP(checkProtectNode *head,uint16_t ModNo){
     checkProtectNode *find=NULL;
     find=find_protect_node(head,ModNo);
     if(find !=NULL){
          head = delete_protect_node(head,ModNo); 
     }
     return head;
}


int updateProtectGroup(uint16_t ModNo,uint16_t SNoA,uint16_t SNoB,uint16_t SwitchPosA,uint16_t SwitchPosB,uint16_t ConnectPos,uint16_t Status){
       sql  *mysql;
       sqlite3 *mydb;
       char *zErrMsg=NULL;
       char strModNo[10], strSNoA[10], strSNoB[10], strSwitchPos[10],strConnectPos[10],strStatus[10];
       char strSQL[100];
       sprintf(strModNo,"%d",ModNo);
       sprintf(strSNoA,"%d",SNoA);
       sprintf(strSNoB,"%d",SNoB);
       sprintf(strSwitchPos,"%d",SwitchPosA);
       sprintf(strConnectPos,"%d",ConnectPos);
       sprintf(strStatus,"%d",Status);

       printf("strModNo,%d \n",ModNo);
       printf("strSNoA,%d \n",SNoA);
       printf("strSNoB,%d \n",SNoB);
       printf("strSwitchPos,%d \n",SwitchPosA);
       printf("strConnectPos,%d \n",ConnectPos);
       printf("strStatus,%d \n",Status);

       mysql = SQL_Create();
       if(SQLITE_OK!=sqlite3_open("/web/cgi-bin/System.db", &mydb)){
          printf( "Lookup SQL error: %s\n", zErrMsg);
	  sqlite3_free(zErrMsg);
       }
     
       mysql->db = mydb;
       mysql->tableName    = "SlaveProtectTable";	
       mysql->mainKeyValue =  strModNo; 

       if(1==SQL_existIN_db(mysql)){
		mysql->filedsName="SNoA";     
		mysql->filedsValue=strSNoA;
		SQL_modify(mysql);

		mysql->filedsName="SNoB";     
		mysql->filedsValue=strSNoB;
		SQL_modify(mysql);

		mysql->filedsName="SwitchPos";     
		mysql->filedsValue=strSwitchPos;
		SQL_modify(mysql);

		mysql->filedsName="ConnectPos";     
		mysql->filedsValue="0";
		SQL_modify(mysql);

		mysql->filedsName="Status";     
		mysql->filedsValue=strStatus;
		SQL_modify(mysql); 
  
       }else{          
                   //PNo,ModuleNo,SNoA,SNoB,GateA,GateB,SwitchPos,ConnectPos,belongCM,belongCLP,Status)
 
                   sprintf(strSQL,"%d,%d,%d,%d,%f,%f,%d,%d,%d,%d,%d",
                   ModNo+ConnectPos,  //PNo
                   ModNo,             //ModuleNo
                   SNoA,
                   SNoB,
                   -70.0,
                   -70.0,
                   SwitchPosA,
                   ConnectPos,
                   0,
                   0,
                   Status
                   );
                mysql->filedsValue = strSQL;
	        if(SQLITE_OK!=SQL_add(mysql)) printf( "Save SQL error\n");
	        else { printf("%s",strSQL);return -1;}
       }  

       SQL_Destory(mysql);  

       sqlite3_close(mydb);

       return 0;
}

int updateProtectGate(uint16_t ModNo,uint16_t powerGateA,uint16_t powerGateB,uint16_t Status){
       sql  *mysql;
       sqlite3 *mydb;
       char *zErrMsg=NULL;
       char strModNo[10], strPowerGateA[10], strPowerGateB[10],strStatus[10];
       char strSQL[100];
       int16_t tmp;

       float float_powerGateA,float_powerGateB;

       sprintf(strModNo,"%d",ModNo);

       tmp=uint16toint16(powerGateA);
       float_powerGateA=(float)tmp/100.0;
       sprintf(strPowerGateA,"%f",float_powerGateA);

       tmp=uint16toint16(powerGateB);
       float_powerGateB=(float)tmp/100.0;
       sprintf(strPowerGateB,"%f",float_powerGateB);

       sprintf(strStatus,"%d",Status);

       if(SQLITE_OK!=sqlite3_open("/web/cgi-bin/System.db", &mydb)){
          printf( "Lookup SQL error: %s\n", zErrMsg);
	  sqlite3_free(zErrMsg);
       }
       mysql = SQL_Create();
       mysql->db = mydb;
       mysql->tableName    = "SlaveProtectTable";	
       mysql->mainKeyValue =  strModNo; 
 
       if(1==SQL_existIN_db(mysql)){
		mysql->filedsName="GateA";     
		mysql->filedsValue=strPowerGateA;
		SQL_modify(mysql);
		mysql->filedsName="GateB";     
		mysql->filedsValue=strPowerGateB;
		SQL_modify(mysql);
		mysql->filedsName="Status";     
		mysql->filedsValue=strStatus;
		SQL_modify(mysql);    
       }else{          
                //PNo,ModuleNo,SNoA,SNoB,GateA,GateB,SwitchPos,ConnectPos,belongCM,belongCLP,Status)
                sprintf(strSQL,"%d,%d,%d,%d,%f,%f,%d,%d,%d,%d,%d",
                   ModNo+0,  //PNo
                   ModNo,             //ModuleNo
                   0,
                   0,
                   float_powerGateA,
                   float_powerGateB,
                   0,
                   0,
                   0,
                   0,
                   Status
                   );
                mysql->filedsValue = strSQL;
	        if(SQLITE_OK!=SQL_add(mysql)){
                      printf("Save SQL error\n");
                      return 0;
                }
	        else  printf("%s",strSQL);
       }  

       SQL_Destory(mysql);  
       sqlite3_close(mydb);
       return 1;
}
int updateBelongRTU(uint16_t ModNo,uint16_t belongCM,uint16_t belongCLP,uint16_t Status){
       sql  *mysql;
       sqlite3 *mydb;
       char *zErrMsg=NULL;
       char strModNo[10], strBelongCM[10], strBelongCLP[10],strStatus[10];
       char strSQL[100];
       int16_t tmp;
       float float_powerGateA,float_powerGateB;
       sprintf(strModNo,"%d",ModNo);
       sprintf(strBelongCM,"%d",belongCM);
       sprintf(strBelongCLP,"%d",belongCLP);
       sprintf(strStatus,"%d",Status);
       mysql = SQL_Create();

       if(SQLITE_OK!=sqlite3_open("/web/cgi-bin/System.db", &mydb)){
          printf( "Lookup SQL error: %s\n", zErrMsg);
	  sqlite3_free(zErrMsg);
       }
       mysql->db = mydb;
  
       mysql->tableName    = "SlaveProtectTable";	
       mysql->mainKeyValue =  strModNo; 
  
       if(1==SQL_existIN_db(mysql)){
		mysql->filedsName="ModuleNo";     
		mysql->filedsValue=strModNo;
		SQL_modify(mysql);

		mysql->filedsName="belongCM";     
		mysql->filedsValue=strBelongCM;       
		SQL_modify(mysql);

		mysql->filedsName="belongCLP";     
		mysql->filedsValue=strBelongCLP;
		SQL_modify(mysql);

		mysql->filedsName="Status";     
		mysql->filedsValue=strStatus;                                
		SQL_modify(mysql);

       }else{          
                //PNo,ModuleNo,SNoA,SNoB,GateA,GateB,SwitchPos,ConnectPos,belongCM,belongCLP,Status)
                sprintf(strSQL,"%d,%d,%d,%d,%f,%f,%d,%d,%d,%d,%d",
                   ModNo+0,           //PNo
                   ModNo,             //ModuleNo
                   0,
                   0,
                   -70.0,
                   -70.0,
                   0,
                   0,
                   belongCM,
                   belongCLP,
                   Status
                   );
                mysql->filedsValue = strSQL;
	        if(SQLITE_OK!=SQL_add(mysql)){
                      printf("Save SQL error\n");
                      return 0;
                }
	        else  printf("%s",strSQL);
       }  

       SQL_Destory(mysql);  

       sqlite3_close(mydb);

       return 1;

}

int updateSwitchPos(uint16_t ModNo,uint16_t SwitchPosA,uint16_t SwitchPosB){
       sql  *mysql;
       sqlite3 *mydb ;
       char *zErrMsg=NULL;
       char strModNo[10], strSwitchPosA[10], strSwitchPosB[10];
       if(SwitchPosA == SwitchPosB){
	       sprintf(strModNo,"%d",ModNo);
	       sprintf(strSwitchPosA,"%d",SwitchPosA);
	       sprintf(strSwitchPosB,"%d",SwitchPosB);
	       mysql = SQL_Create();
	       if(SQLITE_OK!=sqlite3_open("/web/cgi-bin/System.db", &mydb)){
		  printf( "Lookup SQL error: %s\n", zErrMsg);
		  sqlite3_free(zErrMsg);
	       }
	       mysql->db = mydb;
	       mysql->tableName    = "SlaveProtectTable";	
	       mysql->mainKeyValue =  strModNo; 
  
	       if(1==SQL_existIN_db(mysql)){
			mysql->filedsName="ModuleNo";     
			mysql->filedsValue=strModNo;
			SQL_modify(mysql);
			mysql->filedsName="SwitchPos";     
			mysql->filedsValue=strSwitchPosA;
			SQL_modify(mysql);  
	       }else{          
		        printf("Don't have protect module:%d\n",ModNo);
		        return 0;
	       }  

	       SQL_Destory(mysql);  
               sqlite3_close(mydb);
       }else{
             printf("illegal SwitchPosA:%d != SwitchPosB:%d\n",SwitchPosA,SwitchPosB);
             return 0;
       }
       return 1;

}
int deleteSlaverRecode(uint16_t ModNo){
       sql  *mysql;
       sqlite3 *mydb;
       char *zErrMsg=NULL;
       char strModNo[10];
       sprintf(strModNo,"%d",ModNo);
       mysql = SQL_Create();
       if(SQLITE_OK!=sqlite3_open("/web/cgi-bin/System.db", &mydb)){
          printf( "Lookup SQL error: %s\n", zErrMsg);
	  sqlite3_free(zErrMsg);
       }
       mysql->db = mydb;
       mysql->tableName    = "SlaveProtectTable";	
       mysql->mainKeyValue   = strModNo;
       if(1==SQL_existIN_db(mysql)){
              SQL_delete(mysql);
       }else{          
	      printf("Don't have protect module:%d\n",ModNo);
              return 0;
       }  
       SQL_Destory(mysql);  
               
       sqlite3_close(mydb);

       return 1;
}

int modbus_server(void)
{
   // modbus_t *modbus_tcp = NULL;
    modbus_t *modbus_rtu =NULL;
    int8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
    int master_socket,server_socket;
    fd_set rdset;
    fd_set refset;
    int    fdmax,rc,j;
    int    RegisterNumber,WriteAddress,ReadAddress;
    uint16_t ConnectPos;
    slaverModuleInformatin  *moduleState=NULL;
    modbus_tcp = modbus_new_tcp("0.0.0.0", 1502);
    modbus_mapping = modbus_mapping_new(MODBUS_MAX_READ_BITS, 0,
                                    MODBUS_MAX_READ_REGISTERS, 0);
    if (modbus_mapping == NULL) {
        fprintf(stderr, "Failed to allocate the mapping: %s\n",
                modbus_strerror(errno));
        modbus_free(modbus_tcp);
        return -1;
    }
    modbus_set_debug(modbus_tcp, FALSE);
    serverInfor.server_socket = modbus_tcp_listen(modbus_tcp, NB_CONNECTION);
    if (serverInfor.server_socket == -1) {
        fprintf(stderr, "Unable to listen TCP connection\n");
        modbus_free(modbus_tcp);
        return -1;
    }
    FD_ZERO(&refset);
    FD_SET(serverInfor.server_socket, &refset);
    fdmax = serverInfor.server_socket;
    for(;;){
        rdset = refset;
        if (select(fdmax+1, &rdset, NULL, NULL, NULL) == -1) {
            perror("Server select() failure.");
            close_sigint(1);
        }
        for (master_socket = 0; master_socket <= fdmax; master_socket++) {
            if(!FD_ISSET(master_socket, &rdset)) {
                continue;
            }
            if(master_socket ==serverInfor.server_socket) {                
                socklen_t addrlen;
                struct sockaddr_in clientaddr;
                int newfd;
                addrlen = sizeof(clientaddr);
                memset(&clientaddr, 0, sizeof(clientaddr));
                newfd = accept(serverInfor.server_socket, (struct sockaddr *)&clientaddr, &addrlen);
                if (newfd == -1) {
                    perror("Server accept() error");
                } else {
                    FD_SET(newfd, &refset);
                    if (newfd > fdmax) {
                        fdmax = newfd;
                    }
                    printf("New connection from %s:%d on socket %d\n",inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port, newfd);
                }
            }else{
                    modbus_set_socket(modbus_tcp, master_socket);                
                    rc = modbus_receive(modbus_tcp, query);
                    if (rc > 0) {

                    if(query[MBAP_LENGTH] == _FC_WRITE_MULTIPLE_REGISTERS){    //0x10
                       RegisterNumber    = (query[MBAP_LENGTH + NB_HIGH] << 8)     + query[MBAP_LENGTH + NB_LOW];
                       WriteAddress      = (query[MBAP_LENGTH + OFFSET_HIGH] << 8) + query[MBAP_LENGTH + OFFSET_LOW];   
                       modbus_reply(modbus_tcp, query, rc, modbus_mapping);                                                                                   
                       switch(WriteAddress){
                            case SET_SLAVER_GROUP_ADDRESS:{
                                     
                                     printf("设置从模块保护组!\n"); 
                                     moduleState=newSlaverModule();

                                     moduleState->ModNo=modbus_mapping->tab_registers[WriteAddress+MODULE_NUMBER];
                                     ReadAddress = moduleState->ModNo*0x0010;  
                                     //setLink_P();   
                      
                                     memcpy(moduleState,modbus_mapping->tab_registers+ReadAddress,sizeof(uint16_t)*9);  
       
                                     moduleState->detail.SNoA       = modbus_mapping->tab_registers[WriteAddress+SET_SLAVER_GROUP_SNoA];
                         
                                     moduleState->detail.SNoB       = modbus_mapping->tab_registers[WriteAddress+SET_SLAVER_GROUP_SNoB]; 
                     
                                     moduleState->detail.SwitchPosA = modbus_mapping->tab_registers[WriteAddress+SET_SLAVER_GROUP_SwitchPos];
                                 
                                     moduleState->detail.SwitchPosB = modbus_mapping->tab_registers[WriteAddress+SET_SLAVER_GROUP_SwitchPos];
                                 
                                     ConnectPos = modbus_mapping->tab_registers[WriteAddress+SET_SLAVER_GROUP_Connect];
                                  
                                     moduleState->detail.useFlag   |= ON_onlyGROUP; 
                                   
                                     memcpy(modbus_mapping->tab_registers+ReadAddress,moduleState,sizeof(uint16_t)*9);

                                  
                                     //setLink_V();
                                     //更新数据库
/*
                                     if(!semaphore_p())  
	                                exit(EXIT_FAILURE);  
                                     updateProtectGroup(
                                                        moduleState->ModNo,
                                                        moduleState->detail.SNoA,
                                                        moduleState->detail.SNoB,
                                                        moduleState->detail.SwitchPosA,
                                                        moduleState->detail.SwitchPosB,
                                                        ConnectPos,
                                                        moduleState->detail.useFlag);    
                                     if(!semaphore_v())  
	                                exit(EXIT_FAILURE);  
*/
				     //1*2开关控制 

				     if(!setModbus_P())                                              
					 exit(EXIT_FAILURE); 
                                   
				      modbus_rtu =newModbus(MODBUS_DEV,MODBUS_BUAD);
 
				      doOpticalProtectSwitch(modbus_rtu,(moduleState->ModNo-1)*4+SW_A,moduleState->detail.SwitchPosA,MODE5_PROTECT_SLAVER);
 
		                      doOpticalProtectSwitch(modbus_rtu,(moduleState->ModNo-1)*4+SW_B,moduleState->detail.SwitchPosB,MODE5_PROTECT_SLAVER);
 
				      freeModbus(modbus_rtu);
 
                                      modbus_rtu=NULL;  
 
                                     //插入测试节点
                                     if(moduleState->detail.useFlag==ON_autoPROTECT){
                                           printf("开启自动保护测试,模块%d!\n",moduleState->ModNo);
                                           
                                           linkHead_check_A=insertNode_from_modbusTCP(linkHead_check_A,moduleState->ModNo);
                                           
                                     }
                                      //flagNew=0;
                                      //usleep(1000);
			              if(!setModbus_V())                                                              
				          exit(EXIT_FAILURE);

          
  
         

                                     printf("模块状态:%d\n",moduleState->ModNo);
                                     for(j=0;j<9;j++){
                                         printf("[%d]=%d\n",j+ReadAddress,uint16toint16(modbus_mapping->tab_registers[j+ReadAddress]));
                                     }
                                     freeSlaverModule(moduleState);
                                     moduleState=NULL;
                                     
                                 }break;
                            case SET_SLAVER_GATE_ADDRESS:{

                                     printf("设置从模块检测门限   mb_mapping[%d]!\n",WriteAddress);
                                     moduleState=newSlaverModule();
                                     moduleState->ModNo=modbus_mapping->tab_registers[WriteAddress+MODULE_NUMBER];
                                     ReadAddress = moduleState->ModNo*0x0010;      
                                     //setLink_P();                            
                                     memcpy(moduleState,modbus_mapping->tab_registers+ReadAddress,sizeof(uint16_t)*9);  
                                     moduleState->detail.useFlag    |= ON_onlyGATE;
                                     memcpy(modbus_mapping->tab_registers+ReadAddress,moduleState,sizeof(uint16_t)*9);
                                     //setLink_V();
                                     printf("powerGateA:%d  powerGateB:%d \n",
                                             modbus_mapping->tab_registers[WriteAddress+SET_SLAVER_GATE_powerGateA],
                                             modbus_mapping->tab_registers[WriteAddress+SET_SLAVER_GATE_powerGateB]);
                                     //更新数据库
                                     if(!semaphore_p())  
	                                exit(EXIT_FAILURE); 
                                     updateProtectGate(
                                                       moduleState->ModNo,
                                                       modbus_mapping->tab_registers[WriteAddress+SET_SLAVER_GATE_powerGateA],
                                                       modbus_mapping->tab_registers[WriteAddress+SET_SLAVER_GATE_powerGateB],
                                                       moduleState->detail.useFlag);                                   
                                     //插入测试节点
                                     if(moduleState->detail.useFlag==ON_autoPROTECT){
                                           printf("开启自动保护测试,模块%d!\n",moduleState->ModNo); 
                                           flagNew=1;
                                           linkHead_check_A=insertNode_from_modbusTCP(linkHead_check_A,moduleState->ModNo);
                                           flagNew=0; 
                                     }

                                     if(!semaphore_v())  
	                                exit(EXIT_FAILURE); 

                                     printf("模块状态:%d\n",moduleState->ModNo);
                                     for(j=0;j<9;j++){
                                         printf("[%d]=%d\n",j+ReadAddress,uint16toint16(modbus_mapping->tab_registers[j+ReadAddress]));
                                     }
                                     freeSlaverModule(moduleState);
                                     moduleState=NULL;
                                     
                                 }break;
                            case DELETE_SLAVER_GROUP_ADDRESS:{

                                     printf("删除从模块保护组     mb_mapping[%d]!\n",WriteAddress);
                                     moduleState=newSlaverModule();
                                     moduleState->ModNo=modbus_mapping->tab_registers[WriteAddress+MODULE_NUMBER];
                                     ReadAddress = moduleState->ModNo*0x0010;  
                                     moduleState->detail.SNoA       = 0;
                                     moduleState->detail.SNoB       = 0;  
                                     moduleState->detail.SwitchPosA = 0;
                                     moduleState->detail.SwitchPosB = 0;
                                     moduleState->detail.powerValueA= 0;
                                     moduleState->detail.powerValueB= 0;  
                                     moduleState->detail.belongCM   = 0;
                                     moduleState->detail.belongCLP  = 0;
                                     moduleState->detail.useFlag    = 0;
                                     //setLink_P();
                                     memcpy(modbus_mapping->tab_registers+ReadAddress,moduleState,sizeof(uint16_t)*9);
                                     //setLink_V();
                                     printf("结束自动保护测试,模块%d!\n",moduleState->ModNo); 

                                     //删除数据库
                                     if(!semaphore_p())  
	                                exit(EXIT_FAILURE); 
                                     deleteSlaverRecode(moduleState->ModNo);
                                     //删除节点
                                     flagNew=1;
                                     linkHead_check_A=deleteNode_from_modbusTCP(linkHead_check_A,moduleState->ModNo);
                                     flagNew=0;
                                     if(!semaphore_v())  
	                                exit(EXIT_FAILURE); 
                                     printf("模块状态:%d\n",moduleState->ModNo);
                                     for(j=0;j<9;j++){
                                         printf("[%d]=%d\n",j+ReadAddress,uint16toint16(modbus_mapping->tab_registers[j+ReadAddress]));
                                     }
                                     freeSlaverModule(moduleState);
                                     moduleState=NULL;
                                    
                                 }break;
                            case DO_SLAVER_SWITCH_ADDRESS:{
                                     printf("手动切换光开关       mb_mapping[%d]!\n",WriteAddress);
                                     moduleState=newSlaverModule();
                                     moduleState->ModNo=modbus_mapping->tab_registers[WriteAddress+MODULE_NUMBER];
                                     ReadAddress = moduleState->ModNo*0x0010;      
                                     //setLink_P();                            
                                     memcpy(moduleState,modbus_mapping->tab_registers+ReadAddress,sizeof(uint16_t)*9);
                                     if(moduleState->detail.useFlag==ON_autoPROTECT || moduleState->detail.useFlag==ON_onlyGROUP){   
                                     	moduleState->detail.SwitchPosA = modbus_mapping->tab_registers[WriteAddress+DO_SLAVER_SWITCH_Pos];
                                     	moduleState->detail.SwitchPosB = modbus_mapping->tab_registers[WriteAddress+DO_SLAVER_SWITCH_Pos];
                                     	memcpy(modbus_mapping->tab_registers+ReadAddress,moduleState,sizeof(uint16_t)*9);   
                                     }    
                                     //setLink_V();
                                     //更新数据库
                                     if(!semaphore_p())  
	                                exit(EXIT_FAILURE); 
                                     updateSwitchPos(
                                                     moduleState->ModNo,
                                                     moduleState->detail.SwitchPosA,
                                                     moduleState->detail.SwitchPosB);
                                     if(!semaphore_v())  
	                                exit(EXIT_FAILURE); 
				     /* 1*2开关控制 */
                                     printf("--------------->Here ModNo:%d POS:%d \n ",moduleState->ModNo,moduleState->detail.SwitchPosA);
				     if(!setModbus_P())                                              
					 exit(EXIT_FAILURE);    
				      modbus_rtu =newModbus(MODBUS_DEV,MODBUS_BUAD);
				      doOpticalProtectSwitch(modbus_rtu,(moduleState->ModNo-1)*4+SW_A,moduleState->detail.SwitchPosA,MODE5_PROTECT_SLAVER);
		                      doOpticalProtectSwitch(modbus_rtu,(moduleState->ModNo-1)*4+SW_B,moduleState->detail.SwitchPosB,MODE5_PROTECT_SLAVER);
				      freeModbus(modbus_rtu);
                                      modbus_rtu=NULL;            
			              if(!setModbus_V())                                                              
				          exit(EXIT_FAILURE);

                                     printf("模块状态:%d\n",moduleState->ModNo);
                                     for(j=0;j<9;j++){
                                         printf("[%d]=%d\n",j+ReadAddress,uint16toint16(modbus_mapping->tab_registers[j+ReadAddress]));
                                     }
                                     freeSlaverModule(moduleState);
                                     moduleState=NULL;
                                 }break;
                            case MODULE_1_INFORMATION_ADDRESS+MODULE_INFORMATION_MasterCM:                                
                            case MODULE_2_INFORMATION_ADDRESS+MODULE_INFORMATION_MasterCM:                                 
                            case MODULE_3_INFORMATION_ADDRESS+MODULE_INFORMATION_MasterCM:                                 
                            case MODULE_4_INFORMATION_ADDRESS+MODULE_INFORMATION_MasterCM:
                            case MODULE_5_INFORMATION_ADDRESS+MODULE_INFORMATION_MasterCM:
                            case MODULE_6_INFORMATION_ADDRESS+MODULE_INFORMATION_MasterCM:
                            case MODULE_7_INFORMATION_ADDRESS+MODULE_INFORMATION_MasterCM:
                            case MODULE_8_INFORMATION_ADDRESS+MODULE_INFORMATION_MasterCM:{
                                     printf("创建保护从模块_%x    mb_mapping[%d]!\n",WriteAddress/0x0010,WriteAddress);
                                     moduleState=newSlaverModule();
                                     moduleState->ModNo=WriteAddress/0x0010;
                                     ReadAddress = moduleState->ModNo*0x0010;
                                     //setLink_P();
                                     memcpy(moduleState,modbus_mapping->tab_registers+ReadAddress,sizeof(uint16_t)*9);
                                     moduleState->detail.useFlag    = OFF_PROTECT;
                                     memcpy(modbus_mapping->tab_registers+ReadAddress,moduleState,sizeof(uint16_t)*9);
                                     //setLink_V();
                                     //更新数据库
                                     if(!semaphore_p())  
	                                exit(EXIT_FAILURE); 
                                     updateBelongRTU(
                                                     moduleState->ModNo,
                                                     moduleState->detail.belongCM,
                                                     moduleState->detail.belongCLP,
                                                     moduleState->detail.useFlag);
                                     //删除节点
                                     if(!semaphore_v())  
	                                exit(EXIT_FAILURE); 

                                     flagNew=1;
                                     linkHead_check_A=deleteNode_from_modbusTCP(linkHead_check_A,moduleState->ModNo);
                                     flagNew=0;
                                     printf("模块状态:%d\n",moduleState->ModNo);
                                     for(j=0;j<9;j++){
                                         printf("[%d]=%d\n",j+ReadAddress,uint16toint16(modbus_mapping->tab_registers[j+ReadAddress]));
                                     }
                                     freeSlaverModule(moduleState);
                                     moduleState=NULL;
                                 }break;
                            default: printf("该地址不支持写操作   mb_mapping[%d]!\n",WriteAddress);
                       }
 
                    }else if(query[MBAP_LENGTH] == _FC_READ_HOLDING_REGISTERS){//0x03
                             RegisterNumber    = (query[MBAP_LENGTH + NB_HIGH] << 8)     + query[MBAP_LENGTH + NB_LOW];
                             ReadAddress       = (query[MBAP_LENGTH + OFFSET_HIGH] << 8) + query[MBAP_LENGTH + OFFSET_LOW]; 
				     //获取光功率                        
                             float powerValue[2];
                             int16_t powerValueInt[2];
                             uint16_t ModuleNo = ReadAddress/0x00010;
		             uint16_t SNoA= (ModuleNo-1)*8+PD_A; 
		             uint16_t SNoB= (ModuleNo-1)*8+PD_B; 
		             if(!setModbus_P())                                              
			         exit(EXIT_FAILURE);  
		             modbus_rtu =newModbus(MODBUS_DEV,MODBUS_BUAD);
		             powerValue[0] =getOneOpticalValue(modbus_rtu,SNoA,MODE5_PROTECT_SLAVER);   
                             powerValue[1] =getOneOpticalValue(modbus_rtu,SNoB,MODE5_PROTECT_SLAVER);  
		             freeModbus(modbus_rtu);
                             modbus_rtu=NULL;            
			     if(!setModbus_V())                                                              
				 exit(EXIT_FAILURE);
                             powerValueInt[0] = (int16_t)(powerValue[0]*100);
                             powerValueInt[1] = (int16_t)(powerValue[1]*100);
                             //setLink_P();
                             memcpy(modbus_mapping->tab_registers+ReadAddress+MODULE_INFORMATION_PowerValueA,powerValueInt,sizeof(uint16_t)*2);
                             //setLink_V();
                             modbus_reply(modbus_tcp, query, rc, modbus_mapping);
                             printf("powerValueA:%d,powerValueB:%d\n", powerValueInt[0],powerValueInt[1]);
                             
                    //}else if(query[MBAP_LENGTH] == _FC_WRITE_SINGLE_REGISTER){//0x06
                    }else{
                         printf("Don't support funtion!\n");
                    }

                }else if (rc == -1) {

                    printf("Connection closed on socket %d\n", master_socket);
                    close(master_socket);

                    FD_CLR(master_socket, &refset);

                    if (master_socket == fdmax) {
                        fdmax--;
                    }
                }
            }
        }
    }

    return 0;
}
void work_line(void)
{
        checkProtectNode *node=NULL;
        modbus_t  *mb=NULL;
        float powerValueA=0.0,powerValueB=0.0,PowerGateA=0.0,PowerGateB=0.0;
        int ModuleNo=0,nowState=0,beforeState=0,SwitchPos=0,SNoA=0,SNoB=0,SWNoA=0,SWNoB=0;
        int swFlag=0;
        uint16_t SwitchPosStatus[2];
        /*初始化测试链表*/
        linkHead_check_A=InitA_CycleLink();

        if(linkHead_check_A !=NULL)
            output_all_protect_node(linkHead_check_A);
        else
            printf("linkA Head:NULL\n");
        /*执行调度程序*/
        while(1){
                node=frist_protect_node(linkHead_check_A);                                          //获取待服务节点（头节点）                      
                  
		while(node!=NULL){
                        swFlag=0;   
		        if(node!=NULL  && flagNew ==0){
                           ModuleNo     = node->ModuleNo;
		           PowerGateA   = node->PowerGateA; 
		           PowerGateB   = node->PowerGateB;
		           nowState     = node->nowState; 
		           beforeState  = node->beforeState;  
		           SwitchPos    = node->SwitchPos; 		            
		        }else{break;}    
		        if(!setModbus_P())                                             
			   exit(EXIT_FAILURE); 		        
		        mb=newModbus(MODBUS_DEV,MODBUS_BUAD);
		        SNoA= (ModuleNo-1)*8+PD_A; 
		        powerValueA = getOneOpticalValue(mb,SNoA,MODE5_PROTECT_SLAVER);    
		        SNoB= (ModuleNo-1)*8+PD_B;      
		        powerValueB = getOneOpticalValue(mb,SNoB,MODE5_PROTECT_SLAVER);   
                        freeModbus(mb);
                        mb=NULL;
                        if(!setModbus_V())                                               
		            exit(EXIT_FAILURE); 
		        if     ((powerValueA > PowerGateA) && (powerValueB < PowerGateB)) nowState = StateA;   
		        else if((powerValueA < PowerGateA) && (powerValueB > PowerGateB)) nowState = StateB; 
		        else                                                              nowState = StateC;   
		        if(nowState != beforeState){
                                swFlag=1;  
				if(nowState == StateA){
                                         if(!setModbus_P())                                                
		                            exit(EXIT_FAILURE); 
                                         mb=newModbus(MODBUS_DEV,MODBUS_BUAD);
		                         SWNoA= (ModuleNo-1)*4+SW_A;
		                         doOpticalProtectSwitch(mb,SWNoA,PARALLEL,MODE5_PROTECT_SLAVER);
		                         SWNoB= (ModuleNo-1)*4+SW_B;
		                         doOpticalProtectSwitch(mb,SWNoB,PARALLEL,MODE5_PROTECT_SLAVER);
                                         freeModbus(mb);
                                         mb=NULL;
                                         if(!setModbus_V())                                                
		                            exit(EXIT_FAILURE); 		                       
		                         
		                         if(node !=NULL && flagNew==0){
				                 node->beforeState = node->nowState;
				                 node->nowState    = nowState;
				                 node->SwitchPos   = PARALLEL;
		                                 printf("切换到:A1为在纤(收)  光开关A状态为:%d\n",node->SwitchPos);
		                                 printf("      :B3为在纤(发)  光开关B状态为:%d\n",node->SwitchPos);
		                                 if(!semaphore_p())  
			                             exit(EXIT_FAILURE); 
		                                 updateSwitchPos(node->ModuleNo,node->SwitchPos,node->SwitchPos);
		                                 if(!semaphore_v())  
			                             exit(EXIT_FAILURE); 
		                                 SwitchPosStatus[0]=(uint16_t)node->SwitchPos;
		                                 SwitchPosStatus[1]=(uint16_t)node->SwitchPos; 
                                                 //setLink_P();
		                                 memcpy(modbus_mapping->tab_registers+(uint16_t)node->ModuleNo*16 + MODULE_INFORMATION_SwitchPosA,SwitchPosStatus,sizeof(uint16_t)*2);
                                                 //setLink_V();
		                         }else{break;}
		                }
				else if(nowState == StateB){
                                         if(!setModbus_P())                                                
		                            exit(EXIT_FAILURE); 
                                         mb=newModbus(MODBUS_DEV,MODBUS_BUAD);
		                         SWNoA= (ModuleNo-1)*4+SW_A;
		                         doOpticalProtectSwitch(mb,SWNoA,ACROSS,MODE5_PROTECT_SLAVER);
		                         SWNoB= (ModuleNo-1)*4+SW_B;
		                         doOpticalProtectSwitch(mb,SWNoB,ACROSS,MODE5_PROTECT_SLAVER);
		                         freeModbus(mb);  
                                         mb=NULL;
                                         if(!setModbus_V())                                                
		                            exit(EXIT_FAILURE); 
		                         if(node !=NULL && flagNew==0){
				                 node->beforeState = node->nowState;
				                 node->nowState    = nowState;
				                 node->SwitchPos   = ACROSS;
		                                 printf("切换到:A2为在纤(收)  光开关A状态为:%d\n",node->SwitchPos);
		                                 printf("      :B2为在纤(发)  光开关B状态为:%d\n",node->SwitchPos);
		                                 if(!semaphore_p())  
			                             exit(EXIT_FAILURE); 
		                                 updateSwitchPos(node->ModuleNo,node->SwitchPos,node->SwitchPos);   
		                                 if(!semaphore_v())  
			                             exit(EXIT_FAILURE); 
		                                 SwitchPosStatus[0]=(uint16_t)node->SwitchPos;
		                                 SwitchPosStatus[1]=(uint16_t)node->SwitchPos;
                                                 //setLink_P(); 
		                                 memcpy(modbus_mapping->tab_registers+(uint16_t)node->ModuleNo*16 + MODULE_INFORMATION_SwitchPosA,SwitchPosStatus,sizeof(uint16_t)*2);
                                                 //setLink_V();
		                         }else{break;}
		                }else if(nowState == State0){
                                         printf("插入新的测试节点\n");
                                }else{
                                         if(node !=NULL && flagNew==0){
		                                 node->beforeState = node->nowState;
		                                 node->nowState    = nowState;
		                                 if(!semaphore_p())  
			                             exit(EXIT_FAILURE); 
		                                 updateSwitchPos(node->ModuleNo,0,0);   
		                                 if(!semaphore_v())  
			                             exit(EXIT_FAILURE); 
		                                 SwitchPosStatus[0]=(uint16_t)node->SwitchPos;
		                                 SwitchPosStatus[1]=(uint16_t)node->SwitchPos; 
                                                 //setLink_P();
		                                 memcpy(modbus_mapping->tab_registers+(uint16_t)node->ModuleNo*16 + MODULE_INFORMATION_SwitchPosA,SwitchPosStatus,sizeof(uint16_t)*2);
                                                 //setLink_V();
		                                 printf("两路光功率均异常\n");
                                        }else{break;}
                                }
		        }	                  
		       if(node!=NULL && flagNew==0){
		          node = node->next; 
                       }else{break;}
                }
                if(node!=NULL){free(node); node=NULL;}
                if(linkHead_check_A !=NULL && swFlag==1  ){
                   output_all_protect_node(linkHead_check_A);
                   printf("PowerValueA:%f   PowerValueB:%f\n",powerValueA,powerValueB);
                }
        }
}


#include <signal.h>  
#include <execinfo.h>  
#define SIZE 1000  
void *buffer[SIZE];  
void fault_trap(int n,struct siginfo *siginfo,void *myact)  
{  
        int i, num;  
        char **calls;  
        char buf[1024];
        char cmd[1024];
        FILE *fh;

        printf("Fault address:%X\n",siginfo->si_addr);     
        num = backtrace(buffer, SIZE);  
        calls = backtrace_symbols(buffer, num);  
        for (i = 0; i < num; i++)  
                printf("%s\n", calls[i]);  
        exit(0);
}  
void setuptrap()  
{  
        struct sigaction act;  
        sigemptyset(&act.sa_mask);     
        act.sa_flags=SA_SIGINFO;      
        act.sa_sigaction=fault_trap;  
        sigaction(SIGSEGV,&act,NULL);  
}

int main()  
{  
    pthread_t tha,thb;  
    void *retval;  
    /*初始化信号量*/
    sem_id        = semget((key_t)1234, 1, 4777 | IPC_CREAT);                         
    modbus_sem_id = semget((key_t)5678, 1, 4777 | IPC_CREAT);     
    link_sem_id   = semget((key_t)2345, 1, 7774 | IPC_CREAT);
    //initLinkPV();
    setuptrap() ;
    signal(SIGINT, close_sigint);

    pthread_create(&tha,NULL,work_line,0);  
    pthread_create(&thb,NULL,modbus_server,0);  
    pthread_join(tha,&retval);  
    pthread_join(thb,&retval);  
    return 0;  
  
}



