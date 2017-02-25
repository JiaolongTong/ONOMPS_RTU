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
	    mxml_node_t *PX,*PN,*PNo,*SNoA,*SNoB,*Recv,*Send,*Node;
	    uint32_t  uint_a;
            int       intPN,i;
            //char      strPX[3]="Px";
            char *strPX=NULL,*strNum=NULL;
            strPX=malloc(sizeof(char)*10);
            strNum=malloc(sizeof(char)*10);

            PN =mxmlFindElement(root, tree, "PN",NULL, NULL,MXML_DESCEND);
            intPN =  strtoul(PN->child->value.text.string, NULL, 0);
            protectmpar->PN =intPN;
            protectmpar->Action = -1;
            for (i=0;i<intPN;i++)
            { 
		      *strPX='\0';
		      strcat(strPX,"P");
		      uint32tostring(i+1,strNum);
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
	sqlite3 *mydb=NULL;
	sql  *mysql=NULL;  
        char **result=NULL; 
        char strSQL[100];
        char strMNo[10],strSNoA1[10],strSNoA2[10],strSNoB1[10],strSNoB2[10];
        int ModType=0,DoSwitchCounter=0,ProtectCounter=0,PortFlagA1=0,PortFlagA2=0,PortFlagB1=0,PortFlagB2=0,UseFlag=0;
        int ParmerFlagA1=0,ParmerFlagA2=0,ParmerFlagB1=0,ParmerFlagB2=0,flagSW=0,flagMaster=0;
        int i=0,j=0,rc=0,rednum=0,PN=0,ErrorSNo=0;
        PN=protectmpar->PN;
        for (i=0;i<PN;i++){
             printf("\n----------Number:%d------------\n",i);              
             printf("    Recv: PNo:%d------ SNoA:%d------ SNoB:%d------ SwitchPos:%d \n",
             protectmpar->Group[i].PNo,protectmpar->Group[i].SNoA,protectmpar->Group[i].SNoB,protectmpar->Group[i].SwitchPos);      
             printf("    Send:sPNo:%d------sSNoA:%d------sSNoB:%d------sSwitchPos:%d\n",
             protectmpar->Group[i].sPNo,protectmpar->Group[i].sSNoA,protectmpar->Group[i].sSNoB,protectmpar->Group[i].sSwitchPos);  
             printf("    Comm:ConnectPos:%d------ModNo:%d------IP:%s\n",
             protectmpar->Group[i].ConnectPos,protectmpar->Group[i].ModNo,protectmpar->Group[i].IP);  
        } 
	mysql = SQL_Create();
	rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
        if( rc != SQLITE_OK ){
	   printf( "Open Database error.\n");
	}
        mysql->db =mydb;  

        for(i=0;i<PN;i++)
        { 
               	mysql->tableName ="SubModuleTypeTable";                  //检查子单元模块是否存在
		uint32tostring(((protectmpar->Group[i].PNo-1)/4)+1,strMNo);
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
                if(protectmpar->Group[i].SNoA < protectmpar->Group[i].SNoB){
		        uint32tostring(protectmpar->Group[i].SNoA, strSNoA1);
		        uint32tostring(protectmpar->Group[i].SNoB, strSNoA2);
                }else{
		        uint32tostring(protectmpar->Group[i].SNoA, strSNoA2);
		        uint32tostring(protectmpar->Group[i].SNoB, strSNoA1);
                }
                   
		if(protectmpar->Group[i].sSNoA < protectmpar->Group[i].sSNoB){
		        uint32tostring(protectmpar->Group[i].sSNoA,strSNoB1);
			uint32tostring(protectmpar->Group[i].sSNoB,strSNoB2);
                }else{
		        uint32tostring(protectmpar->Group[i].sSNoA,strSNoB2);
			uint32tostring(protectmpar->Group[i].sSNoB,strSNoB1);
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
                           exit(EXIT_FAILURE);                                    //P
			SQL_modify(mysql);  
                        if(!semaphore_v())  
                           exit(EXIT_FAILURE);                                    //V			     
		}
		mysql->mainKeyValue = strSNoA2;
		ParmerFlagA2=SQL_existIN_db(mysql);
		if(ParmerFlagA2 == 1){
			mysql->filedsName   = "Status";
			mysql->filedsValue  = "1";
                        if(!semaphore_p())  
                           exit(EXIT_FAILURE);                                    //P
			SQL_modify(mysql);  
                        if(!semaphore_v())  
                           exit(EXIT_FAILURE);                                    //V			     
		}
		mysql->mainKeyValue = strSNoB1;
		ParmerFlagB1=SQL_existIN_db(mysql);
		if(ParmerFlagB1 == 1){
			mysql->filedsName   = "Status";
			mysql->filedsValue  = "1";
                        if(!semaphore_p())  
                           exit(EXIT_FAILURE);                                    //P
			SQL_modify(mysql);  
                        if(!semaphore_v())  
                           exit(EXIT_FAILURE);                                    //V			     
		}
		mysql->mainKeyValue = strSNoB2;
		ParmerFlagB2=SQL_existIN_db(mysql);
		if(ParmerFlagB2 == 1){
			mysql->filedsName   = "Status";
			mysql->filedsValue  = "1";
                        if(!semaphore_p())  
                           exit(EXIT_FAILURE);                                    //P
			SQL_modify(mysql);  
                        if(!semaphore_v())  
                           exit(EXIT_FAILURE);                                    //V			     
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

                }else if(PortFlagA1!=1 || PortFlagA2!=1 || PortFlagB1!=1 || PortFlagB2!=1){
		      resp->RespondCode  = 14;
                      if(resp->SNorPN!=TYPE_PNo){
		          resp->ErrorSN      = 4;                     
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
                      if(ModType==3){
		                 if(ParmerFlagA1 ==1 && ParmerFlagA2 ==1 && ParmerFlagB1 ==1 && ParmerFlagB2 ==1){    //设置保护组的时候，四条光路均有参数，则自动启动保护模式 Status =-1(待启动保护模式)
				                /*           初始化接收端1*2开关位置
						DoSwitchCounter=5;
						if(!setModbus_P())                                              
							exit(EXIT_FAILURE);    
						while(DoSwitchCounter>0){
							modbus_t * mb =newModbus(MODBUS_DEV,MODBUS_BUAD);
							flagSW= doOpticalProtectSwitch(mb,protectmpar->Group[i].PNo,protectmpar->Group[i].SwitchPos);
		                                        if(flagSW==0){
		                                              flagSW= doOpticalProtectSwitch(mb,protectmpar->Group[i].sPNo,protectmpar->Group[i].sSwitchPos);
		                                           if(flagSW==0)break;
		                                           else DoSwitchCounter--;
		                                        }
							freeModbus(mb);   
						}           
						if(!setModbus_V())                                                              
							exit(EXIT_FAILURE);

		                                          初始化发送端1*2开关位置
		                                
		                                DoSwitchCounter=5;
		                                while(DoSwitchCounter>0){
							modbus_t * tm =newTCPMaster(MODBUS_DEV,MODBUS_BUAD);
							flagMaster= sendPowerGate(mb,protectmpar->Group[i].PNo,protectmpar->Group[i].SwitchPos);
		                                        if(flagMaster==0)break;
		                                        else DoSwitchCounter--;
							freeTCPMaster(tm);   
						}           
				               */
                                                DoSwitchCounter=5;
                                                flagSW=0;
                                                flagMaster=0;

		                                          /*写入保护组数据库*/
		                                if(DoSwitchCounter>0 && flagSW==0 && flagMaster==0 ){
				                        mysql->tableName ="ProtectGroupTable";  
							protectmpar->Action =-1; 
							printf("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s',%d\n"         //PNo,rtuCM,rtuCLP,SNoA,SNoB,Status
									  ,protectmpar->Group[i].PNo             //PNo
									  ,protectmpar->Group[i].SNoA            //SNoA
									  ,protectmpar->Group[i].SNoB            //SNoB
									  ,protectmpar->Group[i].SwitchPos       //SwitchPos

									  ,protectmpar->Group[i].sPNo            //sPNo
									  ,protectmpar->Group[i].sSNoA           //sSNoA
									  ,protectmpar->Group[i].sSNoB           //sSNoB
									  ,protectmpar->Group[i].sSwitchPos      //sSwitchPos

				                                          ,protectmpar->Group[i].ConnectPos      //ConnectPos
				                                          ,protectmpar->Group[i].ModNo           //sModNo
				                                          ,protectmpar->Group[i].IP              //sIP
									  ,protectmpar->Action                   //Status
									  );

							sprintf(strSQL,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s',%d\n" //PNo,rtuCM,rtuCLP,SNoA,SNoB,Status
									  ,protectmpar->Group[i].PNo             //PNo
									  ,protectmpar->Group[i].SNoA            //SNoA
									  ,protectmpar->Group[i].SNoB            //SNoB
									  ,protectmpar->Group[i].SwitchPos       //SwitchPos

									  ,protectmpar->Group[i].sPNo            //sPNo
									  ,protectmpar->Group[i].sSNoA           //sSNoA
									  ,protectmpar->Group[i].sSNoB           //sSNoB
									  ,protectmpar->Group[i].sSwitchPos      //sSwitchPos

				                                          ,protectmpar->Group[i].ConnectPos      //ConnectPos
				                                          ,protectmpar->Group[i].ModNo            //sModNo
				                                          ,protectmpar->Group[i].IP              //sIP
									  ,protectmpar->Action                   //Status
									  );
							mysql->filedsValue = strSQL;
							if(!semaphore_p())  
							   exit(EXIT_FAILURE);                                   //P
							SQL_add(mysql);                                          //更新或者插入新的纪录

		                                             /*根据光开关位置修改光路类型*/
		                                        mysql->tableName  ="AlarmTestSegmentTable";  
		                                        mysql->filedsName ="fiberType";
		                                        if(protectmpar->Group[i].SwitchPos == PARALLEL ){
		                                             mysql->mainKeyValue= strSNoA1;
		                                             mysql->filedsValue = "1"; 
		                                             SQL_modify(mysql); 
		                                             mysql->mainKeyValue= strSNoA2;
		                                             mysql->filedsValue = "0"; 
		                                             SQL_modify(mysql); 
	   
		                                        }else if(protectmpar->Group[i].SwitchPos == ACROSS ){
		                                             mysql->mainKeyValue= strSNoA1;
		                                             mysql->filedsValue = "0"; 
		                                             SQL_modify(mysql); 
		                                             mysql->mainKeyValue= strSNoA2;
		                                             mysql->filedsValue = "1"; 
		                                             SQL_modify(mysql); 
		                                        }

		                                        if(protectmpar->Group[i].sSwitchPos == PARALLEL ){
		                                             mysql->mainKeyValue= strSNoB1;
		                                             mysql->filedsValue = "1"; 
		                                             SQL_modify(mysql); 
		                                             mysql->mainKeyValue= strSNoB2;
		                                             mysql->filedsValue = "0"; 
		                                             SQL_modify(mysql); 
	   
		                                        }else if(protectmpar->Group[i].sSwitchPos == ACROSS ){
		                                             mysql->mainKeyValue= strSNoB1;
		                                             mysql->filedsValue = "0"; 
		                                             SQL_modify(mysql); 
		                                             mysql->mainKeyValue= strSNoB2;
		                                             mysql->filedsValue = "1"; 
		                                             SQL_modify(mysql); 
		                                        }

		                                        mysql->tableName  ="PortOccopyTable";  
		                                        mysql->filedsName ="FiberType";
		                                        if(protectmpar->Group[i].SwitchPos == PARALLEL ){
		                                             mysql->mainKeyValue= strSNoA1;
		                                             mysql->filedsValue = "2"; 
		                                             SQL_modify(mysql); 
		                                             mysql->mainKeyValue= strSNoA2;
		                                             mysql->filedsValue = "1"; 
		                                             SQL_modify(mysql); 
	   
		                                        }else if(protectmpar->Group[i].SwitchPos == ACROSS ){
		                                             mysql->mainKeyValue= strSNoA1;
		                                             mysql->filedsValue = "1"; 
		                                             SQL_modify(mysql); 
		                                             mysql->mainKeyValue= strSNoA2;
		                                             mysql->filedsValue = "2"; 
		                                             SQL_modify(mysql); 
		                                        }

		                                        if(protectmpar->Group[i].sSwitchPos == PARALLEL ){
		                                             mysql->mainKeyValue= strSNoB1;
		                                             mysql->filedsValue = "2"; 
		                                             SQL_modify(mysql); 
		                                             mysql->mainKeyValue= strSNoB2;
		                                             mysql->filedsValue = "1"; 
		                                             SQL_modify(mysql); 
	   
		                                        }else if(protectmpar->Group[i].sSwitchPos == ACROSS ){
		                                             mysql->mainKeyValue= strSNoB1;
		                                             mysql->filedsValue = "1"; 
		                                             SQL_modify(mysql); 
		                                             mysql->mainKeyValue= strSNoB2;
		                                             mysql->filedsValue = "2"; 
		                                             SQL_modify(mysql); 
		                                        }
		                                       if(!semaphore_v())                                       //V
							  exit(EXIT_FAILURE);

				                             /*更新后台测试节点*/
                                                             ProtectCounter++;
		                                }else{
		                                         /* 1*2开关状态还原
							DoSwitchCounter=5;
							if(!setModbus_P())                                       
								exit(EXIT_FAILURE);    
							while(DoSwitchCounter>0){
							modbus_t * mb =newModbus(MODBUS_DEV,MODBUS_BUAD);
							flagSW= doOpticalProtectSwitch(mb,protectmpar->Group[i].PNo,protectmpar->Group[i].SwitchPos);
							if(flagSW==0)break;
							else DoSwitchCounter--;
							freeModbus(mb);   
							}       
							if(!setModbus_V())                                                      
								exit(EXIT_FAILURE);
		                                        */ 
							resp->RespondCode = 13 ;                             
							if(resp->SNorPN!=TYPE_SNo){
								resp->ErrorSN      = 1;                     
								resp->SNorPN       = TYPE_PNo;
								resp->Group[ErrorSNo].SNo = protectmpar->Group[i].PNo;
								resp->Group[ErrorSNo].Main_inform  = "光保护切换:光开关切换失败";
								resp->Group[ErrorSNo].Error_inform = "Error: Optical Switch Failed[光保护切换:光开关切换失败]!\n";
								ErrorSNo++;  
							} 
		                                }
		                   }else{            //若没有参数，则只保存保护组信息 Status=0 (等待配置测试参数) 
		                                mysql->tableName ="ProtectGroupTable";  
					        protectmpar->Action =0; 
						printf("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s',%d\n"           /*PNo,rtuCM,rtuCLP,SNoA,SNoB,Status*/
								  ,protectmpar->Group[i].PNo             //PNo
								  ,protectmpar->Group[i].SNoA            //SNoA
								  ,protectmpar->Group[i].SNoB            //SNoB
								  ,protectmpar->Group[i].SwitchPos       //SwitchPos

								  ,protectmpar->Group[i].sPNo            //sPNo
								  ,protectmpar->Group[i].sSNoA           //sSNoA
								  ,protectmpar->Group[i].sSNoB           //sSNoB
								  ,protectmpar->Group[i].sSwitchPos      //sSwitchPos

		                                                  ,protectmpar->Group[i].ConnectPos      //ConnectPos
		                                                  ,protectmpar->Group[i].ModNo            //sModNo
		                                                  ,protectmpar->Group[i].IP              //sIP
								  ,protectmpar->Action                   //Status
								  );

						sprintf(strSQL,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s',%d\n"   //PNo,rtuCM,rtuCLP,SNoA,SNoB,Status
								  ,protectmpar->Group[i].PNo             //PNo
								  ,protectmpar->Group[i].SNoA            //SNoA
								  ,protectmpar->Group[i].SNoB            //SNoB
								  ,protectmpar->Group[i].SwitchPos       //SwitchPos

								  ,protectmpar->Group[i].sPNo            //sPNo
								  ,protectmpar->Group[i].sSNoA           //sSNoA
								  ,protectmpar->Group[i].sSNoB           //sSNoB
								  ,protectmpar->Group[i].sSwitchPos      //sSwitchPos

		                                                  ,protectmpar->Group[i].ConnectPos      //ConnectPos
		                                                  ,protectmpar->Group[i].ModNo            //sModNo
		                                                  ,protectmpar->Group[i].IP              //sIP
								  ,protectmpar->Action                   //Status
								  );
						mysql->filedsValue = strSQL;
						if(!semaphore_p())  
						   exit(EXIT_FAILURE);                                    //P
						SQL_add(mysql);                                           //更新或者插入新的纪录
						if(!semaphore_v())                                        //V
						    exit(EXIT_FAILURE);		                     
		                 }
                   
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

                    }
                       
             }

        }
  

       if(resp->RespondCode != 0 ){
		 resp->ErrorSN     =  ErrorSNo;                  //错误光路总条数
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
	       process ="/web/cgi-bin/ProtectMasterMain";                        
	       retProcess = get_pid_by_name(process, protectPID, MAX_PID_NUM);  
	       if(retProcess>0){
		       printf("process '%s' is existed? (%d): %c\n", process, retProcess, (retProcess > 0)?'y':'n');  
		       signum=SIGUSR1;                                         //设置信号值:插入或修改周期测试链表节点值
		       mysigval.sival_int = 170;                               //设置信号的附加信息 (启动障碍告警测试)                               
		       for(n=0;n<retProcess;n++){  
		       printf("alarmMaim-PID:%u\n", protectPID[n]);                //获取障碍告警测试守护进程PID
		       if(sigqueue(protectPID[n],signum,mysigval)==-1)
			       printf("send signal error\n");
		      }  
			   /*等待成功信号*/
                    recvStr = (char *) malloc(sizeof(char)*10);
		    recvStr = recvMessageQueue_C();
		    if(strncmp(recvStr, "170-OK", 6) == 0)                     //遇"370-OK"结束
			    printf("Set Protect Group operation sucessful!\n");
		    else{
		            printf("Set Protect Group operation failed!\n");
			    resp->RespondCode=3;
			    resp->Group[0].Main_inform  = "切换保护节点更新失败-->未收到回复消息";
			    resp->Group[0].Error_inform = "Error:Don't get back massgae![切换保护节点更新失败-->未收到回复消息]";
                            free(recvStr);
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
      

   }  
   return resp;
}

/*
CMDcode	370（固定）	INT(1Bytes)	每个指令或数据的唯一标识
R		        INT	预留
CM		        String(10Bytes)	RTU编号
CLP		        String(10Bytes)	局站代码
PN		        INT(3Bytes)	可进行切换的组数
G1		        INT(3Bytes)	第1个要切换的保护组编号 
…Gi		        INT(3Bytes)	第i个要切换的保护组编号 
Gcsn		        INT(3Bytes)	第CSN个要切换的保护组编号 

Gi的结构
PNo		        INT(3Bytes)	保护组号（下行光路所在组号）
SNoA		        INT(3Bytes)	配对光路端口号A
SNoB		        INT(3Bytes)	配对光路端口号B
SwitchPos		INT(3Bytes)	1*2光开关状态

*/

void getProtectSwitchParameter(mxml_node_t *root,mxml_node_t *tree,opticalprotect *protectmpar){
	    mxml_node_t *GX,*PN,*PNo,*SNoA,*SNoB,*SwitchPos;

	    uint32_t  uint_a;
            int       intPN,i;
            //char      strGX[3]="Gx";
            char *strGX=NULL,*strNum=NULL;
            strGX=malloc(sizeof(char)*10);
            strNum=malloc(sizeof(char)*10);
            PN =mxmlFindElement(root, tree, "PN",NULL, NULL,MXML_DESCEND);
            intPN =  strtoul(PN->child->value.text.string, NULL, 0);
            protectmpar->PN =intPN;
            protectmpar->Action = -1;
            for (i=0;i<intPN;i++)
            { 
		// strGX[1]=i+0x31;
		 *strGX='\0';
		 strcat(strGX,"G");
		 uint32tostring(i+1,strNum);
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

                          
            }

}

	 ;
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
      mysql->mainKeyValue  =  PNo;
      mysql->filedsName    =  "SwitchPos";                                
      mysql->filedsValue   =  mSwPos; 
      SQL_modify(mysql); 

      mysql->filedsName    =  "sSwitchPos";                                
      mysql->filedsValue   =  sSwPos; 
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
            protectmpar->Action= 1; 
            printf("PN=%d\n",protectmpar->PN); 
            for (i=0;i<PN;i++){
                    printf("----------requstGx:%d------------\n",protectmpar->Group[i].PNo);              
                    printf("    SNoA:%d------SNoB:%d -------SwitchPos:%d\n",protectmpar->Group[i].SNoA,protectmpar->Group[i].SNoB,protectmpar->Group[i].SwitchPos);      
            }  

	 sqlite3 *mydb;
	 sql *mysql;
	 int rc=0,rednum=0,flagMatch=0,UseFlag=0,PortFlagA=0,PortFlagB=0,flagSW=0,existFlag=0,flagStatus=0,haveFlag=0;
         int ErrorSNo=0,ModuleNo=0,SwitchStatus=0,ModType=0,SwitchPos=0,sSwitchPos=0,DoSwitchCounter=0,ProtectCounter=0,fiberType=0;
         char *strPNo=NULL,*strMNo=NULL,*strSNoA=NULL,*strSNoB=NULL; 
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

				    mysql->filedsName   =  "sSwitchPos";
				    SQL_lookupPar(mysql,&result,&rednum);
				    sSwitchPos =atoi(result[0]);
				    SQL_freeResult(&result,&rednum);


				    mysql->filedsName   =  "Status";
				    SQL_lookupPar(mysql,&result,&rednum);
				    SwitchStatus =atoi(result[0]);
				    SQL_freeResult(&result,&rednum);

				    if(SwitchStatus==1){				                 
				            DoSwitchCounter=5;
					    if(!setModbus_P())                                              //ModBus总线保护
						  exit(EXIT_FAILURE);    
				            while(DoSwitchCounter>0){
		                               usleep(2000);
					       modbus_t * mb =newModbus(MODBUS_DEV,MODBUS_BUAD);
					       flagSW= doOpticalProtectSwitch(mb,protectmpar->Group[i].PNo,protectmpar->Group[i].SwitchPos);
                                               if(flagSW==1)
                                                  flagSW= doOpticalProtectSwitch(mb,protectmpar->Group[i].PNo+1,protectmpar->Group[i].SwitchPos);
				               if(flagSW==0)break;
				               else DoSwitchCounter--;
					       freeModbus(mb);   
				            }           
					    if(!setModbus_V())                                              //解除ModBus总线保护                  
						 exit(EXIT_FAILURE);
				               
				                   
				            if(DoSwitchCounter>0 && !flagSW){  
                                                  
                                                                                                     

                                                   if(haveFlag=ifHaveTestSegment(mydb,mysql,protectmpar->Group[i].SNoA,protectmpar->Group[i].SNoB,protectmpar->Group[i].SNoA+1,protectmpar->Group[i].SNoB+1))
                                                       ProtectCounter++;   //存在测试参数，表明后台有测试节点
                                                   printf("------------------------------------------>Here %d \n",haveFlag);
						    if(!semaphore_p())  
							exit(EXIT_FAILURE);                                //数据库修改保护                 
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
		                                    if(!semaphore_v())                                          //解除数据库修改保护
							 exit(EXIT_FAILURE); 


				            }else{
						    DoSwitchCounter=5;
						    if(!setModbus_P())                                          //ModBus总线保护
							  exit(EXIT_FAILURE);    
						    while(DoSwitchCounter>0){
				                       usleep(2000);
						       modbus_t * mb =newModbus(MODBUS_DEV,MODBUS_BUAD);
					               flagSW= doOpticalProtectSwitch(mb,protectmpar->Group[i].PNo,SwitchPos);
                                                       if(flagSW==1)
                                                           flagSW= doOpticalProtectSwitch(mb,protectmpar->Group[i].PNo+1,sSwitchPos);
						       if(flagSW==0)break;
						       else DoSwitchCounter--;
						       freeModbus(mb);   
						    }          
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
				   } //end if(SwitchStatus==1)
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
        OpticalProtect_Destory(protectmpar);
	SQL_Destory(mysql);  
	sqlite3_close(mydb);

	if(resp->RespondCode != 0 ){
	   resp->ErrorSN     =  ErrorSNo;                  //错误光路总条数
           return resp;
	}
       
       int signum;
       union sigval mysigval;
       char* process;  
       int retProcess = 0,n=0;  
       pid_t protectPID[MAX_PID_NUM];  
       char*  recvStr=NULL;

        if(ProtectCounter>0){      
	       process ="/web/cgi-bin/ProtectMasterMain";                        
	       retProcess = get_pid_by_name(process, protectPID, MAX_PID_NUM);  
	       if(retProcess>0){
		       printf("process '%s' is existed? (%d): %c\n", process, retProcess, (retProcess > 0)?'y':'n');  
		       signum=SIGUSR1;                                         //设置信号值:插入或修改周期测试链表节点值
		       mysigval.sival_int = 370;                               //设置信号的附加信息 (启动障碍告警测试)                               
		       for(n=0;n<retProcess;n++){  
		       printf("alarmMaim-PID:%u\n", protectPID[n]);                //获取障碍告警测试守护进程PID
		       if(sigqueue(protectPID[n],signum,mysigval)==-1)
			       printf("send signal error\n");
		      }  
			   //等待成功信号
                    recvStr = (char *) malloc(sizeof(char)*10);
		    recvStr = recvMessageQueue_C();
		    if(strncmp(recvStr, "370-OK", 6) == 0)                     //遇"370-OK"结束
			    printf("Do  Protect Switch operation sucessful!\n");
		    else{
		            printf("Set Protect Switch operation failed!\n");
			    resp->RespondCode=3;
			    resp->Group[0].Main_inform  = "切换保护节点更新失败-->未收到回复消息";
			    resp->Group[0].Error_inform = "Error:Don't get back massgae![切换保护节点更新失败-->未收到回复消息]";
                            free(recvStr);
			    return resp;   
		    }
                    free(recvStr);		   
	      }else{
		      resp->RespondCode=3;
		      resp->Group[0].Main_inform  = "切换保护节点更新失败-->未找到后台进程";
		      resp->Group[0].Error_inform = "Error:Don't have back process! 切换保护节点更新失败-->未找到后台进程]";
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
   int i=0,j=0, rc=0,rednum=0,UseFlag=0,existFlag=0,flagStatus=0,ProtectCounter=0;
   int ErrorSNo=0,ModuleNo=0,resPN=0,ModType=0,Status=0;
   char *strQX=NULL,*strNum=NULL;
   char strMNo[10],strSNoA[10],strSNoB[10],strSNoC[10],strSNoD[10],strPNo[10]; 
   char **result=NULL;
   sqlite3 *mydb=NULL;
   sql *mysql=NULL;

   strQX=malloc(sizeof(char)*10);
   strNum=malloc(sizeof(char)*10);

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
		*strQX='\0';
		strcat(strQX,"Q");
		uint32tostring(i+1,strNum);
		strcat(strQX,strNum);

		QX  = mxmlFindElement(cmd, tree, strQX ,NULL, NULL,MXML_DESCEND);     
		uint_a = strtoul (QX->child->value.text.string, NULL, 0);          
		protectmpar->Group[i].PNo = uint_a;
		printf("Cancel PNo---------->%d\n",protectmpar->Group[i].PNo);
	    }
/**************************执行操作********************************/   
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
				mysql->mainKeyValue  =  strPNo;
                                existFlag=SQL_existIN_db(mysql);
                                 printf("existFlag:%d\n",existFlag);
                                if(existFlag){ 

 
                                    mysql->filedsName   = "SNoA";
                                    SQL_lookupPar(mysql,&result,&rednum);
                                    strcpy(strSNoA,result[0]);

                                    mysql->filedsName   = "SNoB";
                                    SQL_lookupPar(mysql,&result,&rednum);
                                    strcpy(strSNoB,result[0]);

                                    mysql->filedsName   = "sSNoA";
                                    SQL_lookupPar(mysql,&result,&rednum);
                                    strcpy(strSNoC,result[0]);

                                    mysql->filedsName   = "sSNoB";
                                    SQL_lookupPar(mysql,&result,&rednum);
                                    strcpy(strSNoD,result[0]);

                                    mysql->filedsName   = "Status";
                                    SQL_lookupPar(mysql,&result,&rednum);
                                    Status=atoi(result[0]);
                                    if(Status==1)ProtectCounter++; 
                                    SQL_delete(mysql);
                                } 

                                         /*删除障碍告警测试*/    
				if( existFlag ){  
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

                                }else //删除失败 end if(resPN>0)                                 
                                    printf("Warming: Don't have such Group  :%d \n",protectmpar->Group[i].PNo);
                                
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

      OpticalProtect_Destory(protectmpar);
      SQL_Destory(mysql);  
      sqlite3_close(mydb);
      if(resp->RespondCode != 0 ){
	   resp->ErrorSN     =  ErrorSNo;                  //错误光路总条数
           return resp;
      }

      int signum;
      union sigval mysigval;
      char* process;  
      int retProcess = 0,n=0;  
      pid_t protectPID[MAX_PID_NUM];  
      char*  recvStr=NULL;

      if(ProtectCounter>0){
	       process ="/web/cgi-bin/ProtectMasterMain";                        
	       retProcess = get_pid_by_name(process, protectPID, MAX_PID_NUM);  
	       if(retProcess>0){
		       printf("process '%s' is existed? (%d): %c\n", process, retProcess, (retProcess > 0)?'y':'n');  
		       signum=SIGUSR1;                                         //设置信号值:插入或修改周期测试链表节点值
		       mysigval.sival_int = 250;                               //设置信号的附加信息 (启动障碍告警测试)                               
		       for(n=0;n<retProcess;n++){  
		       printf("alarmMaim-PID:%u\n", protectPID[n]);                //获取障碍告警测试守护进程PID
		       if(sigqueue(protectPID[n],signum,mysigval)==-1)
			       printf("send signal error\n");
		      }  
			   /*等待成功信号*/
                    recvStr = (char *) malloc(sizeof(char)*10);
		    recvStr = recvMessageQueue_C();
		    if(strncmp(recvStr, "250-OK", 6) == 0)                     //遇"370-OK"结束
			    printf("Cancel Protect Group operation sucessful!\n");
		    else{
		            printf("Cancel Protect Group operation failed!\n");
			    resp->RespondCode=3;
			    resp->Group[0].Main_inform  = "切换保护节点删除失败-->未收到回复消息";
			    resp->Group[0].Error_inform = "Error:Don't get back massgae![切换保护节点删除失败-->未收到回复消息]";
                            free(recvStr);
			    return resp;   
		    }
                    free(recvStr);		   
	      }
      }

   }   //end if(atoi(perCMDcode->child->value.text.string) !=cmdCode)
   return resp;
}

