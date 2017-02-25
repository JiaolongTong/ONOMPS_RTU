#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h> 

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

int main(int argc, char* argv[])
{
   sqlite3 *db;
   char *zErrMsg = 0;
   int  rc;
   char *sql;

   /* Open database */
   rc = sqlite3_open("System.db", &db);
   if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      exit(0);
   }else{
      fprintf(stdout, "Opened database successfully\n");
   }


//  create table ProtectGroupTable(PNo INT PRIMARY KEY,SNoA INT,SNoB INT,SwitchPos INT,sPNo INT,sSNoA INT,sSNoB INT,sSwitchPos INT,ConnectPos INT,sModNo INT,sIP VARCHAR,Status INT );
   /* Create SQL statement */
   sql = "CREATE TABLE ProtectGroupTable("  \
         "PNo INT PRIMARY KEY   ," \
         "SNoA     INT     ," \
         "SNoB    INT     ," \
         "SwitchPos      INT     ," \

         "sPNo      INT     ," \
         "sSNoA     INT     ," \
         "sSNoB    INT     ," \
         "sSwitchPos      INT     ," \

         "ConnectPos      INT     ," \
         "sModNo     INT     ," \
         "sIP    VARCHAR     ," \
         "Status    INT     );" \
;

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
   fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      fprintf(stdout, "Table created successfully\n");
   }
   sqlite3_close(db);
   return 0;
}


/*                                优化参数表
Name	Declared Type	Type	Size	Precision	Not Null	Not Null On Conflict	Default Value	Collate	Position	Old Position
SNo	INT	INT	0	0	False	""	0	""	0	0
rtuCM	INT	INT	0	0	False	""	0	""	1	1
rtuCLP	INT	INT	0	0	False	""	0	""	2	2
rtuSN	INT	INT	0	0	False	""	0	""	3	3
P01	INT	INT	0	0	False	""	""	""	4	4
P02	INT	INT	0	0	False	""	""	""	5	5
P03	INT	INT	0	0	False	""	""	""	6	6
P04	INT	INT	0	0	False	""	""	""	7	7
P05	FLOAT	FLOAT	0	0	False	""	""	""	8	8
P06	FLOAT	FLOAT	0	0	False	""	""	""	9	9
P07	FLOAT	FLOAT	0	0	False	""	""	""	10	10
*/

/*                               点名测试表
SNo
PS
P11
P12
P13
P14
P15
P16
P17
*/


/*                               周期测试表
Name	Declared Type	Type	Size	Precision	Not Null	Not Null On Conflict	Default Value	Collate	Position	Old Position
SNo	INT(3)	INT	3	0	False	""	""	""	0	0
rtuCM	INT(20)	INT	20	0	False	""	""	""	1	1
rtuCLP	INT(20)	INT	20	0	False	""	""	""	2	2
rtuSN	INT	INT	0	0	False	""	""	""	3	3
T1	VARCHAR(20)	VARCHAR	20	0	False	""	""	""	4	4
T2	VARCHAR(10)	VARCHAR	10	0	False	""	""	""	5	5
IP01	VARCHAR	VARCHAR	0	0	False	""	""	""	6	6
IP02	VARCHAR	VARCHAR	0	0	False	""	""	""	7	7
IP03	VARCHAR	VARCHAR	0	0	False	""	""	""	8	8
IP04	VARCHAR	VARCHAR	0	0	False	""	""	""	9	9
IP05	VARCHAR	VARCHAR	0	0	False	""	""	""	10	10
IP06	VARCHAR	VARCHAR	0	0	False	""	""	""	11	11
Status	BOOL	BOOL	0	0	False	""	""	""	12	12
PID	INT	INT	0	0	False	""	""	""	13	13
*/
