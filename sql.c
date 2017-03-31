#include "sql.h"
#include "common.h"
sql *SQL_Create()
{
	sql * me= (sql*)malloc(sizeof(sql));
	return me;
}
void SQL_Destory(sql *me)
{
	free(me);
}

char * getMainKeyname(sql* const me)
{
	char * out;
	if((0==strcmp(me->tableName,"DefaultTsetSegmentTable"))||(0==strcmp(me->tableName,"NamedTestSegmentTable")) 
           ||(0==strcmp(me->tableName,"AlarmTestSegmentTable"))||(0==strcmp(me->tableName,"CycleTestSegnemtTable")) 
           ||(0==strcmp(me->tableName,"PortOccopyTable")))
          return  out = "SNo";
        else if(0==strcmp(me->tableName,"ProtectGroupTable"))
          return  out = "PNo";
        else if (0==strcmp(me->tableName,"SubModuleTypeTable") || 0==strcmp(me->tableName,"SlaveProtectTable"))
          return out ="ModuleNo";
        else
          return  out ="rtuCM";
}

char * getFieldsName(sql *const me)
{
	char * out;
        if(0==strcmp(me->tableName,"NamedTestSegmentTable"))
             return  out = "(SNo,PS,P11,P12,P13,P14,P15,P16,P17,masterPID)";
	if(0==strcmp(me->tableName,"DefaultTsetSegmentTable"))
             return  out = "(SNo,rtuCM,rtuCLP,rtuSN,P01,P02,P03,P04,P05,P06,P07)";
	if(0==strcmp(me->tableName,"CycleTestSegnemtTable"))
             return  out = "(SNo,rtuCM,rtuCLP,rtuSN,T1,T2,IP01,IP02,IP03,IP04,IP05,IP06,Status,PID)";
	if(0==strcmp(me->tableName,"AlarmTestSegmentTable"))
             return  out = "(SNo,rtuCM,rtuCLP,Level,PS,P21,P22,P23,P24,P25,P26,P27,AT01,AT02,AT03,AT04,AT05,AT06,IP01,IP02,IP03,IP04,IP05,IP06,T3,T4,fiberType,protectFlag,Status)";
        if(0==strcmp(me->tableName,"ProtectGroupTable"))
             return  out = "(PNo,SNoA,SNoB,SwitchPos,sPNo,sSNoA,sSNoB,sSwitchPos,ConnectPos,sModNo,sIP,Status)";
        if(0==strcmp(me->tableName,"PortOccopyTable"))
             return  out = "(SNo,ModuleNo,SubPort,FiberType)";
        if(0==strcmp(me->tableName,"SubModuleTypeTable"))
             return  out = "(ModuleNo,rtuCM,rtuCLP,ModuleType,ComAddr,UseFlag)";
        if(0==strcmp(me->tableName,"SlaveProtectTable"))
             return  out = "(PNo,ModuleNo,SNoA,SNoB,GateA,GateB,SwitchPos,ConnectPos,belongCM,belongCLP,Status)";
}


char **lookup =NULL;
int   recordnum;

static int callback(void *NotUsed, int argc, char **argv, char **azColName){

        recordnum = argc;
        int j = 0;
	unsigned int length,len = 0;

	char **p_first = NULL;


	if(argv == NULL || argc == 0 )
	{
		return -1;
	}


	//分配内存
	p_first = (char**)malloc(sizeof(char*)*argc);
	if(p_first != NULL)
	{
		for(j=0; j<argc; j++)
		{
                        p_first[j] = (char*)malloc(sizeof(char)*strlen(argv[j])+1);
                        memset(p_first[j], 0, strlen(argv[j])+1); 
        
		}	 
	}

        //将argv中的数据缓存到分配好的内存中.

	for(j=0; j<argc; j++)
	{
		strncpy(p_first[j], argv[j],strlen(argv[j])+1);
  
	}

	//将而极指针传给全局而极指针，供主程序调用.
	lookup = p_first;
                          
        

   return 0;
}

int  SQL_lookupPar(sql* const me,char ***result,int *rednum)
{
      char *zErrMsg = 0;
      int rc;
      char sql_s[1024];  
      const char* dat= "Callback function called";
      char *mainKeyName = getMainKeyname(me); 
      //if( SQL_existIN_db(me) ){
	      if("*"==me->filedsName)
		 sprintf(sql_s,"SELECT * from %s where %s=%s;",me->tableName,mainKeyName,me->mainKeyValue);
	      else
		 sprintf(sql_s,"SELECT %s from %s where %s=%s;",me->filedsName,me->tableName,mainKeyName,me->mainKeyValue);
	      rc = sqlite3_exec(me->db, sql_s,callback, (void*)dat, &zErrMsg);
	      if( rc != SQLITE_OK ){
		//fprintf(stderr, "SQL error: %s\n", zErrMsg);
		 printf("SQL error: %s\n", zErrMsg); 
		 sqlite3_free(zErrMsg);
		 return rc;
	      }
	      *result = lookup;
	      *rednum = recordnum;
      //}else rc=-1;
      return rc;

}

void SQL_freeResult(char ***result,int * rednum)
{
       int i=0;
       char ** rest=*result ;
       if(rest!= NULL)
	   {
               for(i =0 ;i<*rednum;i++){
	         if(rest[i] != NULL)
	            {
		      free(rest[i]);
		      rest[i] = NULL;
		    }

		  free(rest);
		  *result = NULL;
               }
	   }                    
}
/*
每当执行完一次搜索，调用一次回调函数，同时输出结果。若搜索结果是多条记录，则需要使用全局计数器来保存数据。
通过字段名+值搜索光路号
注意与SQL_lookup过程区别: 多条记录一个字段
*/
char  SNo[64][5];
int   SN=0;
static int callbackSNo(void *NotUsed, int argc, char **argv, char **azColName){
      sprintf(SNo[SN],"%s\n",argv[0] ? argv[0] : "NULL");
      SN++;
   return 0;
}
int  SQL_findSNo(sql * const me,char result[][5])                    
{
      char *zErrMsg = 0;
      int rc;
      char sql_s[1024];  
      sprintf(sql_s,"SELECT SNo from %s where %s=%s;",me->tableName,me->filedsName,me->filedsValue);
      rc = sqlite3_exec(me->db, sql_s,callbackSNo,NULL, &zErrMsg);
      if( rc != SQLITE_OK ){
        printf("SQL error: %s\n", zErrMsg); 
        sqlite3_free(zErrMsg);
        return rc;
       }else{
           int i;
           for (i= 0 ;i<SN ;i++){
               strcpy(result[i],SNo[i]);            
           }
         i=SN;
         SN=0;
         return i;
     }
}

char  PNo[64][5];
int   PN=0;
static int callbackPNo(void *NotUsed, int argc, char **argv, char **azColName){
      sprintf(PNo[PN],"%s",argv[0] ? argv[0] : "NULL");
      PN++;
   return 0;
}
int  SQL_findPNo(sql * const me,char result[][5])                    
{
      char *zErrMsg = 0;
      int rc;
      char sql_s[1024];  
      sprintf(sql_s,"SELECT PNo from %s where %s=%s;",me->tableName,me->filedsName,me->filedsValue);
      rc = sqlite3_exec(me->db, sql_s,callbackPNo,NULL, &zErrMsg);
      if( rc != SQLITE_OK ){
        printf("SQL error: %s\n", zErrMsg); 
        sqlite3_free(zErrMsg);
        return rc;
       }else{
           int i;
           for (i= 0 ;i<PN ;i++){
               strcpy(result[i],PNo[i]);            
           }
         i=PN;
         PN=0;
         return i;
     }
}


char  ModNo[64][5];
int   ModNum=0;
static int callbackModNo(void *NotUsed, int argc, char **argv, char **azColName){
      sprintf(ModNo[ModNum],"%s",argv[0] ? argv[0] : "NULL");
      ModNum++;
   return 0;
}
int  SQL_findModNo(sql * const me,char result[][5])                 
{
      char *zErrMsg = 0;
      int rc;
      char sql_s[1024];  
      sprintf(sql_s,"SELECT ModuleNo from %s where %s=%s;",me->tableName,me->filedsName,me->filedsValue);
      rc = sqlite3_exec(me->db, sql_s,callbackModNo,NULL, &zErrMsg);
      if( rc != SQLITE_OK ){
        printf("SQL error: %s\n", zErrMsg); 
        sqlite3_free(zErrMsg);
        return rc;
       }else{

         int i;
         for (i= 0 ;i<ModNum ;i++){
               strcpy(result[i],ModNo[i]);            
           }
         i=ModNum;
         ModNum=0;
         return i;

     }
}
/*以整体的方式插入一条记录
SQL语句:INSERT INTO  CycleTestSegnemtTable(SNo,rtuCM,rtuCLP,rtuSN,T1,T2,IP01,IP02,IP03,IP04,IP05,IP06,Status,PID) values (1,2,3,4,5,6,7,8,9,10,11,12,13,14);
需要设置的结构体成员:  
   me-> tableName;            //需要操作的表
   me-> filedsValue;          //需要插入的字段值，按字段名顺序排列. 格式:1,2,3,4,5,6,7,8,9,10,11,12,13,14
*/
int  SQL_add(sql * const me)
{
      char *zErrMsg = 0;
      char *p1,*p2;
      int rc,i;
      char sql_s[1024]; 
      char mainKeyValue[20]; 
      char *Fileds=getFieldsName(me);
      char *mainKeyName = getMainKeyname(me); 
      sprintf(sql_s,"INSERT INTO %s%s values (%s);",me->tableName,Fileds,me->filedsValue);
      if((rc = sqlite3_exec(me->db,sql_s,NULL,0, &zErrMsg)) != SQLITE_OK ){  
	      i=0;               
	      while(me->filedsValue[i] !=',')
	      {
		mainKeyValue[i] = me->filedsValue[i];
		i++;
	      }
	      mainKeyValue[i] ='\0';
	      int  flagA=Search_Keyword(zErrMsg,"UNIQUE");
	      int  flagB=Search_Keyword(zErrMsg,"unique");
	      if(flagA!=0 || flagB!=0){
		  sprintf(sql_s,"DELETE from %s where %s=%s;",me->tableName,mainKeyName,mainKeyValue);
		  rc= sqlite3_exec(me->db, sql_s, callback, 0, &zErrMsg);
		   if( rc != SQLITE_OK ){
			 printf("SQL delete: %s\n", zErrMsg); 
			 sqlite3_free(zErrMsg);
			 return rc;
		    } 
		    sprintf(sql_s,"INSERT INTO %s%s values (%s);",me->tableName,Fileds,me->filedsValue);

		    rc= sqlite3_exec(me->db, sql_s, callback, 0, &zErrMsg);
		    if( rc != SQLITE_OK ){
			 printf("SQL insert: %s\n", zErrMsg); 
			 sqlite3_free(zErrMsg);
			 return rc;
		    } 
		    return rc;
		   }
		 else
		    return rc;
      }else
         return rc;
     free(sql_s);	
}




/*删除一条记录
SQL语句:DELETE from CycleTestSegnemtTable where SNo=1;
需要设置的结构体成员:  
   me-> tableName;            //需要操作的表
   me-> mainKeyValue;         //需要删除的记录主键值（光路号SNo或者RTU编号CM） (* delete all)
*/
int  SQL_delete(sql * const me)
{
      char *zErrMsg = 0;
      int rc;
      char sql_s[1024];  
      char *mainKeyName = getMainKeyname(me);

      sprintf(sql_s,"DELETE from %s where %s=%s;",me->tableName,mainKeyName,me->mainKeyValue);
      rc= sqlite3_exec(me->db, sql_s, NULL, 0, &zErrMsg);
      if( rc != SQLITE_OK ){
	printf("SQL delete: %s\n", zErrMsg); 
	sqlite3_free(zErrMsg);
	return rc;
      } 
      return rc;
}

int SQL_clearTable(sql *const me)
{

    char * zErrMsg=0;
    int rc;
    char sql_s[1024]; 
    sprintf(sql_s,"DELETE from %s;",me->tableName);
    rc= sqlite3_exec(me->db, sql_s, NULL, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
		 printf("SQL clear: %s\n", zErrMsg); 
		 sqlite3_free(zErrMsg);
		 return rc;
    }
    return rc;
  
}
/*修改某个字段值
SQL语句:UPDATE CycleTestSegnemtTable set Status=1 where SNo=1;
需要设置的结构体成员:  
   me-> tableName;            //需要操作的表
   me-> filedsName;           //需要修改的字段名
   me-> filedsValue;          //需要修改的字段值
   me-> mainKeyValue;         //需要修改的记录的光路号或者RTU编号   （主键值）
*/
int SQL_modify(sql * const me)
{
     char *zErrMsg = 0;
     int rc;
     char sql_s[1024]; 
     char *mainKeyName = getMainKeyname(me);
     sprintf(sql_s,"UPDATE %s set %s=%s where %s=%s;",me->tableName,me->filedsName,me->filedsValue,mainKeyName,me->mainKeyValue); 
     //printf("UPDATE %s set %s=%s where %s=%s;",me->tableName,me->filedsName,me->filedsValue,mainKeyName,me->mainKeyValue);
     rc= sqlite3_exec(me->db, sql_s, NULL, 0, &zErrMsg);
     if( rc != SQLITE_OK ){
         printf("SQL error: %s\n", zErrMsg); 
         sqlite3_free(zErrMsg);
         return rc;
    }
     return rc; 
}

int SQL_existIN_db(sql * const me)
{
    char sql_query[128]={0};
    char *mainKeyName = getMainKeyname(me);
    sprintf(sql_query,"select %s from %s where %s='%s'",mainKeyName,me->tableName,mainKeyName,me->mainKeyValue);
    sqlite3_stmt *pstmt;
    sqlite3_prepare(me->db, sql_query, strlen(sql_query), &pstmt, NULL);
    sqlite3_step(pstmt);
    int count=sqlite3_column_int(pstmt,0);
    sqlite3_finalize(pstmt);
     
    if(count > 0)
        return 1;                //exist
 
    return 0;                    //don't
}

int SQL_Unique(sql * const me)
{
    char sql_query[128]={0};
    char *mainKeyName = getMainKeyname(me);
    sprintf(sql_query,"select %s from %s where %s='%s'",mainKeyName,me->tableName,me->filedsName,me->filedsValue);
    sqlite3_stmt *pstmt;
    sqlite3_prepare(me->db, sql_query, strlen(sql_query), &pstmt, NULL);
    sqlite3_step(pstmt);
    int count=sqlite3_column_int(pstmt,0);
    sqlite3_finalize(pstmt);
    if(count > 0)
        return 1;                //No unique
    return 0;                    //Unique
}




