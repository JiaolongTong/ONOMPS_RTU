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

/*SubModuleTypeTable*/
/*0 1 2 3 4 5 */
enum SubModuleTypeTable {eSMT_ModuleNo,eSMT_rtuCM,eSMT_rtuCLP,eSMT_ModuleType,eSMT_ComAddr,eSMT_UseFlag};  
/*SlaveProtectTable*/
/*0 1 2 3 4 5 6 7 8 9 10*/
enum SlaveProtectTable  {eSPT_PNo,eSPT_ModuleNo,eSPT_SNoA,eSPT_SNoB,eSPT_GateA,eSPT_GateB,eSPT_SwitchPos,eSPT_ConnectPos,eSPT_belongCM,eSPT_belongCLP,eSPT_Status};
/*ProtectGroupTable*/
enum ProtectGroupTable  {ePGT_PNo,ePGT_SNoA,ePGT_SNoB,ePGT_SwitchPos,ePGT_sPNo,ePGT_sSNoA,ePGT_sSNoB,ePGT_sSwitchPos,ePGT_ConnectPos,ePGT_sModNo,ePGT_sIP,ePGT_Status};
/*PortOccopyTable*/
enum PortOccopyTable    {ePOT_SNo,ePOT_ModuleNo,ePOT_SubPort,ePOT_FiberType};
/*NamedTestSegmentTable*/
enum NamedTestSegmentTable {eNTT_SNo,eNTT_PS,eNTT_P11,eNTT_P12,eNTT_P13,eNTT_P14,eNTT_P15,eNTT_P16,eNTT_P17,eNTT_masterPID};
/*DefaultTsetSegmentTable*/
enum DefaultTsetSegmentTable {eDTT_SNo,eDTT_rtuCM,eDTT_rtuCLP,eDTT_rtuSN,eDTT_P01,eDTT_P02,eDTT_P03,eDTT_P04,eDTT_P05,eDTT_P06,eDTT_P07};
/*CycleTestSegnemtTable*/
enum CycleTestSegnemtTable  {eCTT_SNo,eCTT_rtuCM,eCTT_rtuCLP,eCTT_rtuSN,eCTT_T1,eCTT_T2,eCTT_IP01,eCTT_IP02,eCTT_IP03,eCTT_IP04,eCTT_IP05,eCTT_IP06,eCTT_Status,eCTT_PID};
/*AlarmTestSegmentTable*/
enum AlarmTestSegmentTable {eATT_SNo,eATT_rtuCM,eATT_rtuCLP,eATT_Level,eATT_PS,
                            eATT_P21,eATT_P22,eATT_P23,eATT_P24,eATT_P25,eATT_P26,eATT_P27,
                            eATT_AT01,eATT_AT02,eATT_AT03,eATT_AT04,eATT_AT05,eATT_AT06,
                            eATT_IP01,eATT_IP02,eATT_IP03,eATT_IP04,eATT_IP05,eATT_IP06,
                            eATT_T3,eATT_T4,eATT_fiberType,eATT_protectFlag,eATT_Status};
typedef struct sql
{      
        sqlite3 *db;
        char    * tableName;                  
	char    * filedsName;                             
	char    * filedsValue;               
        char    * mainKeyValue;               
}sql;

sql *SQL_Create();
void SQL_Destory(sql *me);

char * getMainKeyname(sql* const me);
char * getFieldsName(sql *const me);

int  SQL_add(sql * const me);
int  SQL_lookup(sql * const me,char ***result);
int  SQL_lookupPar(sql* const me,char ***result,int *rednum );
void SQL_freeResult(char ***result,int * rednum);
int  SQL_findSNo(sql * const me,char result[][5]);
int  SQL_findPNo(sql * const me,char result[][5]);
int  SQL_findModNo(sql * const me,char result[][5]);
int  SQL_delete(sql * const me);
int  SQL_clearTable(sql *const me);
int  SQL_modify(sql * const me);
int  SQL_existIN_db(sql * const me);
int  SQL_Unique(sql * const me);

int deleteSNoRecord(char * tableName,int mainKeyValue, int masterPID);

#endif
