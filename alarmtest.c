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

           //char *strCX=NULL,*strDX=NULL,*strNum=NULL;
           // strCX=malloc(sizeof(char)*10);
           //strDX=malloc(sizeof(char)*10);
           //strNum=malloc(sizeof(char)*10);

            AN =mxmlFindElement(root, tree, "AN",NULL, NULL,MXML_DESCEND);
            intAN =  strtoul(AN->child->value.text.string, NULL, 0);
            alarmpar->AN =intAN;
            
            for (i=0;i<intAN;i++)
               { 
		      strCX[1]=i+0x31;
                     // *strCX='\0';
                     // strcat(strCX,"C");
                     // uint32tostring(i+1,strNum);
                     // strcat(strCX,strNum);

		      CX   = mxmlFindElement(root, tree, strCX ,NULL, NULL,MXML_DESCEND); 
   
                      ASN  = mxmlFindElement(CX, tree, "ASN" ,NULL, NULL,MXML_DESCEND); 
                      intASN =  strtoul(ASN->child->value.text.string, NULL, 0);  
                      alarmpar->levelGroup[i].ASN=intASN;

                      ANo  = mxmlFindElement(CX, tree, "ANo" ,NULL, NULL,MXML_DESCEND); 
                      intANo =  strtoul(ANo->child->value.text.string, NULL, 0);  
                      alarmpar->levelGroup[i].ANo=intANo;

	              for(j=0;j<intASN;j++){
                              strDX[1]=j+0x31;

                             // *strDX='\0';
                             // strcat(strDX,"D");
                             // uint32tostring(i+1,strNum);
                             // strcat(strDX,strNum);

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
   mxml_node_t *CM=NULL,*CLP=NULL,*perCMDcode=NULL;
   alarmtest *alarmpar=NULL,*alarmTmp=NULL; 
   responed *resp=NULL;   
   int intCM=0,intCLP=0,PS=0;
   sqlite3 *mydb=NULL;
   sql *mysql=NULL; 
   char * strSQL=NULL; 	                               
   char resultPNo[64][5];
   char **result;
   char *strSNo=NULL,*strMNo=NULL;
   char *dbSNoA=NULL,*dbSNoB=NULL,*strSNoA=NULL,*strSNoB=NULL,*strPNo=NULL;
   int  rc=0,m=0,n=0;
   int  fiberType,fiberTypeA,fiberTypeB;
   int  ModuleNo,ErrorSNo=0,existPS=0,rednum=0;
   int  checkFlag=0,saveFlag=0,UseFlag=0,PortFlag=0,ModType=0,intStatus=0;

   alarmpar = Alarm_Create();   
   alarmTmp = Alarm_Create();
   resp     = Responed_Create();
   resp->RespondCode=0;        
   perCMDcode = mxmlFindElement(cmd, tree, "CMDcode",NULL, NULL,MXML_DESCEND);
   if(atoi(perCMDcode->child->value.text.string) !=cmdCode) {
        resp->RespondCode=3;
	resp->Group[0].Main_inform  = "指令号错误";
	resp->Group[0].Error_inform = "Error:CMDcode Error[指令号错误]";
        return resp;    
   }else{
/**************************解析XML消息***************************************/
            getAlarmtestParameter(cmd,tree,alarmpar);
	    CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	    intCM = strtoul (CM->child->value.text.string, NULL, 0); 
	    CLP = mxmlFindElement(cmd, tree, "CLP",NULL, NULL,MXML_DESCEND);
	    intCLP = strtoul (CM->child->value.text.string, NULL, 0); 
	
            int i,j;
            int AN,ASN;
            AN=alarmpar->AN;
             

            for(i=0;i<AN;i++){
                ASN=alarmpar->levelGroup[i].ASN;
                for(j=0;j<ASN-1;j++){
                    if(alarmpar->levelGroup[i].portGroup[j].fibreType > alarmpar->levelGroup[i].portGroup[j+1].fibreType){
                       alarmTmp->levelGroup[i].portGroup[j]=alarmpar->levelGroup[i].portGroup[j+1];
                       alarmpar->levelGroup[i].portGroup[j+1]=alarmpar->levelGroup[i].portGroup[j];
                       alarmpar->levelGroup[i].portGroup[j]=alarmTmp->levelGroup[i].portGroup[j];
                    }
                }
            }


            for(i=0;i<AN;i++){
                printf("----------After Sort:%d------------\n"       ,alarmpar->levelGroup[i].ANo);              
                ASN=alarmpar->levelGroup[i].ASN;          
                for(j=0;j<ASN;j++){
                            printf("----------SNo:%d------------\n"  ,alarmpar->levelGroup[i].portGroup[j].ASNo);
			    printf("Type-uint   -[%d]\n"             ,alarmpar->levelGroup[i].portGroup[j].fibreType);              //控制参数
                            printf("PS-uint     -[%d]\n"             ,alarmpar->levelGroup[i].portGroup[j].PS);
			    printf("Flag-uint   -[%d]\n"             ,alarmpar->levelGroup[i].portGroup[j].protectFlag);

                            printf("AT01-float  -[%f]\n"             ,alarmpar->levelGroup[i].portGroup[j].alarmGate.AT01);                    //门限参数
                            printf("AT02-float  -[%f]\n"             ,alarmpar->levelGroup[i].portGroup[j].alarmGate.AT02);
                            printf("AT03-float  -[%f]\n"             ,alarmpar->levelGroup[i].portGroup[j].alarmGate.AT03);
                            printf("AT04-float  -[%f]\n"             ,alarmpar->levelGroup[i].portGroup[j].alarmGate.AT04);
                            printf("AT05-float  -[%f]\n"             ,alarmpar->levelGroup[i].portGroup[j].alarmGate.AT05);
                            printf("AT05-float  -[%f]\n"             ,alarmpar->levelGroup[i].portGroup[j].alarmGate.AT06);
 
                            printf("P21-uint    -[%d]\n"             ,alarmpar->levelGroup[i].portGroup[j].paramter.MeasureLength_m);           //测试参数
                            printf("P22-uint    -[%d]\n"             ,alarmpar->levelGroup[i].portGroup[j].paramter.PulseWidth_ns);
                            printf("P23-uint    -[%d]\n"             ,alarmpar->levelGroup[i].portGroup[j].paramter.Lambda_nm);
                            printf("P24-uint    -[%d]\n"             ,alarmpar->levelGroup[i].portGroup[j].paramter.MeasureTime_ms);
                            printf("P25-float   -[%f]\n"             ,alarmpar->levelGroup[i].portGroup[j].paramter.n);
                            printf("P26-float   -[%f]\n"             ,alarmpar->levelGroup[i].portGroup[j].paramter.NonRelectThreshold);                  
                            printf("P27-float   -[%f]\n"             ,alarmpar->levelGroup[i].portGroup[j].paramter.EndThreshold);   

                            printf("IP01-string -[%s]\n"             ,alarmpar->levelGroup[i].portGroup[j].IP01 );                            //通信参数
                            printf("IP02-string -[%s]\n"             ,alarmpar->levelGroup[i].portGroup[j].IP02 );
                            printf("IP03-string -[%s]\n"             ,alarmpar->levelGroup[i].portGroup[j].IP03 );
                            printf("IP04-string -[%s]\n"             ,alarmpar->levelGroup[i].portGroup[j].IP04 );
                            printf("IP05-string -[%s]\n"             ,alarmpar->levelGroup[i].portGroup[j].IP05 );
                            printf("IP06-string -[%s]\n"             ,alarmpar->levelGroup[i].portGroup[j].IP06 );
                            printf("T3-string   -[%s]\n"             ,alarmpar->levelGroup[i].portGroup[j].T3 );
                            printf("T4-string   -[%s]\n"             ,alarmpar->levelGroup[i].portGroup[j].T4 );                    
 
               }		    
          }

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

    strSNo   = (char *) malloc(sizeof(char)*10);
    strMNo   = (char *) malloc(sizeof(char)*10);    
    dbSNoA   = (char *) malloc(sizeof(char)*10);
    dbSNoB   = (char *) malloc(sizeof(char)*10);
    strSNoA  = (char *) malloc(sizeof(char)*10);
    strSNoB  = (char *) malloc(sizeof(char)*10);
    strPNo   = (char *) malloc(sizeof(char)*10);
    mysql = SQL_Create();
    rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
    if( rc != SQLITE_OK )
	printf( "Open Database error.\n");
    mysql->db =mydb;
    for(i=0;i<AN;i++){                        
            ASN=alarmpar->levelGroup[i].ASN;         
            for(j=0;j<ASN;j++){
                   checkFlag   =0;
                   ModType     =0;
                   UseFlag     =0;
                   PortFlag    =0;
                   existPS     =0;
                   ModType     =0;
                             /*查看模块设置和端口占用*/
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
		      resp->RespondCode  =14;
                      if(resp->SNorPN!=TYPE_PNo){
		          resp->ErrorSN      = 1;                     
		          resp->SNorPN       = TYPE_SNo;
		          resp->Group[ErrorSNo].SNo =  alarmpar->levelGroup[i].portGroup[j].ASNo;
		          resp->Group[ErrorSNo].Main_inform  = "模块未激活";
			  resp->Group[ErrorSNo].Error_inform = "Error: Don't have such Module\n";
		          ErrorSNo++;
                      }                           
		   }else if(PortFlag!=1)
		   {
		      resp->RespondCode  = 14;
                      if(resp->SNorPN!=TYPE_PNo){
		          resp->ErrorSN      = 1;                     
		          resp->SNorPN       = TYPE_SNo;
		          resp->Group[ErrorSNo].SNo =  alarmpar->levelGroup[i].portGroup[j].ASNo;
		          resp->Group[ErrorSNo].Main_inform  = "端口未配置";
			  resp->Group[ErrorSNo].Error_inform = "Error: Don't have such Port\n";
		          ErrorSNo++;  
                      }                                                         
		   }else if(UseFlag==1 && PortFlag==1 ){   
                       printf("存在模块%d和端口%d: ---，模块类型为：%d  端口类型为:%d \n",ModuleNo,alarmpar->levelGroup[i].portGroup[j].ASNo,ModType,alarmpar->levelGroup[i].portGroup[j].fibreType);
                       if(ModType==1){        //一般在纤模式 
 				checkFlag =0;
				resp->RespondCode = 14 ;                                               
				if(resp->SNorPN!=TYPE_PNo){
					resp->SNorPN              = TYPE_SNo;
					resp->Group[ErrorSNo].PNo = atoi(resultPNo[0]); 
					resp->Group[ErrorSNo].SNo = alarmpar->levelGroup[i].portGroup[j].ASNo; 
					resp->Group[ErrorSNo].Main_inform  = "在纤模式(一般):无法进行障碍告警测试";
					resp->Group[ErrorSNo].Error_inform = "Error: Sqlite don't match -->PortTable don't match whih Segment.[在纤模式(一般):无法进行障碍告警测试]";
					ErrorSNo++; 
				}  
                       }//end if(ModType=1) 
                       if(ModType==2){        //备纤模式  
				   mysql->tableName    = "PortOccopyTable";     //端口表中获取当前SNo的fiberType
				   mysql->filedsName   = "FiberType";
                         	   mysql->mainKeyValue = strSNo;        
				   SQL_lookupPar(mysql,&result,&rednum);
				   fiberType=atoi(result[0])-1;
				   printf("PortTable SNo=%s,fiberType=%d\n",strSNo,fiberType);
                                   printf("Segment   SNo=%s,fiberType=%d\n",strSNo,alarmpar->levelGroup[i].portGroup[j].fibreType);
				   SQL_freeResult(&result,&rednum);
                                   if(alarmpar->levelGroup[i].portGroup[j].fibreType==0 && fiberType==0) checkFlag=1;
                                   else if(alarmpar->levelGroup[i].portGroup[j].fibreType==0 && fiberType==1){
 						checkFlag =0;
						resp->RespondCode = 14 ;                                               
						if(resp->SNorPN!=TYPE_PNo){
							 resp->SNorPN              = TYPE_SNo;
							 resp->Group[ErrorSNo].PNo = atoi(resultPNo[0]); 
							 resp->Group[ErrorSNo].SNo = alarmpar->levelGroup[i].portGroup[j].ASNo; 
							 resp->Group[ErrorSNo].Main_inform  = "备纤模式:指令为备纤但端口表为在纤";
							 resp->Group[ErrorSNo].Error_inform = "Error: Sqlite don't match -->PortTable don't match whih Segment.[备纤模式:指令为备纤但端口表为在纤]";
							 ErrorSNo++; 
						}
                                   }else{
 						checkFlag =0;
						resp->RespondCode = 14 ;                                               
						if(resp->SNorPN!=TYPE_PNo){
							 resp->SNorPN              = TYPE_SNo;
							 resp->Group[ErrorSNo].PNo = atoi(resultPNo[0]); 
							 resp->Group[ErrorSNo].SNo = alarmpar->levelGroup[i].portGroup[j].ASNo; 
							 resp->Group[ErrorSNo].Main_inform  = "备纤模式:指令为在纤";
							 resp->Group[ErrorSNo].Error_inform = "Error: Sqlite don't match -->PortTable don't match whih Segment.[备纤模式:指令为在纤]";
							 ErrorSNo++; 
						}                                          
                                   }
                       } //end if(ModType=2) 

                       if(ModType==3){         //保护模式(主) 
                                   checkFlag           = 1;    /*
				   mysql->tableName    = "PortOccopyTable";     //端口表中获取当前SNo的fiberType
				   mysql->filedsName   = "FiberType";
                         	   mysql->mainKeyValue = strSNo;        
				   SQL_lookupPar(mysql,&result,&rednum);
                                   fiberType=atoi(result[0])-1;                                                         
                                   SQL_freeResult(&result,&rednum);
                                   if(alarmpar->levelGroup[i].portGroup[j].fibreType == fiberType) checkFlag=1;
                                   else{
                                        checkFlag =0;
					resp->RespondCode = 14 ;                                               
					if(resp->SNorPN!=TYPE_PNo){
						resp->SNorPN              = TYPE_SNo;
						resp->Group[ErrorSNo].PNo = atoi(resultPNo[0]); 
						resp->Group[ErrorSNo].SNo = alarmpar->levelGroup[i].portGroup[j].ASNo; 
						resp->Group[ErrorSNo].Main_inform  = "保护模式（主）:参数指定光纤类型与端口设置类型不匹配";
						resp->Group[ErrorSNo].Error_inform = "Error: Sqlite don't match -->PortTable don't match whih Segment.[保护模式（主）:指令为在纤但端口表为备纤]";
						ErrorSNo++; 
					}
                                   }*/
                       }//end if(ModType=3) 

                       if(ModType==4){  //带光功率采集的在纤模式
				   mysql->tableName    = "PortOccopyTable";     //端口表中获取当前SNo的fiberType
				   mysql->filedsName   = "FiberType";
                         	   mysql->mainKeyValue = strSNo;        
				   SQL_lookupPar(mysql,&result,&rednum);
				   fiberType=atoi(result[0])-1;
				   printf("PortTable SNo=%s,fiberType=%d\n",strSNo,fiberType);
                                   printf("Segment   SNo=%s,fiberType=%d\n",strSNo,alarmpar->levelGroup[i].portGroup[j].fibreType);
				   SQL_freeResult(&result,&rednum);
                                   if(alarmpar->levelGroup[i].portGroup[j].fibreType==1 && fiberType==1 ) checkFlag=1;
                                   else if(alarmpar->levelGroup[i].portGroup[j].fibreType==1 && fiberType==0){
 						checkFlag =0;
						resp->RespondCode = 14 ;                                               
						if(resp->SNorPN!=TYPE_PNo){
							 resp->SNorPN              = TYPE_SNo;
							 resp->Group[ErrorSNo].PNo = atoi(resultPNo[0]); 
							 resp->Group[ErrorSNo].SNo = alarmpar->levelGroup[i].portGroup[j].ASNo; 
							 resp->Group[ErrorSNo].Main_inform  = "在纤模式（OPM）:指令为在纤但端口表为备纤";
							 resp->Group[ErrorSNo].Error_inform = "Error: Sqlite don't match -->PortTable don't match whih Segment.[在纤模式（OPM）:指令为在纤但端口表为备纤]";
							 ErrorSNo++; 
						}
                                   }else{
 						checkFlag =0;
						resp->RespondCode = 14 ;                                               
						if(resp->SNorPN!=TYPE_PNo){
							 resp->SNorPN              = TYPE_SNo;
							 resp->Group[ErrorSNo].PNo = atoi(resultPNo[0]); 
							 resp->Group[ErrorSNo].SNo = alarmpar->levelGroup[i].portGroup[j].ASNo; 
							 resp->Group[ErrorSNo].Main_inform  = "在纤模式（OPM）:指令为备纤";
							 resp->Group[ErrorSNo].Error_inform = "Error: Sqlite don't match -->PortTable don't match whih Segment.[在纤模式（OPM）:指令为备纤]";
							 ErrorSNo++; 
						}                                          
                                   }
                      
                       }//end if(ModType=4) 

                                          /*检查是否使用优化参数*/
		       if(alarmpar->levelGroup[i].portGroup[j].PS ==0){
		       mysql->tableName    = "DefaultTsetSegmentTable";
		       mysql->mainKeyValue =  strSNo; 
		       mysql->filedsName   = "SNo";
		       existPS=SQL_existIN_db(mysql);
		       if(existPS==0){
		             resp->RespondCode = 3 ;                                                 // 参数错误
			     if(resp->SNorPN!=TYPE_PNo){
					resp->SNorPN      = TYPE_SNo;
					resp->Group[ErrorSNo].SNo = alarmpar->levelGroup[i].portGroup[j].ASNo; 
					resp->Group[ErrorSNo].Main_inform  = "没有优化测试参数";
					resp->Group[ErrorSNo].Error_inform = "Error: Sqlite don't match -->Error SNo [没有优化测试参数]";
					ErrorSNo++; 
			     } 
		         }
		      }	


                     if(ModType==3 && checkFlag==1 ){
                          saveFlag=1;
                          alarmpar->levelGroup[i].portGroup[j].protectFlag=1;
                          alarmpar->Action= 1;
                     }else if (ModType==2 && checkFlag==1 ) {
                          saveFlag=1;
                          alarmpar->levelGroup[i].portGroup[j].protectFlag=0;
                          alarmpar->Action=-1; 
                     }else if (ModType==4 && checkFlag==1 ) {
                          saveFlag=1;
                          alarmpar->levelGroup[i].portGroup[j].protectFlag=0;
                          alarmpar->Action=-1; 
                     }else{
                          saveFlag=0;
                          alarmpar->Action= 2;
                     }
                     
                                          /*数据库存储有效光路*/             
                     if(saveFlag==1){ 
                          if(alarmpar->levelGroup[i].portGroup[j].PS ==0 && existPS==1){
				     strSQL   = (char *) malloc(sizeof(char)*400);
				     mysql->tableName   = "AlarmTestSegmentTable";   
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
				     rc = SQL_add(mysql);                                  //更新或者插入新的纪录
				     if(rc!=SQLITE_OK) printf( "Save SQL error\n");
				     else              printf("%s",strSQL);
				     if(!semaphore_v())                                    //V
				        exit(EXIT_FAILURE);
		                     free(strSQL);
		                     strSQL=NULL;      
                          }else if( alarmpar->levelGroup[i].portGroup[j].PS ==1){
				     strSQL   = (char *) malloc(sizeof(char)*400);
				     mysql->tableName   = "AlarmTestSegmentTable";   
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
				     if(rc!=SQLITE_OK)  printf( "Save SQL error\n");
				     else               printf("%s",strSQL);
				     if(!semaphore_v())                                           //V
				        exit(EXIT_FAILURE);
		                     free(strSQL);
		                     strSQL=NULL;
                           }    
                       }//end if (ModType)
                }//end if (UseFlag && portFlag)
            }//end for
          
    }  //end for

     if(resp->RespondCode != 0 ){
	resp->ErrorSN     =  ErrorSNo;                  //错误光路总条数
        free(strSNo);
        free(strMNo);
        free(dbSNoB);
        free(dbSNoA);
        SQL_Destory(mysql);  
        sqlite3_close(mydb);
        Alarm_Destory(alarmpar);
        return resp;
     }
  
     free(strSNo);
     free(strMNo);
     free(dbSNoB);
     free(dbSNoA);
     SQL_Destory(mysql);  
     sqlite3_close(mydb);
     Alarm_Destory(alarmpar);
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
       int retProcess = 0;  
       pid_t cycPID[MAX_PID_NUM];  
      
       process ="/web/cgi-bin/alarmMain";                        
       retProcess = get_pid_by_name(process, cycPID, MAX_PID_NUM); 
       
       if(retProcess>0 && ModType !=3 ){
	       printf("process '%s' is existed? (%d): %c\n", process, retProcess, (retProcess > 0)?'y':'n');  
	       signum=SIGUSR1;                                         //设置信号值:插入或修改周期测试链表节点值
	       mysigval.sival_int = 130;                               //设置信号的附加信息 (启动障碍告警测试)                               
	       for(n=0;n<retProcess;n++){  
	       printf("alarmMaim-PID:%u\n", cycPID[n]);                //获取障碍告警测试守护进程PID
	       if(sigqueue(cycPID[n],signum,mysigval)==-1)
		       printf("send signal error\n");
	      }  
	           /*等待启动障碍告警测试成功信号*/

	    char * recvStr;  
	    recvStr = (char *) malloc (sizeof (char)*10);
	    recvStr = recvMessageQueue_C();
	    if(strncmp(recvStr, "130-OK", 6) == 0)                     //遇"130-OK"结束
	            printf("Set Alarmtest sucessful!\n");
	    else{
                    printf("SetCycleSegment failed!\n");
		    resp->RespondCode=3;
		    resp->Group[0].Main_inform  = "障碍告警测试设置失败-->未收到回复消息";
		    resp->Group[0].Error_inform = "Error:Don't get back massgae![障碍告警测试设置失败-->未收到回复消息]";
		    return resp;   
            }
	       free(recvStr);
	   
      }else if(ModType !=3){
	      resp->RespondCode=3;
	      resp->Group[0].Main_inform  = "障碍告警测试设置失败-->未找到后台进程";
	      resp->Group[0].Error_inform = "Error:Don't have back process![障碍告警测试设置失败-->未找到后台进程]";
	      return resp; 
      }
 }
   return resp;

}


responed *  setAlarmInformation(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode){

   mxml_node_t    *CM,*AI,*perCMDcode;
   alarminfom *alarmPar;
   responed *resp; 
   int intCM;
  
   alarmPar = AlarmInfo_Create();   
   resp     = Responed_Create();
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
	    CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	    alarmPar->CM = strtoul (CM->child->value.text.string, NULL, 0); 	
	    AI = mxmlFindElement(cmd, tree, "AI",NULL, NULL,MXML_DESCEND);
	    alarmPar->AI = strtoul (AI->child->value.text.string, NULL, 0); 	  
            printf("------setAlarm------\n");    
            printf("CM=%d\n",alarmPar->CM); 
            printf("AI=%d\n",alarmPar->AI);  
   }  
   AlarmInfo_Destory(alarmPar);
   return resp;

}


responed * endAlarmtestSegment(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)        //230
{
   mxml_node_t  *SN=NULL,*CM=NULL,*CLP=NULL,*FX=NULL,*perCMDcode=NULL;
   responed *resp=NULL; 
   int rtuCM,rtuCLP,intSN,i,uint_a;
   char *strFX=NULL,*strNum=NULL;
   strFX=malloc(sizeof(char)*10);
   strNum=malloc(sizeof(char)*10);

   cancelAlarmtest * alarmEndpar=NULL;
   alarmEndpar=endAlarm_Create();
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
	    SN =mxmlFindElement(cmd, tree, "SN",NULL, NULL,MXML_DESCEND);
	    intSN =  strtoul(SN->child->value.text.string, NULL, 0);
	    alarmEndpar->SN = intSN;
	    alarmEndpar->Action = -2;                                                                          //将状态设置为-2，等待从取障碍告警测试中删除
	    CM =mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	    rtuCM =  strtoul(CM->child->value.text.string, NULL, 0);   
	    CLP =mxmlFindElement(cmd, tree, "CLP",NULL, NULL,MXML_DESCEND);
	    rtuCLP =  strtoul(CLP->child->value.text.string, NULL, 0);    
	    for (i=0;i<intSN;i++){ 
		//strFX[1]=i+0x31;
                *strFX='\0';
                strcat(strFX,"F");
                uint32tostring(i+1,strNum);
                strcat(strFX,strNum);
		FX  = mxmlFindElement(cmd, tree, strFX ,NULL, NULL,MXML_DESCEND);     
		uint_a = strtoul (FX->child->value.text.string, NULL, 0);          
		alarmEndpar->Group[i].SNo = uint_a;
		if(alarmEndpar->Group[i].SNo == 0) break; 
	    }

/*****************************修改光路状态********************************************/

	   sqlite3 *mydb=NULL;
	   sql     *mysql=NULL;
	   int rc=0,rednum=0,existFlag=-1;
	   int ErrorSNo=0;
	   char **result=NULL,*strSNo=NULL;
	   strSNo = (char *) malloc(sizeof(char)*10);
	   mysql = SQL_Create();
	   rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
	   if(rc != SQLITE_OK)
	       printf( "Open Database error.\n");
	   mysql->db =mydb;
	   for(i=0;i<intSN;i++){
		 uint32tostring(alarmEndpar->Group[i].SNo,strSNo);
		 mysql->tableName    =  "AlarmTestSegmentTable"; 
		 mysql->filedsName   =  "Status";
		 mysql->mainKeyValue = strSNo;  
		 existFlag=SQL_existIN_db(mysql);
		 if(existFlag==1){
		    mysql->filedsValue  =  "-2";                            //更新光路状态为“-2”   
		    if(!semaphore_p())  
		        exit(EXIT_FAILURE);                                 //P
		    rc=SQL_modify(mysql);
		    if( rc != SQLITE_OK ){
		       printf( "Modify SQL error\n");
		     }
		    if(!semaphore_v())                                      //V
		       exit(EXIT_FAILURE);
		 }else{
		     resp->RespondCode = 14 ;                               // 参数错误
		     resp->SNorPN      = TYPE_SNo;
		     resp->Group[ErrorSNo].SNo = alarmEndpar->Group[i].SNo;
		     if(existFlag!=1)
		        resp->Group[ErrorSNo].Error_inform = "Error: Sqlite don't match -->lack of SNo\n";
		     ErrorSNo++;  
		 }
	  }
	  free(strSNo);
	  SQL_Destory(mysql);  
	  sqlite3_close(mydb);
          endAlarm_Destory(alarmEndpar);
	  if(resp->RespondCode != 0 ){
		resp->ErrorSN     =  ErrorSNo;                              //错误光路总条数
                return resp;
	  }
/***************************障碍告警测试进程发送信号**********************
(1)向障碍告警测试守护进程发送取消障碍告警测试信号
(2)注意一定要将发送程序和接收程序划到一个用户组，并且都具有root权限，否则信号发射会失败(BoaCgi 4777)
(3)程可以通过sigqueue函数向包括它本身在内的其他进程发送一个信号，如果程序没有发送这个信号的权限，对sigqueue函数的调用就将失败，而失败的常见原因是目标进程由另一个用户所拥有
(4)现阶段使用的是不可靠信号,不支持排队，信号可能丢失。
***********************************************************************/
         int signum;
         union sigval mysigval;
         char* process;  
         int retProcess = 0,n=0;  
         pid_t cycPID[MAX_PID_NUM];  
	 char * recvStr;  
	 recvStr = (char *) malloc (sizeof (char)*10);      

         process ="/web/cgi-bin/alarmMain";                        
         retProcess = get_pid_by_name(process, cycPID, MAX_PID_NUM);  
         if(retProcess>0){
	       printf("process '%s' is existed? (%d): %c\n", process, retProcess, (retProcess > 0)?'y':'n');  
	       signum=SIGUSR1;                                         //设置信号值:插入或修改周期测试链表节点值
	       mysigval.sival_int = 230;                               //设置信号的附加信息 (启动障碍告警测试)                               
	       for(n=0;n<retProcess;n++){  
		       printf("alarmMaim-PID:%u\n", cycPID[n]);        //获取障碍告警测试守护进程PID
		       if(sigqueue(cycPID[n],signum,mysigval)==-1)
			       printf("send signal error\n");
	       }  
	           /*等待启动障碍告警测试成功信号*/
	      recvStr = recvMessageQueue_C();
	      if(strncmp(recvStr, "230-OK", 6) == 0)                     //遇"130-OK"结束
	         printf("Set Alarmtest sucessful!\n");
	      else{
                 printf("SetCycleSegment failed!\n");
	         resp->RespondCode=3;
	         resp->Group[0].Main_inform  = "障碍告警测试取消失败-->未收到回复消息";
	         resp->Group[0].Error_inform = "Error:Don't get back massgae![障碍告警测试取消失败-->未收到回复消息]";
	         free(recvStr);
	         return resp;  
              }
	      free(recvStr);	   
        }else{
	      resp->RespondCode=3;
	      resp->Group[0].Main_inform  = "障碍告警测试取消失败-->未找到后台进程";
	      resp->Group[0].Error_inform = "Error:Don't have back process![障碍告警测试取消失败-->未找到后台进程]";
	      return resp;  
        }
     
   } 
   return resp;
}


responed * endAlarmInfo(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)
{
   mxml_node_t    *CM,*AI,*perCMDcode;
   alarminfom *alarmPar;
   responed *resp; 
   int intCM;
  
   alarmPar = AlarmInfo_Create();   
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
/**************************解析XML消息******************************/
	    CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	    alarmPar->CM = strtoul (CM->child->value.text.string, NULL, 0); 	
            printf("------cancelAlarm------\n");
            printf("CM=%d\n",alarmPar->CM); 
       }  
   AlarmInfo_Destory(alarmPar);
   return resp;
}

