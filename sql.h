#ifndef SQL_H
#define SQL_H
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include <sys/time.h>  
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct sql
{      
        sqlite3 *db;
        char    * tableName;                  // lookup + add
	char    * filedsName;                 // lookup                  
	char    * filedsValue;                //          add
        char    * mainKeyValue;               // lookup
}sql;

sql *SQL_Create();
void SQL_Destory(sql *me);

char * getMainKeyname(sql* const me);
char * getFieldsName(sql *const me);

int  SQL_add(sql * const me);
int  SQL_lookup(sql * const me,char ***result);
int  SQL_findSNo(sql * const me,char result[][5]);
int  SQL_findPNo(sql * const me,char result[][5]);
int  SQL_delete(sql * const me);
int  SQL_modify(sql * const me);
int  SQL_existIN_db(sql * const me);
int  SQL_Unique(sql * const me);

#endif
