#include "namedtest.h"
#include "process.h"
#include "sql.h"
namedtest * NamedP_Create()
{
    namedtest * me = (namedtest *) malloc(sizeof(namedtest));
    return me;
}
void NamedP_Destory(namedtest *me)
{
    free(me);
}

void  getNamedParameter(mxml_node_t *root,mxml_node_t *tree, namedtest *namedpar)
{
	    mxml_node_t *SN=NULL,*PS=NULL,*SNo=NULL,*P01=NULL,*P02=NULL,*P03=NULL,*P04=NULL,*P05=NULL,*P06=NULL,*P07=NULL;

	    uint32_t  uint_a=0;
	    float     float_a=0;
 	    PS = mxmlFindElement(root, tree,  "PS",NULL, NULL,MXML_DESCEND);
	    uint_a = strtoul (PS->child->value.text.string, NULL, 0);
	    namedpar->PS = uint_a;  

	    SNo = mxmlFindElement(root, tree, "SNo",NULL, NULL,MXML_DESCEND);
	    uint_a = strtoul (SNo->child->value.text.string, NULL, 0);
	    namedpar->SNo = uint_a;

	    P01 = mxmlFindElement(root, tree, "P11",NULL, NULL,MXML_DESCEND);
	    uint_a = strtoul (P01->child->value.text.string, NULL, 0);  
	    namedpar->paramter.MeasureLength_m = uint_a;

 
	    P02 = mxmlFindElement(root, tree, "P12",NULL, NULL,MXML_DESCEND);
	    uint_a = strtoul (P02->child->value.text.string, NULL, 0); 
	    namedpar->paramter.PulseWidth_ns = uint_a;    

	    P03 = mxmlFindElement(root, tree, "P13",NULL, NULL,MXML_DESCEND);
	    uint_a = strtoul (P03->child->value.text.string, NULL, 0); 
	    namedpar->paramter.Lambda_nm = uint_a;
  
	    P04 = mxmlFindElement(root, tree, "P14",NULL, NULL,MXML_DESCEND);
	    uint_a = strtoul (P04->child->value.text.string, NULL, 0); 
	    namedpar->paramter.MeasureTime_ms = uint_a;  

	    P05 = mxmlFindElement(root, tree, "P15",NULL, NULL,MXML_DESCEND); 
	    float_a =atof(P05->child->value.text.string);
	    namedpar->paramter.n = float_a;   

	    P06 = mxmlFindElement(root, tree, "P16",NULL, NULL,MXML_DESCEND);
	    float_a =atof(P06->child->value.text.string);
	    namedpar->paramter.NonRelectThreshold = float_a; 	          

	    P07 = mxmlFindElement(root, tree, "P17",NULL, NULL,MXML_DESCEND);
	    float_a =atof(P07->child->value.text.string);
            namedpar->paramter.EndThreshold = float_a;

}
responed * setNamedTestSegment(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)
{	
   mxml_node_t *CM=NULL,*perCMDcode=NULL;
   namedtest * nmdpar=NULL;
   otdr * testPar=NULL;
   responed *resp=NULL; 
   int intCM=0,PS=0,SNo=0;
   testPar = OTDR_Create();
   nmdpar  = NamedP_Create();   
   resp    = Responed_Create();

   resp->RespondCode=0;        
   perCMDcode = mxmlFindElement(cmd, tree, "CMDcode",NULL, NULL,MXML_DESCEND);
      if(atoi(perCMDcode->child->value.text.string) !=cmdCode) 
       {
            resp->RespondCode=3;
	    resp->Group[0].Main_inform  = "指令号错误";
	    resp->Group[0].Error_inform = "Error:CMDcode Error[指令号错误]";
            return resp;   
       }
      else
      {
/**************************解析XML消息***************************************/
            getNamedParameter(cmd,tree,nmdpar);
	    CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	    intCM = strtoul (CM->child->value.text.string, NULL, 0); 	    
	    PS=                         nmdpar->PS;
	    SNo =                       nmdpar->SNo;
	    testPar->MeasureLength_m=    nmdpar->paramter.MeasureLength_m;
	    testPar->PulseWidth_ns=      nmdpar->paramter.PulseWidth_ns;
	    testPar->Lambda_nm =         nmdpar->paramter.Lambda_nm;
	    testPar->MeasureTime_ms=     nmdpar->paramter.MeasureTime_ms;   
	    testPar->n=                  nmdpar->paramter.n;
	    testPar->EndThreshold=       nmdpar->paramter.EndThreshold;
	    testPar->NonRelectThreshold= nmdpar->paramter.NonRelectThreshold;

            printf("-----------------------\n");

            printf("PS -uint -[%d]\n",  nmdpar->PS);

            printf("SNo-uint -[%d]\n",  nmdpar->SNo);

	    printf("P11-uint -[%d]\n",  nmdpar->paramter.MeasureLength_m);

	    printf("P12-uint -[%d]\n",  nmdpar->paramter.PulseWidth_ns);

	    printf("P13-uint -[%d]\n",  nmdpar->paramter.Lambda_nm);

	    printf("P14-uint -[%d]\n",  nmdpar->paramter.MeasureTime_ms);

	    printf("P15-float-[%f]\n",  nmdpar->paramter.n);

	    printf("P16-float-[%f]\n",  nmdpar->paramter.NonRelectThreshold);

	    printf("P17-float-[%f]\n",  nmdpar->paramter.EndThreshold);

            printf("-----------------------\n");

/*****************************数据库同步检查*********************************/
           int rc,j=0,ErrorSNo =0,PortFlag=0,existFlag=-1,UseFlag=0; 
           char **result = NULL;
           char *strSNO,*strSQL;
           int  ModuleNo=0,rednum=0;
           namedtest *namedTemp;
           sqlite3 *mydb;
           sql *mysql;

           strSQL   = (char *) malloc(sizeof(char)*200);
           strSNO = (char *) malloc(sizeof(char)*10);
           namedTemp = NamedP_Create();
           mysql = SQL_Create();
           rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
           if( rc != SQLITE_OK )
	      printf( "Open SQL error\n");
	  
           mysql->db =mydb;

           mysql->tableName ="SubModuleTypeTable";                 //检查子单元模块是否存在
           ModuleNo = ((SNo-1)/8)+1;
           uint32tostring(ModuleNo,strSNO);
           mysql->mainKeyValue = strSNO;
           mysql->filedsName   = "UseFlag";
           SQL_lookupPar(mysql,&result,&rednum);
           UseFlag=atoi(result[0]);
           SQL_freeResult(&result,&rednum);
     
           if(UseFlag!=1)
           {
                  resp->RespondCode  = 14;
                  resp->ErrorSN      = 1;                     
                  resp->SNorPN       = TYPE_SNo;
                  resp->Group[0].SNo = nmdpar->SNo;
                  resp->Group[0].Main_inform  = "模块未激活";
		  resp->Group[0].Error_inform = "Error: Don't have such Module\n";
		  NamedP_Destory(nmdpar);  
		  NamedP_Destory(namedTemp);
		  return  resp;                                   
           }


           mysql->tableName ="PortOccopyTable";                      //检查子端口是否占用
	   uint32tostring(SNo,strSNO);
	   mysql->mainKeyValue = strSNO;
           PortFlag  = SQL_existIN_db(mysql); 
           if(PortFlag !=1)
           {
                  resp->RespondCode  = 14;
                  resp->ErrorSN      = 1;                     
                  resp->SNorPN       = TYPE_SNo;
                  resp->Group[0].SNo = nmdpar->SNo;
                  resp->Group[0].Main_inform  = "端口未配置";
		  resp->Group[0].Error_inform = "Error: Don't have such Port\n";
		  NamedP_Destory(nmdpar);  
		  NamedP_Destory(namedTemp);
		  return  resp;                                  
           } 
          
           if(0==PS){
	       mysql->tableName    = "DefaultTsetSegmentTable";     //检查是否存在优化测试参数              
	       uint32tostring(SNo,strSNO);
	       mysql->mainKeyValue =  strSNO;
	       mysql->filedsName   = "SNo";
	       existFlag = SQL_existIN_db(mysql);                                              
	           if(existFlag==0){
			   printf("This is NULL:SNo=%d",SNo);                                     
			   resp->RespondCode  = 14 ;      
                           resp->ErrorSN      = 1; 
                           resp->SNorPN       = TYPE_SNo;                                         
			   resp->Group[0].SNo = nmdpar->SNo;
                           resp->Group[0].Main_inform  = "没有优化测试参数";
			   resp->Group[0].Error_inform = "Error: Sqlite don't match --->Don't have DefaluetSegment\n";
                           NamedP_Destory(nmdpar);  
		           NamedP_Destory(namedTemp);
		           return  resp; 
         
		   }else{                                                                

			   namedTemp->SNo                           =nmdpar->SNo;
			   namedTemp->PS                            =nmdpar->PS;
			   namedTemp->paramter.MeasureLength_m      =0;
			   namedTemp->paramter.PulseWidth_ns        =0;
			   namedTemp->paramter.Lambda_nm            =0;
			   namedTemp->paramter.MeasureTime_ms       =0;
			   namedTemp->paramter.n                    =0;
			   namedTemp->paramter.NonRelectThreshold   =0;
			   namedTemp->paramter.EndThreshold         =0;
		   } 		                                                                         
	    }else{
		   namedTemp->SNo                           =nmdpar->SNo;
	           namedTemp->PS                            =nmdpar->PS;
	      	   namedTemp->paramter.MeasureLength_m      =nmdpar->paramter.MeasureLength_m;
		   namedTemp->paramter.PulseWidth_ns        =nmdpar->paramter.PulseWidth_ns;
		   namedTemp->paramter.Lambda_nm            =nmdpar->paramter.Lambda_nm;
		   namedTemp->paramter.MeasureTime_ms       =nmdpar->paramter.MeasureTime_ms;
		   namedTemp->paramter.n                    =nmdpar->paramter.n;
		   namedTemp->paramter.NonRelectThreshold   =nmdpar->paramter.NonRelectThreshold;
		   namedTemp->paramter.EndThreshold         =nmdpar->paramter.EndThreshold;
	    }

	    if(resp->RespondCode != 0 ){
		   resp->ErrorSN     =  ErrorSNo;                 
		   NamedP_Destory(nmdpar);  
		   NamedP_Destory(namedTemp);
		   return  resp;                                   
	    }


/******************************向数据库写入点名测试参数*******************************/
         uint32tostring(SNo,strSNO);
         sprintf(strSQL,"%d,%d,%d,%d,%d,%d,%f,%f,%f\n",namedTemp->SNo                        //SNo PS P11 P12 P13 P14 P15 P16 P17
		                                   ,namedTemp->PS
		                                   ,namedTemp->paramter.MeasureLength_m
		                                   ,namedTemp->paramter.PulseWidth_ns
		                                   ,namedTemp->paramter.Lambda_nm
		                                   ,namedTemp->paramter.MeasureTime_ms
		                                   ,namedTemp->paramter.n
		                                   ,namedTemp->paramter.NonRelectThreshold
		                                   ,namedTemp->paramter.EndThreshold);
         mysql->tableName    =  "NamedTestSegmentTable";  
         mysql->mainKeyValue =  strSNO;                
         mysql->filedsValue  =  strSQL;
         if(!semaphore_p())  
            exit(EXIT_FAILURE);                                //P
         rc = SQL_add(mysql);                                  //更新或者插入新的纪录
         if( rc != SQLITE_OK ){
            printf( "Save SQL error\n");
	 }else  printf("%s",strSQL);
         if(!semaphore_v())                                    //V
            exit(EXIT_FAILURE);

        NamedP_Destory(nmdpar);  
        NamedP_Destory(namedTemp);
        SQL_Destory(mysql);  
        sqlite3_close(mydb);

/***************************向OTDR测试进程发送消息，并等待测试完成*****************/
        int signum=0,retProcess = 0,n=0;
        char* process=NULL,*recvStr=NULL;  
        pid_t cycPID[MAX_PID_NUM];
        union sigval mysigval;  
                                    /*向otdrMain发送启动信号*/
        process ="/web/cgi-bin/otdrMain";                        
        retProcess = get_pid_by_name(process, cycPID, MAX_PID_NUM);  
	printf("process '%s' is existed? (%d): %c ", process, retProcess, (retProcess > 0)?'y':'n');  
        if(retProcess>0){
		signum=SIGRTMIN;                                         
		mysigval.sival_int = SNo+100;                            //发送需要启动测试的光路号  及测试类型：百位表示类型+十位个位表示SNo   (eg:101,type=1,SNo=1)                         
		for(n=0;n<retProcess;n++){                                      
		     printf("otdrMain PID:%u\n", cycPID[n]);             //获取OTDR测试进程PID
		     if(sigqueue(cycPID[n],signum,mysigval)==-1)
			 printf("send signal error\n");
		}	 
		                    /*等待信号的成功处理消息*/			    
		recvStr = (char *) malloc (sizeof (char)*10);
		recvStr = recvMessageQueue_A();                          //阻塞等待测试执行完毕             
		if(strncmp(recvStr, "1-OK", 4) == 0){                    //遇"1-OK"结束		      
		      printf("Named test from otdrMain  sucessful!\n");
		}
		else{
		      printf("Don't have any messges from otdrMain!\n");
		      resp->RespondCode=3;
		      resp->Group[0].Main_inform  = "点名测试设置失败-->未收到回复消息";
		      resp->Group[0].Error_inform = "Error:Don't get back massage![点名测试设置失败-->未收到回复消息]";
	              free(recvStr);
		      return resp;  
		}
                free(recvStr);
	                           /*回传点名测试测试数据文件*/
		RespondMessage_TestData(testPar,PS,SNo,intCM,FILE_MODE_NAMED);
		OTDR_Destory(testPar);
        }else{
		resp->RespondCode=3;
		resp->Group[0].Main_inform  = "点名测试设置失败-->未找到后台进程";
		resp->Group[0].Error_inform = "Error:Don't get back massage![点名测试设置失败-->未找到后台进程]";
	        free(recvStr);
		return resp;  
        }
      }
   return resp;
}
