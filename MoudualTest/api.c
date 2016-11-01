responed * setNetwork(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)
{
   mxml_node_t    *CM,*CLP,*perCMDcode;
   rtuInform *rtuReferenceTime;
   responed *resp; 
   char IP[16],Netmask[16];
  
   rtuReferenceTime = RTU_Create();   
   resp   = Responed_Create();
   resp  -> RespondCode=0;  
      
   perCMDcode = mxmlFindElement(cmd, tree, "CMDcode",NULL, NULL,MXML_DESCEND);
      if(atoi(perCMDcode->child->value.text.string) !=cmdCode) {
            printf("<RespondCode>3</RespondCode>\n");
	    printf("<Data>CMDcode Error [ %s:%s]</Data>\n",perCMDcode->value.element.name,perCMDcode->child->value.text.string);
            resp->RespondCode=-1;
            return resp;   
       }
      else{
/**************************解析XML消息***************************************/
	    CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	    rtuReferenceTime->rtuCM = strtoul(CM->child->value.text.string, NULL, 0); 	
	    CLP = mxmlFindElement(cmd, tree, "CLP",NULL, NULL,MXML_DESCEND);
            rtuReferenceTime->rtuCLP= strtoul(CLP->child->value.text.string, NULL, 0);
	    IP = mxmlFindElement(cmd, tree, "IP",NULL, NULL,MXML_DESCEND);
	    Netmask = mxmlFindElement(cmd, tree, "Netmask",NULL, NULL,MXML_DESCEND);
            set_ip(IP->child->value.text.string);
            set_ip_mask(Netmask->child->value.text.string);

	
       }  
   RTU_Destory(rtuReferenceTime);
   return resp;   

}
responed * requestNetwork(mxml_node_t *cmd,mxml_node_t *tree,int cmdCode)
{
   mxml_node_t    *CM,*CLP,*perCMDcode;
   rtuInform *rtuReferenceTime;
   responed *resp; 
   char IP[16],Netmask[16];
  
   rtuReferenceTime = RTU_Create();   
   resp   = Responed_Create();
   resp  -> RespondCode=0;  
      
   perCMDcode = mxmlFindElement(cmd, tree, "CMDcode",NULL, NULL,MXML_DESCEND);
      if(atoi(perCMDcode->child->value.text.string) !=cmdCode) {
            printf("<RespondCode>3</RespondCode>\n");
	    printf("<Data>CMDcode Error [ %s:%s]</Data>\n",perCMDcode->value.element.name,perCMDcode->child->value.text.string);
            resp->RespondCode=-1;
            return resp;   
       }
      else{
/**************************解析XML消息***************************************/
	    CM = mxmlFindElement(cmd, tree, "CM",NULL, NULL,MXML_DESCEND);
	    rtuReferenceTime->rtuCM = strtoul(CM->child->value.text.string, NULL, 0); 	
	    CLP = mxmlFindElement(cmd, tree, "CLP",NULL, NULL,MXML_DESCEND);
            rtuReferenceTime->rtuCLP= strtoul(CLP->child->value.text.string, NULL, 0);

            get_ip(IP);
            get_ip_mask(Netmask);

 	    printf("<RespondCode>12</RespondCode>\n");
            printf("<Data>\n");
            printf("<CMDcode>513</CMDcode>\n");
            printf("	<R>*</R>\n");
            printf("	<CM>%d</CM>\n",rtuReferenceTime->rtuCM);
            printf("	<CLP>%d</CLP>\n",rtuReferenceTime->rtuCLP);
            printf("	<IP>%s</IP>\n" ,IP);
            printf("	<Netmask>%s</Netmask>\n",Netmask);
            printf("	<Gateway>192.168.0.1</Gateway>");
            printf("</Data>\n");	
       }  
   RTU_Destory(rtuReferenceTime);
   return resp;   
}
