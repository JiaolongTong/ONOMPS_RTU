     	checkNode *p=NULL;
        alarmNode *q=NULL,*node=NULL;    
        float powerValue=-100.0,PowerGate=0,powerGateOnline=0,powerValueOnline=0; 
        backData *bData=NULL; 
        int i=0,j=0;
	sqlite3 *mydb=NULL;
	sql *mysql=NULL;
	char **result = NULL;
        char strSNo[10],strPNo[10],strSNoOnline[10],strNewSwitchPos[10];
	int  rednum =0;
        int  oldSwitchPos=0,newSwitchPos=0,flagSW=-1;
	int rc=0,intSNo=0,intANo=0,intCM=0;
        int protectFlag=0,fristAlarmFlag=0,flagFlush=0;
        time_t nowTime=0,nextAlarmTime=0,alarmClick=0;
        char buffer[1024],check,SwitchPos=0;
        buffer[0]=0x7E;
        buffer[1]=0x7E;
        buffer[2]=0x05;
        buffer[3]=0xFF;
        buffer[4]=0xE4;


	p= headA;
        q= headB;

	if(p==NULL){
		//printf("This is a void excel!\n");
		return q;
	} 
	else
	  while(p!=NULL){
      
               if(p!=NULL){
                  if(flagNew == 0 ){                                                                   
		      if(p!=NULL)intSNo=p->SNo;
		      if(p!=NULL)intANo=p->ANo; 
		      if(p!=NULL)intCM =p->CM;    
		      if(p!=NULL)protectFlag=p->protectFlag;  
		      if(p!=NULL)fristAlarmFlag=p->fristAlarmFlag;     
		      if(p!=NULL)PowerGate=p->PowerGate;   
		      if(p!=NULL)nextAlarmTime=p->nextAlarmTime; 
		      if(p!=NULL)alarmClick=p->alarmClick;
                      if(p!=NULL)powerGateOnline = p->PowerGateOnline;
                      if(p!=NULL)oldSwitchPos =p->SwitchPos;   
	              if(!setModbus_P())                                                //P  获取当前光路光功率值
		           exit(EXIT_FAILURE);   		        
		      modbus_t * mb=newModbus(MODBUS_DEV,MODBUS_BUAD);
                      //usleep(50000);
	              if(protectFlag==1){                                      
                              if(intSNo==1 || intSNo==2 ){                                            
					if(oldSwitchPos==PARALLEL){
		                                           
						powerValue = getOneOpticalValue(mb,4+RefModule,2);           //for test Beijing 使用临近模块的相同位置端口作为在纤光功率检测
                                                PowerGate  = p->PowerGate;
		                        }
					if(oldSwitchPos==ACROSS){
		                                           
						powerValue = getOneOpticalValue(mb,8+RefModule,2);
                                                PowerGate  = p->PowerGateOnline; 
		                        }
                                                        
                              }else {
                                        powerValue = getOneOpticalValue(mb,intSNo,3);                       //保护模式(3) 
                                        PowerGate  = p->PowerGate;
                              }   
			}else if(protectFlag==0){
                            if(((intSNo-1)/8+1)==1 || ((intSNo-1)/8+1)==7){
                                powerValue = getOneOpticalValue(mb,intSNo,3);                              //非保护模式(2,4)
                                PowerGate  = p->PowerGate;
                            }else{
				powerValue = getOneOpticalValue(mb,intSNo,2);                              //非保护模式(2,4)
                                PowerGate  = p->PowerGate;
                            }
			} 	
                                 
			freeModbus(mb);  		                 
			if(!setModbus_V())                                                                  //V
                                exit(EXIT_FAILURE);  

                        if(powerValue==0)powerValue=-70.0; 
                        if(powerValueOnline==0)powerValueOnline=-70.0;   

                  }else{
                       if(p!=NULL){
		               if(p!=NULL)p=p->next; 
		               if(p!=NULL){
				       if(p!=NULL)intSNo=p->SNo;
				       if(p!=NULL)intANo=p->ANo; 
				       if(p!=NULL)intCM =p->CM;    
				       if(p!=NULL)protectFlag=p->protectFlag;  
				       if(p!=NULL)fristAlarmFlag=p->fristAlarmFlag;     
				       if(p!=NULL)PowerGate=p->PowerGate;                   //bei 
				       if(p!=NULL)nextAlarmTime=p->nextAlarmTime; 
				       if(p!=NULL)alarmClick=p->alarmClick; 
                                       if(p!=NULL)powerGateOnline = p->PowerGateOnline;     //zai
                                       if(p!=NULL)oldSwitchPos =p->SwitchPos;  
				       if(!setModbus_P())                                                //P  获取当前光路光功率值
					   exit(EXIT_FAILURE);   			
				       modbus_t *mb=newModbus(MODBUS_DEV,MODBUS_BUAD);
					if(protectFlag==1){
                                                if(intSNo==1 || intSNo==2 ){                                            
						        if(oldSwitchPos==PARALLEL){
		                                           
						           powerValue = getOneOpticalValue(mb,4+RefModule,2);              //for test Beijing 使用临近模块的相同位置端口作为在纤光功率检测
                                                           PowerGate  = p->PowerGate;
		                                        }
						        if(oldSwitchPos==ACROSS){
		                                           
						           powerValue = getOneOpticalValue(mb,8+RefModule,2);
                                                           PowerGate  = p->PowerGateOnline; 
		                                        }
                                                        
                                                }
                                                else {
				                        powerValue = getOneOpticalValue(mb,intSNo,3);                           //保护模式(3) 
				                        PowerGate  = p->PowerGate;
                                                } 
                                                       
					}else if(protectFlag==0){
                                                   
                                                  if(((intSNo-1)/8+1)==1 || ((intSNo-1)/8+1)==7){
                                                      powerValue = getOneOpticalValue(mb,intSNo,3);                        //非保护模式(2,4)
                                                      PowerGate  = p->PowerGate;
                                                   }
                                                   else{
						      powerValue = getOneOpticalValue(mb,intSNo,2);                        //非保护模式(2,4)
                                                      PowerGate  = p->PowerGate;
                                                   }
					} 		           
					freeModbus(mb);  		       
				        if(powerValue==0)powerValue=-70.0;               
					if(!setModbus_V())                                                //V
					    exit(EXIT_FAILURE); 
                                        flagNew=0;
		                                        
		               }else{
		                  p=NULL;
                                  flagNew=0;
		                  break;
		               }
                       }else{
			       p=NULL;
                               flagNew=0;
			       break;                           
                       }
                       flagNew=0;  
                  }
                }else{
		       p==NULL;
                       flagNew=0;
		       break;
                }
               /*
                if(intSNo==1 || intSNo==2 ){ 
                        if(oldSwitchPos==PARALLEL){
               	 	    printf("SNo:%d--------------------------->powerGateOnline:%f  powerValueOnline:%f    powerValue:%f\n",4+RefModule,powerGateOnline,powerValueOnline,powerValue);
                        }else{
                            printf("SNo:%d--------------------------->powerGateOnline:%f  powerValueOnline:%f    powerValue:%f\n",8+RefModule,powerGateOnline,powerValueOnline,powerValue);
                        }
                }else {
                       
                        printf("SNo:%d--------------------------->powerGateOnline:%f  powerValueOnline:%f    powerValue:%f\n",intSNo,powerGateOnline,powerValueOnline,powerValue);
                }
               */
                if( powerValue < PowerGate ){    //异常	 
                     nowTime = getLocalTimestamp(); 
                    // printf("                                   intSNo:%d---intANo:%d--intCM:%d--protectFlag:%d--fristAlarmFlag:%d--PowerGate:%f--nextAlarmTime:%ld--nowTime:%ld--powerValue:%f\n",
                    //       intSNo,intANo,intCM,protectFlag,fristAlarmFlag,PowerGate,nextAlarmTime,nowTime,powerValue);
                    
                     if(fristAlarmFlag ==0){                                   //状态C: 首次出现异常 -->fristAlarmFlag=0   实际光功率值<阈值                 
                          printf("StateC--->SNo  powerValue:%f <---> gateValue:%f  -->OnlinSNo:%d powerValueOnline:%f<--->powerGateOnline:%f \n",
                                 intSNo,powerValue,PowerGate,intSNo+RefModule,powerValueOnline,powerGateOnline);
		          fristAlarmFlag = 1;
                          if(protectFlag==0){
				  node=(alarmNode *)malloc(sizeof(alarmNode));
				  node->SNo = intSNo;
				  node->CM  = intCM;
				  node->ANo = intANo;     
				  node->Order = (intANo)*100 +intSNo; 
                                  if(p!=NULL){
				  	q=insert_B(q,node);
                                  }
                                  else{
		                          p=NULL;
		                          break;
                                  }
                          }else if(protectFlag==1){
				  printf("General a alarm warmming on SNo= %d powerValue=%f, gateValue:%f",intSNo,powerValue,PowerGate);
                                  bData=backData_Create();
                                  bData->powerValue =powerValue;
                                  bData->powerGate  =PowerGate;
                                  bData->level      =intANo;
                                  if(p!=NULL){
				          mysql = SQL_Create();
					  rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
					  if( rc != SQLITE_OK )
						printf( "Lookup IP SQL error\n");			 
					  mysql->db = mydb;
					  mysql->tableName        = "AlarmTestSegmentTable";
					  mysql->filedsName       = "IP01";
					  uint32tostring(intSNo,strSNo);
					  mysql->mainKeyValue = strSNo;  
					  SQL_lookupPar(mysql,&result,&rednum);
					  strcpy(bData->backIP,result[0]);
					  SQL_freeResult(&result,&rednum); 

// for Beijing Test                       
                                        
                                          if(intSNo==1 || intSNo==2 ){   		                           
		                                       if(intSNo%2==0)uint32tostring(intSNo-1,strSNoOnline);
		                                       else uint32tostring(intSNo+1,strSNoOnline);  
		                                       if(intSNo%2==0)
							   uint32tostring(intSNo/2,strPNo);
		                                       else
		                                           uint32tostring((intSNo-1)/2+1,strPNo);
		                                        
						       printf("在纤故障:upload IP :%s onlineSNo:%s\n",bData->backIP,strSNoOnline);
		                                               //状态查询
		                                       
		                                       if(oldSwitchPos == PARALLEL) newSwitchPos=ACROSS;   //
		                                       if(oldSwitchPos == ACROSS)   newSwitchPos=PARALLEL;
		                                       bData->SwitchPos=newSwitchPos;
		                                       if(p!=NULL) p->SwitchPos=newSwitchPos;
				                               //向服务器上传切换信息
						        //upload(bData,intSNo,intCM,4);
                                                        if(newSwitchPos==ACROSS) SwitchPos=1;
                                                        if(newSwitchPos==PARALLEL) SwitchPos=0;
                                                        tcp_sendMssage(net_par,2,SwitchPos,1);
			        
		                                        mysql->tableName    =  "ProtectGroupTable";    //更新保护组表
		                                        mysql->mainKeyValue =  strPNo; 
							mysql->filedsName   =  "SwitchPos";                                    
							uint32tostring(newSwitchPos,strNewSwitchPos);
							mysql->filedsValue = strNewSwitchPos;
				                        SQL_modify(mysql);
								//更新端口表的光纤类型
		 
							mysql->tableName   = "PortOccopyTable";
						 	mysql->filedsName  = "fiberType";
						        mysql->mainKeyValue= strSNo;                  //原来的备纤更新为在纤类型
						        mysql->filedsValue = "2";            
						        SQL_modify(mysql);                              

						        mysql->mainKeyValue= strSNoOnline;            //原来的在纤更新为备纤类型
				                        mysql->filedsValue = "1";                          
						        SQL_modify(mysql);  
				            
								//更新障碍告警表
							mysql->tableName   = "AlarmTestSegmentTable";   
						 	mysql->filedsName  = "fiberType";
						        mysql->mainKeyValue= strSNo;                  //原来的备纤更新为在纤类型
						        mysql->filedsValue = "1";            
						        SQL_modify(mysql);                              

						        mysql->mainKeyValue= strSNoOnline;            //原来的在纤更新为备纤类型
				                        mysql->filedsValue = "0";       
						        SQL_modify(mysql);               

							mysql->filedsName   ="Status";                  //更新测试节点
							mysql->filedsValue  = "1";                       
							SQL_modify(mysql);  
                                                                                 
				                                //更新测试节点
							//linkHead_check_A=execOpticalSwich(linkHead_check_A);     
							//outPutALL_A(linkHead_check_A);
                                           /*  
                                                        if(oldSwitchPos==PARALLEL){
								node=(alarmNode *)malloc(sizeof(alarmNode));
								node->SNo = 2;//4+RefModule;//2
								node->CM  = intCM;
								node->ANo = intANo;     
								node->Order = (intANo)*100 +intSNo; 
								if(p!=NULL){
								  	q=insert_B(q,node);
								}else{
									  p=NULL;
									  break;
								}
                                                        }
                                                        else{
								node=(alarmNode *)malloc(sizeof(alarmNode));
								node->SNo = 2;//8+RefModule;//1
								node->CM  = intCM;
								node->ANo = intANo;     
								node->Order = (intANo)*100 +intSNo; 
								if(p!=NULL){
								  	q=insert_B(q,node);
								}else{
									  p=NULL;
									  break;
								}
                                                        }
                                                        				                        
                                           */		                               
                                          }else{
					       printf("备纤故障:upload IP :%s\n",bData->backIP);
				               //upload(bData,intSNo,intCM,1);
                                          }
// end 
                                       	  SQL_Destory(mysql);  
					  sqlite3_close(mydb); 
                                  }else{
		                          p=NULL;
		                          break;
                                  } 
                                  backData_Destory(bData);                         
                          }
	                  nextAlarmTime  = getLocalTimestamp()+alarmClick;                  
                     }else if(nowTime >= nextAlarmTime){                       //状态D:长期处于异常 -->fristAlarmFlag=1   实际光功率值<阈值  
                          printf("StateD--->SNo:%d  powerValue:%f <---> gateValue:%f\n",intSNo,powerValue,PowerGate);
                          if(protectFlag==0){      
				  node=(alarmNode *)malloc(sizeof(alarmNode));
				  node->SNo = intSNo;
				  node->CM  = intCM;
				  node->ANo = intANo;     
				  node->Order = (intANo)*100 +intSNo; 
                                  if(p!=NULL){
				  	q=insert_B(q,node);
                                  }else{
		                          p=NULL;
		                          break;
                                  }
                          }else if(protectFlag==1){
				  printf("General a alarm warmming on SNo= %d  powerValue=%f, gateValue:%f",intSNo,powerValue,PowerGate);
                                  bData=backData_Create();
                                  bData->powerValue =powerValue;
                                  bData->powerGate  =PowerGate;
                                  bData->level      =intANo;
                                  if(p!=NULL){
                                          mysql = SQL_Create();
					  rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
					  if( rc != SQLITE_OK )
						printf( "Lookup  SQL error\n");			 
					  mysql->db = mydb;
					  mysql->tableName        = "AlarmTestSegmentTable";
					  mysql->filedsName       = "IP01";
					  uint32tostring(intSNo,strSNo);
					  mysql->mainKeyValue = strSNo;  
					  SQL_lookupPar(mysql,&result,&rednum);
					  strcpy(bData->backIP,result[0]);
					  SQL_freeResult(&result,&rednum); 

// for Beijing Test                       
                                          if(intSNo==1 || intSNo==2 ){   		                           
		                                       if(intSNo%2==0)uint32tostring(intSNo-1,strSNoOnline);
		                                       else uint32tostring(intSNo+1,strSNoOnline);  
		                                       if(intSNo%2==0)
							   uint32tostring(intSNo/2,strPNo);
		                                       else
		                                           uint32tostring((intSNo-1)/2+1,strPNo);
		                                        
						       printf("在纤故障:upload IP :%s onlineSNo:%s\n",bData->backIP,strSNoOnline);
		                                               //状态查询
		                                       
		                                       if(oldSwitchPos == PARALLEL) newSwitchPos=ACROSS;   //
		                                       if(oldSwitchPos == ACROSS)   newSwitchPos=PARALLEL;
		                                       bData->SwitchPos=newSwitchPos;
		                                       if(p!=NULL) p->SwitchPos=newSwitchPos;
				                               //向服务器上传切换信息
						        //upload(bData,intSNo,intCM,4);
                                                        if(newSwitchPos==ACROSS) SwitchPos=1;
                                                        if(newSwitchPos==PARALLEL) SwitchPos=0;
                                                        tcp_sendMssage(net_par,2,SwitchPos,1);
		        
		                                        mysql->tableName    =  "ProtectGroupTable";    //更新保护组表
		                                        mysql->mainKeyValue =  strPNo; 
							mysql->filedsName   =  "SwitchPos";                                    
							uint32tostring(newSwitchPos,strNewSwitchPos);
							mysql->filedsValue = strNewSwitchPos;
				                        SQL_modify(mysql);
								//更新端口表的光纤类型
		 
							mysql->tableName   = "PortOccopyTable";
						 	mysql->filedsName  = "fiberType";
						        mysql->mainKeyValue= strSNo;                  //原来的备纤更新为在纤类型
						        mysql->filedsValue = "2";            
						        SQL_modify(mysql);                              

						        mysql->mainKeyValue= strSNoOnline;            //原来的在纤更新为备纤类型
				                        mysql->filedsValue = "1";                          
						        SQL_modify(mysql);  
				            
										      //更新障碍告警表
							mysql->tableName   = "AlarmTestSegmentTable";   
						 	mysql->filedsName  = "fiberType";
						        mysql->mainKeyValue= strSNo;                  //原来的备纤更新为在纤类型
						        mysql->filedsValue = "1";            
						        SQL_modify(mysql);                              

						        mysql->mainKeyValue= strSNoOnline;            //原来的在纤更新为备纤类型
				                        mysql->filedsValue = "0";       
						        SQL_modify(mysql);               

							mysql->filedsName   ="Status";                  //更新测试节点
							mysql->filedsValue  = "1";                       
							SQL_modify(mysql);                            
				                                                      //更新测试节点
							//linkHead_check_A=execOpticalSwich(linkHead_check_A);     
							//outPutALL_A(linkHead_check_A);  
/*
                                                        if(oldSwitchPos==PARALLEL){
								node=(alarmNode *)malloc(sizeof(alarmNode));
								node->SNo = 4+RefModule;//2
								node->CM  = intCM;
								node->ANo = intANo;     
								node->Order = (intANo)*100 +intSNo; 
								if(p!=NULL){
								  	q=insert_B(q,node);
								}else{
									  p=NULL;
									  break;
								}
                                                        }
                                                        else{
								node=(alarmNode *)malloc(sizeof(alarmNode));
								node->SNo = 8+RefModule; //1
								node->CM  = intCM;
								node->ANo = intANo;     
								node->Order = (intANo)*100 +intSNo; 
								if(p!=NULL){
								  	q=insert_B(q,node);
								}else{
									  p=NULL;
									  break;
								}
                                                       }
 */                                                        				                        
		                               
                                          }else{
						printf("备纤故障:upload IP :%s\n",bData->backIP);
				                //upload(bData,intSNo,intCM,1);
                                          }   
                                       
		                          SQL_Destory(mysql);  
					  sqlite3_close(mydb); 
		                          
                                  }else{
                                               
		                          p=NULL;
		                          break;
                                  } 
                                  backData_Destory(bData);                   
                          }
	                  nextAlarmTime  = getLocalTimestamp()+alarmClick;
                          
                     }  
                 }                                 //正常             
                 else if(fristAlarmFlag!=0){                                 //状态A:从异常中首次恢复 --> fristAlarmFlag=1 实际光功率值>=阈值      
                          printf("StateA--->SNo:%d  powerValue:%f <---> gateValue:%f\n",intSNo,powerValue,PowerGate);  
                          bData=backData_Create();
                          bData->powerValue =powerValue;
                          bData->powerGate  =PowerGate;
                          bData->level      =0;
                          if(p!=NULL){
		                  mysql = SQL_Create();
				  rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
				  if( rc != SQLITE_OK )
					printf( "Lookup IP SQL error\n");			 
				  mysql->db = mydb;
				  mysql->tableName        = "AlarmTestSegmentTable";
				  mysql->filedsName       = "IP01";
		                  //strSNo=malloc(sizeof(char)*10);
				  uint32tostring(intSNo,strSNo);
				  mysql->mainKeyValue = strSNo;  
				  SQL_lookupPar(mysql,&result,&rednum);
				  strcpy(bData->backIP,result[0]);
				  SQL_freeResult(&result,&rednum); 
			     	  SQL_Destory(mysql);  
				  sqlite3_close(mydb); 
		                  printf("upload IP :%s\n",bData->backIP);
		                  //upload(bData,intSNo,intCM,1);
                          }else{
		                  p=NULL;
		                  break;
                          }
                          backData_Destory(bData);      
                          fristAlarmFlag= 0;
                      } 
                      else ;                                                     //状态B:正常  -->  fristAlarmFlag=0 实际光功率值>=阈值            

                if(p!=NULL){
                      if(flagNew==0){
			      if(p!=NULL) p->fristAlarmFlag=fristAlarmFlag;
			      if(p!=NULL) p->nextAlarmTime=nextAlarmTime  ;
			      if(p!=NULL) p=p->next;
                              
                              //printf("--------------------------->Here 3!\n");   
                      }else{
		              if(p!=NULL) p=p->next;
                              flagNew=0;   
		              break;
                      }
  
                }else{
                        p=NULL; 
                        flagNew=0; 
                        break;
                }                    
         }
       return q;
