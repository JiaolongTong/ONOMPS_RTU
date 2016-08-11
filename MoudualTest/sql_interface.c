

#include "sql.h"

void int2string(int lNum,char chWord[])

{
   int i=0,j;
   char chTemp;
   while(lNum!=0)                      /*依次取整数的末位，存入chWord */
   {
     chWord[i]='0'+lNum%10;           /*转换为数字的ASCII码*/
     i++;
     lNum=lNum/10;  
   }  
   chWord[i]='\0';                    /*字符串最后一位加'\0'*/   
   for(j=0;j<i/2;j++)                  /*将字符串转置*/
   {
   chTemp=chWord[j];
   chWord[j]=chWord[i-1-j];
   chWord[i-1-j]=chTemp;
   }
}
int main(int argc, char* argv[])
{

  char   str[200];
  int    SNo =2; 
  int    rtuCM=2;
  int    rtuCLP =3;
  int    rtuSN=4;
  char * beginTime ="20160716132132";
  char * period    ="000002";
  char * IP01 = "192.168.0.110";
  char * IP02 = "127.0.0.1";
  char * IP03 = "127.0.0.1";
  char * IP04 = "127.0.0.1";
  char * IP05 = "127.0.0.1";
  char * IP06 = "127.0.0.1"; 
  int    Action = 1;
  int    PID    = 1; 
 sqlite3 *mydb;
 char *zErrMsg = 0;
 int rc;
 sql *mysql;
 mysql=SQL_Create();
 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);

/*

 // sprintf(str,"%d,%d,%d,%d,%d,%d,%d,%d,%6f,%6f,%6f\n",SNo,rtuCM,rtuCLP,rtuSN,P01,P02,P03,P04,P05,P06,P07);
                           //SNo CM CLP SN bg pd 1 2  3  4  5  6  Ac PID
		 sprintf(str,"%d,%d,%d,%d,'%s','%s','%s','%s','%s','%s','%s','%s',%d,%d\n",SNo
		                                                          ,rtuCM
		                                                          ,rtuCLP
		                                                          ,rtuSN
		                                                          ,beginTime
		                                                          ,period
		                                                          ,IP01
		                                                          ,IP02
		                                                          ,IP03
		                                                          ,IP04
		                                                          ,IP05
                                                                          ,IP06
		                                                          ,Action
                                                                          ,PID);

  mysql->db =mydb;
  mysql->tableName   = "DefaultTsetSegmentTable";
  mysql->s = str;                                  //"20,2,3,4,1440,60000,80,10,1.4685,0.0,5.0";
  rc = SQL_add(mysql);
  if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }
   char result[64][5];int i,SN=0;
   char *s = (char *) malloc(sizeof(char)*10);
   mysql->db            =  mydb;
   mysql->tableName     = "CycleTestSegnemtTable";
   mysql->filedsName    = "P01"; 
   int2string(1,s);
   mysql->mainKeyValue  = s;

   rc= SQL_lookup(mysql,result);
   printf("%s\n",result);

  if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }
*/
         char result[40*64];
         mysql->db          =  mydb;
	 mysql->tableName   = "DefaultTsetSegmentTable";	
	 mysql->mainKeyValue  ="1";
         mysql->filedsName    =  "P01";
         rc=SQL_lookup(mysql,result);
         printf("P01:%s\n",result);

  if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }

 SQL_Destory(mysql);  
 sqlite3_close(mydb);

}


