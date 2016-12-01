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
/**************************解析XML消息***************************************/
        getOpticalProtectParameter(cmd,tree,protectmpar);
	CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	intCM = strtoul (CM->child->value.text.string, NULL, 0); 
	CLP = mxmlFindElement(cmd, tree, "CLP",NULL, NULL,MXML_DESCEND);
	intCLP = strtoul (CLP->child->value.text.string, NULL, 0); 	
        int i=0,j=0;
        int PN;
        PN=protectmpar->PN;
        for (i=0;i<PN;i++){
             printf("----------Px:%d------------\n",protectmpar->Group[i].PNo);              
             printf("    SNoA:%d------SNoB:%d  -----SwitchPos:%d\n",protectmpar->Group[i].SNoA,protectmpar->Group[i].SNoB,protectmpar->Group[i].SwitchPos);      
        } 
/**************************执行操作********************************/   
	sqlite3 *mydb=NULL;
	sql *mysql=NULL;
	int rc=0,rednum=0,flagA=0,flagB=0,UseFlag=0,PortFlagA=0,PortFlagB=0,flagSW=0,existFlag=0,flagStatus=0;
        int ErrorSNo=0,ModuleNo=0,resPN=0,ModType=0,SwitchPos=0,DoSwitchCounter=0,AlarmCounter=0,fiberTypeA=0,fiberTypeB=0;
        char *strMNo=NULL,*strPNo=NULL,*strSNoA=NULL,*strSNoB=NULL; 
        char **result=NULL;
        char * strSQL=NULL;
        char resultPNo[64][5];
        strSQL   = (char *) malloc(sizeof(char)*400);
        strSNoA  = (char *) malloc(sizeof(char)*10);
        strSNoB  = (char *) malloc(sizeof(char)*10);
	strMNo   = (char *) malloc(sizeof(char)*10);
        strPNo   = (char *) malloc(sizeof(char)*10);
	mysql = SQL_Create();
	rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
        if( rc != SQLITE_OK ){
	   printf( "Open Database error.\n");
	}
        mysql->db =mydb;             
        mysql->tableName   = "ProtectGroupTable"; 
        for(i=0;i<PN;i++)
        { 
                        /*查看模块设置和端口占用*/
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
                uint32tostring(protectmpar->Group[i].PNo,strPNo);
		mysql->tableName ="PortOccopyTable";                    //检查子端口是否占用
		mysql->mainKeyValue = strSNoA;
		PortFlagA = SQL_existIN_db(mysql); 
                if(PortFlagA==1){
                        mysql->filedsName   = "fiberType";
			SQL_lookupPar(mysql,&result,&rednum);
                        fiberTypeA=atoi(result[0])-1; 
                        SQL_freeResult(&result,&rednum);
                }

		mysql->mainKeyValue = strSNoB;
		PortFlagB = SQL_existIN_db(mysql); 
                if(PortFlagB==1){
                        mysql->filedsName   = "fiberType";
			SQL_lookupPar(mysql,&result,&rednum);
                        fiberTypeB=atoi(result[0])-1; 
                        SQL_freeResult(&result,&rednum);
                }
           
               if(UseFlag!=1 ){
		      resp->RespondCode  =14;
                      if(resp->SNorPN!=TYPE_SNo){
		          resp->ErrorSN      = 1;                     
		          resp->SNorPN       = TYPE_PNo;
		          resp->Group[ErrorSNo].SNo =  protectmpar->Group[i].PNo;
		          resp->Group[ErrorSNo].Main_inform  = "模块未激活";
			  resp->Group[ErrorSNo].Error_inform = "Error: Don't have such Module\n";
		          ErrorSNo++;
                      }  

                }else if(PortFlagA!=1 || PortFlagB!=1){
		      resp->RespondCode  = 14;
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
                                
                       if(ModType==3){
		                          /*检查是否已有配对组*/
                                mysql->tableName    = "ProtectGroupTable";
				mysql->filedsName   = "SNoA";
				mysql->filedsValue  =  strSNoA;
				flagA = SQL_Unique(mysql);                                                // 检查SNoA在配对表中是否已经存在
				if(flagA){
				    resPN=SQL_findPNo(mysql,resultPNo);
				    for(j=0;j<resPN;j++){
					  mysql->mainKeyValue = resultPNo[j];
					  SQL_delete(mysql);
				    }   
				}
				mysql->filedsName   = "SNoB";
				mysql->filedsValue  =  strSNoB;
				flagB = SQL_Unique(mysql);                                                 // 检查SNoB在配对表中是否已经存在
				if(flagB){
				    resPN=SQL_findPNo(mysql,resultPNo);
				    for(j=0;j<resPN;j++){
				       mysql->mainKeyValue = resultPNo[j];
				       SQL_delete(mysql);
				    }      
			       }

				           /*初始化2*2开关位置*/
				DoSwitchCounter=5;
				if(!setModbus_P())                                              //ModBus总线保护
					exit(EXIT_FAILURE);    
				//usleep(40000);
				while(DoSwitchCounter>0){
		                        //usleep(2000);
					modbus_t * mb =newModbus(MODBUS_DEV,MODBUS_BUAD);
					flagSW= doOpticalProtectSwitch(mb,protectmpar->Group[i].PNo,protectmpar->Group[i].SwitchPos);
				        if(flagSW==0)break;
				        else DoSwitchCounter--;
					freeModbus(mb);   
				} 
				//usleep(50000);            
				if(!setModbus_V())                                              //解除ModBus总线保护                  
					exit(EXIT_FAILURE);

				         /*切换成功则更新数据库及后台测试节点*/     
				if(DoSwitchCounter>0 && flagSW==0){
							/*更新增加配对组*/						     
					sprintf(strSQL,"%d,%d,%d,%d,%d,%d,%d\n"                 /*PNo,rtuCM,rtuCLP,SNoA,SNoB,Status*/
							  ,protectmpar->Group[i].PNo            //PNo
							  ,intCM                                //rtuCM
							  ,intCLP                               //rtuCLP
							  ,protectmpar->Group[i].SNoA           //SNoA
							  ,protectmpar->Group[i].SNoB           //SNoB
							  ,protectmpar->Action                  //Status
							  ,protectmpar->Group[i].SwitchPos      //SwitchPos
							  );
					mysql->filedsValue = strSQL;
					if(!semaphore_p())  
					   exit(EXIT_FAILURE);                                    //P
					SQL_add(mysql);                                            //更新或者插入新的纪录
					if(!semaphore_v())                                         //V
					    exit(EXIT_FAILURE);
		                                        /*检查是否已有障碍告警测试*/
					mysql->tableName   = "AlarmTestSegmentTable";   
					mysql->mainKeyValue= strSNoA;
					existFlag=SQL_existIN_db(mysql);  
					if(existFlag){                                           //SNoA存在
						mysql->filedsName ="Status";
						SQL_lookupPar(mysql,&result,&rednum);
						flagStatus=atoi(result[0]); 
						SQL_freeResult(&result,&rednum);	
						if(flagStatus==1){
							AlarmCounter++; 
							mysql->filedsValue  = "-1";                       
							SQL_modify(mysql);                       //更新障碍告警后台测试节点
				                         mysql->filedsName  = "fiberType";
							 if(protectmpar->Group[i].SwitchPos == PARALLEL ){
						                 if(protectmpar->Group[i].SNoA%2==1)
								       mysql->filedsValue = "0";                          //SNoA更新为备纤类型
						                 else 
						                 if(protectmpar->Group[i].SNoA%2==0)
								       mysql->filedsValue = "1";                          //SNoA更新为在纤类型
								  SQL_modify(mysql);                              
							 }else  
							 if(protectmpar->Group[i].SwitchPos == ACROSS){
								 if(protectmpar->Group[i].SNoA%2==1)
								       mysql->filedsValue = "1";                          //SNoA更新为备纤类型
								 else 
							         if(protectmpar->Group[i].SNoA%2==0)
									mysql->filedsValue = "0";                          //SNoA更新为在纤类型
								 SQL_modify(mysql);                              
							 }

                                                        mysql->filedsValue  = "1";
                                                        mysql->filedsName   = "protectFlag";
                                                        SQL_modify(mysql);     
						}
					}//end if(existFlag 1)

					mysql->mainKeyValue= strSNoB;
					existFlag=SQL_existIN_db(mysql); 
					if(existFlag){                                           //SNoB存在
						mysql->filedsName ="Status";
						SQL_lookupPar(mysql,&result,&rednum);
						flagStatus=atoi(result[0]); 
						SQL_freeResult(&result,&rednum);
						if(flagStatus==1){
				                        AlarmCounter++;
							mysql->filedsValue = "-1";                       
							SQL_modify(mysql);                                                //更新障碍告警后台测试节点

					 		mysql->filedsName ="fiberType";
							if(protectmpar->Group[i].SwitchPos == PARALLEL ){
								if(protectmpar->Group[i].SNoB%2==1)
									mysql->filedsValue = "0";                          //SNoB更新为备纤类型
								else 
									if(protectmpar->Group[i].SNoB%2==0)
										mysql->filedsValue = "1";                          //SNoB更新为在纤类型
										SQL_modify(mysql);                              
									}else  
							if(protectmpar->Group[i].SwitchPos == ACROSS){
								if(protectmpar->Group[i].SNoB%2==1)
									mysql->filedsValue = "1";                          //SNoB更新为在纤类型
								else 
								if(protectmpar->Group[i].SNoB%2==0)
									mysql->filedsValue = "0";                          //SNoB更新为备纤类型
									SQL_modify(mysql);                              
							}

                                                        mysql->filedsValue  = "1";
                                                        mysql->filedsName   = "protectFlag";
                                                        SQL_modify(mysql);                                                           
						}
					}  //end if(existFlag 2)


                                }else{
					DoSwitchCounter=5;
					if(!setModbus_P())                                       //ModBus总线保护
						exit(EXIT_FAILURE);    
					usleep(40000);
					while(DoSwitchCounter>0){
					usleep(2000);
					modbus_t * mb =newModbus(MODBUS_DEV,MODBUS_BUAD);
					flagSW= doOpticalProtectSwitch(mb,protectmpar->Group[i].PNo,protectmpar->Group[i].SwitchPos);
					if(flagSW==0)break;
					else DoSwitchCounter--;
					freeModbus(mb);   
					} 
					usleep(50000);            
					if(!setModbus_V())                                       //解除ModBus总线保护                  
						exit(EXIT_FAILURE); 
					resp->RespondCode = 13 ;                             
					if(resp->SNorPN!=TYPE_SNo){
						resp->ErrorSN      = 1;                     
						resp->SNorPN       = TYPE_PNo;
						resp->Group[ErrorSNo].SNo = protectmpar->Group[i].PNo;
						resp->Group[ErrorSNo].Main_inform  = "光保护切换:光开关切换失败";
						resp->Group[ErrorSNo].Error_inform = "Error: Optical Switch Failed[光保护切换:光开关切换失败]!\n";
						ErrorSNo++;  
					} 

                                }//end if(DoSwitchCounter>0 && flagSW==0)
                       }else{
				resp->RespondCode  =14;
				if(resp->SNorPN!=TYPE_SNo){
				       resp->ErrorSN      = 1;                     
				       resp->SNorPN       = TYPE_PNo;
				       resp->Group[ErrorSNo].SNo =  protectmpar->Group[i].PNo;
				       resp->Group[ErrorSNo].Main_inform  = "所属模块不支持光保护切换";
				       resp->Group[ErrorSNo].Error_inform = "Error: Don't support protect Module.[所属模块不支持光保护切换]\n";
				       ErrorSNo++;
				} 
                      }// end if(ModType =3)
               }// end if(UseFlag && PortFlag) 
      }
      free(strMNo);
      free(strSNoA);
      free(strSNoB);
      free(strPNo);
      free(strSQL);
      OpticalProtect_Destory(protectmpar);
      SQL_Destory(mysql);  
      sqlite3_close(mydb);
      if(resp->RespondCode != 0 ){
	   resp->ErrorSN     =  ErrorSNo;                  //错误光路总条数
      }
/***************************障碍告警测试进程发送信号**********************
(1)向障碍告警测试守护进程发送取消障碍告警测试信号
(2)注意一定要将发送程序和接收程序划到一个用户组，并且都具有root权限，否则信号发射会失败(BoaCgi 4777)
(3)程可以通过sigqueue函数向包括它本身在内的其他进程发送一个信号，如果程序没有发送这个信号的权限，对sigqueue函数的调用就将失败，而失败的常见原因是目标进程由另一个用户所拥有
(4)现阶段使用的是不可靠信号,不支持排队，信号可能丢失。
***********************************************************************/ 
       if(AlarmCounter>0){
	       int signum;
	       union sigval mysigval;
	       char* process;  
	       int retProcess = 0,n=0;  
	       pid_t cycPID[MAX_PID_NUM];  
	      
	       process ="/web/cgi-bin/alarmMain";                        
	       retProcess = get_pid_by_name(process, cycPID, MAX_PID_NUM);  
	       if(retProcess>0){
		       printf("process '%s' is existed? (%d): %c\n", process, retProcess, (retProcess > 0)?'y':'n');  
		       signum=SIGUSR1;                                         //设置信号值:插入或修改周期测试链表节点值
		       mysigval.sival_int = 170;                               //设置信号的附加信息 (启动障碍告警测试)                               
		       for(n=0;n<retProcess;n++){  
		       printf("alarmMaim-PID:%u\n", cycPID[n]);                //获取障碍告警测试守护进程PID
		       if(sigqueue(cycPID[n],signum,mysigval)==-1)
			       printf("send signal error\n");
		      }  
			   /*等待成功信号*/

		    char * recvStr;  
		    recvStr = (char *) malloc (sizeof (char)*10);
		    recvStr = recvMessageQueue_C();
		    if(strncmp(recvStr, "170-OK", 6) == 0)                     //遇"370-OK"结束
			    printf("Set Protect Group sucessful!\n");
		    else{
		            printf("Protect Group failed!\n");
			    resp->RespondCode=3;
			    resp->Group[0].Main_inform  = "设置切换保护组失败-->未收到回复消息";
			    resp->Group[0].Error_inform = "Error:Don't get back massgae![设置切换保护组失败-->未收到回复消息]";
			    return resp;   
		    }
		    free(recvStr);
		   
	      }else{
		      resp->RespondCode=3;
		      resp->Group[0].Main_inform  = "设置切换保护组失败-->未找到后台进程";
		      resp->Group[0].Error_inform = "Error:Don't have back process![设置切换保护组失败-->未找到后台进程]";
		      return resp; 
	      }	
       }

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

           char **result = NULL;
           int flagA=-1,flagB=-1;
           int rc=0,rednum=0,dbSNoA=0,dbSNoB=0;
	   mysql->filedsName   =  "SNoA";
	   SQL_lookupPar(mysql,&result,&rednum);
           dbSNoA=atoi(result[0]);
           SQL_freeResult(&result,&rednum);
           printf("PNo=%s,SNoA=%d\n",mysql->mainKeyValue,dbSNoA);
	   

	   mysql->filedsName   =  "SNoB";
	   SQL_lookupPar(mysql,&result,&rednum);
           dbSNoB=atoi(result[0]);
           SQL_freeResult(&result,&rednum);

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
	      resp->Group[0].Main_inform  = "指令号错误";
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
            resp->RespondCode  =0;
            protectmpar->Action= 1; 
            printf("PN=%d\n",protectmpar->PN); 
            for (i=0;i<PN;i++){
                    printf("----------requstPx:%d------------\n",protectmpar->Group[i].PNo);              
                    printf("    SNoA:%d------SNoB:%d -------SwitchPos:%d\n",protectmpar->Group[i].SNoA,protectmpar->Group[i].SNoB,protectmpar->Group[i].SwitchPos);      
            }  
/**************************执行光保护***************************/
	 sqlite3 *mydb;
	 sql *mysql;
	 int rc=0,rednum=0,flagMatch=0,UseFlag=0,PortFlagA=0,PortFlagB=0,flagSW=0,existFlag=0,flagStatus=0;
         int ErrorSNo=0,ModuleNo=0,SwitchStatus=0,ModType=0,SwitchPos=0,DoSwitchCounter=0,AlarmCounter=0,fiberType=0;
         char *strPNo=NULL,*strMNo=NULL,*strSNoA=NULL,*strSNoB=NULL,*strSwitchPos=NULL; 
         char **result=NULL;
	 strPNo    = (char *) malloc(sizeof(char)*10);
         strMNo    = (char *) malloc(sizeof(char)*10); 
         strSNoA   = (char *) malloc(sizeof(char)*10);
         strSNoB   = (char *) malloc(sizeof(char)*10); 
         strSwitchPos =(char *) malloc(sizeof(char)*10);

	 mysql = SQL_Create();
	 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
	 if( rc != SQLITE_OK ){
	     printf( "Open Database error.\n");
	 }
	 mysql->db =mydb;
	 mysql->tableName    =  "ProtectGroupTable"; 
         for (i=0;i<PN;i++){

                        /*查看模块设置和端口占用*/
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
		      resp->RespondCode  =14;
                      if(resp->SNorPN!=TYPE_SNo){
		          resp->ErrorSN      = 1;                     
		          resp->SNorPN       = TYPE_PNo;
		          resp->Group[ErrorSNo].SNo =  protectmpar->Group[i].PNo;
		          resp->Group[ErrorSNo].Main_inform  = "模块未激活";
			  resp->Group[ErrorSNo].Error_inform = "Error: Don't have such Module\n";
		          ErrorSNo++;
                      }  

                }else if(PortFlagA!=1 || PortFlagB!=1){
		      resp->RespondCode  = 14;
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
              
                    if(ModType==3){
		                          /*查看保护组*/
		            mysql->tableName    ="ProtectGroupTable";
			    uint32tostring(protectmpar->Group[i].PNo,strPNo);
			    mysql->mainKeyValue = strPNo;
                            existFlag=SQL_existIN_db(mysql); 
                            if(existFlag){
				    mysql->filedsName   =  "Status";
				    SQL_lookupPar(mysql,&result,&rednum);
				    SwitchStatus =atoi(result[0]);
				    SQL_freeResult(&result,&rednum);

				    mysql->filedsName   =  "SwitchPos";
				    SQL_lookupPar(mysql,&result,&rednum);
				    SwitchPos =atoi(result[0]);
				    SQL_freeResult(&result,&rednum);

				    flagMatch=ifProtectMatch(protectmpar->Group[i].SNoA,protectmpar->Group[i].SNoB,mysql); 

				    if(flagMatch==1 && SwitchStatus==1 ){
				                 /*执行保护切换*/
				            DoSwitchCounter=5;
					    if(!setModbus_P())                                              //ModBus总线保护
						  exit(EXIT_FAILURE);    
					    usleep(40000);
				            while(DoSwitchCounter>0){
		                               usleep(2000);
					       modbus_t * mb =newModbus(MODBUS_DEV,MODBUS_BUAD);
					       flagSW= doOpticalProtectSwitch(mb,protectmpar->Group[i].PNo,protectmpar->Group[i].SwitchPos);
				               if(flagSW==0)break;
				               else DoSwitchCounter--;
					       freeModbus(mb);   
				            } 
					    usleep(50000);            
					    if(!setModbus_V())                                              //解除ModBus总线保护                  
						 exit(EXIT_FAILURE);
				               
				                 /*切换成功则更新数据库及后台测试节点*/     
				            if(DoSwitchCounter>0 && flagSW==0){  
						    if(!semaphore_p())  
							exit(EXIT_FAILURE);                                //数据库修改保护                 
						                        /*更新保护组表*/
				                    mysql->mainKeyValue = strPNo;
				                    mysql->filedsName  = "SwitchPos";                                    
				                    uint32tostring(protectmpar->Group[i].SwitchPos,strSwitchPos);
				                    mysql->filedsValue = strSwitchPos;
				                    SQL_modify(mysql);

								     /*更新端口表的光纤类型*/
				                    mysql->tableName   = "PortOccopyTable";
				 		    mysql->filedsName  = "fiberType";
		                                    mysql->mainKeyValue= strSNoA;
		                                    if(protectmpar->Group[i].SwitchPos == PARALLEL ){
                                                        if(protectmpar->Group[i].SNoA%2==1)
		                                    	     mysql->filedsValue = "1";                          //SNoA更新为备纤类型
                                                        else 
                                                        if(protectmpar->Group[i].SNoA%2==0)
                                                             mysql->filedsValue = "2";                          //SNoA更新为在纤类型
		                                        SQL_modify(mysql);                              
		                                    }else
		                                    if(protectmpar->Group[i].SwitchPos == ACROSS ){
                                                        if(protectmpar->Group[i].SNoA%2==1)
		                                    	     mysql->filedsValue = "2";                          //SNoA更新为备纤类型
                                                        else 
                                                        if(protectmpar->Group[i].SNoA%2==0)
                                                             mysql->filedsValue = "1";                          //SNoA更新为在纤类型
		                                        SQL_modify(mysql);                              
		                                    }

		                                    mysql->mainKeyValue= strSNoB;
                                                    if(protectmpar->Group[i].SwitchPos == PARALLEL){
                                                        if(protectmpar->Group[i].SNoB%2==1)
		                                    	     mysql->filedsValue = "1";                          //SNoA更新为备纤类型
                                                        else 
                                                        if(protectmpar->Group[i].SNoB%2==0)
                                                             mysql->filedsValue = "2";                          //SNoA更新为在纤类型
		                                        SQL_modify(mysql);                              
		                                    }else
		                                    if(protectmpar->Group[i].SwitchPos == ACROSS ){
                                                        if(protectmpar->Group[i].SNoB%2==1)
		                                    	     mysql->filedsValue = "2";                          //SNoA更新为备纤类型
                                                        else 
                                                        if(protectmpar->Group[i].SNoB%2==0)
                                                             mysql->filedsValue = "1";                          //SNoA更新为在纤类型
		                                        SQL_modify(mysql);                              
		                                    }

						                        /*更新障碍告警表*/
						    mysql->tableName   = "AlarmTestSegmentTable";   
						    mysql->mainKeyValue= strSNoA;
						    existFlag=SQL_existIN_db(mysql);  
						    if(existFlag){                                              //SNoA存在
							    mysql->filedsName ="Status";
							    SQL_lookupPar(mysql,&result,&rednum);
							    flagStatus=atoi(result[0]); 
							    SQL_freeResult(&result,&rednum);
	
							    if(flagStatus==1){
						                    AlarmCounter++; 
						                    mysql->filedsValue  = "-1";                       
						                    SQL_modify(mysql);                          //更新障碍告警后台测试节点
		                                                    mysql->filedsName  = "fiberType";
						                    if(protectmpar->Group[i].SwitchPos == PARALLEL ){
				                                        if(protectmpar->Group[i].SNoA%2==1)
						                    	     mysql->filedsValue = "0";                          //SNoA更新为备纤类型
				                                        else 
				                                        if(protectmpar->Group[i].SNoA%2==0)
				                                             mysql->filedsValue = "1";                          //SNoA更新为在纤类型
						                        SQL_modify(mysql);                              
						                    }else  
				                                    if(protectmpar->Group[i].SwitchPos == ACROSS){
				                                        if(protectmpar->Group[i].SNoA%2==1)
						                    	     mysql->filedsValue = "1";                          //SNoA更新为备纤类型
				                                        else 
				                                        if(protectmpar->Group[i].SNoA%2==0)
				                                             mysql->filedsValue = "0";                          //SNoA更新为在纤类型
						                        SQL_modify(mysql);                              
						                    }   
						            }
						    }//end if(existFlag 1)

						    mysql->mainKeyValue= strSNoB;
						    existFlag=SQL_existIN_db(mysql); 
						    if(existFlag){                                              //SNoB存在
							    mysql->filedsName ="Status";
							    SQL_lookupPar(mysql,&result,&rednum);
							    flagStatus=atoi(result[0]); 
							    SQL_freeResult(&result,&rednum);
							    if(flagStatus==1){
		                                                    AlarmCounter++;
						                    mysql->filedsValue = "-1";                       
						                    SQL_modify(mysql);                          //更新障碍告警后台测试节点

				 				    mysql->filedsName ="fiberType";
						                    if(protectmpar->Group[i].SwitchPos == PARALLEL ){
				                                        if(protectmpar->Group[i].SNoB%2==1)
						                    	     mysql->filedsValue = "0";                          //SNoB更新为备纤类型
				                                        else 
				                                        if(protectmpar->Group[i].SNoB%2==0)
				                                             mysql->filedsValue = "1";                          //SNoB更新为在纤类型
						                        SQL_modify(mysql);                              
						                    }else  
				                                    if(protectmpar->Group[i].SwitchPos == ACROSS){
				                                        if(protectmpar->Group[i].SNoB%2==1)
						                    	     mysql->filedsValue = "1";                          //SNoB更新为在纤类型
				                                        else 
				                                        if(protectmpar->Group[i].SNoB%2==0)
				                                             mysql->filedsValue = "0";                          //SNoB更新为备纤类型
						                        SQL_modify(mysql);                              
						                    }                                                          
						            }
						    }  //end if(existFlag 2)

		                                    if(!semaphore_v())                                          //解除数据库修改保护
							 exit(EXIT_FAILURE); 
				            }else{
						    DoSwitchCounter=5;
						    if(!setModbus_P())                                          //ModBus总线保护
							  exit(EXIT_FAILURE);    
						    usleep(40000);
						    while(DoSwitchCounter>0){
				                       usleep(2000);
						       modbus_t * mb =newModbus(MODBUS_DEV,MODBUS_BUAD);
						       flagSW= doOpticalProtectSwitch(mb,protectmpar->Group[i].PNo,SwitchPos);
						       if(flagSW==0)break;
						       else DoSwitchCounter--;
						       freeModbus(mb);   
						    } 
						    usleep(50000);            
						    if(!setModbus_V())                                          //解除ModBus总线保护                  
							 exit(EXIT_FAILURE); 
				                    resp->RespondCode = 13 ;                             
						    if(resp->SNorPN!=TYPE_SNo){
							  resp->ErrorSN      = 1;                     
							  resp->SNorPN       = TYPE_PNo;
							  resp->Group[ErrorSNo].SNo = protectmpar->Group[i].PNo;
							  resp->Group[ErrorSNo].Main_inform  = "光保护切换:光开关切换失败";
							  resp->Group[ErrorSNo].Error_inform = "Error: Optical Switch Failed[光保护切换:光开关切换失败]!\n";
							  ErrorSNo++;  
						    } 
				            } // end if(DoSwitchCounter>0 && flagSW==0)

				    }else{
				          if(resp->SNorPN!=TYPE_SNo){
						  resp->RespondCode = 14 ;                                      // 参数错误
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
				   } //end if(flagMatch==1 && SwitchStatus==1)
                           }else{
		            	resp->RespondCode = 14 ;                             
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
			resp->RespondCode  =14;
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
	free(strSwitchPos);  
        OpticalProtect_Destory(protectmpar);
	SQL_Destory(mysql);  
	sqlite3_close(mydb);
	if(resp->RespondCode != 0 ){
	   resp->ErrorSN     =  ErrorSNo;                  //错误光路总条数
	}
       
/***************************障碍告警测试进程发送信号**********************
(1)向障碍告警测试守护进程发送取消障碍告警测试信号
(2)注意一定要将发送程序和接收程序划到一个用户组，并且都具有root权限，否则信号发射会失败(BoaCgi 4777)
(3)程可以通过sigqueue函数向包括它本身在内的其他进程发送一个信号，如果程序没有发送这个信号的权限，对sigqueue函数的调用就将失败，而失败的常见原因是目标进程由另一个用户所拥有
(4)现阶段使用的是不可靠信号,不支持排队，信号可能丢失。
***********************************************************************/ 
       if(AlarmCounter>0){
	       int signum;
	       union sigval mysigval;
	       char* process;  
	       int retProcess = 0,n=0;  
	       pid_t cycPID[MAX_PID_NUM];  
	      
	       process ="/web/cgi-bin/alarmMain";                        
	       retProcess = get_pid_by_name(process, cycPID, MAX_PID_NUM);  
	       if(retProcess>0){
		       printf("process '%s' is existed? (%d): %c\n", process, retProcess, (retProcess > 0)?'y':'n');  
		       signum=SIGUSR1;                                         //设置信号值:插入或修改周期测试链表节点值
		       mysigval.sival_int = 370;                               //设置信号的附加信息 (启动障碍告警测试)                               
		       for(n=0;n<retProcess;n++){  
		       printf("alarmMaim-PID:%u\n", cycPID[n]);                //获取障碍告警测试守护进程PID
		       if(sigqueue(cycPID[n],signum,mysigval)==-1)
			       printf("send signal error\n");
		      }  
			   /*等待成功信号*/

		    char * recvStr;  
		    recvStr = (char *) malloc (sizeof (char)*10);
		    recvStr = recvMessageQueue_C();
		    if(strncmp(recvStr, "370-OK", 6) == 0)                     //遇"370-OK"结束
			    printf("Do Switch operation sucessful!\n");
		    else{
		            printf("Do Switch operation failed!\n");
			    resp->RespondCode=3;
			    resp->Group[0].Main_inform  = "切换保护节点更新失败-->未收到回复消息";
			    resp->Group[0].Error_inform = "Error:Don't get back massgae![切换保护节点更新失败-->未收到回复消息]";
			    return resp;   
		    }
		    free(recvStr);
		   
	      }else{
		      resp->RespondCode=3;
		      resp->Group[0].Main_inform  = "切换保护节点更新失败-->未找到后台进程";
		      resp->Group[0].Error_inform = "Error:Don't have back process![切换保护节点更新失败-->未找到后台进程]";
		      return resp; 
	      }
      }

  }  //end if(atoi(perCMDcode->child->value.text.string) !=cmdCode)
  return resp;
}

responed *  endOpticalProtectSegment(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)          //250
{
   mxml_node_t *CM=NULL,*CLP=NULL,*QX=NULL,*PN=NULL,*perCMDcode=NULL;
   opticalprotect *protectmpar=NULL;
   responed *resp=NULL; 
   int rtuCM=0,rtuCLP=0,intPN=0,SNo=0,uint_a=0;
   int i=0,j=0;
   char      strQX[3]="Qx";
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
/**************************解析XML消息***************************************/
	    PN =mxmlFindElement(cmd, tree, "PN",NULL, NULL,MXML_DESCEND);
	    intPN =  strtoul(PN->child->value.text.string, NULL, 0);
	    protectmpar->PN = intPN;
	    protectmpar->Action = 2;                                                                          //将状态设置为-2，等待从取障碍告警测试中删除
	    CM =mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	    rtuCM =  strtoul(CM->child->value.text.string, NULL, 0);   
	    CLP =mxmlFindElement(cmd, tree, "CLP",NULL, NULL,MXML_DESCEND);
	    rtuCLP =  strtoul(CLP->child->value.text.string, NULL, 0);    
	    for (i=0;i<intPN;i++)
	    { 
		strQX[1]=i+0x31;
		QX  = mxmlFindElement(cmd, tree, strQX ,NULL, NULL,MXML_DESCEND);     
		uint_a = strtoul (QX->child->value.text.string, NULL, 0);          
		protectmpar->Group[i].PNo = uint_a;
		printf("Cancel PNo---------->%d\n",protectmpar->Group[i].PNo);
	    }
/**************************执行操作********************************/   
	sqlite3 *mydb=NULL;
	sql *mysql=NULL;
	int rc=0,rednum=0,UseFlag=0,existFlag=0,flagStatus=0;
        int ErrorSNo=0,ModuleNo=0,resPN=0,ModType=0,AlarmCounter=0;
        char *strMNo=NULL,*strSNoA=NULL,*strSNoB=NULL,*strPNo=NULL; 
        char **result=NULL;

        char resultPNo[64][5];
        strSNoA  = (char *) malloc(sizeof(char)*10);
        strSNoB  = (char *) malloc(sizeof(char)*10);
        strPNo   = (char *) malloc(sizeof(char)*10);
	strMNo   = (char *) malloc(sizeof(char)*10);
	mysql = SQL_Create();
	rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
        if( rc != SQLITE_OK ){
	   printf( "Open Database error.\n");
	}
        mysql->db =mydb;             
        mysql->tableName   = "ProtectGroupTable"; 
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
                if(UseFlag==1){
			mysql->mainKeyValue = strMNo;
			mysql->filedsName   = "ModuleType";
			SQL_lookupPar(mysql,&result,&rednum);
                        ModType=atoi(result[0]); 
                        SQL_freeResult(&result,&rednum);
                }         
                if(UseFlag!=1){
		      resp->RespondCode  =14;
                      if(resp->SNorPN!=TYPE_SNo){
		          resp->ErrorSN      = 1;                     
		          resp->SNorPN       = TYPE_PNo;
		          resp->Group[ErrorSNo].SNo =  protectmpar->Group[i].PNo;
		          resp->Group[ErrorSNo].Main_inform  = "模块未激活";
			  resp->Group[ErrorSNo].Error_inform = "Error: Don't have such Module\n";
		          ErrorSNo++;
                      }  

                }else{
                                
                       if(ModType==3){
                                uint32tostring( protectmpar->Group[i].PNo,strPNo); 
		                            /*删除已有配对组*/
                                mysql->tableName    = "ProtectGroupTable";
				mysql->filedsName   = "PNo";
				mysql->filedsValue  =  strPNo;
				resPN=SQL_findPNo(mysql,resultPNo);
				for(j=0;j<resPN;j++){
					mysql->mainKeyValue = resultPNo[j];
					SQL_delete(mysql);
				}   
				         /*更新数据库及后台测试节点*/     
				if(resPN>0){  //删除成功
					uint32tostring((protectmpar->Group[i].PNo)*2-1,strSNoA);
					uint32tostring((protectmpar->Group[i].PNo)*2  ,strSNoB);
		                                        /*检查是否已有障碍告警测试*/
					mysql->tableName   = "AlarmTestSegmentTable";   
					mysql->mainKeyValue= strSNoA;
					existFlag=SQL_existIN_db(mysql);  
					if(existFlag){                                           //SNoA存在
						mysql->filedsName ="Status";
						SQL_lookupPar(mysql,&result,&rednum);
						flagStatus=atoi(result[0]); 
						SQL_freeResult(&result,&rednum);
						if(flagStatus==1){
							AlarmCounter++; 
							mysql->filedsValue  = "-1";                       
							SQL_modify(mysql);                       //待删除障碍告警后台测试节点
                                                        mysql->filedsValue  = "0";
                                                        mysql->filedsName   = "protectFlag";
                                                        SQL_modify(mysql);                       
                                                }
					}//end if(existFlag 1)

					mysql->mainKeyValue= strSNoB;
					existFlag=SQL_existIN_db(mysql); 
					if(existFlag){                                           //SNoB存在
						mysql->filedsName ="Status";
						SQL_lookupPar(mysql,&result,&rednum);
						flagStatus=atoi(result[0]); 
						SQL_freeResult(&result,&rednum);
						if(flagStatus==1){
				                        AlarmCounter++;
							mysql->filedsValue = "-1";                       
							SQL_modify(mysql);                       //待删除更新障碍告警后台测试节点
                                                        mysql->filedsValue  = "0";
                                                        mysql->filedsName   = "protectFlag";
                                                        SQL_modify(mysql);  
                                                }
					}//end if(existFlag 2)

                                }else //删除失败 end if(resPN>0)                                 
                                    printf("Warming: Don't have such PNo :%d \n",protectmpar->Group[i].PNo);
                                
                       }else{
				resp->RespondCode  =14;
				if(resp->SNorPN!=TYPE_SNo){
				       resp->ErrorSN      = 1;                     
				       resp->SNorPN       = TYPE_PNo;
				       resp->Group[ErrorSNo].SNo =  protectmpar->Group[i].PNo;
				       resp->Group[ErrorSNo].Main_inform  = "所属模块不支持光保护";
				       resp->Group[ErrorSNo].Error_inform = "Error: Don't support protect Module.[所属模块不支持光保护]\n";
				       ErrorSNo++;
				} 
                      }// end if(ModType =3)
               }// end if(UseFlag) 
      }
      free(strMNo);
      free(strPNo);
      free(strSNoA);
      free(strSNoB);
      OpticalProtect_Destory(protectmpar);
      SQL_Destory(mysql);  
      sqlite3_close(mydb);
      if(resp->RespondCode != 0 ){
	   resp->ErrorSN     =  ErrorSNo;                  //错误光路总条数
      }
/***************************障碍告警测试进程发送信号**********************
(1)向障碍告警测试守护进程发送取消障碍告警测试信号
(2)注意一定要将发送程序和接收程序划到一个用户组，并且都具有root权限，否则信号发射会失败(BoaCgi 4777)
(3)程可以通过sigqueue函数向包括它本身在内的其他进程发送一个信号，如果程序没有发送这个信号的权限，对sigqueue函数的调用就将失败，而失败的常见原因是目标进程由另一个用户所拥有
(4)现阶段使用的是不可靠信号,不支持排队，信号可能丢失。
***********************************************************************/ 
       if(AlarmCounter>0){
	       int signum;
	       union sigval mysigval;
	       char* process;  
	       int retProcess = 0,n=0;  
	       pid_t cycPID[MAX_PID_NUM];  
	      
	       process ="/web/cgi-bin/alarmMain";                        
	       retProcess = get_pid_by_name(process, cycPID, MAX_PID_NUM);  
	       if(retProcess>0){
		       printf("process '%s' is existed? (%d): %c\n", process, retProcess, (retProcess > 0)?'y':'n');  
		       signum=SIGUSR1;                                         //设置信号值:插入或修改周期测试链表节点值
		       mysigval.sival_int = 250;                               //设置信号的附加信息 (启动障碍告警测试)                               
		       for(n=0;n<retProcess;n++){  
		       printf("alarmMaim-PID:%u\n", cycPID[n]);                //获取障碍告警测试守护进程PID
		       if(sigqueue(cycPID[n],signum,mysigval)==-1)
			       printf("send signal error\n");
		      }  
			   /*等待成功信号*/
		    char * recvStr;  
		    recvStr = (char *) malloc (sizeof (char)*10);
		    recvStr = recvMessageQueue_C();
		    if(strncmp(recvStr, "250-OK", 6) == 0)                     //遇"370-OK"结束
			    printf("Cancel Protect Group sucessful!\n");
		    else{
		            printf("Protect Group failed!\n");
			    resp->RespondCode=3;
			    resp->Group[0].Main_inform  = "取消光保护组失败-->未收到回复消息";
			    resp->Group[0].Error_inform = "Error:Don't get back massgae![取消光保护组失败-->未收到回复消息]";
			    return resp;   
		    }
		    free(recvStr);
		   
	      }else{
		      resp->RespondCode=3;
		      resp->Group[0].Main_inform  = "取消光保护组失败-->未找到后台进程";
		      resp->Group[0].Error_inform = "Error:Don't have back process![取消光保护组失败-->未找到后台进程]";
		      return resp; 
	      }	
       }

   }   //end if(atoi(perCMDcode->child->value.text.string) !=cmdCode)
   return resp;
}

