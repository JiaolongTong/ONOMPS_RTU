#include "sql.h"

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
	if((0==strcmp(me->tableName,"DefaultTsetSegmentTable"))||(0==strcmp(me->tableName,"AlarmSegmentTestTable")) ||(0==strcmp(me->tableName,"CycleTestSegnemtTable"))
            ||(0==strcmp(me->tableName,"OTDRTestDataTable")))
          return  out = "SNo";
        else
          return  out ="rtuCM";
}


char * getFieldsName(sql *const me)
{
	char * out;
	if(0==strcmp(me->tableName,"DefaultTsetSegmentTable"))
             return  out = "(SNo,rtuCM,rtuCLP,rtuSN,P01,P02,P03,P04,P05,P06,P07)";
	if(0==strcmp(me->tableName,"CycleTestSegnemtTable"))
             return  out = "(SNo,rtuCM,rtuCLP,rtuSN,T1,T2,IP01,IP02,IP03,IP04,IP05,IP06,Status,PID)";
}
/*周期测试表CycleTestSegnemtTable

字段名:SNo	rtuCM	rtuCLP	rtuSN	T1	T2	IP01	IP02	IP03	IP04	IP05	IP06	Status	PID	
                                 
说明:(1)Status :   -2    -1        0       1      2
                 待取消 待启动   不操作  已启动  已取消

     (2)PID : 采用有序链表调度策略不考虑PID
*/





/*
通过光路号查询记录，结果输出该记录下对应的所有字段的值。这些值以字符串数组的方式存放在二维数组中.
argc保持了一共输出了多少字段内容。
注意与SQL_findSNo过程区别: 一条记录多个字段
*/
//char  lookup[40][256];
char **lookup =NULL;

int   recordnum;
static int callback(void *NotUsed, int argc, char **argv, char **azColName){

   recordnum = argc;
   int j = 0;
/*
   for(j=0; j<recordnum; j++){
      sprintf(lookup[j],"%s",argv[j] ? argv[j] : "NULL");
   }
*/

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
int  SQL_lookup(sql * const me,char ***result)
{
      char *zErrMsg = 0;
      int rc;
      char sql_s[1024];  
      const char* dat= "Callback function called";
      char *mainKeyName = getMainKeyname(me); 

      if("*"==me->filedsName)
         sprintf(sql_s,"SELECT * from %s where %s=%s;",me->tableName,mainKeyName,me->mainKeyValue);
      else
         sprintf(sql_s,"SELECT %s from %s where %s=%s;",me->filedsName,me->tableName,mainKeyName,me->mainKeyValue);

 
       rc = sqlite3_exec(me->db, sql_s,callback, (void*)dat, &zErrMsg);
       if( rc != SQLITE_OK ){
        // fprintf(stderr, "SQL error: %s\n", zErrMsg);
         printf("SQL error: %s\n", zErrMsg); 
         sqlite3_free(zErrMsg);
         return rc;
        }else{

           *result = lookup;

/*
            int i;
            char *s =(char *) malloc(sizeof(char)*1024);     //[1024];
            // *s='\0';
            for (i= 0 ;i<recordnum ;i++){
		 strcat(s,lookup[i]);            
            }

            strcpy(result,s);

            for (i= 0 ;i<recordnum ;i++){
		free(lookup[i]);        
            }

            free(s);
            free(lookup);
*/
            return rc;
      }
           lookup = NULL;
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
      //const char* dat= "Callback function called";
       sprintf(sql_s,"SELECT SNo from %s where %s=%s;",me->tableName,me->filedsName,me->filedsValue);
       rc = sqlite3_exec(me->db, sql_s,callbackSNo,NULL, &zErrMsg);
       if( rc != SQLITE_OK ){
        // fprintf(stderr, "SQL error: %s\n", zErrMsg);
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





/*以整体的方式插入一条记录
SQL语句:INSERT INTO  CycleTestSegnemtTable(SNo,rtuCM,rtuCLP,rtuSN,T1,T2,IP01,IP02,IP03,IP04,IP05,IP06,Status,PID) values (1,2,3,4,5,6,7,8,9,10,11,12,13,14);
需要设置的结构体成员:  
   me-> tableName;            //需要操作的表
   me-> filedsValue;          //需要插入的字段值，按字段名顺序排列. 格式:1,2,3,4,5,6,7,8,9,10,11,12,13,14
*/

int  SQL_add(sql * const me)
{
      char *zErrMsg = 0;
      int rc;
      char sql_s[1024]; 
      char mainKeyValue[20]; 
      char *Fileds=getFieldsName(me);
      char *mainKeyName = getMainKeyname(me); 
      sprintf(sql_s,"INSERT INTO %s%s values (%s);",me->tableName,Fileds,me->filedsValue);
      printf("%s\n",sql_s);
      if((rc = sqlite3_exec(me->db,sql_s,callback,0, &zErrMsg)) != SQLITE_OK ){  
      //   printf("%s\n",zErrMsg);                   
         int i ;
         char unique[7];
         for (i=0;i<6;i++)
	      unique[i] =zErrMsg[i];
         unique[i] ='\0';
         i=0;
         while(me->filedsValue[i] !=',')
              {
		mainKeyValue[i] = me->filedsValue[i];
		i++;
	      }
         mainKeyValue[i] ='\0';
         //printf("%s",mainKeyValue);
         if(!strcmp(unique,"UNIQUE"))
	   {
            	sprintf(sql_s,"DELETE from %s where %s=%s;",me->tableName,mainKeyName,mainKeyValue);
               // printf("%s",sql_s);
                rc= sqlite3_exec(me->db, sql_s, callback, 0, &zErrMsg);
                sprintf(sql_s,"INSERT INTO %s%s values (%s);",me->tableName,Fileds,me->filedsValue);
               // printf("%s",sql_s);
                rc= sqlite3_exec(me->db, sql_s, callback, 0, &zErrMsg);
                return rc;
           }
         else
                return rc;
      }else{
         return rc;
      }
     free(sql_s);
	
}




/*删除一条记录
SQL语句:DELETE from CycleTestSegnemtTable where SNo=1;
需要设置的结构体成员:  
   me-> tableName;            //需要操作的表
   me-> mainKeyValue;         //需要删除的记录主键值（光路号SNo或者RTU编号CM）
*/
int  SQL_delete(sql * const me)
{
      char *zErrMsg = 0;
      int rc;
      char sql_s[1024];  
      char *mainKeyName = getMainKeyname(me);
      sprintf(sql_s,"DELETE from %s where %s=%s;",me->tableName,mainKeyName,me->mainKeyValue);
      rc= sqlite3_exec(me->db, sql_s, callback, 0, &zErrMsg);
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
     printf("%s\n",sql_s);   
     rc= sqlite3_exec(me->db, sql_s, callback, 0, &zErrMsg);
     if( rc != SQLITE_OK ){
        // fprintf(stderr, "SQL error: %s\n", zErrMsg);
         printf("SQL error: %s\n", zErrMsg); 
         sqlite3_free(zErrMsg);
         return rc;
    }
     return rc; 
}

int SQL_existIN_db(sql * const me)
{
    char sql_query[128]={0};
    sprintf(sql_query,"select SNo from %s where SNo='%s'",me->tableName,me->mainKeyValue);
    sqlite3_stmt *pstmt;
    sqlite3_prepare(me->db, sql_query, strlen(sql_query), &pstmt, NULL);
    sqlite3_step(pstmt);
    int count=sqlite3_column_int(pstmt,0);
    sqlite3_finalize(pstmt);
     
    if(count > 0)
        return 1;                //exist
 
    return 0;                    //don't
}



