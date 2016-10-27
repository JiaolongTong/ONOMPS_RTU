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
	    mxml_node_t *SN,*PS,*SNo,*P01,*P02,*P03,*P04,*P05,*P06,*P07;

	    uint32_t  uint_a;
	    float     float_a;
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
   mxml_node_t *CM,*perCMDcode;
   namedtest * nmdpar;
   otdr * test_p;
   responed *resp; 
   int intCM;
   int PS,SNo; 

   test_p = OTDR_Create();
   nmdpar = NamedP_Create();   
   resp   = Responed_Create();

   resp->RespondCode=0;        
   perCMDcode = mxmlFindElement(cmd, tree, "CMDcode",NULL, NULL,MXML_DESCEND);
      if(atoi(perCMDcode->child->value.text.string) !=cmdCode) 
       {
            printf("<RespondCode>3</RespondCode>\n");
	    printf("<Data>CMDcode Error [ %s:%s]</Data>\n",perCMDcode->value.element.name,perCMDcode->child->value.text.string);
            return (responed *)-1;
            
       }
      else
      {
/**************************解析XML消息***************************************/
            getNamedParameter(cmd,tree,nmdpar);
	    CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	    intCM = strtoul (CM->child->value.text.string, NULL, 0); 	    
	    PS=                         nmdpar->PS;
	    SNo =                       nmdpar->SNo;
	    test_p->MeasureLength_m=    nmdpar->paramter.MeasureLength_m;
	    test_p->PulseWidth_ns=      nmdpar->paramter.PulseWidth_ns;
	    test_p->Lambda_nm =         nmdpar->paramter.Lambda_nm;
	    test_p->MeasureTime_ms=     nmdpar->paramter.MeasureTime_ms;   
	    test_p->n=                  nmdpar->paramter.n;
	    test_p->EndThreshold=       nmdpar->paramter.EndThreshold;
	    test_p->NonRelectThreshold= nmdpar->paramter.NonRelectThreshold;

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
           int rc,j=0,ErrorSNo =0,flag=-1; 
           char *zErrMsg = 0;
           char **result = NULL;
           char *s,*str;
           namedtest *namedTemp;
           sqlite3 *mydb;
           sql *mysql;

           str   = (char *) malloc(sizeof(char)*200);
           s = (char *) malloc(sizeof(char)*10);
           namedTemp = NamedP_Create();
           mysql = SQL_Create();
           rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
           if( rc != SQLITE_OK ){
	      printf( "Open SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	   }
           mysql->db =mydb;
           if(0==PS){
	       mysql->tableName    = "DefaultTsetSegmentTable";                  
	       uint32tostring(SNo,s);
	       mysql->mainKeyValue =  s;
	       mysql->filedsName   = "SNo";
	       flag = SQL_existIN_db(mysql);                                              // 检查SNo对应的测试参数是否与本地数据库同步
	           if(flag==0){
		   printf("This is NULL:SNo=%d",SNo);                                     // 若不同步 把不存在的光路号记录下来.ErrorSN++  , resp->Group[0].SNo=cycfpar->Group[i].SNo
		   resp->RespondCode = 14 ;                                               // 通信参数错误（时间表、IP地址）
		   resp->Group[ErrorSNo].SNo = nmdpar->SNo;
		   resp->Group[ErrorSNo].Error_inform = "Error: Sqlite don't match\n";
		   ErrorSNo++;          
		   }else{                                                                 // 若同步   

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
		  resp->ErrorSN     =  ErrorSNo;                  //错误光路总条数
		  NamedP_Destory(nmdpar);  
		  NamedP_Destory(namedTemp);
		 return  resp;                                   //保存所有正确的光路，一旦有错误就拒绝执行周期测试启动，并返回错误的光路号
	    }


/******************************向数据库写入点名测试参数*******************************/
         uint32tostring(SNo,s);
         sprintf(str,"%d,%d,%d,%d,%d,%d,%f,%f,%f\n",namedTemp->SNo                        //SNo PS P11 P12 P13 P14 P15 P16 P17
		                                   ,namedTemp->PS
		                                   ,namedTemp->paramter.MeasureLength_m
		                                   ,namedTemp->paramter.PulseWidth_ns
		                                   ,namedTemp->paramter.Lambda_nm
		                                   ,namedTemp->paramter.MeasureTime_ms
		                                   ,namedTemp->paramter.n
		                                   ,namedTemp->paramter.NonRelectThreshold
		                                   ,namedTemp->paramter.EndThreshold);
         mysql->tableName    =  "NamedTestSegmentTable";  
         mysql->mainKeyValue =  s;                
         mysql->filedsValue  =  str;
         if(!semaphore_p())  
            exit(EXIT_FAILURE);                                //P
         rc = SQL_add(mysql);                                  //更新或者插入新的纪录
         if( rc != SQLITE_OK ){
            printf( "Save SQL error\n");
            sqlite3_free(zErrMsg);
	 }else  printf("%s",str);
         if(!semaphore_v())                                    //V
            exit(EXIT_FAILURE);
        NamedP_Destory(nmdpar);  
        NamedP_Destory(namedTemp);
        SQL_Destory(mysql);  
        sqlite3_close(mydb);


/***************************向OTDR测试进程发送消息，并等待测试完成*****************/
       int signum;
       union sigval mysigval;
       char* process;  
       char* recvStr;
       int ret = 0;  
       int n;  
       pid_t cycPID[MAX_PID_NUM];  
                             /*向otdrMain发送启动信号*/
        process ="/web/cgi-bin/otdrMain";                        
        ret = get_pid_by_name(process, cycPID, MAX_PID_NUM);  
	printf("process '%s' is existed? (%d): %c ", process, ret, (ret > 0)?'y':'n');  
	signum=SIGRTMIN;                                         
	mysigval.sival_int = SNo+100;                           //发送需要启动测试的光路号  及测试类型：百位表示类型+十位个位表示SNo   (eg:101,type=1,SNo=1)                         
	for(n=0;n<ret;n++){                                      
             printf("otdrMain PID:%u\n", cycPID[n]);            //获取OTDR测试进程PID
             if(sigqueue(cycPID[n],signum,mysigval)==-1)
		 printf("send signal error\n");
	}  
                            /*等待信号的成功处理消息*/			    
	recvStr = (char *) malloc (sizeof (char)*10);
        recvStr = recvMessageQueue_A();                          //阻塞等待测试执行完毕             
	if(strncmp(recvStr, "1-OK", 4) == 0)                     //遇"1-OK"结束
	      printf("Named test from otdrMain  sucessful!\n");
	else
	      printf("Don't have any messges from otdrMain!\n");
	free(recvStr);


/****************************回传点名测试测试数据文件***********************/
         RespondMessage_TestData(test_p,PS,SNo,intCM,FILE_MODE_NAMED);
	 OTDR_Destory(test_p);
      }
   return resp;
}
