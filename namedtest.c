#include "namedtest.h"

namedtest * NamedP_Create()
{
    namedtest * me = (namedtest *) malloc(sizeof(namedtest));
    return me;
}
void NamedP_Destory(namedtest *me)
{
    free(me);
}

void  getNamedParameter(mxml_node_t *root,mxml_node_t *tree, namedtest *namedpar)
{
	    mxml_node_t *SN,*PS,*SNo,*P01,*P02,*P03,*P04,*P05,*P06,*P07;

	    uint32_t  uint_a;
	    float     float_a;
 	    PS = mxmlFindElement(root, tree,  "PS",NULL, NULL,MXML_DESCEND);
	    uint_a = strtoul (PS->child->value.text.string, NULL, 0);
	    namedpar->PS = uint_a;  

	    SNo = mxmlFindElement(root, tree, "SNo",NULL, NULL,MXML_DESCEND);
	    uint_a = strtoul (SNo->child->value.text.string, NULL, 0);
	    namedpar->SNo = uint_a;

	    P01 = mxmlFindElement(root, tree, "P11",NULL, NULL,MXML_DESCEND);
	    uint_a = strtoul (P01->child->value.text.string, NULL, 0);  
	    namedpar->paramter.MeasureLength_m = uint_a;

 
	    P02 = mxmlFindElement(root, tree, "P12",NULL, NULL,MXML_DESCEND);
	    uint_a = strtoul (P02->child->value.text.string, NULL, 0); 
	    namedpar->paramter.PulseWidth_ns = uint_a;    

	    P03 = mxmlFindElement(root, tree, "P13",NULL, NULL,MXML_DESCEND);
	    uint_a = strtoul (P03->child->value.text.string, NULL, 0); 
	    namedpar->paramter.Lambda_nm = uint_a;
  
	    P04 = mxmlFindElement(root, tree, "P14",NULL, NULL,MXML_DESCEND);
	    uint_a = strtoul (P04->child->value.text.string, NULL, 0); 
	    namedpar->paramter.MeasureTime_ms = uint_a;  

	    P05 = mxmlFindElement(root, tree, "P15",NULL, NULL,MXML_DESCEND); 
	    float_a =atof(P05->child->value.text.string);
	    namedpar->paramter.n = float_a;   

	    P06 = mxmlFindElement(root, tree, "P16",NULL, NULL,MXML_DESCEND);
	    float_a =atof(P06->child->value.text.string);
	    namedpar->paramter.NonRelectThreshold = float_a; 	          

	    P07 = mxmlFindElement(root, tree, "P17",NULL, NULL,MXML_DESCEND);
	    float_a =atof(P07->child->value.text.string);
            namedpar->paramter.EndThreshold = float_a;

}
int SetNamedTestSegment(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)
{
	
   mxml_node_t *node,*perCMDcode;
   namedtest * nmdpar;
   otdr * test_p;
   test_p = OTDR_Create();
   nmdpar = NamedP_Create();
   int PS,SNo;        
   perCMDcode = mxmlFindElement(cmd, tree, "CMDcode",NULL, NULL,MXML_DESCEND);
      if(atoi(perCMDcode->child->value.text.string) !=cmdCode) 
       {
            printf("<RespondCode>3</RespondCode>\n");
	    printf("<Data>CMDcode Error [ %s:%s]</Data>\n",perCMDcode->value.element.name,perCMDcode->child->value.text.string);
            return -1;
            
       }
      else
      {
              getNamedParameter(cmd,tree,nmdpar);

            printf("-----------------------\n");
              printf("PS -uint -[%d]\n",nmdpar->PS);
	      printf("SNo-uint -[%d]\n",nmdpar->SNo);
	      printf("P11-uint -[%d]\n",nmdpar->paramter.MeasureLength_m);
	      printf("P12-uint -[%d]\n",nmdpar->paramter.PulseWidth_ns);
	      printf("P13-uint -[%d]\n",nmdpar->paramter.Lambda_nm);
	      printf("P14-uint -[%d]\n",nmdpar->paramter.MeasureTime_ms);
	      printf("P15-float-[%f]\n",nmdpar->paramter.n);
	      printf("P16-float-[%f]\n",nmdpar->paramter.NonRelectThreshold);
	      printf("P17-float-[%f]\n",nmdpar->paramter.EndThreshold);
              printf("-----------------------\n");

	     PS=                         nmdpar->PS;
	     SNo =                       nmdpar->SNo;
	     test_p->Lambda_nm =         nmdpar->paramter.Lambda_nm;
	     test_p->MeasureLength_m=    nmdpar->paramter.MeasureLength_m;
	     test_p->PulseWidth_ns=      nmdpar->paramter.PulseWidth_ns;
	     test_p->MeasureTime_ms=     nmdpar->paramter.MeasureTime_ms;   
	     test_p->n=                  nmdpar->paramter.n;
	     test_p->EndThreshold=       nmdpar->paramter.EndThreshold;
	     test_p->NonRelectThreshold= nmdpar->paramter.NonRelectThreshold;

   
/*
		test_p->Lambda_nm =1550;
		test_p->MeasureLength_m=60000; 
		test_p->PulseWidth_ns=80;
		test_p->MeasureTime_ms=10;
		test_p->n=1.4685;
		test_p->NonRelectThreshold=0;
		test_p->EndThreshold=5.0;
*/
	 OtdrTest(test_p);
         RespondMessage_TestData(test_p,PS,SNo,FILE_MODE_NAMED);
	 OTDR_Destory(test_p);
         NamedP_Destory(nmdpar);
        
      }
    return 0;
}
