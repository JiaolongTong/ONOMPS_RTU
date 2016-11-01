#include "alarmtest.h"
#include "process.h"
#include "sql.h"
alarmtest * Alarm_Create()
{
    alarmtest * me = (alarmtest *) malloc(sizeof(alarmtest));
    return me;
}
void Alarm_Destory(alarmtest *me){
    free(me);
}

cancelAlarmtest * endAlarm_Create()
{
    cancelAlarmtest * me = (cancelAlarmtest *) malloc(sizeof(cancelAlarmtest));
    return me;
}
void endAlarm_Destory(cancelAlarmtest *me){
    free(me);
}

alarminfom * AlarmInfo_Create(){
    alarminfom * me = (alarminfom *) malloc(sizeof(alarminfom));
    return me;
}


void AlarmInfo_Destory(alarminfom *me){
    free(me);
}

void getAlarmtestParameter(mxml_node_t *root,mxml_node_t *tree,alarmtest *alarmpar)
{
	    mxml_node_t *CX,*DX,*AN,*ASN,*ANo,*ASNo;
            mxml_node_t *ATXX;                  
            mxml_node_t *IPXX;                  
            mxml_node_t *PXX;                   
            mxml_node_t *T3,*T4,*PS,*TYPE;

	    uint32_t  uint_a;
	    float     float_a;
            int       intAN,intASN,intANo,i,j;
            char      strCX[3]="Cx",strDX[3]="Dx";

            AN =mxmlFindElement(root, tree, "AN",NULL, NULL,MXML_DESCEND);
            intAN =  strtoul(AN->child->value.text.string, NULL, 0);
            alarmpar->AN =intAN;
            
            for (i=0;i<intAN;i++)
               { 
		      strCX[1]=i+0x31;
		      CX   = mxmlFindElement(root, tree, strCX ,NULL, NULL,MXML_DESCEND); 
   
                      ASN  = mxmlFindElement(CX, tree, "ASN" ,NULL, NULL,MXML_DESCEND); 
                      intASN =  strtoul(ASN->child->value.text.string, NULL, 0);  
                      alarmpar->levelGroup[i].ASN=intASN;

                      ANo  = mxmlFindElement(CX, tree, "ANo" ,NULL, NULL,MXML_DESCEND); 
                      intANo =  strtoul(ANo->child->value.text.string, NULL, 0);  
                      alarmpar->levelGroup[i].ANo=intANo;

	              for(j=0;j<intASN;j++){
                              strDX[1]=j+0x31;
                              DX  = mxmlFindElement(CX, tree, strDX ,NULL, NULL,MXML_DESCEND); 
  
			      ASNo = mxmlFindElement(DX, tree, "ASNo",NULL, NULL,MXML_DESCEND);
			      uint_a = strtoul (ASNo->child->value.text.string, NULL, 0);          
			      alarmpar->levelGroup[i].portGroup[j].ASNo = uint_a;

			      TYPE = mxmlFindElement(DX, tree, "TYPE",NULL, NULL,MXML_DESCEND);
			      uint_a = strtoul (TYPE->child->value.text.string, NULL, 0);          
			      alarmpar->levelGroup[i].portGroup[j].fibreType = uint_a;                             //光纤类型
			      alarmpar->levelGroup[i].portGroup[j].protectFlag = 0;                                //默认为一般告警测试，不存在光保护

			      PS=mxmlFindElement(DX, tree, "PS",NULL, NULL,MXML_DESCEND);
			      uint_a = strtoul (PS->child->value.text.string, NULL, 0);          
			      alarmpar->levelGroup[i].portGroup[j].PS = uint_a;

			      ATXX = mxmlFindElement(DX, tree, "AT01",NULL, NULL,MXML_DESCEND);
			      float_a = atof (ATXX->child->value.text.string);          
			      alarmpar->levelGroup[i].portGroup[j].alarmGate.AT01 = float_a;
			      ATXX = mxmlFindElement(DX, tree, "AT02",NULL, NULL,MXML_DESCEND);
			      float_a = atof (ATXX->child->value.text.string);          
			      alarmpar->levelGroup[i].portGroup[j].alarmGate.AT02 = float_a;
			      ATXX = mxmlFindElement(DX, tree, "AT03",NULL, NULL,MXML_DESCEND);
			      float_a = atof (ATXX->child->value.text.string);          
			      alarmpar->levelGroup[i].portGroup[j].alarmGate.AT03 = float_a;
			      ATXX = mxmlFindElement(DX, tree, "AT04",NULL, NULL,MXML_DESCEND);
			      float_a = atof (ATXX->child->value.text.string);          
			      alarmpar->levelGroup[i].portGroup[j].alarmGate.AT04 = float_a;
			      ATXX = mxmlFindElement(DX, tree, "AT05",NULL, NULL,MXML_DESCEND);
			      float_a = atof (ATXX->child->value.text.string);          
			      alarmpar->levelGroup[i].portGroup[j].alarmGate.AT05 = float_a;
			      ATXX = mxmlFindElement(DX, tree, "AT06",NULL, NULL,MXML_DESCEND);
			      float_a = atof (ATXX->child->value.text.string);          
			      alarmpar->levelGroup[i].portGroup[j].alarmGate.AT06 = float_a;                     

			      PXX = mxmlFindElement(DX, tree, "P21",NULL, NULL,MXML_DESCEND);
			      uint_a = strtoul (PXX->child->value.text.string, NULL, 0);  
			      alarmpar->levelGroup[i].portGroup[j].paramter.MeasureLength_m = uint_a;	 
			      PXX = mxmlFindElement(DX, tree, "P22",NULL, NULL,MXML_DESCEND);
			      uint_a = strtoul (PXX->child->value.text.string, NULL, 0); 
			      alarmpar->levelGroup[i].portGroup[j].paramter.PulseWidth_ns = uint_a;    
			      PXX = mxmlFindElement(DX, tree, "P23",NULL, NULL,MXML_DESCEND);
			      uint_a = strtoul (PXX->child->value.text.string, NULL, 0); 
			      alarmpar->levelGroup[i].portGroup[j].paramter.Lambda_nm = uint_a;		  
			      PXX = mxmlFindElement(DX, tree, "P24",NULL, NULL,MXML_DESCEND);
			      uint_a = strtoul (PXX->child->value.text.string, NULL, 0); 
			      alarmpar->levelGroup[i].portGroup[j].paramter.MeasureTime_ms = uint_a;  
			      PXX = mxmlFindElement(DX, tree, "P25",NULL, NULL,MXML_DESCEND); 
			      float_a =atof(PXX->child->value.text.string);
			      alarmpar->levelGroup[i].portGroup[j].paramter.n = float_a;   
			      PXX = mxmlFindElement(DX, tree, "P26",NULL, NULL,MXML_DESCEND);
			      float_a =atof(PXX->child->value.text.string);
			      alarmpar->levelGroup[i].portGroup[j].paramter.NonRelectThreshold = float_a; 	          
			      PXX = mxmlFindElement(DX, tree, "P27",NULL, NULL,MXML_DESCEND);
			      float_a =atof(PXX->child->value.text.string);
			      alarmpar->levelGroup[i].portGroup[j].paramter.EndThreshold = float_a;   

			      IPXX = mxmlFindElement(DX, tree, "IP01",NULL, NULL,MXML_DESCEND);
			      alarmpar->levelGroup[i].portGroup[j].IP01 = IPXX->child->value.text.string; 
			      IPXX = mxmlFindElement(DX, tree, "IP02",NULL, NULL,MXML_DESCEND);
			      alarmpar->levelGroup[i].portGroup[j].IP02 = IPXX->child->value.text.string; 
			      IPXX = mxmlFindElement(DX, tree, "IP03",NULL, NULL,MXML_DESCEND);
			      alarmpar->levelGroup[i].portGroup[j].IP03 = IPXX->child->value.text.string; 
			      IPXX = mxmlFindElement(DX, tree, "IP04",NULL, NULL,MXML_DESCEND);
			      alarmpar->levelGroup[i].portGroup[j].IP04 = IPXX->child->value.text.string; 
			      IPXX = mxmlFindElement(DX, tree, "IP05",NULL, NULL,MXML_DESCEND);
			      alarmpar->levelGroup[i].portGroup[j].IP05 = IPXX->child->value.text.string; 
			      IPXX = mxmlFindElement(DX, tree, "IP06",NULL, NULL,MXML_DESCEND);
			      alarmpar->levelGroup[i].portGroup[j].IP06 = IPXX->child->value.text.string;  
   
                              T3 = mxmlFindElement(DX, tree, "T3",NULL, NULL,MXML_DESCEND);
	                      alarmpar->levelGroup[i].portGroup[j].T3 = T3->child->value.text.string;  
                              T4 = mxmlFindElement(DX, tree, "T4",NULL, NULL,MXML_DESCEND);
	                      alarmpar->levelGroup[i].portGroup[j].T4 = T4->child->value.text.string;  
                                                     
			      if(alarmpar->levelGroup[i].portGroup[j].ASNo == 0){                     
                                 break;
                              }
                       }
            }
}

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
 
                            printf("P21-uint -[%d]\n",alarmpar->levelGroup[i].portGroup[j].paramter.MeasureLength_m);            //测试参数
                            printf("P22-uint -[%d]\n",alarmpar->levelGroup[i].portGroup[j].paramter.PulseWidth_ns);
                            printf("P23-uint -[%d]\n",alarmpar->levelGroup[i].portGroup[j].paramter.Lambda_nm);
                            printf("P24-uint -[%d]\n",alarmpar->levelGroup[i].portGroup[j].paramter.MeasureTime_ms);
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

/****************************数据库校验*****************************************/
    char resultPNo[64][5];
    char **result;
    char *value;
    char *dbSNoA,*dbSNoB;
    int  fiberTypeA,fiberTypeB;
    int  resPN,ErrorSNo=0,flag=0,flagStut;
    int  protectFlag=0,intStatus;
    value    = (char *) malloc(sizeof(char)*10);
    dbSNoA   = (char *) malloc(sizeof(char)*10);
    dbSNoB   = (char *) malloc(sizeof(char)*10);
    for(i=0;i<AN;i++){                        
       ASN=alarmpar->levelGroup[i].ASN;            
            for(j=0;j<ASN;j++){
               flag=0;
               resPN=0;
               flagStut=0;
               protectFlag = 0;
               uint32tostring(alarmpar->levelGroup[i].portGroup[j].ASNo,value);
               printf("SNo=%s\n",value);
               mysql->filedsValue  =  value;
               mysql->tableName    = "ProtectGroupTable";
               mysql->filedsName   = "SNoA";
               resPN=SQL_findPNo(mysql,resultPNo);               //在配对表中查看是否存在，Status =0 or Status=1
               mysql->filedsName   =  "Status";
               if(resPN==1){
                 mysql->mainKeyValue = resultPNo[0];    //PNo
                 rc= SQL_lookup(mysql,&result);
	         if( rc != SQLITE_OK ){
		   printf( "Lookup SQL error.\n");
	         }else{
		   intStatus =atoi(result[0]);
                  
                   if((intStatus==1) || (intStatus ==0))flagStut=1;
                   
	         } 
               }
               if(resPN==1 && flagStut ==1)flag=1;
               else{
                      
                      mysql->filedsName   = "SNoB";
                      resPN=SQL_findPNo(mysql,resultPNo);         //在配对表中查看是否存在，Status =0 or Status=1
                      mysql->filedsName   =  "Status";
                      if(resPN==1){
                          mysql->mainKeyValue = resultPNo[0];    //PNo
                          rc= SQL_lookup(mysql,&result);
	                  if( rc != SQLITE_OK ){
		            printf( "Lookup SQL error.\n");
	                  }else{
		            intStatus =atoi(result[0]);
                             
                           if((intStatus==1) || (intStatus ==0))flagStut=1;
	                  } 
                      }
                     if(resPN==1 && flagStut==1)flag=1;
               }



               if(flag==1){
                   mysql->mainKeyValue = resultPNo[0];    //PNo
                   mysql->filedsName   = "SNoA";          //配对表中获取 SNoA
                   rc= SQL_lookup(mysql,&result);
	           if( rc != SQLITE_OK ){
		      printf( "Lookup SQL error: SNoA\n");
                   }else{
		      dbSNoA=result[0];
                      printf("PNo=%s,SNoA=%s\n",mysql->mainKeyValue,dbSNoA);
	           }

                   mysql->filedsName   = "SNoB";          //配对表中获取 SNoB
                   rc= SQL_lookup(mysql,&result);
	           if( rc != SQLITE_OK ){
		      printf( "Lookup SQL error: SNoB\n");
                   }else{
		      dbSNoB=result[0];
                      printf("PNo=%s,SNoB=%s\n",mysql->mainKeyValue,dbSNoB);
	           }

                   mysql->tableName    = "AlarmTestSegmentTable";
                   mysql->filedsName   = "fiberType";

                   mysql->mainKeyValue = dbSNoA;         //参数表中获取SNoA的fiberType
                   rc= SQL_lookup(mysql,&result);
	           if( rc != SQLITE_OK ){
		      printf( "Lookup SQL error: fiberType\n");
                   }else{
		      fiberTypeA=atoi(result[0]);
                      printf("SNo=%s,fiberTypeA=%d\n",dbSNoA,fiberTypeA);
	           }

                   mysql->mainKeyValue = dbSNoB;        //参数表中获取SNoB的fiberType
                   rc= SQL_lookup(mysql,&result);
	           if( rc != SQLITE_OK ){
		      printf( "Lookup SQL error: fiberType\n");
                   }else{
		      fiberTypeB=atoi(result[0]);
                      printf("SNo=%s,fiberTypeB=%d\n",dbSNoB,fiberTypeB);
	           }

                  if(fiberTypeA==0 && fiberTypeB==1) protectFlag = 1;
                  else if(fiberTypeA==1 && fiberTypeB==0) protectFlag =1;
                  else{
                       protectFlag =0;
                       resp->RespondCode = 14 ;                                                         // 参数错误
                       if(resp->SNorPN!=TYPE_SNo){
                         resp->SNorPN      = TYPE_PNo;
                         resp->Group[ErrorSNo].PNo = atoi(resultPNo[0]); 
		         resp->Group[ErrorSNo].SNo = alarmpar->levelGroup[i].portGroup[j].ASNo; 
                         resp->Group[ErrorSNo].Main_inform  = "[存在配对组 但 光纤类型相同]";
                         resp->Group[ErrorSNo].Error_inform = "Error: Sqlite don't match -->same fiberType[存在配对组 但 光纤类型相同]";
                         ErrorSNo++; 
                       }
                  }
               }else{

                    if(alarmpar->levelGroup[i].portGroup[j].fibreType ==1){
		       resp->RespondCode = 14 ;                                                 // 参数错误
                       if(resp->SNorPN!=TYPE_PNo){
		         resp->SNorPN      = TYPE_SNo;
		         resp->Group[ErrorSNo].SNo = alarmpar->levelGroup[i].portGroup[j].ASNo; 
                         resp->Group[ErrorSNo].Main_inform  = "[不存在配对组 但 光纤类型为在纤]";
		         resp->Group[ErrorSNo].Error_inform = "Error: Sqlite don't match -->Error SNo [不存在配对组 但 光纤类型为在纤]";
		         ErrorSNo++; 
                       }
                    }  
                                    
               }
                                
               mysql->tableName    = "AlarmTestSegmentTable";
               mysql->mainKeyValue =  value;  
               if(!semaphore_p())  
                  exit(EXIT_FAILURE);                                  //P                 
               if(protectFlag ==1 && flag ==1){ 

                 mysql->filedsName   =  "protectFlag";
                 mysql->filedsValue  =  "1";                           // 更新光路状态为“1”   
                 rc=SQL_modify(mysql);
                 if( rc != SQLITE_OK )
	            printf( "Modify SQL error\n");
               }else if(protectFlag ==0 && flag ==1){ 
                 rc=SQL_delete(mysql);
                 if( rc != SQLITE_OK )
	            printf( "Modify SQL error\n");
               }else if(flag ==0 && alarmpar->levelGroup[i].portGroup[j].fibreType ==1){ 
             
                 rc=SQL_delete(mysql);
                 if( rc != SQLITE_OK )
	            printf( "Modify SQL error\n");
               }
               if(!semaphore_v())                                      //V
                 exit(EXIT_FAILURE);  
  
            }
    }
    free(strSQL);
    free(value);
    Alarm_Destory(alarmpar);
    SQL_Destory(mysql);  
    sqlite3_close(mydb);
    if(resp->RespondCode != 0 ){
	resp->ErrorSN     =  ErrorSNo;                  //错误光路总条数
     }
   printf("There  4\n");
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




responed *  setAlarmInformation(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode){

   mxml_node_t    *CM,*AI,*perCMDcode;
   alarminfom *alarm_param;
   responed *resp; 
   int intCM;
  
   alarm_param = AlarmInfo_Create();   
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
	    CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	    alarm_param->CM = strtoul (CM->child->value.text.string, NULL, 0); 	
	    AI = mxmlFindElement(cmd, tree, "AI",NULL, NULL,MXML_DESCEND);
	    alarm_param->AI = strtoul (AI->child->value.text.string, NULL, 0); 	  
            printf("------setAlarm------\n");    
            printf("CM=%d\n",alarm_param->CM); 
            printf("AI=%d\n",alarm_param->AI);  
       }  
   AlarmInfo_Destory(alarm_param);
   return resp;

}


responed * endAlarmtestSegment(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)        //230
{
   mxml_node_t  *SN,*CM,*CLP,*FX,*perCMDcode;
   responed *resp; 
   int rtuCM,rtuCLP,intSN,i,uint_a;
   char strFX[3]="Fx";

   cancelAlarmtest * alarmEndpar;
   alarmEndpar=endAlarm_Create();
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
	    SN =mxmlFindElement(cmd, tree, "SN",NULL, NULL,MXML_DESCEND);
	    intSN =  strtoul(SN->child->value.text.string, NULL, 0);
	    alarmEndpar->SN = intSN;
	    alarmEndpar->Action = -2;                                                                          //将状态设置为-2，等待从取障碍告警测试中删除
	    CM =mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	    rtuCM =  strtoul(CM->child->value.text.string, NULL, 0);   
	    CLP =mxmlFindElement(cmd, tree, "CLP",NULL, NULL,MXML_DESCEND);
	    rtuCLP =  strtoul(CLP->child->value.text.string, NULL, 0);    
	    for (i=0;i<intSN;i++)
	    { 
		strFX[1]=i+0x31;
		FX  = mxmlFindElement(cmd, tree, strFX ,NULL, NULL,MXML_DESCEND);     
		uint_a = strtoul (FX->child->value.text.string, NULL, 0);          
		alarmEndpar->Group[i].SNo = uint_a;
		if(alarmEndpar->Group[i].SNo == 0) break; 
	    }
       }  

/*****************************修改光路状态********************************************/

 sqlite3 *mydb;
 char *zErrMsg = 0;
 int rc,flag=-1;
 int intStatus,ErrorSNo=0;
 sql *mysql;
 char **result;char *value;
 value = (char *) malloc(sizeof(char)*10);
 mysql = SQL_Create();
 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
 if( rc != SQLITE_OK ){
	      printf( "Lookup SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	   }
 mysql->db =mydb;
 mysql->tableName   = "AlarmTestSegmentTable"; 
 mysql->filedsName   =  "Status";
 for(i=0;i<intSN;i++)
   {
         uint32tostring(alarmEndpar->Group[i].SNo,value);
	 mysql->mainKeyValue = value;  
         flag=SQL_existIN_db(mysql);
         rc= SQL_lookup(mysql,&result);
         if( rc != SQLITE_OK ){
	      printf( "Lookup SQL error.\n");
	 }else{
	      intStatus =atoi(result[0]);
	 }
        if((flag==1) && (intStatus==1)){
            mysql->filedsValue  =  "-2";                            // 更新光路状态为“-2”   
            if(!semaphore_p())  
                exit(EXIT_FAILURE);                                 //P
            rc=SQL_modify(mysql);
            if( rc != SQLITE_OK ){
	       printf( "Modify SQL error\n");
	     }
             if(!semaphore_v())                                     //V
               exit(EXIT_FAILURE);
        }else{
             resp->RespondCode = 14 ;                                                         // 参数错误
             resp->SNorPN      = TYPE_SNo;
             resp->Group[ErrorSNo].SNo = alarmEndpar->Group[i].SNo;
             if(intStatus!=1)
                resp->Group[ErrorSNo].Error_inform = "Error: Sqlite don't match -->ready Cancel [Status!=1]\n";
             if(flag!=1)
                resp->Group[ErrorSNo].Error_inform = "Error: Sqlite don't match -->lack of SNo\n";
              ErrorSNo++;  
        }
   }
    free(value);
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
       int   signum;
       union sigval mysigval;
       char* process;  
       int ret = 0;  
       int n;  
       pid_t cycPID[MAX_PID_NUM];  
      
       process ="/web/cgi-bin/alarmMain";                        
       ret = get_pid_by_name(process, cycPID, MAX_PID_NUM);  
       printf("process '%s' is existed? (%d): %c\n", process, ret, (ret > 0)?'y':'n');  
       signum=SIGUSR1;                                         //设置信号值:插入或修改或取消周期测试链表节点值
       mysigval.sival_int = 230;                               //设置信号的附加信息 (取消周期测试)                               
       for(n=0;n<ret;n++){  
        printf("alarmPID:%u\n", cycPID[n]);                    //获取障碍告警测试进程PID
        if(sigqueue(cycPID[n],signum,mysigval)==-1)
               printf("send signal error\n");
      }  
/*************************等待取消成功********************************/
       char * recvStr;
       recvStr = (char *) malloc (sizeof (char)*10);
       recvStr = recvMessageQueue_C();
       if(strncmp(recvStr, "230-OK", 6) == 0){                 //遇"230-OK"结束
           printf("Cancel alarmtest sucessful!\n");
           }
           else{
                printf("Cancel alarmtest failed!\n");
               }

       free(recvStr);


/*******************************************************************/
   endAlarm_Destory(alarmEndpar);
   return resp;
}


responed * endAlarmInfo(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)
{
   mxml_node_t    *CM,*AI,*perCMDcode;
   alarminfom *alarm_param;
   responed *resp; 
   int intCM;
  
   alarm_param = AlarmInfo_Create();   
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
/**************************解析XML消息******************************/
	    CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	    alarm_param->CM = strtoul (CM->child->value.text.string, NULL, 0); 	
            printf("------cancelAlarm------\n");
            printf("CM=%d\n",alarm_param->CM); 
       }  
   AlarmInfo_Destory(alarm_param);
   return resp;
}

