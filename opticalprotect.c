#include "opticalprotect.h"
#include "process.h"
#include "sql.h"
#include "myModbus.h"
opticalprotect * OpticalProtect_Create(){
    opticalprotect * me = (opticalprotect *) malloc(sizeof(opticalprotect));
    return me;
}
void OpticalProtect_Destory(opticalprotect *me){
    free(me);
}
/*    更新保护组信息
    A: 更新ProtectGroupTable 表中主从光开关状态和保护组状态信息
    B: 更新AlarmTestSegmentTable表光路类型信息（"0"：备纤 "1"：在纤）
    C: 更新PortOccopyTable表光路类型信息（"1"：备纤 "2"：在纤）
*/
int updateProtectInformation(sqlite3 *mydb,sql *mysql,int recvPNo,int recvSNoOnline,int recvSNoBackup, int sendSNoOnline ,int sendSNoBackup, int masterSwitchPos, int slaverSwitchPos, int haveFlag )
{

      char PNo[10],RecvON[10],RecvBK[10],SendON[10],SendBK[10],mSwPos[10],sSwPos[10];
      uint32tostring(recvPNo        , PNo);
      uint32tostring(recvSNoOnline  , RecvON);
      uint32tostring(recvSNoBackup  , RecvBK);
      uint32tostring(sendSNoOnline  , SendON);
      uint32tostring(sendSNoBackup  , SendBK);
      uint32tostring(masterSwitchPos, mSwPos);
      uint32tostring(slaverSwitchPos, sSwPos); 
      mysql->db = mydb;
      mysql->tableName     =  "ProtectGroupTable";	
      mysql->mainKeyValue  =   PNo;
      mysql->filedsName    =  "SwitchPos";                                
      mysql->filedsValue   =   mSwPos; 
      SQL_modify(mysql); 

      mysql->filedsName    =  "sSwitchPos";                                
      mysql->filedsValue   =   sSwPos; 
      SQL_modify(mysql);

      if(haveFlag==1){
	      mysql->filedsName    =  "Status";                                
	      mysql->filedsValue   =  "-1"; 
	      SQL_modify(mysql); 
      }
  
      mysql->tableName     =  "AlarmTestSegmentTable";
      mysql->filedsName    =  "fiberType";  
      mysql->mainKeyValue  =  RecvON;                             
      mysql->filedsValue   =   "1"; 
      
      SQL_modify(mysql); 

      mysql->mainKeyValue  =  RecvBK;                             
      mysql->filedsValue   =   "0"; 
      SQL_modify(mysql); 

      mysql->mainKeyValue  =  SendON;                             
      mysql->filedsValue   =   "1"; 
      SQL_modify(mysql); 

      mysql->mainKeyValue  =  SendBK;                             
      mysql->filedsValue   =   "0"; 
      SQL_modify(mysql); 


      mysql->tableName     =  "PortOccopyTable";
      mysql->filedsName    =  "FiberType";  
      mysql->mainKeyValue  =  RecvON;                             
      mysql->filedsValue   =   "2"; 
      SQL_modify(mysql); 

      mysql->mainKeyValue  =  RecvBK;                             
      mysql->filedsValue   =   "1"; 
      SQL_modify(mysql); 

      mysql->mainKeyValue  =  SendON;                             
      mysql->filedsValue   =   "2"; 
      SQL_modify(mysql); 

      mysql->mainKeyValue  =  SendBK;                             
      mysql->filedsValue   =   "1"; 
      SQL_modify(mysql); 
  
      return 1;
}
/*判断是否存在测试参数*/
int ifHaveTestSegment(sqlite3 *mydb,sql *mysql,uint32_t SNoA1,uint32_t SNoA2,uint32_t SNoB1,uint32_t SNoB2 ){


	      char strSNoA1[10],strSNoA2[10],strSNoB1[10],strSNoB2[10];
	      int  ParmerFlagA1=0,ParmerFlagA2=0,ParmerFlagB1=0,ParmerFlagB2=0;
	
	      uint32tostring(SNoA1  , strSNoA1);
	      uint32tostring(SNoA2  , strSNoA2);
	      uint32tostring(SNoB1  , strSNoB1);
	      uint32tostring(SNoB2  , strSNoB2);

	      mysql->tableName ="AlarmTestSegmentTable";                       //检查是否存在障碍告警参数
	      mysql->mainKeyValue = strSNoA1;
	      ParmerFlagA1=SQL_existIN_db(mysql);

	      mysql->mainKeyValue = strSNoA2;
	      ParmerFlagA2=SQL_existIN_db(mysql);

	      mysql->mainKeyValue = strSNoB1;
	      ParmerFlagB1=SQL_existIN_db(mysql);

	      mysql->mainKeyValue = strSNoB2;
	      ParmerFlagB2=SQL_existIN_db(mysql);

              if(ParmerFlagA1 == 1 && ParmerFlagA2 == 1 && ParmerFlagB1 ==1 && ParmerFlagB2==1)
                  return  1;
              else
                  return  0;

}
/*      解析保护组设置指令
指令结构:
-----------------------------------------
CMDcode	170（固定）	指令唯一标识
R		        预留
CM		        RTU编号
CLP		        局站代码
PN		        保护光路组数
P1		        第1组保护光路配对
…Pi		        第i组保护光路配对
Ppn		        第PN组保护光路配对
-----------------------------------------
Pi结构
	Recv		接收端信息(RX光路)
	Send		发送端信息(TX光路)
	ConnectPos      收发双方连接关系 0:A-A B-B    1: A-C B-D
	ModNo		从模块所在RTU 模块号
	IP		从模块所在RTU IP地址
	------------------------------------------
	Recv结构
		PNo		接收端组号（A C组） 
		SNoA		保护光路A的端口号(A1或A2 C1或C2)
		SNoB		保护光路B的端口号(A2或A1 C2或C1)
		SwitchPos	1*2光开关状态
	-------------------------------------------
	Send结构
		PNo		发送端组号（B D组）
		SNoA		保护光路A的端口号(B1或B2 D1或D2)
		SNoB		保护光路B的端口号(B2或B1 D2或D1)
		SwitchPos	1*2光开关状态

----------------------------------------------------

*/
void getOpticalProtectParameter(mxml_node_t *root,mxml_node_t *tree,opticalprotect *protectmpar)
{
	    mxml_node_t *PX,*PN,*PNo,*SNoA,*SNoB,*Recv,*Send,*Node;
	    uint32_t  uint_a;
            int       intPN,i;
            char      strPX[10],strNum[10];
            PN =mxmlFindElement(root, tree, "CM",NULL, NULL,MXML_DESCEND);
            protectmpar->CM =  strtoul(PN->child->value.text.string, NULL, 0);

            PN =mxmlFindElement(root, tree, "CLP",NULL, NULL,MXML_DESCEND);
            protectmpar->CLP =  strtoul(PN->child->value.text.string, NULL, 0);

            PN =mxmlFindElement(root, tree, "PN",NULL, NULL,MXML_DESCEND);
            intPN =  strtoul(PN->child->value.text.string, NULL, 0);
            protectmpar->PN =intPN;
            protectmpar->Action = STATUS_PerSTATUS_PROTECT;
            for (i=0;i<intPN;i++){ 
		      strPX[0]='\0';
		      strcat(strPX,"P");
                      sprintf(strNum,"%d",i+1);
		      strcat(strPX,strNum);

		      PX     = mxmlFindElement(root, tree, strPX ,NULL, NULL,MXML_DESCEND); 
                      Node  = mxmlFindElement(PX,tree, "ConnectPos",NULL,NULL,MXML_DESCEND); 
                      uint_a =  strtoul(Node->child->value.text.string, NULL, 0);  
                      protectmpar->Group[i].ConnectPos=uint_a;   

                      Node  = mxmlFindElement(PX,tree, "ModNo",NULL,NULL,MXML_DESCEND); 
                      uint_a =  strtoul(Node->child->value.text.string, NULL, 0);  
                      protectmpar->Group[i].ModNo=uint_a;  

                      Node  = mxmlFindElement(PX,tree,"IP",NULL, NULL,MXML_DESCEND); 
                      protectmpar->Group[i].IP = Node->child->value.text.string;  ;

                      Recv  = mxmlFindElement(PX,tree, "Recv",NULL,NULL,MXML_DESCEND); 
                      Node  = mxmlFindElement(Recv,tree, "PNo",NULL,NULL,MXML_DESCEND); 
                      uint_a =  strtoul(Node->child->value.text.string, NULL, 0);  
                      protectmpar->Group[i].PNo=uint_a;          

                      Node  = mxmlFindElement(Recv,tree, "SNoA",NULL,NULL,MXML_DESCEND); 
                      uint_a =  strtoul(Node->child->value.text.string, NULL, 0);  
                      protectmpar->Group[i].SNoA=uint_a;    

                      Node  = mxmlFindElement(Recv,tree, "SNoB",NULL,NULL,MXML_DESCEND); 
                      uint_a =  strtoul(Node->child->value.text.string, NULL, 0);  
                      protectmpar->Group[i].SNoB=uint_a;         

                      Node  = mxmlFindElement(Recv,tree, "SwitchPos",NULL,NULL,MXML_DESCEND); 
                      uint_a =  strtoul(Node->child->value.text.string, NULL, 0);  
                      protectmpar->Group[i].SwitchPos=uint_a;    
                         
                      Send  = mxmlFindElement(PX,tree, "Send",NULL,NULL,MXML_DESCEND); 
                      Node  = mxmlFindElement(Send,tree, "PNo",NULL,NULL,MXML_DESCEND); 
                      uint_a =  strtoul(Node->child->value.text.string, NULL, 0);  
                      protectmpar->Group[i].sPNo=uint_a;          

                      Node  = mxmlFindElement(Send,tree, "SNoA",NULL,NULL,MXML_DESCEND); 
                      uint_a =  strtoul(Node->child->value.text.string, NULL, 0);  
                      protectmpar->Group[i].sSNoA=uint_a;    

                      Node  = mxmlFindElement(Send,tree, "SNoB",NULL,NULL,MXML_DESCEND); 
                      uint_a =  strtoul(Node->child->value.text.string, NULL, 0);  
                      protectmpar->Group[i].sSNoB=uint_a;  

                      Node  = mxmlFindElement(Send,tree, "SwitchPos",NULL,NULL,MXML_DESCEND); 
                      uint_a =  strtoul(Node->child->value.text.string, NULL, 0);  
                      protectmpar->Group[i].sSwitchPos=uint_a;     
                     
                      /*根据实际光开关，选择对状态取反*/
                      if(protectmpar->Group[i].sSwitchPos ==PARALLEL)protectmpar->Group[i].sSwitchPos = ACROSS;
                      else protectmpar->Group[i].sSwitchPos = PARALLEL;

                      if(protectmpar->Group[i].SwitchPos ==PARALLEL)protectmpar->Group[i].SwitchPos = ACROSS;
                      else protectmpar->Group[i].SwitchPos = PARALLEL;
                            
            }
}

/*
      执行保护组设置指令

Step 1:   解析XML消息  
Step 2:   循环处理每个保护组
Step 3:   校验模块类型，端口信息
Step 4:   校验未通过，回复相应的错误异常
Step 5:   校验通过,初始化主/从端RTU
Step 6:   初始化失败,将主从RTU回复原始状态,回复相应的错误异常
Step 7:   初始化成功,校验是否有测试参数
Step 8:   有参数,在保护表中插入保护组,状态为预备自动保护状态(STATUS_PerSTATUS_PROTECT)
                 根据保护组内容更新相关数据库表的内容
                 设置后台请求标识
Step 9:   无参数,在保护表中插入保护组,状态为等待设置自动检测门限状态(STATUS_WAIT_GATE)

Setp 10:  根据设置后台请求标识向后台发送启动自动保护信号

*/
responed *  setOpticalProtectSegment(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)     //170
{	
   mxml_node_t *CM=NULL,*CLP=NULL,*perCMDcode=NULL;
   opticalprotect *protectmpar=NULL;
   responed *resp=NULL; 
   int intCM=0,intCLP=0,SNo=0;
   protectmpar = OpticalProtect_Create();   
   resp   = Responed_Create();
   resp->RespondCode=0;      
   perCMDcode = mxmlFindElement(cmd, tree, "CMDcode",NULL, NULL,MXML_DESCEND);
   if(atoi(perCMDcode->child->value.text.string) !=cmdCode) {
	      resp->RespondCode=3;
	      resp->Group[0].Main_inform  = "指令号错误";
	      resp->Group[0].Error_inform = "Error:CMDcode Error[指令号错误]";
	      return resp;    
   }
   else{
               /*解析XML消息*/
                                 
        getOpticalProtectParameter(cmd,tree,protectmpar);
	CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	intCM = strtoul (CM->child->value.text.string, NULL, 0); 
	CLP = mxmlFindElement(cmd, tree, "CLP",NULL, NULL,MXML_DESCEND);
	intCLP = strtoul (CLP->child->value.text.string, NULL, 0);
	sqlite3  *mydb=NULL;
	sql      *mysql=NULL;  
        modbus_t *ctx=NULL;
        char **result=NULL; 
        char strSQL[100];
        char strMNo[10],strSNoA1[10],strSNoA2[10],strSNoB1[10],strSNoB2[10];
        int  ModType=0,DoSwitchCounter=0,ProtectCounter=0,PortFlagA1=0,PortFlagA2=0,PortFlagB1=0,PortFlagB2=0,UseFlag=0;
        int  ParmerFlagA1=0,ParmerFlagA2=0,ParmerFlagB1=0,ParmerFlagB2=0,flagSetMaster=0,flagSetSlaver=0;
        int  i=0,j=0,rc=0,rednum=0,PN=0,ErrorSNo=0;
        PN=protectmpar->PN; 
	mysql = SQL_Create();
	rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
        if( rc != SQLITE_OK ){
	   printf( "Open Database error.\n");
	}
        mysql->db =mydb; 
                                   
             /*循环处理每个保护组*/ 
        for(i=0;i<PN;i++){ 
               	mysql->tableName ="SubModuleTypeTable";                           //检查子单元模块是否存在
		uint32tostring(((protectmpar->Group[i].PNo-1)/4)+1,strMNo);
		uint32tostring(protectmpar->Group[i].SNoA, strSNoA1);
		uint32tostring(protectmpar->Group[i].SNoB, strSNoA2);
		uint32tostring(protectmpar->Group[i].sSNoA,strSNoB1);
		uint32tostring(protectmpar->Group[i].sSNoB,strSNoB2);

		mysql->mainKeyValue = strMNo;
		mysql->filedsName   = "UseFlag";
		SQL_lookupPar(mysql,&result,&rednum);
		UseFlag=atoi(result[0]); 
                SQL_freeResult(&result,&rednum);
                if(UseFlag==1){
			mysql->mainKeyValue = strMNo;
			mysql->filedsName   = "ModuleType";
			SQL_lookupPar(mysql,&result,&rednum);
                        ModType=atoi(result[0]); 
                        SQL_freeResult(&result,&rednum);
                }
		mysql->tableName ="PortOccopyTable";                              //检查子端口是否占用
		mysql->mainKeyValue = strSNoA1;
                PortFlagA1=SQL_existIN_db(mysql);
		mysql->mainKeyValue = strSNoA2;
                PortFlagA2=SQL_existIN_db(mysql);
		mysql->mainKeyValue = strSNoB1;
                PortFlagB1=SQL_existIN_db(mysql);
		mysql->mainKeyValue = strSNoB2;
                PortFlagB2=SQL_existIN_db(mysql);

		mysql->tableName ="AlarmTestSegmentTable";                       //检查是否存在障碍告警参数
		mysql->mainKeyValue = strSNoA1;
		ParmerFlagA1=SQL_existIN_db(mysql);
		if(ParmerFlagA1 == 1){
			mysql->filedsName   = "Status";
			mysql->filedsValue  = "1";
                        if(!semaphore_p())  
                           exit(EXIT_FAILURE);                                    
			SQL_modify(mysql);  
                        if(!semaphore_v())  
                          exit(EXIT_FAILURE);                                    	     
		}
		mysql->mainKeyValue = strSNoA2;
		ParmerFlagA2=SQL_existIN_db(mysql);
		if(ParmerFlagA2 == 1){
			mysql->filedsName   = "Status";
			mysql->filedsValue  = "1";
                        if(!semaphore_p())  
                           exit(EXIT_FAILURE);                                    
			SQL_modify(mysql);  
                        if(!semaphore_v())  
                           exit(EXIT_FAILURE);                                    			     
		}
		mysql->mainKeyValue = strSNoB1;
		ParmerFlagB1=SQL_existIN_db(mysql);
		if(ParmerFlagB1 == 1){
			mysql->filedsName   = "Status";
			mysql->filedsValue  = "1";
                        if(!semaphore_p())  
                           exit(EXIT_FAILURE);                                    
			SQL_modify(mysql);  
                        if(!semaphore_v())  
                           exit(EXIT_FAILURE);                                    
                        mysql->filedsName   = "AT06";
                        SQL_lookupPar(mysql,&result,&rednum);
                        protectmpar->Group[i].sPowerGateA = atof(result[0]);	
                        SQL_freeResult(&result,&rednum);		     
		}
		mysql->mainKeyValue = strSNoB2;
		ParmerFlagB2=SQL_existIN_db(mysql);
		if(ParmerFlagB2 == 1){
			mysql->filedsName   = "Status";
			mysql->filedsValue  = "1";
                        if(!semaphore_p())  
                           exit(EXIT_FAILURE);                                    
			SQL_modify(mysql);  
                        if(!semaphore_v())  
                           exit(EXIT_FAILURE);                                    		
                        mysql->filedsName   = "AT06";
                        SQL_lookupPar(mysql,&result,&rednum);
                        protectmpar->Group[i].sPowerGateB = atof(result[0]);	
                        SQL_freeResult(&result,&rednum);	     
		}

 
                if(UseFlag!=1 ){
		      resp->RespondCode  =ERROR_CODE_14_DBMATCH_FAILED;
                      if(resp->SNorPN!=TYPE_SNo){
		          resp->ErrorSN      = ERROR_CODE_14_DBMATCH_FAILED;                     
		          resp->SNorPN       = TYPE_PNo;
		          resp->Group[ErrorSNo].SNo =  protectmpar->Group[i].PNo;
		          resp->Group[ErrorSNo].Main_inform  = "模块未激活";
			  resp->Group[ErrorSNo].Error_inform = "Error: Don't have such Module\n";
		          ErrorSNo++;
                      }  

                }else if(PortFlagA1!=1 || PortFlagA2!=1 || PortFlagB1!=1 || PortFlagB2!=1){
		      resp->RespondCode  = ERROR_CODE_14_DBMATCH_FAILED;
                      if(resp->SNorPN!=TYPE_PNo){
		          resp->ErrorSN      = ERROR_CODE_14_DBMATCH_FAILED;                     
		          resp->SNorPN       = TYPE_SNo;
		          resp->Group[ErrorSNo].SNo =  protectmpar->Group[i].SNoA;
		          resp->Group[ErrorSNo].Main_inform  = "端口未配置";
			  resp->Group[ErrorSNo].Error_inform = "Error: Don't have such Port\n";
		          ErrorSNo++;  
		          resp->Group[ErrorSNo].SNo =  protectmpar->Group[i].SNoB;
		          resp->Group[ErrorSNo].Main_inform  = "端口未配置";
			  resp->Group[ErrorSNo].Error_inform = "Error: Don't have such Port\n";
		          ErrorSNo++;
		          resp->Group[ErrorSNo].SNo =  protectmpar->Group[i].sSNoA;
		          resp->Group[ErrorSNo].Main_inform  = "端口未配置";
			  resp->Group[ErrorSNo].Error_inform = "Error: Don't have such Port\n";
		          ErrorSNo++;
		          resp->Group[ErrorSNo].SNo =  protectmpar->Group[i].sSNoB;
		          resp->Group[ErrorSNo].Main_inform  = "端口未配置";
			  resp->Group[ErrorSNo].Error_inform = "Error: Don't have such Port\n";
		          ErrorSNo++;
                       }
                }else if(UseFlag ==1  && PortFlagA1==1 && PortFlagA2==1 &&  PortFlagB1==1 &&  PortFlagB2==1 ){
                      if(ModType==MODE3_PROTECT_MASTER){
		              /*初始化从端RTU*/    
                                   
                              ctx=newModBus_TCP_Client(protectmpar->Group[i].IP);
                              if( ctx== NULL){
				  resp->RespondCode =ERROR_CODE_13_PROTECT_FAILED;
				  if(resp->SNorPN!=TYPE_SNo){
					resp->ErrorSN      = ERROR_CODE_13_PROTECT_FAILED;                     
					resp->SNorPN       = TYPE_PNo;
					resp->Group[ErrorSNo].SNo =  protectmpar->Group[i].PNo;
					resp->Group[ErrorSNo].Main_inform  = "从端模块操作失败";
					resp->Group[ErrorSNo].Error_inform = "Error: Slaver Switch Failed.[从端模块操作失败]\n";
					ErrorSNo++;
				  } 
                              }else{
                              flagSetMaster = initSlaver_Full(ctx,
                                                             protectmpar->Group[i].ModNo,
                                                             protectmpar->CM,
                                                             protectmpar->CLP,
                                                             protectmpar->Group[i].sPowerGateA,
                                                             protectmpar->Group[i].sPowerGateB,
                                                             atoi(strSNoB1),
                                                             atoi(strSNoB2),
                                                             protectmpar->Group[i].sSwitchPos,
                                                             protectmpar->Group[i].ConnectPos);                   
			       freeModbus_TCP_Client(ctx); 
			       ctx=NULL;

			       /*初始化主端RTU*/
			       if(!setModbus_P())                                              
				   exit(EXIT_FAILURE);    
			       ctx =newModbus(MODBUS_DEV,MODBUS_BUAD);
			       flagSetSlaver=doOpticalProtectSwitch(ctx,protectmpar->Group[i].PNo,protectmpar->Group[i].SwitchPos,MODE3_PROTECT_MASTER);
		               if(flagSetSlaver>=0)flagSetSlaver=doOpticalProtectSwitch(ctx,protectmpar->Group[i].PNo+1,protectmpar->Group[i].SwitchPos,MODE3_PROTECT_MASTER);
			       freeModbus(ctx);
                               ctx=NULL;    
			       if(!setModbus_V())                                                              
					exit(EXIT_FAILURE);
                                  
                               /*主从RTU初始化失败*/
                               if(flagSetSlaver >= 0 && flagSetMaster >= 0 ){ 
                                        /*均有测试参数，则自动启动保护模式*/
		                	if(ParmerFlagA1 == 1 && ParmerFlagA2 ==1 && ParmerFlagB1 ==1 && ParmerFlagB2 ==1){    
				                         
                                                        ProtectCounter++;                                        //更新后台测试节点
		                            		                             
				                        mysql->tableName ="ProtectGroupTable";  
							protectmpar->Action =STATUS_PerSTATUS_PROTECT;           //写入保护组数据库

							sprintf(strSQL,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s',%d\n" 
									  ,protectmpar->Group[i].PNo             
									  ,protectmpar->Group[i].SNoA            
									  ,protectmpar->Group[i].SNoB          
									  ,protectmpar->Group[i].SwitchPos       

									  ,protectmpar->Group[i].sPNo            
									  ,protectmpar->Group[i].sSNoA           
									  ,protectmpar->Group[i].sSNoB           
									  ,protectmpar->Group[i].sSwitchPos     

				                                          ,protectmpar->Group[i].ConnectPos      
				                                          ,protectmpar->Group[i].ModNo          
				                                          ,protectmpar->Group[i].IP              
									  ,protectmpar->Action                  
									  );
							mysql->filedsValue = strSQL;
							if(!semaphore_p())  
							   exit(EXIT_FAILURE);                                   
							SQL_add(mysql);                                     
           
							if(protectmpar->Group[i].SwitchPos == PARALLEL){                   
		                                             updateProtectInformation(mydb,mysql,
		                                                                     protectmpar->Group[i].PNo,
		                                                                     protectmpar->Group[i].SNoA < protectmpar->Group[i].SNoB  ? protectmpar->Group[i].SNoA:protectmpar->Group[i].SNoB ,
		                                                                     protectmpar->Group[i].SNoA > protectmpar->Group[i].SNoB  ? protectmpar->Group[i].SNoA:protectmpar->Group[i].SNoB ,
		                                                                     protectmpar->Group[i].sSNoA <protectmpar->Group[i].sSNoB ? protectmpar->Group[i].sSNoA:protectmpar->Group[i].sSNoB,
		                                                                     protectmpar->Group[i].sSNoA >protectmpar->Group[i].sSNoB ? protectmpar->Group[i].sSNoA:protectmpar->Group[i].sSNoB,
		                                                                     protectmpar->Group[i].SwitchPos,
		                                                                     protectmpar->Group[i].SwitchPos,1);
		                                        }
							if(protectmpar->Group[i].SwitchPos == ACROSS){                   
		                                             updateProtectInformation(mydb,mysql,
		                                                                     protectmpar->Group[i].PNo,
		                                                                     protectmpar->Group[i].SNoA > protectmpar->Group[i].SNoB ? protectmpar->Group[i].SNoA:protectmpar->Group[i].SNoB ,
		                                                                     protectmpar->Group[i].SNoA < protectmpar->Group[i].SNoB ? protectmpar->Group[i].SNoA:protectmpar->Group[i].SNoB ,
		                                                                     protectmpar->Group[i].sSNoA > protectmpar->Group[i].sSNoB ? protectmpar->Group[i].sSNoA:protectmpar->Group[i].sSNoB,
		                                                                     protectmpar->Group[i].sSNoA < protectmpar->Group[i].sSNoB ? protectmpar->Group[i].sSNoA:protectmpar->Group[i].sSNoB,
		                                                                     protectmpar->Group[i].SwitchPos,
		                                                                     protectmpar->Group[i].SwitchPos,1);
		                                        }
				                        if(!semaphore_v())                                         
							    exit(EXIT_FAILURE);


                                          
		                	}else{    /*若没有参数，则只保存保护组信息*/          
		                                    
						       mysql->tableName ="ProtectGroupTable";  
						       protectmpar->Action =STATUS_WAIT_GATE;            //等待配置测试参数
						       sprintf(strSQL,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s',%d\n"  
						         		,protectmpar->Group[i].PNo             
									,protectmpar->Group[i].SNoA            
									,protectmpar->Group[i].SNoB            
									,protectmpar->Group[i].SwitchPos      
									,protectmpar->Group[i].sPNo           
									,protectmpar->Group[i].sSNoA           
									,protectmpar->Group[i].sSNoB           
									,protectmpar->Group[i].sSwitchPos     
						                        ,protectmpar->Group[i].ConnectPos     
						                        ,protectmpar->Group[i].ModNo           
						                        ,protectmpar->Group[i].IP              
									,protectmpar->Action);
							mysql->filedsValue = strSQL;
                                                        
							if(!semaphore_p())  
							    exit(EXIT_FAILURE);                                    
							SQL_add(mysql);                                  //更新或者插入新的纪录
							if(!semaphore_v())                                        
							    exit(EXIT_FAILURE);	
	                     
		                	}
		                                		                                            
		                }else{  /*主从RTU初始化成功*/
                                      ctx=newModBus_TCP_Client(protectmpar->Group[i].IP);  
		                      if( ctx== NULL ){
						resp->RespondCode  =ERROR_CODE_13_PROTECT_FAILED;
						if(resp->SNorPN!=TYPE_SNo){
							resp->ErrorSN      = ERROR_CODE_13_PROTECT_FAILED;                     
							resp->SNorPN       = TYPE_PNo;
							resp->Group[ErrorSNo].SNo =  protectmpar->Group[i].PNo;
							resp->Group[ErrorSNo].Main_inform  = "从端模块操作失败";
							resp->Group[ErrorSNo].Error_inform = "Error: Slaver Switch Failed.[从端模块操作失败]\n";
							ErrorSNo++;
						} 
		                      }else{
						if(deleteSlaverProtectModule(ctx,protectmpar->Group[i].ModNo)<0)
							printf("Error to delete slaver protect module!\n");   
                                                else
							printf("delete slaver protect module successful!\n");                 	
		                      }
				      freeModbus_TCP_Client(ctx); 
				      ctx=NULL;		                                         
				      resp->RespondCode = ERROR_CODE_13_PROTECT_FAILED ;                             
				      if(resp->SNorPN!=TYPE_SNo){
				      		resp->ErrorSN      = ERROR_CODE_13_PROTECT_FAILED;                     
				      		resp->SNorPN       = TYPE_PNo;
						resp->Group[ErrorSNo].SNo = protectmpar->Group[i].PNo;
						resp->Group[ErrorSNo].Main_inform  = "光保护切换:光开关切换失败";
						resp->Group[ErrorSNo].Error_inform = "Error: Optical Switch Failed[光保护切换:光开关切换失败]!\n";
						ErrorSNo++;  
				      } 


		                }
                           }
                    }else{
				resp->RespondCode  =ERROR_CODE_14_DBMATCH_FAILED;
				if(resp->SNorPN!=TYPE_SNo){
				       resp->ErrorSN      = ERROR_CODE_14_DBMATCH_FAILED;                     
				       resp->SNorPN       = TYPE_PNo;
				       resp->Group[ErrorSNo].SNo =  protectmpar->Group[i].PNo;
				       resp->Group[ErrorSNo].Main_inform  = "所属模块不支持光保护切换";
				       resp->Group[ErrorSNo].Error_inform = "Error: Don't support protect Module.[所属模块不支持光保护切换]\n";
				       ErrorSNo++;
				} 

                    }
                       
             }

        }

       if(resp->RespondCode != SUCCESS_CODE_OK ){
		 resp->ErrorSN     =  ErrorSNo;                  
		 return resp;
       }
       SQL_Destory(mysql);  
       sqlite3_close(mydb);

       int signum;
       union sigval mysigval;
       char* process;  
       int retProcess = 0,n=0;  
       pid_t protectPID[MAX_PID_NUM];  
       char*  recvStr=NULL;
        
       if(ProtectCounter>0){ 
                            /*请求自动保护*/     
	       process ="/web/cgi-bin/ProtectMasterMain";                        
	       retProcess = get_pid_by_name(process, protectPID, MAX_PID_NUM);  
	       if(retProcess>0){
		       printf("process '%s' is existed? (%d): %c\n", process, retProcess, (retProcess > 0)?'y':'n');  
		       signum=SIGRTMIN+1;                                      //设置信号值
		       mysigval.sival_int = 170;                               //设置信号的附加信息 (启动自动保护)                               
		       for(n=0;n<retProcess;n++){  
			       printf("ProtectMasterMain-PID:%u\n", protectPID[n]);                
			       if(sigqueue(protectPID[n],signum,mysigval)==-1)printf("send signal error\n");
				       
                       }
			   /*等待成功信号*/
		    if(recvMessageQueue_Backstage("170-OK",PROTECT_MESSAGE_TYPE) == 1){                     
			    printf("Set Protect Group operation sucessful!\n");
		    }else{
		            printf("Set Protect Group operation failed!\n");
			    resp->RespondCode=3;
			    resp->Group[0].Main_inform  = "切换保护节点更新失败-->未收到回复消息";
			    resp->Group[0].Error_inform = "Error:Don't get back massgae![切换保护节点更新失败-->未收到回复消息]";
                            free(recvStr);
			    return resp;   
		    }
	   
	      }else{
		      resp->RespondCode=ERROR_CODE_3_EXECULTE_FAILED;
		      resp->Group[0].Main_inform  = "切换保护节点更新失败-->未找到后台进程";
		      resp->Group[0].Error_inform = "Error:Don't have back process![切换保护节点更新失败-->未找到后台进程]";
		      return resp; 
	      }
       }
      

   }  
   return resp;
}


/*              解析请求光保护切换指令
CMDcode	370（固定）     每个指令或数据的唯一标识
R			预留
CM			RTU编号
CLP			局站代码
PN			可进行切换的组数
G1			第1个要切换的保护组编号 
…Gi			第i个要切换的保护组编号 
Gcsn			第CSN个要切换的保护组编号 
Gi的结构
	PNo			保护组号,下行光路(Recv)所在组号
	SNoA			保护光路A的端口号(A1或A2 C1或C2)
	SNoB			保护光路B的端口号(A2或A1 C2或C1)
	SwitchPos		1*2光开关状态

*/
void getProtectSwitchParameter(mxml_node_t *root,mxml_node_t *tree,opticalprotect *protectmpar){
	    mxml_node_t *GX,*PN,*PNo,*SNoA,*SNoB,*SwitchPos;

	    uint32_t  uint_a;
            int       intPN,i;
            char strGX[10],strNum[10];
            PN =mxmlFindElement(root, tree, "PN",NULL, NULL,MXML_DESCEND);
            intPN =  strtoul(PN->child->value.text.string, NULL, 0);
            protectmpar->PN =intPN;
            protectmpar->Action = -1;
            for (i=0;i<intPN;i++)
            { 
		 strGX[0]='\0';
		 strcat(strGX,"G");
		 sprintf(strNum,"%d",i+1);
		 strcat(strGX,strNum);
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
    
                 if(protectmpar->Group[i].SwitchPos ==PARALLEL)protectmpar->Group[i].SwitchPos = ACROSS;
                 else protectmpar->Group[i].SwitchPos = PARALLEL;

            }

}
/*        执行光保护切换（手动切换）指令

Step 1:   解析XML消息  
Step 2:   循环处理每个手动切换组
Step 3:   校验模块类型，端口信息
Step 4:   校验未通过，回复相应的错误异常
Step 5:   校验通过,获取保护组当前状态,以及从端RTU相关信息
Step 6:   若保护组当前状态处于0 1 -1种的任意一种 则Step7  
Step 7:  检查是否存在测试参数,若有参数，则设置更新后台节点标志
Step 8:   根据指令中的光开关位置，以及从端模块信息，控制从端RTU光开关
Step 9:   根据指令中的光开关位置，控制主端RTU光开关
Step 10:   若Step 7 Step 8操作成功,更新与保护相关的数据表
Setp 11:  根据设置后台请求标识向后台发送启动自动保护信号

*/
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
	      resp->Group[0].Main_inform  = "指令号错误";
	      resp->Group[0].Error_inform = "Error:CMDcode Error[指令号错误]";
	      return resp;  
      }else{

         getProtectSwitchParameter(cmd,tree,protectmpar);
	 CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	 intCM = strtoul (CM->child->value.text.string, NULL, 0); 	
         int i;
         int PN;
         PN=protectmpar->PN;
         resp->RespondCode  =0;
         protectmpar->Action= STATUS_AUTO_PROTECT;  
	 sqlite3 *mydb;
	 sql *mysql;
         modbus_t *ctx=NULL,*mb=NULL;
	 int rc=0,rednum=0,flagMatch=0,UseFlag=0,PortFlagA=0,PortFlagB=0,flagSetMaster=0,existFlag=0,flagStatus=0,haveFlag=0;
         int ErrorSNo=0,ModuleNo=0,SwitchStatus=0,ModType=0,SwitchPos=0,sSwitchPos=0,DoSwitchCounter=0,ProtectCounter=0,fiberType=0,slaverModuleNo=0;
         char *strPNo=NULL,*strMNo=NULL,*strSNoA=NULL,*strSNoB=NULL;
         char sIP[16]; 
         char **result=NULL;
	 strPNo    = (char *) malloc(sizeof(char)*10);
         strMNo    = (char *) malloc(sizeof(char)*10); 
         strSNoA   = (char *) malloc(sizeof(char)*10);
         strSNoB   = (char *) malloc(sizeof(char)*10); 

	 mysql = SQL_Create();
	 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
	 if( rc != SQLITE_OK ){
	     printf( "Open Database error.\n");
	 }
	 mysql->db =mydb;
	 mysql->tableName    =  "ProtectGroupTable"; 
         for (i=0;i<PN;i++){            
		mysql->tableName ="SubModuleTypeTable";                  //检查子单元模块是否存在
		ModuleNo = ((protectmpar->Group[i].PNo-1)/4)+1;
		uint32tostring(ModuleNo,strMNo);
		mysql->mainKeyValue = strMNo;
		mysql->filedsName   = "UseFlag";
		SQL_lookupPar(mysql,&result,&rednum);
		UseFlag=atoi(result[0]); 
                SQL_freeResult(&result,&rednum);
                if(UseFlag==1){
			mysql->mainKeyValue = strMNo;
			mysql->filedsName   = "ModuleType";
			SQL_lookupPar(mysql,&result,&rednum);
                        ModType=atoi(result[0]); 
                        SQL_freeResult(&result,&rednum);
                } 
                uint32tostring(protectmpar->Group[i].SNoA,strSNoA);
		uint32tostring(protectmpar->Group[i].SNoB,strSNoB);
		mysql->tableName ="PortOccopyTable";                    //检查子端口是否占用
		mysql->mainKeyValue = strSNoA;
		PortFlagA = SQL_existIN_db(mysql); 
		mysql->mainKeyValue = strSNoB;
		PortFlagB = SQL_existIN_db(mysql); 


                if(UseFlag!=1 ){
		      resp->RespondCode  =ERROR_CODE_14_DBMATCH_FAILED;
                      if(resp->SNorPN!=TYPE_SNo){
		          resp->ErrorSN      = 1;                     
		          resp->SNorPN       = TYPE_PNo;
		          resp->Group[ErrorSNo].SNo =  protectmpar->Group[i].PNo;
		          resp->Group[ErrorSNo].Main_inform  = "模块未激活";
			  resp->Group[ErrorSNo].Error_inform = "Error: Don't have such Module\n";
		          ErrorSNo++;
                      }  

                }else if(PortFlagA!=1 || PortFlagB!=1){
		      resp->RespondCode  = ERROR_CODE_14_DBMATCH_FAILED;
                      if(resp->SNorPN!=TYPE_PNo){
		          resp->ErrorSN      = 2;                     
		          resp->SNorPN       = TYPE_SNo;
		          resp->Group[ErrorSNo].SNo =  protectmpar->Group[i].SNoA;
		          resp->Group[ErrorSNo].Main_inform  = "端口未配置";
			  resp->Group[ErrorSNo].Error_inform = "Error: Don't have such Port\n";
		          ErrorSNo++;  
		          resp->Group[ErrorSNo].SNo =  protectmpar->Group[i].SNoB;
		          resp->Group[ErrorSNo].Main_inform  = "端口未配置";
			  resp->Group[ErrorSNo].Error_inform = "Error: Don't have such Port\n";
		          ErrorSNo++;
                       }
                }else if(UseFlag ==1  && PortFlagA==1 && PortFlagB==1){
              
                    if(ModType==MODE3_PROTECT_MASTER){
		                          
		            mysql->tableName    ="ProtectGroupTable";
			    uint32tostring(protectmpar->Group[i].PNo,strPNo);
			    mysql->mainKeyValue = strPNo;
                            existFlag=SQL_existIN_db(mysql); 
                            if(existFlag){
                                    /*获取保护组状态及从端RTU信息*/
				    mysql->filedsName   =  "Status";
				    SQL_lookupPar(mysql,&result,&rednum);
				    SwitchStatus =atoi(result[0]);
				    SQL_freeResult(&result,&rednum);


				    mysql->filedsName   =  "SwitchPos";
				    SQL_lookupPar(mysql,&result,&rednum);
				    SwitchPos =atoi(result[0]);
				    SQL_freeResult(&result,&rednum);

				    mysql->filedsName   =  "sSwitchPos";
				    SQL_lookupPar(mysql,&result,&rednum);
				    sSwitchPos =atoi(result[0]);
				    SQL_freeResult(&result,&rednum);

				    mysql->filedsName   =  "sIP";
				    SQL_lookupPar(mysql,&result,&rednum);
				    sprintf(sIP,"%s",result[0]);
				    SQL_freeResult(&result,&rednum);

				    mysql->filedsName   =  "sModNo";
				    SQL_lookupPar(mysql,&result,&rednum);
				    slaverModuleNo=atoi(result[0]);
				    SQL_freeResult(&result,&rednum);

				    mysql->filedsName   =  "Status";
				    SQL_lookupPar(mysql,&result,&rednum);
				    SwitchStatus =atoi(result[0]);
				    SQL_freeResult(&result,&rednum);

				    if(SwitchStatus == STATUS_AUTO_PROTECT ||  SwitchStatus == STATUS_PerSTATUS_PROTECT || SwitchStatus == STATUS_WAIT_GATE){	
                                           /*存在测试参数，表明后台有测试节点*/
                                           if(haveFlag=ifHaveTestSegment(mydb,mysql,protectmpar->Group[i].SNoA,protectmpar->Group[i].SNoB,protectmpar->Group[i].SNoA+1,protectmpar->Group[i].SNoB+1)){
                                              ProtectCounter++;   
                                           }
                                           /*向从端发送光开关切换指令*/			                 
                                           ctx=newModBus_TCP_Client(sIP);
		                           if(ctx==NULL){
						resp->RespondCode  =ERROR_CODE_13_PROTECT_FAILED;
					        if(resp->SNorPN!=TYPE_SNo){
							resp->ErrorSN      = 1;                     
							resp->SNorPN       = TYPE_PNo;
							resp->Group[ErrorSNo].SNo =  protectmpar->Group[i].PNo;
							resp->Group[ErrorSNo].Main_inform  = "从端模块操作失败";
							resp->Group[ErrorSNo].Error_inform = "Error: Slaver Switch Failed.[从端模块操作失败]\n";
							ErrorSNo++;
					        } 
		                           }else{
						if(doSlaverProtectSwitch(ctx,slaverModuleNo,protectmpar->Group[i].SwitchPos)<0){
							printf("Error to do slaver protect switch!\n");   
							resp->RespondCode  =ERROR_CODE_13_PROTECT_FAILED;
							if(resp->SNorPN!=TYPE_SNo){
								resp->ErrorSN      = 1;                     
								resp->SNorPN       = TYPE_PNo;
								resp->Group[ErrorSNo].SNo =  protectmpar->Group[i].PNo;
								resp->Group[ErrorSNo].Main_inform  = "从端模块操作失败";
								resp->Group[ErrorSNo].Error_inform = "Error: Slaver Switch Failed.[从端模块操作失败]\n";
								ErrorSNo++;
							} 
						}else{
                                                        printf("do slaver protect switch successful!\n");
                                                        /*向主端发送光开关切换指令*/
							if(!setModbus_P())                                              
							    exit(EXIT_FAILURE);    
							mb =newModbus(MODBUS_DEV,MODBUS_BUAD);
							doOpticalProtectSwitch(mb,protectmpar->Group[i].PNo,protectmpar->Group[i].SwitchPos,MODE3_PROTECT_MASTER);
				                        doOpticalProtectSwitch(mb,protectmpar->Group[i].PNo+1,protectmpar->Group[i].SwitchPos,MODE3_PROTECT_MASTER);
							freeModbus(mb);
							mb=NULL;           
							if(!setModbus_V())                                                               
							    exit(EXIT_FAILURE);    

							if(!semaphore_p())  
							    exit(EXIT_FAILURE);                                            
							if(protectmpar->Group[i].SwitchPos == PARALLEL){                   
		                                             updateProtectInformation(mydb,mysql,
		                                                                     protectmpar->Group[i].PNo,
		                                                                     protectmpar->Group[i].SNoA < protectmpar->Group[i].SNoB ? protectmpar->Group[i].SNoA:protectmpar->Group[i].SNoB ,
		                                                                     protectmpar->Group[i].SNoA > protectmpar->Group[i].SNoB ? protectmpar->Group[i].SNoA:protectmpar->Group[i].SNoB ,
		                                                                     protectmpar->Group[i].SNoA < protectmpar->Group[i].SNoB ? protectmpar->Group[i].SNoA+1:protectmpar->Group[i].SNoB+1,
		                                                                     protectmpar->Group[i].SNoA > protectmpar->Group[i].SNoB ? protectmpar->Group[i].SNoA+1:protectmpar->Group[i].SNoB+1,
		                                                                     protectmpar->Group[i].SwitchPos,
		                                                                     protectmpar->Group[i].SwitchPos,haveFlag);
		                                        }
							if(protectmpar->Group[i].SwitchPos == ACROSS){                   
		                                             updateProtectInformation(mydb,mysql,
		                                                                     protectmpar->Group[i].PNo,
		                                                                     protectmpar->Group[i].SNoA > protectmpar->Group[i].SNoB ? protectmpar->Group[i].SNoA:protectmpar->Group[i].SNoB ,
		                                                                     protectmpar->Group[i].SNoA < protectmpar->Group[i].SNoB ? protectmpar->Group[i].SNoA:protectmpar->Group[i].SNoB ,
		                                                                     protectmpar->Group[i].SNoA > protectmpar->Group[i].SNoB ? protectmpar->Group[i].SNoA+1:protectmpar->Group[i].SNoB+1,
		                                                                     protectmpar->Group[i].SNoA < protectmpar->Group[i].SNoB ? protectmpar->Group[i].SNoA+1:protectmpar->Group[i].SNoB+1,
		                                                                     protectmpar->Group[i].SwitchPos,
		                                                                     protectmpar->Group[i].SwitchPos,haveFlag);
		                                        }
				                        if(!semaphore_v())                                         
							    exit(EXIT_FAILURE);
                                                }
		                           }
                                           freeModbus_TCP_Client(ctx); 
                                           ctx=NULL;
				    }else{
				          if(resp->SNorPN!=TYPE_SNo){
						  resp->RespondCode = ERROR_CODE_14_DBMATCH_FAILED ; 
						  resp->SNorPN      = TYPE_PNo;
						  resp->Group[ErrorSNo].PNo = protectmpar->Group[i].PNo;
						  if(SwitchStatus==2){
							  resp->Group[ErrorSNo].Main_inform ="光保护切换：保护组不存在";
							  resp->Group[ErrorSNo].Error_inform = "Error: Sqlite don't match -->PNo don't exist [Status=2][光保护切换：保护组不存在].\n";
							  ErrorSNo++;
						  }
						  if(flagMatch!=1){
							  resp->Group[ErrorSNo].Main_inform ="光保护切换：保护组内的光路号指令与数据库不匹配";
							  resp->Group[ErrorSNo].Error_inform = "Error: Sqlite don't match -->command and database not same[光保护切换：保护组内的光路号指令与数据库不匹配].\n";
							  ErrorSNo++;
						  }
		                          }
				    } //end if(SwitchStatus==STATUS_AUTO_PROTECT)
                           }else{
		            	resp->RespondCode = ERROR_CODE_14_DBMATCH_FAILED ;                             
		            	if(resp->SNorPN!=TYPE_SNo){
					 resp->ErrorSN      = 1;                     
					 resp->SNorPN       = TYPE_PNo;
			      	   	 resp->Group[ErrorSNo].SNo = protectmpar->Group[i].PNo;
				         resp->Group[ErrorSNo].Main_inform  = "光保护切换:保护组不存在";
				         resp->Group[ErrorSNo].Error_inform = "Error: Optical Protect don't Exist![光保护切换:保护组不存在]!\n";
				         ErrorSNo++;  
		                } 
                           }//end if(existFlag PNo)
                   }else{ 
			resp->RespondCode  =ERROR_CODE_14_DBMATCH_FAILED;
		        if(resp->SNorPN!=TYPE_SNo){
			       resp->ErrorSN      = 1;                     
			       resp->SNorPN       = TYPE_PNo;
			       resp->Group[ErrorSNo].SNo =  protectmpar->Group[i].PNo;
			       resp->Group[ErrorSNo].Main_inform  = "所属模块不支持光保护切换";
			       resp->Group[ErrorSNo].Error_inform = "Error: Don't support protect Module.[所属模块不支持光保护切换]\n";
			       ErrorSNo++;
		        } 
                   }// end if(ModType =3)
                 
               } // end if(UseFlag && PortFlag)
           
        }//end for

	free(strPNo);    
	free(strSNoA);    
	free(strSNoB);    
	free(strMNo);    
        OpticalProtect_Destory(protectmpar);
	SQL_Destory(mysql);  
	sqlite3_close(mydb);

	if(resp->RespondCode != SUCCESS_CODE_OK ){
	   resp->ErrorSN     =  ErrorSNo;               
           return resp;
	}
       
       int signum;
       union sigval mysigval;
       char* process;  
       int retProcess = 0,n=0;  
       pid_t protectPID[MAX_PID_NUM];  


        if(ProtectCounter>0){   
                            /*发送更新节点信号*/   
	       process ="/web/cgi-bin/ProtectMasterMain";                        
	       retProcess = get_pid_by_name(process, protectPID, MAX_PID_NUM);  
	       if(retProcess>0){
		       printf("process '%s' is existed? (%d): %c\n", process, retProcess, (retProcess > 0)?'y':'n');  
		       signum=SIGRTMIN+1;                                         //设置信号值
		       mysigval.sival_int = 370;                                  //设置信号的附加信息 (更新后台测试节点)                               
		       for(n=0;n<retProcess;n++){  
		       printf("ProtectMasterMain-PID:%u\n", protectPID[n]);                
		       if(sigqueue(protectPID[n],signum,mysigval)==-1)
			       printf("send signal error\n");
		      }  
			   /*等待成功信号*/

		    if(recvMessageQueue_Backstage("370-OK",PROTECT_MESSAGE_TYPE) == 1){                     
			    printf("Do  Protect Switch operation sucessful!\n");
		    }else{
		            printf("Set Protect Switch operation failed!\n");
			    resp->RespondCode=3;
			    resp->Group[0].Main_inform  = "切换保护节点更新失败-->未收到回复消息";
			    resp->Group[0].Error_inform = "Error:Don't get back massgae![切换保护节点更新失败-->未收到回复消息]";

			    return resp;   
		    }
 	   
	      }else{
		      resp->RespondCode=ERROR_CODE_3_EXECULTE_FAILED;
		      resp->Group[0].Main_inform  = "切换保护节点更新失败-->未找到后台进程";
		      resp->Group[0].Error_inform = "Error:Don't have back process! 切换保护节点更新失败-->未找到后台进程]";
		      return resp; 
	      }
       }

  }  //end if(atoi(perCMDcode->child->value.text.string) !=cmdCode)

  return resp;
}
/*        执行取消保护指令
Step 1:   解析XML消息  
Step 2:   循环处理每个取消保护组
Step 3:   校验模块类型，端口信息
Step 4:   校验未通过，回复相应的错误异常
Step 5:   校验通过,则 获取从端模块参数 Step 6
Step 6:   删除已有配对组
Step 6:   删除测试参数
Step 6:   删除从端模块 
Step 6:   若保护组当前状态处于0 1 -1种的任意一种 则Step7  
Step 7:   根据指令中的光开关位置，以及从端模块信息，控制从端RTU光开关
Step 8:   根据指令中的光开关位置，控制主端RTU光开关
Step 9:   若Step 7 Step 8操作成功,更新与保护相关的数据表
Step 10:  检查是否存在测试参数,若有参数，则设置更新后台节点标志
Setp 11:  根据设置后台请求标识向后台发送启动自动保护信号


*/
responed *  endOpticalProtectSegment(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)          //250
{
   mxml_node_t *CM=NULL,*CLP=NULL,*QX=NULL,*PN=NULL,*perCMDcode=NULL;
   opticalprotect *protectmpar=NULL;
   responed *resp=NULL; 
   int rtuCM=0,rtuCLP=0,intPN=0,SNo=0,uint_a=0;
   int i=0,j=0, rc=0,rednum=0,UseFlag=0,existFlag=0,flagStatus=0,ProtectCounter=0,slaverModuleNo=0;
   int ErrorSNo=0,ModuleNo=0,resPN=0,ModType=0,Status=0;
   char strQX[10],strNum[10];
   char strMNo[10],strSNoA[10],strSNoB[10],strSNoC[10],strSNoD[10],strPNo[10],sIP[16]; 
   char **result=NULL;
   sqlite3 *mydb=NULL;
   sql *mysql=NULL;
   modbus_t * ctx=NULL;
   protectmpar = OpticalProtect_Create();   
   resp   = Responed_Create();
   resp->RespondCode=0;      
   perCMDcode = mxmlFindElement(cmd, tree, "CMDcode",NULL, NULL,MXML_DESCEND);
   if(atoi(perCMDcode->child->value.text.string) !=cmdCode) {
	      resp->RespondCode=ERROR_CODE_3_EXECULTE_FAILED;
	      resp->Group[0].Main_inform  = "指令号错误";
	      resp->Group[0].Error_inform = "Error:CMDcode Error[指令号错误]";
	      return resp;    
   }
   else{
            /*解析XML消息*/          
	PN =mxmlFindElement(cmd, tree, "PN",NULL, NULL,MXML_DESCEND);
	intPN =  strtoul(PN->child->value.text.string, NULL, 0);
	protectmpar->PN = intPN;
	CM =mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	rtuCM =  strtoul(CM->child->value.text.string, NULL, 0);   
	CLP =mxmlFindElement(cmd, tree, "CLP",NULL, NULL,MXML_DESCEND);
	rtuCLP =  strtoul(CLP->child->value.text.string, NULL, 0);    
	for(i=0;i<intPN;i++){ 
		strQX[0]='\0';
		strcat(strQX,"Q");
		sprintf(strNum,"%d",i+1);
		strcat(strQX,strNum);

		QX  = mxmlFindElement(cmd, tree, strQX ,NULL, NULL,MXML_DESCEND);     
		uint_a = strtoul (QX->child->value.text.string, NULL, 0);          
		protectmpar->Group[i].PNo = uint_a;
		printf("Cancel PNo:%d\n",protectmpar->Group[i].PNo);
	}
	/*执行操作*/   
	mysql = SQL_Create();
	rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
        if( rc != SQLITE_OK ){
	   printf( "Open Database error.\n");
	}
        mysql->db =mydb;             

        for(i=0;i<intPN;i++)
        {
                        /*查看模块设置*/
		mysql->tableName ="SubModuleTypeTable";                  //检查子单元模块是否存在
		ModuleNo = ((protectmpar->Group[i].PNo-1)/4)+1;
		uint32tostring(ModuleNo,strMNo);

		mysql->mainKeyValue = strMNo;
		mysql->filedsName   = "UseFlag";
		SQL_lookupPar(mysql,&result,&rednum);
		UseFlag=atoi(result[0]); 
                SQL_freeResult(&result,&rednum);  
  
		mysql->mainKeyValue = strMNo;
		mysql->filedsName   = "ModuleType";
		SQL_lookupPar(mysql,&result,&rednum);
		ModType=atoi(result[0]); 
		SQL_freeResult(&result,&rednum);   
                if(UseFlag!=1){
		      resp->RespondCode  =ERROR_CODE_14_DBMATCH_FAILED;
                      if(resp->SNorPN!=TYPE_SNo){
		          resp->ErrorSN      = 1;                     
		          resp->SNorPN       = TYPE_PNo;
		          resp->Group[ErrorSNo].SNo =  protectmpar->Group[i].PNo;
		          resp->Group[ErrorSNo].Main_inform  = "模块未激活";
			  resp->Group[ErrorSNo].Error_inform = "Error: Don't have such Module\n";
		          ErrorSNo++;
                      }  

                }else{
                                
                       if(ModType==MODE3_PROTECT_MASTER){
                                sprintf(strPNo,"%d",protectmpar->Group[i].PNo);   
   
		                            /*删除已有配对组*/
                                mysql->tableName    = "ProtectGroupTable";
				mysql->mainKeyValue  =  strPNo;
                                existFlag=SQL_existIN_db(mysql);
                                if(existFlag){ 
  
                                        mysql->filedsName   = "SNoA";
                                        SQL_lookupPar(mysql,&result,&rednum);
                                        strcpy(strSNoA,result[0]);
                                        SQL_freeResult(&result,&rednum);

                                        mysql->filedsName   = "SNoB";
                                        SQL_lookupPar(mysql,&result,&rednum);
                                        strcpy(strSNoB,result[0]);
                                        SQL_freeResult(&result,&rednum);

                                        mysql->filedsName   = "sSNoA";
                                        SQL_lookupPar(mysql,&result,&rednum);
                                        strcpy(strSNoC,result[0]);
                                        SQL_freeResult(&result,&rednum);

                                        mysql->filedsName   = "sSNoB";
                                        SQL_lookupPar(mysql,&result,&rednum);
                                        strcpy(strSNoD,result[0]);
                                        SQL_freeResult(&result,&rednum);

				         mysql->filedsName   =  "sModNo";
				         SQL_lookupPar(mysql,&result,&rednum);
				         slaverModuleNo=atoi(result[0]);
				         SQL_freeResult(&result,&rednum);


                                         mysql->filedsName   = "sIP";
                                         SQL_lookupPar(mysql,&result,&rednum);
                                         strcpy(sIP,result[0]);
                                         SQL_freeResult(&result,&rednum);

                                          mysql->filedsName   = "Status";
                                          SQL_lookupPar(mysql,&result,&rednum);
                                          Status=atoi(result[0]);
                                          SQL_freeResult(&result,&rednum);
                                          if(Status==STATUS_AUTO_PROTECT)ProtectCounter++; 

                                          if(!semaphore_p())  
				             exit(EXIT_FAILURE); 

                                          SQL_delete(mysql);
                                        
                                          /*删除障碍告警测试*/     
					  mysql->tableName   = "AlarmTestSegmentTable";   
					  mysql->mainKeyValue= strSNoA;
					  existFlag=SQL_existIN_db(mysql);  
					  if(existFlag){   
                                             SQL_delete(mysql);                                     
					  }

					  mysql->mainKeyValue= strSNoB;
					  existFlag=SQL_existIN_db(mysql); 
					  if(existFlag){     
                                             SQL_delete(mysql); 
					  } 

					  mysql->mainKeyValue= strSNoC;
					  existFlag=SQL_existIN_db(mysql); 
					  if(existFlag){     
                                             SQL_delete(mysql); 
					  } 

					  mysql->mainKeyValue= strSNoD;
					  existFlag=SQL_existIN_db(mysql); 
					  if(existFlag){     
                                             SQL_delete(mysql); 
					  } 
                                          if(!semaphore_v())  
				             exit(EXIT_FAILURE); 

				          /*删除从端模块*/ 
		                          ctx=newModBus_TCP_Client(sIP); 
				          if( ctx== NULL ){
		                               printf("Warming: cann't connect to slaver module:%d \n",protectmpar->Group[i].PNo);
				          }else{
					     if(deleteSlaverProtectModule(ctx,slaverModuleNo)<0){
						printf("Warming: Error to delete slaver protect module!\n");   
					     }else{
						printf("Warming: delete slaver protect module successful!\n");                           
					     }
				          }
					  freeModbus_TCP_Client(ctx); 
					  ctx=NULL;
                                }
                                
                       }else{
				resp->RespondCode  =ERROR_CODE_14_DBMATCH_FAILED;
				if(resp->SNorPN!=TYPE_SNo){
				       resp->ErrorSN      = 1;                     
				       resp->SNorPN       = TYPE_PNo;
				       resp->Group[ErrorSNo].SNo =  protectmpar->Group[i].PNo;
				       resp->Group[ErrorSNo].Main_inform  = "所属模块不支持光保护";
				       resp->Group[ErrorSNo].Error_inform = "Error: Don't support protect Module.[所属模块不支持光保护]\n";
				       ErrorSNo++;
				} 
                      }//end if(ModType =3)
               }//end if(UseFlag) 
      }

      OpticalProtect_Destory(protectmpar);
      SQL_Destory(mysql);  
      sqlite3_close(mydb);
      if(resp->RespondCode != 0 ){
	   resp->ErrorSN     =  ErrorSNo;                  
           return resp;
      }

      int signum;
      union sigval mysigval;
      char* process;  
      int retProcess = 0,n=0;  
      pid_t protectPID[MAX_PID_NUM];  


      if(ProtectCounter>0){ 
                            /*发送取消保护信号*/
	       process ="/web/cgi-bin/ProtectMasterMain";                        
	       retProcess = get_pid_by_name(process, protectPID, MAX_PID_NUM);  
	       if(retProcess>0){
		       printf("process '%s' is existed? (%d): %c\n", process, retProcess, (retProcess > 0)?'y':'n');  
		       signum=SIGRTMIN+1;                                          
		       mysigval.sival_int = 250;                               //设置信号的附加信息 (取消保护)                               
		       for(n=0;n<retProcess;n++){  
		       printf("ProtectMasterMain-PID:%u\n", protectPID[n]);                
		       if(sigqueue(protectPID[n],signum,mysigval)==-1)
			       printf("send signal error\n");
		      }  
			   /*等待成功信号*/

		    if(recvMessageQueue_Backstage("250-OK",PROTECT_MESSAGE_TYPE) == 1){                     //遇"250-OK"结束
			    printf("Cancel Protect Group operation sucessful!\n");
		    }else{
		            printf("Cancel Protect Group operation failed!\n");
			    resp->RespondCode=ERROR_CODE_3_EXECULTE_FAILED;
			    resp->Group[0].Main_inform  = "切换保护节点删除失败-->未收到回复消息";
			    resp->Group[0].Error_inform = "Error:Don't get back massgae![切换保护节点删除失败-->未收到回复消息]";

			    return resp;   
		    }
		   
	      }
      }

   }   //end if(atoi(perCMDcode->child->value.text.string) !=cmdCode)
   return resp;
}

