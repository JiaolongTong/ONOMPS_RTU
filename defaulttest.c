#include "defaulttest.h"
#include "sql.h"
defaulttest * DefaultP_Create()
{
	defaulttest * me = (defaulttest *) malloc(sizeof(defaulttest));
        return me;
}
void DefaultP_Destory(defaulttest *me)
{
	free(me);	
}

void  getDefaultParameter(mxml_node_t *root,mxml_node_t *tree,defaulttest *defpar)
{
    mxml_node_t *AX,*SNo,*CM,*CLP,*SN,*P01,*P02,*P03,*P04,*P05,*P06,*P07;

    uint32_t  uint_a;
    float   float_a;
    char srttmp[3]="Ax";
    char i=0;
    uint32_t  intSN = 0;
    SN =mxmlFindElement(root, tree, "SN",NULL, NULL,MXML_DESCEND);
    intSN =  strtoul(SN->child->value.text.string, NULL, 0);
    defpar->SN = intSN;
    
    for (i=0;i<intSN;i++){ 
        srttmp[1]=i+0x31;
        AX  = mxmlFindElement(root, tree, srttmp ,NULL, NULL,MXML_DESCEND);      
        SNo = mxmlFindElement(AX, tree, "SNo",NULL, NULL,MXML_DESCEND);
        uint_a = strtoul (SNo->child->value.text.string, NULL, 0);          
        defpar->Group[i].SNo = uint_a;

	P01 = mxmlFindElement(AX, tree, "P01",NULL, NULL,MXML_DESCEND);
	uint_a = strtoul (P01->child->value.text.string, NULL, 0);  
	defpar->Group[i].paramter.Lambda_nm = uint_a;    
	P02 = mxmlFindElement(AX, tree, "P02",NULL, NULL,MXML_DESCEND);
	uint_a = strtoul (P02->child->value.text.string, NULL, 0); 
	defpar->Group[i].paramter.MeasureLength_m = uint_a;
	P03 = mxmlFindElement(AX, tree, "P03",NULL, NULL,MXML_DESCEND);
	uint_a = strtoul (P03->child->value.text.string, NULL, 0); 
	defpar->Group[i].paramter.PulseWidth_ns = uint_a;   
	P04 = mxmlFindElement(AX, tree, "P04",NULL, NULL,MXML_DESCEND);
	uint_a = strtoul (P04->child->value.text.string, NULL, 0); 
	defpar->Group[i].paramter.MeasureTime_ms = uint_a;  
	P05 = mxmlFindElement(AX, tree, "P05",NULL, NULL,MXML_DESCEND); 
	float_a =atof(P05->child->value.text.string);
	defpar->Group[i].paramter.n = float_a;   
	P06 = mxmlFindElement(AX, tree, "P06",NULL, NULL,MXML_DESCEND);
	float_a =atof(P06->child->value.text.string);
	defpar->Group[i].paramter.EndThreshold = float_a;    
	P07 = mxmlFindElement(AX, tree, "P07",NULL, NULL,MXML_DESCEND);
	float_a =atof(P07->child->value.text.string);
	defpar->Group[i].paramter.NonRelectThreshold = float_a; 

        if(defpar->Group[i].SNo == 0) break;  
 
    }
}
responed * setDefaultTestSegment(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)
{
   defaulttest *df_p;  
   df_p = DefaultP_Create();  
   char i=0;
   uint32_t  intSN = 0,rtuCM =0,rtuCLP=0;
   mxml_node_t *node,*perCMDcode,*CM,*CLP;
   responed *resp; 
   sqlite3 *mydb;
   char *zErrMsg = 0;
   int rc,ErrorSNo=0;
   sql *mysql;
   char * str, *strNo;
   char ** result=NULL;
   int ModuleNo=0,UseFlag=0,PortFlag=0,rednum;
   str    = (char *) malloc(sizeof(char)*1024);
   strNo  = (char *) malloc(sizeof(char)*10);
   mysql  = SQL_Create();
   resp   = Responed_Create();
   resp->RespondCode=0; 
   perCMDcode = mxmlFindElement(cmd, tree, "CMDcode",NULL, NULL,MXML_DESCEND);
   if(atoi(perCMDcode->child->value.text.string) !=cmdCode){
            resp->RespondCode=3;
	    resp->Group[0].Main_inform  = "指令号错误";
	    resp->Group[0].Error_inform = "Error:CMDcode Error[指令号错误]";
            return resp;   
            
   }else{

    CM =mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
    rtuCM = strtoul(CM->child->value.text.string, NULL, 0);
    
    CLP =mxmlFindElement(cmd, tree, "CLP",NULL, NULL,MXML_DESCEND);
    rtuCLP =  strtoul(CLP->child->value.text.string, NULL, 0);

    getDefaultParameter(cmd,tree,df_p);                       //解析XML 获取优化测试参数
  
    rc    = sqlite3_open("/web/cgi-bin/System.db", &mydb);
    if( rc != SQLITE_OK )
        printf( "Open SQL error\n");
    mysql->db =mydb;
   

    for(i=0 ;i<df_p->SN;i++){ 
       mysql->tableName ="SubModuleTypeTable";                 //检查子单元模块是否存在
       ModuleNo = ((df_p->Group[i].SNo-1)/8)+1;
       uint32tostring(ModuleNo,strNo);
       mysql->mainKeyValue = strNo;
       mysql->filedsName   = "UseFlag";
       SQL_lookupPar(mysql,&result,&rednum);
       UseFlag=atoi(result[0]);  
       mysql->tableName ="PortOccopyTable";                    //检查子端口是否占用
       uint32tostring(df_p->Group[i].SNo,strNo);
       mysql->mainKeyValue = strNo;
       PortFlag = SQL_existIN_db(mysql);    
       SQL_freeResult(&result,&rednum);
       if(UseFlag!=1){
          resp->RespondCode  = 3;
          resp->ErrorSN      = 1;                     
          resp->SNorPN       = TYPE_SNo;
          resp->Group[ErrorSNo].SNo =  df_p->Group[i].SNo;
          resp->Group[ErrorSNo].Main_inform  = "模块未激活";
          resp->Group[ErrorSNo].Error_inform = "Error: Don't have such Module\n";
          ErrorSNo++;                           
       }else if(PortFlag!=1){
          resp->RespondCode  = 3;
          resp->ErrorSN      = 1;                     
          resp->SNorPN       = TYPE_SNo;
          resp->Group[ErrorSNo].SNo =  df_p->Group[i].SNo;
          resp->Group[ErrorSNo].Main_inform  = "端口未配置";
          resp->Group[ErrorSNo].Error_inform = "Error: Don't have such Module\n";
          ErrorSNo++;                                                           
       }else{
          mysql->tableName = "DefaultTsetSegmentTable";	
	  sprintf(str,"%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f\n"        ,df_p->Group[i].SNo
				                                  ,rtuCM
				                                  ,rtuCLP
				                                  ,df_p->SN
				                                  ,df_p->Group[i].paramter.Lambda_nm
				                                  ,df_p->Group[i].paramter.MeasureLength_m
			                                          ,df_p->Group[i].paramter.PulseWidth_ns
		                                                  ,df_p->Group[i].paramter.MeasureTime_ms
		                                                  ,df_p->Group[i].paramter.n
			                                          ,df_p->Group[i].paramter.EndThreshold
				                                  ,df_p->Group[i].paramter.NonRelectThreshold);
           mysql->filedsValue =  str;
           rc=SQL_add(mysql);

           if( rc != SQLITE_OK )
	       printf( "Save default segment SQL error\n");
           else
	       printf("%s",str);
       }          
    } //end for

    for(i=0 ;i<intSN;i++)
    { 
	printf("------A%c--------\n",i+0x31);
	printf("A%c-SNo-uint -[%d]\n",i+0x31,df_p->Group[i].SNo);
	printf("A%c-P01-uint -[%d]\n",i+0x31,df_p->Group[i].paramter.Lambda_nm);
	printf("A%c-P02-uint -[%d]\n",i+0x31,df_p->Group[i].paramter.MeasureLength_m);
	printf("A%c-P03-uint -[%d]\n",i+0x31,df_p->Group[i].paramter.PulseWidth_ns);
	printf("A%c-P04-uint -[%d]\n",i+0x31,df_p->Group[i].paramter.MeasureTime_ms);
	printf("A%c-P05-float-[%f]\n",i+0x31,df_p->Group[i].paramter.n);
	printf("A%c-P06-float-[%f]\n",i+0x31,df_p->Group[i].paramter.EndThreshold);
	printf("A%c-P07-float-[%f]\n",i+0x31,df_p->Group[i].paramter.NonRelectThreshold);
	printf("\n");
    }
    
    free(str);
    free(strNo);
    SQL_Destory(mysql);  
    sqlite3_close(mydb);
    DefaultP_Destory(df_p);
    if(resp->RespondCode != 0 )
         resp->ErrorSN     =  ErrorSNo;                  //错误光路总条数
   }
    return resp;	
}
