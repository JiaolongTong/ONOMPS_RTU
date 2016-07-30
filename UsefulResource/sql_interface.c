

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
  char    str[200];
  int    SNo =1; 
  int    rtuCM=2;
  int    rtuCLP =3;
  int    rtuSN=4;
  int    P01 = 1441;
  int    P02 = 1442;
  int    P03 = 1443;
  int    P04 = 15;
  float  P05 = 1.465;
  float  P06 = 0.0; 
  float  P07 = 5.0; 
 sqlite3 *mydb;
 char *zErrMsg = 0;
 int rc;
 sql *mysql;
 mysql=SQL_Create();
 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
  mysql->db =mydb;
  mysql->tableName   = "DefaultTsetSegmentTable";

/*
  sprintf(str,"%d,%d,%d,%d,%d,%d,%d,%d,%6f,%6f,%6f\n",SNo,rtuCM,rtuCLP,rtuSN,P01,P02,P03,P04,P05,P06,P07);
  printf("%s",str);
  mysql->filedsValue = str;//"20,2,3,4,1440,60000,80,10,1.4685,0.0,5.0";

  rc = SQL_add(mysql);

  if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }
*/
   char **result;char *s;
   s = (char *) malloc(sizeof(char)*10);
   mysql->db            =  mydb;
   mysql->tableName     = "DefaultTsetSegmentTable";
   mysql->filedsName    = s; 
   int2string(SNo,s);
   mysql->mainKeyValue  = s;
   int sum=-1;

     sum =SQL_existIN_db(mysql);

    if(sum==0)
        printf("NULL\n");
    else
      printf("Exist\n");


   mysql->mainKeyValue  = "1";
   mysql->filedsName    = "*"; 
   rc= SQL_lookup(mysql,&result);
  if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }
  printf("%s\n",result[0]);
 SQL_Destory(mysql);  
 sqlite3_close(mydb);

}


