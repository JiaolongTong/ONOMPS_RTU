#include "Communicate.h"             //Add XML library     gcc -o xxx xxx.c -lmxml -lpthread
#include "namedtest.h"
#include "defaulttest.h"
#include "cycletest.h"
#include "responed.h"
#include "process.h"
int sem_id=0;
int main(void)  
    {  
	  char *pRequestMethod;
          char *urlline;
          char *postlength;
	  int i,fd,ContentLength;
          char InputBuffer[4096*100];
          char *getbuf,x;  
          int  code;
          FILE *fp;  

          mxml_node_t *tree,*root,*command;

          responed *ret=NULL;
          sem_id = semget((key_t)1234, 1, 4777 | IPC_CREAT);                                //创建信号量 
          if(!set_semvalue())                                                               //程序第一次被调用，初始化信号量
          {  
            fprintf(stderr, "Failed to initialize semaphore\n");  
            exit(EXIT_FAILURE);  
          } 

          printf("Content-type:text/xml\n\n");                                              //This is very important 
          printf("<?xml version='1.0' encoding='UTF-8'?>\n"); 
          printf("<RespondMessage>\n");
          pRequestMethod = getenv("REQUEST_METHOD");    
          if(pRequestMethod==NULL)
           {   
               printf("<Error>Get Request_method Error</Error>");   
               printf("</RespondMessage>");  
               return   0;    
           } 
	  else
          {
               ;//printf("<Method>%s</Method>\n",pRequestMethod);
          }
/******************************************DoGet*******************************************************/
          if (strcmp(pRequestMethod,"GET")==0)   
          {    
                urlline = getenv("QUERY_STRING");                                            //get From data from QUERY_STRING
                      if   (urlline!=NULL)   {                        
                              strncpy(InputBuffer,urlline,sizeof(InputBuffer));    
                               printf("<GetData> %s </GetData>\n",urlline);
                               //saveRecvXML(InputBuffer,strlen(InputBuffer));          //Save data     
                      }
                     else
                      {
			     printf("<Test> QUERY_STRING Empty,Please Check! </Test>\n"); 
                             printf("</RespondMessage>");
                             return   0;  
                      }
/*****************************************DoPost******************************************************/
          } else if (strcmp(pRequestMethod,"POST")==0)   {    
                      postlength=getenv("CONTENT_LENGTH");                                         //Get Length from env-value  

                      if(postlength!=NULL){    
                              ContentLength=atoi(postlength);   
                      }
                      else{    
                              ContentLength = 0;  
                              printf("<RespondCode>3</RespondCode>\n");
                              printf("<Error>Lack of local.xml file!</Error>\n");
                      }    
                      if(ContentLength>sizeof(InputBuffer)-1)   {    
                              ContentLength   =   sizeof(InputBuffer)-1;    
                      }    
                      i=0;    
                      while(i<ContentLength){                                                  //get Form data from stdin
                              x=fgetc(stdin);    
                              if(x==EOF)break;    
                              InputBuffer[i++]   =  x;    
                      }    
                      InputBuffer[i]  =  '\0';    
                      ContentLength   =   i;    
                      if(-1==saveRecvXML(InputBuffer,ContentLength))
                      {
                           printf("<Error>Can't save post data!</Error>\n");
                           printf("</RespondMessage>");
                           exit(0);     
                      }                                                                        //Save data  to file
                      fp = fopen(RCV_FILE, "r");
	              if(fp == NULL){
                    printf("<RespondCode>3</RespondCode>\n");
			        printf("<Error>open the recv.xml error!</Error>\n");
                    printf("</RespondMessage>");
			        exit(0);
					}
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
		    else{
                    code=getCommendCode(root,tree);
                    switch(code){


/*******************************************加入功能块begin*****************************************************/
 			        case 100 :  { //设置优化参数                                   
                                    if (setDefaultTestSegment(root,tree,code)<0){
                                            printf("</RespondMessage>");
                                            mxmlDelete(tree);
			                    exit(0);   
                                        }
                                        RespondMessage_OK(code); 
                                        break;    
                                }                            
                                case 110 :  { //执行点名测试任务
                                     ret=setNamedTestSegment(root,tree,code);                                        
                                     if(ret->RespondCode <0){
                                        printf("</RespondMessage>");
                                        mxmlDelete(tree);
                                        exit(0);  
                                        }
                                     if (ret->RespondCode == 0)RespondMessage_OK(code);
                                     else RespondMessage_Error(code,ret);
                                     free(ret);
                                     break; 
                                }               
                               case 120:{    // 执行周期测试任务
      
                                    ret=setCycletestSegment(root,tree,code);
                                    if(ret->RespondCode <0){
                                        printf("</RespondMessage>");
                                        mxmlDelete(tree);
                                        exit(0);  
                                        }
                                          /*回复消息*/
                                          /*
	                                       (1) "0"   指令处理成功
                                               (2) "1"   测试参数非法: SN,P01,P02,P03,P04,P05,P06,P07
                                               (3) "4"   通信参数非法: 起始时间、时间间隔格式错误,回传IP地址错误
                                               (4) "14"  数据库不同步: 设置时间表时，检查优化参数是否存在，如果不存在报告数据库不同步错误，并回传未同步的光路号(SNo)。
                                          */
                                    if (ret->RespondCode == 0)RespondMessage_OK(code);
                                    else RespondMessage_Error(code,ret);
                                    free(ret);
                                    break;
				    }
                               case 220:{     // 结束周期测试任务
      
                                    ret=endCycletestSegment(root,tree,code);
                                    if(ret->RespondCode <0){
                                        printf("</RespondMessage>");
                                        mxmlDelete(tree);
                                        exit(0);  
                                        }
                                    if (ret->RespondCode == 0)RespondMessage_OK(code);
                                    free(ret);
                                    break;
				    }


/******************************************加入功能块end****************************************************************/                                        
                               default  :  NullPossess();
                             }                                   

            } 
          mxmlDelete(tree);		   
          printf("</RespondMessage>");
          exit(0);  
    }  










