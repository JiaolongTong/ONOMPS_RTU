#include "opticalpower.h"
#include "process.h"
#include "Communicate.h"
#include "myModbus.h"
#include "sql.h"
opticalpower * OpticPower_Create()
{
    opticalpower * me = (opticalpower *) malloc(sizeof(opticalpower));
    return me;
}
void OpticPower_Destory(opticalpower *me)
{
    free(me);
}
responed * getOpticPowerParameter(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)
{
   mxml_node_t *CM,*perCMDcode,*SNo;
   opticalpower * power;
   responed *resp; 
   int intCM;
   int uint_a; 
   float powerValue=-100.0;

   power = OpticPower_Create();   
   resp  = Responed_Create();

   resp->RespondCode=0;        
   perCMDcode = mxmlFindElement(cmd, tree, "CMDcode",NULL, NULL,MXML_DESCEND);
   if(atoi(perCMDcode->child->value.text.string) !=cmdCode) {
            resp->RespondCode=3;
	    resp->Group[0].Main_inform  = "[指令号错误]";
	    resp->Group[0].Error_inform = "Error:CMDcode Error[指令号错误]";
            return resp;  
            
    }else{
/**************************解析XML消息***************************************/
            SNo = mxmlFindElement(cmd, tree, "SNo",NULL, NULL,MXML_DESCEND);
	    uint_a = strtoul (SNo->child->value.text.string, NULL, 0);
	    power->SNo = uint_a;
            CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	    uint_a = strtoul (CM->child->value.text.string, NULL, 0);
            power->CM   =uint_a;


/*****************************************************************************/
           int rc,ModuleNo=0,ErrorSNo =0,Portflag=-1,UseFlag=0,ModType=0,fiberType=0,rednum=0; 
           char **result = NULL;
           char *strMNo,*strSNo;
           sqlite3 *mydb;
           sql *mysql;

           strSNo   = (char *) malloc(sizeof(char)*10);
           strMNo   = (char *) malloc(sizeof(char)*10);
  
           mysql = SQL_Create();
           rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
           if( rc != SQLITE_OK ){
	      printf( "Open SQL error\n");
	   }
           mysql->db =mydb;

           mysql->tableName ="SubModuleTypeTable";                 //检查子单元模块是否存在
           ModuleNo = ((power->SNo-1)/8)+1;
           printf("ModuleNo:%d\n",ModuleNo);
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
           if(UseFlag!=1)
           {
                  resp->RespondCode  = 3;
                  resp->ErrorSN      = 1;                     
                  resp->SNorPN       = TYPE_SNo;
                  resp->Group[0].SNo = power->SNo;
                  resp->Group[0].Main_inform  = "[模块未激活]";
		  resp->Group[0].Error_inform = "Error: Don't have such Module\n";
		  return  resp;                                   
           }
           

           mysql->tableName ="PortOccopyTable";                      //检查子端口是否占用
	   uint32tostring(power->SNo,strSNo);
	   mysql->mainKeyValue = strSNo;
           Portflag = SQL_existIN_db(mysql); 
           if(Portflag!=1)
           {
                  resp->RespondCode  = 3;
                  resp->ErrorSN      = 1;                     
                  resp->SNorPN       = TYPE_SNo;
                  resp->Group[0].SNo = power->SNo;
                  resp->Group[0].Main_inform  = "[端口未配置]";
		  resp->Group[0].Error_inform = "Error: Don't have such Port\n";
		  return  resp;                                  
           } 

/*****************************************************************************/
           if(UseFlag==1 && Portflag==1){
               printf("ModType:%d",ModType);
               if(ModType==2 || ModType==4){
		  if(!setModbus_P())                                //P
		      exit(EXIT_FAILURE);   
		  modbus_t * mb=newModbus(MODBUS_DEV,MODBUS_BUAD);
		  powerValue = getOneOpticalValue(mb,power->SNo); 
		  freeModbus(mb);                     
		  power->value=powerValue;   
		  RespondMessage_OpticPowerData(power, 0);      
		  if(!setModbus_V())                                //V
		      exit(EXIT_FAILURE);  
                 
              }else if(ModType==3){
                  mysql->tableName    = "PortOccopyTable";
                  mysql->mainKeyValue = strSNo;
                  mysql->filedsName   = "fiberType";
                  SQL_lookupPar(mysql,&result,&rednum);
                  fiberType=atoi(result[0])-1; 
                  SQL_freeResult(&result,&rednum);
                  if(fiberType==0){
		          if(!setModbus_P())                        //P
			      exit(EXIT_FAILURE);   
			  modbus_t * mb=newModbus(MODBUS_DEV,MODBUS_BUAD);
			  powerValue = getOneOpticalValue(mb,power->SNo); 
			  freeModbus(mb);                     
			  power->value=powerValue;   
			  RespondMessage_OpticPowerData(power, 0);      
			  if(!setModbus_V())                        //V
			      exit(EXIT_FAILURE);  
                  }else{
                          resp->RespondCode = 3;                    // 参数错误
                          resp->ErrorSN     = 1;  
			  resp->SNorPN      = TYPE_SNo;
		          resp->Group[0].SNo = power->SNo; 
		          resp->Group[0].Main_inform  = "[光纤类型为保护模式的在纤]";
		          resp->Group[0].Error_inform = "Error: Can't get Optical Values -->Error SNo [光纤类型为保护模式的在纤]";
			  return  resp; 
                  }
              }else{
                          resp->RespondCode = 3;                    // 参数错误
                          resp->ErrorSN     = 1;  
			  resp->SNorPN      = TYPE_SNo;
		          resp->Group[0].SNo = power->SNo; 
		          resp->Group[0].Main_inform  = "[光纤类型为不可采光功率的在纤]";
		          resp->Group[0].Error_inform = "Error: Can't get Optical Values -->Error SNo [光纤类型为不可采光功率的在纤]";
			  return  resp;                     
              }
           }
         return resp;
      }
}


