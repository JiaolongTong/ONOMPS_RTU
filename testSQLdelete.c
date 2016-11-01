#include "sql.c"
void main(void)
{
	 sqlite3 *mydb;
	 char *zErrMsg = 0,*SNo;
	 int rc,i=0;
	 sql *mysql;
         char resultModNo[64][5];
         int  resultModNum=0;

	 mysql = SQL_Create();
	 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
	 if( rc != SQLITE_OK )
            printf( "Lookup SQL error: %s\n");

	 mysql->db = mydb;
	 mysql->tableName   = "SubModuleTypeTable";	
         mysql->filedsValue =  "1";                                
         mysql->filedsName  =  "UseFlag";
        
         resultModNum=SQL_findModNo(mysql,resultModNo);
 
         mysql->filedsValue =  "0";
         if(!semaphore_p())
             exit(EXIT_FAILURE);                                //P
         for(i=0;i<resultModNum;i++){
            mysql->mainKeyValue  = resultModNo[i];
            SQL_modify(mysql);
         }

	mysql->mainKeyValue="1";
	mysql->tableName="DefaultTsetSegmentTable";
	SQL_delete(mysql);

	mysql->mainKeyValue="1";
	mysql->tableName="NamedTestSegmentTable";
	SQL_delete(mysql);

	mysql->mainKeyValue="1";
	mysql->tableName="CycleTestSegnemtTable";
	SQL_delete(mysql);

	mysql->mainKeyValue="1";
	mysql->tableName="AlarmTestSegmentTable";
	SQL_delete(mysql);

	mysql->mainKeyValue="1";
	mysql->tableName="PortOccopyTable";
	SQL_delete(mysql);

	mysql->mainKeyValue="1";
	mysql->tableName="ProtectGroupTable";
	SQL_delete(mysql);


         if(!semaphore_v())  
            exit(EXIT_FAILURE);                                //V
}

