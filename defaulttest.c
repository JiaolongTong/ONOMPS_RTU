#include "defaulttest.h"
#include "sql.h"
defaulttest * DefaultP_Create()
{
	defaulttest * me = (defaulttest *) malloc(sizeof(defaulttest));
        return me;
}
void DefaultP_Destory(defaulttest *me)
{
	free(me);	
}

void  getDefaultParameter(mxml_node_t *root,mxml_node_t *tree,defaulttest *defpar)
{
    mxml_node_t *AX,*SNo,*CM,*CLP,*SN,*P01,*P02,*P03,*P04,*P05,*P06,*P07;

    uint32_t  uint_a;
    float   float_a;
    char srttmp[3]="Ax";
    char i=0;
    uint32_t  intSN = 0,rtuCM =0,rtuCLP=0;
    SN =mxmlFindElement(root, tree, "SN",NULL, NULL,MXML_DESCEND);
    intSN =  strtoul(SN->child->value.text.string, NULL, 0);
    defpar->SN = intSN;
    
    CM =mxmlFindElement(root, tree, "CM",NULL, NULL,MXML_DESCEND);
    rtuCM =  strtoul(CM->child->value.text.string, NULL, 0);
    
    CLP =mxmlFindElement(root, tree, "CLP",NULL, NULL,MXML_DESCEND);
    rtuCLP =  strtoul(CLP->child->value.text.string, NULL, 0);
    
    for (i=0;i<intSN;i++)
    { 
	      srttmp[1]=i+0x31;
	      AX  = mxmlFindElement(root, tree, srttmp ,NULL, NULL,MXML_DESCEND);      
	      SNo = mxmlFindElement(AX, tree, "SNo",NULL, NULL,MXML_DESCEND);
	      uint_a = strtoul (SNo->child->value.text.string, NULL, 0);          
	      defpar->Group[i].SNo = uint_a;

	      P01 = mxmlFindElement(AX, tree, "P01",NULL, NULL,MXML_DESCEND);
	      uint_a = strtoul (P01->child->value.text.string, NULL, 0);  
	      defpar->Group[i].paramter.Lambda_nm = uint_a;    
	      P02 = mxmlFindElement(AX, tree, "P02",NULL, NULL,MXML_DESCEND);
	      uint_a = strtoul (P02->child->value.text.string, NULL, 0); 
	      defpar->Group[i].paramter.MeasureLength_m = uint_a;
	      P03 = mxmlFindElement(AX, tree, "P03",NULL, NULL,MXML_DESCEND);
	      uint_a = strtoul (P03->child->value.text.string, NULL, 0); 
	      defpar->Group[i].paramter.PulseWidth_ns = uint_a;   
	      P04 = mxmlFindElement(AX, tree, "P04",NULL, NULL,MXML_DESCEND);
	      uint_a = strtoul (P04->child->value.text.string, NULL, 0); 
	      defpar->Group[i].paramter.MeasureTime_ms = uint_a;  
	      P05 = mxmlFindElement(AX, tree, "P05",NULL, NULL,MXML_DESCEND); 
	      float_a =atof(P05->child->value.text.string);
	      defpar->Group[i].paramter.n = float_a;   
	      P06 = mxmlFindElement(AX, tree, "P06",NULL, NULL,MXML_DESCEND);
	      float_a =atof(P06->child->value.text.string);
	      defpar->Group[i].paramter.EndThreshold = float_a;    
	      P07 = mxmlFindElement(AX, tree, "P07",NULL, NULL,MXML_DESCEND);
	      float_a =atof(P07->child->value.text.string);
	      defpar->Group[i].paramter.NonRelectThreshold = float_a; 

              if(defpar->Group[i].SNo == 0) break;  
 
    }


	 sqlite3 *mydb;
	 char *zErrMsg = 0;
	 int rc;
	 sql *mysql;
         char * str;

         mysql = SQL_Create();
         str   = (char *) malloc(sizeof(char)*1024);
	 rc = sqlite3_open("/web/cgi-bin/System.db", &mydb);
	 mysql->db =mydb;
         mysql->tableName     = "DefaultTsetSegmentTable";
         intSN=i;
         for(i=0 ;i<intSN;i++)
            {                
		 sprintf(str,"%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f\n"   ,defpar->Group[i].SNo
		                                                    ,rtuCM
		                                                    ,rtuCLP
		                                                    ,defpar->SN
		                                                    ,defpar->Group[i].paramter.Lambda_nm
		                                                    ,defpar->Group[i].paramter.MeasureLength_m
		                                                    ,defpar->Group[i].paramter.PulseWidth_ns
		                                                    ,defpar->Group[i].paramter.MeasureTime_ms
		                                                    ,defpar->Group[i].paramter.n
		                                                    ,defpar->Group[i].paramter.EndThreshold
		                                                    ,defpar->Group[i].paramter.NonRelectThreshold);
		  mysql->filedsValue =  str;
		  rc = SQL_add(mysql);
                  printf("i=%d",i);
		  if( rc != SQLITE_OK ){
		      printf( "Save SQL error: %s\n", zErrMsg);
		      sqlite3_free(zErrMsg);
		      SQL_Destory(mysql);  
		      sqlite3_close(mydb);
                      return ;
		  }else
                  printf("%s",str);
                 
          }
       free(str);
/********************Test_readDB***************************
   char result[40*256];char *s;
   s = (char *) malloc(sizeof(char)*10);
   mysql->db            =  mydb;
   mysql->tableName     = "DefaultTsetSegmentTable";
   mysql->filedsName    = "*";
    printf("intSN:%d",intSN);
    for(i=0 ;i<intSN;i++)
    {    
	   uint32tostring(defpar->Group[i].SNo,s);
	   mysql->mainKeyValue  = s;
	   rc= SQL_lookup(mysql,result);
	  if( rc != SQLITE_OK ){
	      printf( "Lookup SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	   } 
          printf("\n-------Lookup:%d-----------\n%s",defpar->Group[i].SNo,result);
   }
	free(str);
	free(s);
*/
 SQL_Destory(mysql);  
 sqlite3_close(mydb);

    for(i=0 ;i<intSN;i++)
    { 
		printf("------A%c--------\n",i+0x31);
		printf("A%c-SNo-uint -[%d]\n",i+0x31,defpar->Group[i].SNo);
		printf("A%c-P01-uint -[%d]\n",i+0x31,defpar->Group[i].paramter.Lambda_nm);
		printf("A%c-P02-uint -[%d]\n",i+0x31,defpar->Group[i].paramter.MeasureLength_m);
		printf("A%c-P03-uint -[%d]\n",i+0x31,defpar->Group[i].paramter.PulseWidth_ns);
		printf("A%c-P04-uint -[%d]\n",i+0x31,defpar->Group[i].paramter.MeasureTime_ms);
		printf("A%c-P05-float-[%f]\n",i+0x31,defpar->Group[i].paramter.n);
		printf("A%c-P06-float-[%f]\n",i+0x31,defpar->Group[i].paramter.EndThreshold);
		printf("A%c-P07-float-[%f]\n",i+0x31,defpar->Group[i].paramter.NonRelectThreshold);
		printf("\n");
    }
    


}
int   setDefaultTestSegment(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)
{
      defaulttest *df_p;  
      df_p = DefaultP_Create();  
      char i=0;
      mxml_node_t *node,*perCMDcode;
      perCMDcode = mxmlFindElement(cmd, tree, "CMDcode",NULL, NULL,MXML_DESCEND);
      if(atoi(perCMDcode->child->value.text.string) !=cmdCode) 
       {
            printf("<RespondCode>3</RespondCode>\n");
	    printf("<Data>CMDcode Error [ %s:%s]</Data>\n",perCMDcode->value.element.name,perCMDcode->child->value.text.string);
            return -1;
            
       }
      else
      {
            getDefaultParameter(cmd,tree,df_p);
            DefaultP_Destory(df_p);
            return 0;
      }	
	
}
