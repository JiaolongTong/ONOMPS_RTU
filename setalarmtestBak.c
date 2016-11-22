responed *setAlarmtestSegment(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)      //130
{	
   mxml_node_t *CM,*CLP,*perCMDcode;
   alarmtest * alarmpar; 
   otdr * test_p;
   responed *resp;   
   int intCM,intCLP;
   int PS; 

   test_p   = OTDR_Create();
   alarmpar = Alarm_Create();   
   resp   = Responed_Create();


   resp->RespondCode=0;        
   perCMDcode = mxmlFindElement(cmd, tree, "CMDcode",NULL, NULL,MXML_DESCEND);


      if(atoi(perCMDcode->child->value.text.string) !=cmdCode) {
            printf("<RespondCode>3</RespondCode>\n");
	    printf("<Data>CMDcode Error [ %s:%s]</Data>\n",perCMDcode->value.element.name,perCMDcode->child->value.text.string);
            resp->RespondCode=-1;
            return resp;    
       }
      else{
/**************************解析XML消息***************************************/
            getAlarmtestParameter(cmd,tree,alarmpar);
	    CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	    intCM = strtoul (CM->child->value.text.string, NULL, 0); 
	    CLP = mxmlFindElement(cmd, tree, "CLP",NULL, NULL,MXML_DESCEND);
	    intCLP = strtoul (CM->child->value.text.string, NULL, 0); 
	
            int i,j;
            int AN,ASN;
            AN=alarmpar->AN;
            alarmpar->Action = -1;  

            for (i=0;i<AN;i++){
                    printf("----------Cx:%d------------\n",alarmpar->levelGroup[i].ANo);              
                    ASN=alarmpar->levelGroup[i].ASN;            
                    for(j=0;j<ASN;j++){
                            printf("----------Dx:%d------------\n",alarmpar->levelGroup[i].portGroup[j].ASNo);
			    printf("PS -uint -[%d]\n",             alarmpar->levelGroup[i].portGroup[j].PS);
			    printf("Type-uint -[%d]\n",            alarmpar->levelGroup[i].portGroup[j].fibreType);              //控制参数
                            printf("PS-uint -[%d]\n",              alarmpar->levelGroup[i].portGroup[j].PS);
			    printf("Flag-uint -[%d]\n",            alarmpar->levelGroup[i].portGroup[j].protectFlag);

                            printf("AT01-float -[%f]\n",alarmpar->levelGroup[i].portGroup[j].alarmGate.AT01);                    //门限参数
                            printf("AT02-float -[%f]\n",alarmpar->levelGroup[i].portGroup[j].alarmGate.AT02);
                            printf("AT03-float -[%f]\n",alarmpar->levelGroup[i].portGroup[j].alarmGate.AT03);
                            printf("AT04-float -[%f]\n",alarmpar->levelGroup[i].portGroup[j].alarmGate.AT04);
                            printf("AT05-float -[%f]\n",alarmpar->levelGroup[i].portGroup[j].alarmGate.AT05);
                            printf("AT05-float -[%f]\n",alarmpar->levelGroup[i].portGroup[j].alarmGate.AT06);
 
                            printf("P21-uint -[%d]\n" ,alarmpar->levelGroup[i].portGroup[j].paramter.MeasureLength_m);            //测试参数
                            printf("P22-uint -[%d]\n" ,alarmpar->levelGroup[i].portGroup[j].paramter.PulseWidth_ns);
                            printf("P23-uint -[%d]\n" ,alarmpar->levelGroup[i].portGroup[j].paramter.Lambda_nm);
                            printf("P24-uint -[%d]\n" ,alarmpar->levelGroup[i].portGroup[j].paramter.MeasureTime_ms);
                            printf("P25-float -[%f]\n",alarmpar->levelGroup[i].portGroup[j].paramter.n);
                            printf("P26-float -[%f]\n",alarmpar->levelGroup[i].portGroup[j].paramter.NonRelectThreshold);                  
                            printf("P27-float -[%f]\n",alarmpar->levelGroup[i].portGroup[j].paramter.EndThreshold);   

                            printf("IP01-string -[%s]\n",alarmpar->levelGroup[i].portGroup[j].IP01 );                            //通信参数
                            printf("IP02-string -[%s]\n",alarmpar->levelGroup[i].portGroup[j].IP02 );
                            printf("IP03-string -[%s]\n",alarmpar->levelGroup[i].portGroup[j].IP03 );
                            printf("IP04-string -[%s]\n",alarmpar->levelGroup[i].portGroup[j].IP04 );
                            printf("IP05-string -[%s]\n",alarmpar->levelGroup[i].portGroup[j].IP05 );
                            printf("IP06-string -[%s]\n",alarmpar->levelGroup[i].portGroup[j].IP06 );
                            printf("T3-string -[%s]\n",alarmpar->levelGroup[i].portGroup[j].T3 );
                            printf("T4-string -[%s]\n",alarmpar->levelGroup[i].portGroup[j].T4 );                    
 
                    }		    
            }
/***************************数据库存储**************************/

	sqlite3 *mydb;
	char *zErrMsg = 0;
	int rc;
	sql *mysql;                                    
	char * strSQL;
	mysql = SQL_Create();
	strSQL   = (char *) malloc(sizeof(char)*400);
	rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
	if( rc != SQLITE_OK ){
	      printf( "Open SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	}
	mysql->db =mydb;
	mysql->tableName   = "AlarmTestSegmentTable";   
        for(i=0;i<AN;i++){                        
            ASN=alarmpar->levelGroup[i].ASN;            
            for(j=0;j<ASN;j++){                             
                           //SNo,rtuCM,rtuCLP,Level,PS,P21,P22,P23,P24,P25,P26,P27,AT01,AT02,AT03,AT04,AT05,AT06,IP01,IP02,IP03,IP04,IP05,IP06,T3,T4,fiberType,protectFlag,Status
		 sprintf(strSQL,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,'%s','%s','%s','%s','%s','%s','%s','%s',%d,%d,%d\n"
                               ,alarmpar->levelGroup[i].portGroup[j].ASNo                               //SNo
		               ,intCM                                                                   //rtuCM
		               ,intCLP                                                                  //rtuCLP
		               ,alarmpar->levelGroup[i].ANo                                             //Level
		               ,alarmpar->levelGroup[i].portGroup[j].PS                                 //PS
		               ,alarmpar->levelGroup[i].portGroup[j].paramter.MeasureLength_m           //P21
		               ,alarmpar->levelGroup[i].portGroup[j].paramter.PulseWidth_ns             //P22
		               ,alarmpar->levelGroup[i].portGroup[j].paramter.Lambda_nm                 //P23
		               ,alarmpar->levelGroup[i].portGroup[j].paramter.MeasureTime_ms            //P24
		               ,alarmpar->levelGroup[i].portGroup[j].paramter.n                         //P25
		               ,alarmpar->levelGroup[i].portGroup[j].paramter.NonRelectThreshold        //P26
                               ,alarmpar->levelGroup[i].portGroup[j].paramter.EndThreshold              //P27
		               ,alarmpar->levelGroup[i].portGroup[j].alarmGate.AT01                     //AT01
		               ,alarmpar->levelGroup[i].portGroup[j].alarmGate.AT02                     //AT02
		               ,alarmpar->levelGroup[i].portGroup[j].alarmGate.AT03                     //AT03
		               ,alarmpar->levelGroup[i].portGroup[j].alarmGate.AT04                     //AT04
		               ,alarmpar->levelGroup[i].portGroup[j].alarmGate.AT05                     //AT05
		               ,alarmpar->levelGroup[i].portGroup[j].alarmGate.AT06                     //AT06
		               ,alarmpar->levelGroup[i].portGroup[j].IP01                               //IP01
		               ,alarmpar->levelGroup[i].portGroup[j].IP02                               //IP02
		               ,alarmpar->levelGroup[i].portGroup[j].IP03                               //IP03
		               ,alarmpar->levelGroup[i].portGroup[j].IP04                               //IP04
		               ,alarmpar->levelGroup[i].portGroup[j].IP05                               //IP05
		               ,alarmpar->levelGroup[i].portGroup[j].IP06                               //IP06
		               ,alarmpar->levelGroup[i].portGroup[j].T3                                 //T3
		               ,alarmpar->levelGroup[i].portGroup[j].T4                                 //T4
                               ,alarmpar->levelGroup[i].portGroup[j].fibreType                          //fiberType
                               ,alarmpar->levelGroup[i].portGroup[j].protectFlag                        //protectFlag
                               ,alarmpar->Action                                                        //Status
                               );
		  mysql->filedsValue = strSQL;
                  if(!semaphore_p())  
                            exit(EXIT_FAILURE);                                //P
		  rc = SQL_add(mysql);                                         //更新或者插入新的纪录
		  if( rc != SQLITE_OK ){
		      printf( "Save SQL error\n");
		      sqlite3_free(zErrMsg);
		  }else
                  printf("%s",strSQL);
                  if(!semaphore_v())                                           //V
                           exit(EXIT_FAILURE);
           }

       }
    free(strSQL);
/****************************数据库校验*****************************************/
/*
(1) 检查模块是否存在，若存在则取出模块类型  
(2) 检查端口是否被占用
(3) 查看光路是否存在配对
(3) 对存在配对的光路验证是否为一个在纤一个备纤
(4) 对没有配对的光纤，检查是否光纤类型是否满足要求(备纤类型、带光功率的在纤类型),与所属模块的类型相关
(5) 剔除不符合要求光纤，保证下发给alarmMain的光纤只有三种类型(模块2 模块3 模块4 )
         ----->模块2:只存在备纤
         ----->模块3:一个备纤和一个在纤形成保护组
         ----->模块4:可进行光功率采集的在纤
(6) 向alarmMain发送更新测试链表请求
*/
    char resultPNo[64][5];
    char **result;
    char *strSNo=NULL,*strMNo=NULL;
    char *dbSNoA,*dbSNoB;
    int  fiberTypeA,fiberTypeB;
    int  resPN,ModuleNo,ErrorSNo=0,flag=0,flagStut=0,existPS=0,rednum=0;
    int  protectFlag=0,UseFlag=0,PortFlag=0,ModType=0,intStatus=0;
    strSNo   = (char *) malloc(sizeof(char)*10);
    strMNo   = (char *) malloc(sizeof(char)*10);    
    dbSNoA   = (char *) malloc(sizeof(char)*10);
    dbSNoB   = (char *) malloc(sizeof(char)*10);
    for(i=0;i<AN;i++){                        
            ASN=alarmpar->levelGroup[i].ASN;            
            for(j=0;j<ASN;j++){
                   flag=0;
                   resPN=0;
                   flagStut=0;
                   protectFlag = 0;
                   ModType =0;
                   UseFlag=0;
                   PortFlag=0;
                   existPS=0;
/***************************查看模块设置和端口占用*****************************/
		   mysql->tableName ="SubModuleTypeTable";                  //检查子单元模块是否存在
		   uint32tostring(alarmpar->levelGroup[i].portGroup[j].ASNo,strSNo);
		   ModuleNo = ((alarmpar->levelGroup[i].portGroup[j].ASNo-1)/8)+1;
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
		   mysql->tableName ="PortOccopyTable";                    //检查子端口是否占用
		   mysql->mainKeyValue = strSNo;
		   PortFlag = SQL_existIN_db(mysql);    

		   if(UseFlag!=1)  
		   {
		      resp->RespondCode  = 3;
                      if(resp->SNorPN!=TYPE_PNo){
		          resp->ErrorSN      = 1;                     
		          resp->SNorPN       = TYPE_SNo;
		          resp->Group[ErrorSNo].SNo =  alarmpar->levelGroup[i].portGroup[j].ASNo;
		          resp->Group[ErrorSNo].Main_inform  = "[模块未激活]";
			  resp->Group[ErrorSNo].Error_inform = "Error: Don't have such Module\n";
		          ErrorSNo++;
                      }                           
		   }else if(PortFlag!=1)
		   {
		      resp->RespondCode  = 3;
                      if(resp->SNorPN!=TYPE_PNo){
		          resp->ErrorSN      = 1;                     
		          resp->SNorPN       = TYPE_SNo;
		          resp->Group[ErrorSNo].SNo =  alarmpar->levelGroup[i].portGroup[j].ASNo;
		          resp->Group[ErrorSNo].Main_inform  = "[端口未配置]";
			  resp->Group[ErrorSNo].Error_inform = "Error: Don't have such Port\n";
		          ErrorSNo++;  
                      }                                                         
		   }else{
         
                   
/***************************查看是否存在配对***********************************/
		       uint32tostring(alarmpar->levelGroup[i].portGroup[j].ASNo,strSNo);
		       mysql->filedsValue  =  strSNo;
		       mysql->tableName    = "ProtectGroupTable";
		       mysql->filedsName   = "SNoA";
		       resPN=SQL_findPNo(mysql,resultPNo);                     //在，Status =0 or Status=1
		       mysql->filedsName   = "Status";
		       if(resPN==1){
			       mysql->mainKeyValue = resultPNo[0];             //PNo
			       SQL_lookupPar(mysql,&result,&rednum);
			       intStatus =atoi(result[0]); 
                               SQL_freeResult(&result,&rednum);
			       if((intStatus==1) || (intStatus ==0))flagStut=1;
		       }

		       if(resPN==1 && flagStut==1)flag=1;
		       else{                 
		              mysql->filedsName   = "SNoB";
		              resPN=SQL_findPNo(mysql,resultPNo);              //在配对表中查看是否存在，Status =0 or Status=1
		              mysql->filedsName   =  "Status";
		              if(resPN==1){
				      mysql->mainKeyValue = resultPNo[0];     //PNo
				      SQL_lookupPar(mysql,&result,&rednum);
				      intStatus =atoi(result[0]);	
                                      SQL_freeResult(&result,&rednum);	                     
				      if((intStatus==1) || (intStatus ==0))flagStut=1;
		              }
		             if(resPN==1 && flagStut==1)flag=1;
		       }
/***************************查看光纤类型是否符合要求****************************/
		       if(flag==1){
		           mysql->mainKeyValue = resultPNo[0];    //PNo
		           mysql->filedsName   = "SNoA";          //配对表中获取 SNoA
		           SQL_lookupPar(mysql,&result,&rednum);
			   dbSNoA=result[0];
		           printf("PNo=%s,SNoA=%s\n",mysql->mainKeyValue,dbSNoA);
			   SQL_freeResult(&result,&rednum);

		           mysql->filedsName   = "SNoB";          //配对表中获取 SNoB
		           SQL_lookupPar(mysql,&result,&rednum);
			   dbSNoB=result[0];
		           printf("PNo=%s,SNoB=%s\n",mysql->mainKeyValue,dbSNoB);
			   SQL_freeResult(&result,&rednum);

		           mysql->tableName    = "AlarmTestSegmentTable";
		           mysql->filedsName   = "fiberType";

		           mysql->mainKeyValue = dbSNoA;         //参数表中获取SNoA的fiberType
		           SQL_lookupPar(mysql,&result,&rednum);
			   fiberTypeA=atoi(result[0]);
		           printf("SNo=%s,fiberTypeA=%d\n",dbSNoA,fiberTypeA);
			   SQL_freeResult(&result,&rednum);

		           mysql->mainKeyValue = dbSNoB;         //参数表中获取SNoB的fiberType
		           SQL_lookupPar(mysql,&result,&rednum);
			   fiberTypeB=atoi(result[0]);
		           printf("SNo=%s,fiberTypeB=%d\n",dbSNoB,fiberTypeB);
			   SQL_freeResult(&result,&rednum);

		           if(fiberTypeA==0 && fiberTypeB==1) protectFlag = 1;
		           else if(fiberTypeA==1 && fiberTypeB==0) protectFlag =1;
		           else{
		                protectFlag =0;
		                resp->RespondCode = 14 ;                                                // 参数错误
		                if(resp->SNorPN!=TYPE_SNo){
				         resp->SNorPN              = TYPE_PNo;
				         resp->Group[ErrorSNo].PNo = atoi(resultPNo[0]); 
					 resp->Group[ErrorSNo].SNo = alarmpar->levelGroup[i].portGroup[j].ASNo; 
				         resp->Group[ErrorSNo].Main_inform  = "[存在配对组 但 光纤类型相同]";
				         resp->Group[ErrorSNo].Error_inform = "Error: Sqlite don't match -->same fiberType[存在配对组 但 光纤类型相同]";
				         ErrorSNo++; 
		                }
		          }
		       }else{
		             
		            if(alarmpar->levelGroup[i].portGroup[j].fibreType ==1 && ModType ==1 ){
			       resp->RespondCode = 14 ;                                                 // 参数错误
		               if(resp->SNorPN!=TYPE_PNo){
				 resp->SNorPN      = TYPE_SNo;
				 resp->Group[ErrorSNo].SNo = alarmpar->levelGroup[i].portGroup[j].ASNo; 
		                 resp->Group[ErrorSNo].Main_inform  = "[不存在配对组 但 光纤类型为不可采集光功率的在纤]";
				 resp->Group[ErrorSNo].Error_inform = "Error: Sqlite don't match -->Error SNo [不存在配对组 但 光纤类型为不可采集光功率的在纤]";
				 ErrorSNo++; 
		               }
		            }  
		                            
		       }
/*******************************检查是否使用优化参数**********************************/

                      if(alarmpar->levelGroup[i].portGroup[j].PS ==0){
                          mysql->tableName    = "DefaultTsetSegmentTable";
		          mysql->mainKeyValue =  strSNo; 
                          mysql->filedsName   = "SNo";
                          existPS=SQL_existIN_db(mysql);
                          if(existPS==0){
                               resp->RespondCode = 3 ;                                                 // 参数错误
                               printf("Here ------------------->");
		               if(resp->SNorPN!=TYPE_PNo){
				       resp->SNorPN      = TYPE_SNo;
				       resp->Group[ErrorSNo].SNo = alarmpar->levelGroup[i].portGroup[j].ASNo; 
				       resp->Group[ErrorSNo].Main_inform  = "[没有优化测试参数]";
				       resp->Group[ErrorSNo].Error_inform = "Error: Sqlite don't match -->Error SNo [没有优化测试参数]";
				       ErrorSNo++; 
		               } 
                          }
                      }
                      
/********************************更新数据库******************************************/		                        
		       mysql->tableName    = "AlarmTestSegmentTable";
		       mysql->mainKeyValue =  strSNo;  
		       if(!semaphore_p())  
		          exit(EXIT_FAILURE);                                  //P                 
		       if(protectFlag ==1 && flag ==1){ 
                               printf( "Have Protect Group\n");
			       mysql->filedsName   =  "protectFlag";           //存在配对组(flag=1)且符合配对要求(protectFlag=1)，则设置保护标识
			       mysql->filedsValue  =  "1";                           
			       rc=SQL_modify(mysql);
			       if( rc != SQLITE_OK ) printf( "Modify SQL error\n");
				    
		       }else if(protectFlag ==0 && flag ==1){                  //不存在配对组(flag=1)但不符合配对要求(protectFlag=0)，则删除原有记录
                               printf( "ProtectFlag=0 error\n");
			       rc=SQL_delete(mysql);
			       if( rc != SQLITE_OK ) printf( "Delete SQL error\n");
				    
		       }else if(flag ==0 && alarmpar->levelGroup[i].portGroup[j].fibreType ==1 && ModType==1){   
		                                                              //不存在配对组(flag=0)且不带光功率监测的在纤(fibreType=1,ModType=1),则删除原有记录
                               printf( "ModType=1 error\n");
			       rc=SQL_delete(mysql);
			       if( rc != SQLITE_OK ) printf( "Delete SQL error\n");
				    
		       }
                       if(alarmpar->levelGroup[i].portGroup[j].PS ==0 && existPS==0){
		                                                              //使用优化参数测试(PS)，但不存在优化参数(existPS),则删除原有记录
                               printf( "PS=0 error\n");
		       	       rc=SQL_delete(mysql);
			       if( rc != SQLITE_OK ) printf( "Delete SQL error\n");
				    
                       }
                       
		       if(!semaphore_v())                                     //V
		         exit(EXIT_FAILURE);  
                }//end if (modtype && port)
                if(UseFlag!=1 || PortFlag!=1){
                      printf( "Module or Port error\n");
                      mysql->tableName    = "AlarmTestSegmentTable";
		      mysql->mainKeyValue =  strSNo;  
		      rc=SQL_delete(mysql);
		      if( rc != SQLITE_OK ) printf( "Delete SQL error\n");  
                }
  
            }//end for
    }  //end for
 
    free(strSNo);
    free(strMNo);
    free(dbSNoB);
    free(dbSNoA);
    Alarm_Destory(alarmpar);
    SQL_Destory(mysql);  
    sqlite3_close(mydb);
    if(resp->RespondCode != 0 ){
	resp->ErrorSN     =  ErrorSNo;                  //错误光路总条数
        return resp;
     }
/***************************向障碍告警测试守护进程发送启动障碍告警测试信号***************************
(1)向障碍告警测试守护进程发送加入新节点信号   
(2)注意一定要将发送程序和接收程序划到一个用户组，并且都具有root权限，否则信号发射会失败
(3)程可以通过sigqueue函数向包括它本身在内的其他进程发送一个信号,
   如果程序没有发送这个信号的权限，对sigqueue函数的调用就将失败,
   而失败的常见原因是目标进程由另一个用户所拥有.
(4)现阶段使用的是不可靠信号,不支持排队，信号可能丢失。
/***********************************************************************************************/
       int signum;
       union sigval mysigval;
       char* process;  
       int ret = 0;  
       int n;  
       pid_t cycPID[MAX_PID_NUM];  
      
       process ="/web/cgi-bin/alarmMain";                        
       ret = get_pid_by_name(process, cycPID, MAX_PID_NUM);  
       printf("process '%s' is existed? (%d): %c\n", process, ret, (ret > 0)?'y':'n');  
       signum=SIGUSR1;                                         //设置信号值:插入或修改周期测试链表节点值
       mysigval.sival_int = 130;                               //设置信号的附加信息 (启动障碍告警测试)                               
       for(n=0;n<ret;n++){  
       printf("alarmMaim-PID:%u\n", cycPID[n]);                //获取障碍告警测试守护进程PID
       if(sigqueue(cycPID[n],signum,mysigval)==-1)
               printf("send signal error\n");
      }  
/***************************等待启动障碍告警测试成功信号**************************************/

    char * recvStr;  
    recvStr = (char *) malloc (sizeof (char)*10);
    recvStr = recvMessageQueue_C();
    if(strncmp(recvStr, "130-OK", 6) == 0)                     //遇"130-OK"结束
       printf("Set Alarmtest sucessful!\n");
    else
       printf("Set Alarmtest failed!:%s\n",recvStr);
       free(recvStr);
   }  
   return resp;
}
