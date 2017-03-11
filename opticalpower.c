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
           int rc,ModuleNo=0,ErrorSNo =0,Portflag=-1,UseFlag=0,ModType=0,subPort=0,rednum=0,ModNo=0,tmp=0,SNoA=0,SNoB=0; 
           char **result = NULL;
           char *strMNo,*strSNo,*strPNo;
           char slaverIP[16];
           modbus_t * mb=NULL;
           sqlite3 *mydb;
           sql *mysql;

           strSNo   = (char *) malloc(sizeof(char)*10);
           strMNo   = (char *) malloc(sizeof(char)*10);
           strPNo   = (char *) malloc(sizeof(char)*10);
           mysql = SQL_Create();
           rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
           if( rc != SQLITE_OK ){
	      printf( "Open SQL error\n");
	   }
           mysql->db =mydb;

           mysql->tableName ="SubModuleTypeTable";                 //检查子单元模块是否存在
           ModuleNo = ((power->SNo-1)/8)+1;
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
               if(ModType==2 || ModType==4){
		  if(!setModbus_P())                                //P
		      exit(EXIT_FAILURE);
		  modbus_t * mb=newModbus(MODBUS_DEV,MODBUS_BUAD);
		  powerValue = getOneOpticalValue(mb,power->SNo,ModType); 
		  freeModbus(mb);                     
		  power->value=powerValue;   
		  RespondMessage_OpticPowerData(power, 0);      
		  if(!setModbus_V())                                //V
		      exit(EXIT_FAILURE);                   
               }else if(ModType==3){
                  subPort = (power->SNo-1)%8+1 ;
                  if( (subPort==1) || (subPort==5) || (subPort==3) || (subPort==7) ){
                          
		          if(!setModbus_P())                        //P
			      exit(EXIT_FAILURE);   
			  mb=newModbus(MODBUS_DEV,MODBUS_BUAD);
			  powerValue = getOneOpticalValue(mb,power->SNo,ModType); 
			  freeModbus(mb);                     
			  power->value=powerValue;   
			  RespondMessage_OpticPowerData(power, 0);   
			  if(!setModbus_V())                        //V
			      exit(EXIT_FAILURE);  

                  }else if( (subPort==2) || (subPort==6) || (subPort==4) || (subPort==8) ){
                       
                       mysql->tableName ="ProtectGroupTable";  
                         
                       if((power->SNo-1)%8+1 ==2 || (power->SNo-1)%8+1 ==6) sprintf(strPNo,"%d",(power->SNo-1)/8 +1);
                       if((power->SNo-1)%8+1 ==4 || (power->SNo-1)%8+1 ==8) sprintf(strPNo,"%d",(power->SNo-1)/8 +3);
 
		       mysql->mainKeyValue = strPNo;
                       if(SQL_existIN_db(mysql)){
			       mysql->filedsName   = "*";
			       SQL_lookupPar(mysql,&result,&rednum);
                               SNoA = atoi(result[5]);
                               SNoB = atoi(result[6]);
                               ModNo= atoi(result[9]);
                               strcpy(slaverIP,result[10]);                                  
			       //SQL_freeResult(&result,&rednum);                                                        
                               if(SNoA>SNoB){
                                       tmp=SNoA ;
                                       SNoA=SNoB; 
                                       SNoB=tmp;
                               } 
                               slaverModuleInformatin * slaverModule=NULL;
                               slaverModule=newSlaverModule();
                               slaverModule->ModNo=ModNo;
                               mb=newModBus_TCP_Client(slaverIP);
                              
                               if(mb!=NULL){
					    if(getSlaverModuleInformation(mb,slaverModule)<0){
						    resp->RespondCode  =ERROR_CODE_13_PROTECT_FAILED;
						    if(resp->SNorPN!=TYPE_SNo){
							resp->ErrorSN      = ERROR_CODE_13_PROTECT_FAILED;                     
							resp->SNorPN       = TYPE_SNo;
							resp->Group[ErrorSNo].SNo =  power->SNo;
							resp->Group[ErrorSNo].Main_inform  = "从端模块操作失败";
							resp->Group[ErrorSNo].Error_inform = "Error: Slaver Switch Failed.[从端模块操作失败]\n";
							ErrorSNo++;
						    }   
					    }else{
	printf("SNoA:%f  SNoB:%f \n ",(float)(uint16toint16(slaverModule->detail.powerValueA)/100.0),(float)(uint16toint16(slaverModule->detail.powerValueB)/100.0));
				                     if(power->SNo == SNoA){
				                               power->value=(float)(uint16toint16(slaverModule->detail.powerValueA)/100.0);
				                               RespondMessage_OpticPowerData(power, 0);
				                     }else if(power->SNo == SNoB){
				                               power->value=(float)(uint16toint16(slaverModule->detail.powerValueB)/100.0);
				                               RespondMessage_OpticPowerData(power, 0);
				                     }else{
							    resp->RespondCode  =ERROR_CODE_13_PROTECT_FAILED;
							    if(resp->SNorPN!=TYPE_SNo){
								resp->ErrorSN      = ERROR_CODE_13_PROTECT_FAILED;                     
								resp->SNorPN       = TYPE_SNo;
								resp->Group[ErrorSNo].SNo = power->SNo;;
								resp->Group[ErrorSNo].Main_inform  = "从端模块操作失败";
								resp->Group[ErrorSNo].Error_inform = "Error: Slaver Switch Failed.[从端模块操作失败]\n";
								ErrorSNo++;
							    } 
				                     }                                        
					    }
                               }else{
				    resp->RespondCode  =ERROR_CODE_13_PROTECT_FAILED;
				    if(resp->SNorPN!=TYPE_SNo){
					resp->ErrorSN      = ERROR_CODE_13_PROTECT_FAILED;                     
					resp->SNorPN       = TYPE_SNo;
					resp->Group[ErrorSNo].SNo =  power->SNo;
					resp->Group[ErrorSNo].Main_inform  = "从端模块操作失败";
					resp->Group[ErrorSNo].Error_inform = "Error: Slaver Switch Failed.[从端模块操作失败]\n";
					ErrorSNo++;
			            } 
                               }
                              freeModbus_TCP_Client(mb);

                       }else{
                              resp->RespondCode = 3;                    // 参数错误
                              resp->ErrorSN     = 1;  
			      resp->SNorPN      = TYPE_SNo;
		              resp->Group[0].SNo = power->SNo; 
		              resp->Group[0].Main_inform  = "未设置保护组 不可采集光功率";
		              resp->Group[0].Error_inform = "Error: Can't get Optical Values no protect group -->Error  [未设置保护组 不可采集光功率]";
			      return  resp; 
                       }



                  }else{
                              resp->RespondCode = 3;                    // 参数错误
                              resp->ErrorSN     = 1;  
			      resp->SNorPN      = TYPE_SNo;
		              resp->Group[0].SNo = power->SNo; 
		              resp->Group[0].Main_inform  = "光路号不支持 不可采集光功率";
		              resp->Group[0].Error_inform = "Error: Can't get Optical Values no protect group -->Error  [未设置保护组 不可采集光功率]";
			      return  resp; 
                  }
              }else{
                          resp->RespondCode = 3;                    // 参数错误
                          resp->ErrorSN     = 1;  
			  resp->SNorPN      = TYPE_SNo;
		          resp->Group[0].SNo = power->SNo; 
		          resp->Group[0].Main_inform  = "[模块类型为不可采光功率]";
		          resp->Group[0].Error_inform = "Error: Can't get Optical Values -->Error Module Type! [模块类型为不可采光功率]";
			  return  resp;                     
              }
           }
        
      }
      return resp;
}


