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
	      resp->Group[0].Main_inform  = "[指令号错误]";
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
	   printf( "Open SQL error\n");
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
                if(UseFlag!=1){
		      resp->RespondCode  =14;
                      if(resp->SNorPN!=TYPE_SNo){
		          resp->ErrorSN      = 1;                     
		          resp->SNorPN       = TYPE_PNo;
		          resp->Group[ErrorSNo].SNo =  protectmpar->Group[i].PNo;
		          resp->Group[ErrorSNo].Main_inform  = "[模块未激活]";
			  resp->Group[ErrorSNo].Error_inform = "Error: Don't have such Module\n";
		          ErrorSNo++;
                      }  

                }else{
                                
                       if(ModType==3){
                                uint32tostring( protectmpar->Group[i].PNo     ,strPNo); 
		                            /*删除已有配对组*/
                                mysql->tableName    = "ProtectGroupTable";
				mysql->filedsName   = "PNo";
				mysql->filedsValue  =  strPNo;
				resPN=SQL_findPNo(mysql,resultPNo);
				for(j=0;j<resPN;j++){
					mysql->mainKeyValue = resultPNo[j];
					SQL_delete(mysql);
				}   
				         /*更新相异数据库及后台测试节点*/     
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
                                                }
					}//end if(existFlag 2)


                                }else //end if(resPN>0)                                 
                                    printf("Warming: Don't have such PNo :%d \n",protectmpar->Group[i].PNo);
                                
                       }else{
				resp->RespondCode  =14;
				if(resp->SNorPN!=TYPE_SNo){
				       resp->ErrorSN      = 1;                     
				       resp->SNorPN       = TYPE_PNo;
				       resp->Group[ErrorSNo].SNo =  protectmpar->Group[i].PNo;
				       resp->Group[ErrorSNo].Main_inform  = "[所属模块不支持光保护]";
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
			    resp->Group[0].Main_inform  = "[取消光保护组失败-->未收到回复消息]";
			    resp->Group[0].Error_inform = "Error:Don't get back massgae![取消光保护组失败-->未收到回复消息]";
			    return resp;   
		    }
		    free(recvStr);
		   
	      }else{
		      resp->RespondCode=3;
		      resp->Group[0].Main_inform  = "[取消光保护组失败-->未找到后台进程]";
		      resp->Group[0].Error_inform = "Error:Don't have back process![取消光保护组失败-->未找到后台进程]";
		      return resp; 
	      }	
       }

   }  
   return resp;
