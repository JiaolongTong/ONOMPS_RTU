
 
#include "defaulttest.h"
#include "cycletest.h"

int main(int argc, char* argv[])
{
  mxml_node_t *tree,*root,*command;
          FILE *fp;
  fp = fopen(RCV_FILE, "r");
  if(fp == NULL){
       printf("<RespondCode>3</RespondCode>\n");
       printf("<Error>open the recv.xml error!</Error>\n");
       printf("</RespondMessage>");
       return -1;}

   tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);

   if(tree == NULL){
        printf("<RespondCode>3</RespondCode>\n");
	printf("<Data>Load XML file error!</Data>\n");
        fclose(fp);
	return -1;}

   fclose(fp);
   root = mxmlFindElement(tree, tree, "SegmentCode",NULL, NULL,MXML_DESCEND);
        if(root == NULL){
		     printf("<RespondCode>3</RespondCode>\n");
		     printf("<Data>Undefined Code :%s</Data>\n",root->value.element.name);
		     printf("</RespondMessage>");
		     mxmlDelete(tree);
		     return -1;}
  setCycletestSegment(root,tree,120);
  mxmlDelete(tree);

  uint32_t uint_a;
  
  uint_a = strtoul ("*", NULL, 0); 
  printf("\nuint_a=%d\n",uint_a);


  return 0;
}


