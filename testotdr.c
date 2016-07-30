#include "defaulttest.h"
#include "common.h"
int main(void)
{
          int  code=100;
          FILE *fp;  
          mxml_node_t *tree,*root,*command;
          fp = fopen("recv.xml", "r");
	  if(fp == NULL){
              printf("<RespondCode>3</RespondCode>\n");
	      printf("<Error>open the recv.xml error!</Error>\n");
              printf("</RespondMessage>");
	      exit(0);
	  }
	      tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
	     if(tree == NULL){
                printf("<RespondCode>3</RespondCode>\n");
		printf("<Data>Load XML file error!</Data>\n");
                fclose(fp);
		exit(0);
		}
    	      fclose(fp);
 	      root = mxmlFindElement(tree, tree, "SegmentCode",NULL, NULL,MXML_DESCEND);
              if(root == NULL){
		        printf("<RespondCode>3</RespondCode>\n");
			printf("<Data>Undefined Code :%s</Data>\n",root->value.element.name);
		        printf("</RespondMessage>");
		        mxmlDelete(tree);
			    exit(0);
			}
                if (setDefaultTestSegment(root,tree,code)<0){
                         printf("</RespondMessage>");
                         mxmlDelete(tree);
			 exit(0);   
                    }else{
			    printf("<OK>OK</OK>");
			  }
mxmlDelete(tree);
}
