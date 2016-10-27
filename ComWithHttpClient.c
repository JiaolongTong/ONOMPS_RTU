#include "Communicate.h"             //Add XML library     gcc -o xxx xxx.c -lmxml -lpthread
#include "namedtest.h"
#include "defaulttest.h"
#include "cycletest.h"
#include "alarmtest.h"
#include "opticalprotect.h"
#include "opticalpower.h"
#include "responed.h"
#include "process.h"
#include "myModbus.h"
#include "rtuIformation.h"

int sem_id=0;
int modbus_sem_id=0;
int otdr_sem_id=0;
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
          /*初始化信号量*/
          sem_id = semget((key_t)1234, 1, 4777 | IPC_CREAT);                                //创建数据库信号量 :每一个需要用到信号量的进程,在第一次启动的时候需要初始化信号量
          modbus_sem_id = semget((key_t)5678, 1, 4777 | IPC_CREAT);                         //创建ModBus信号量 :每一个需要用到信号量的进程,在第一次启动的时候需要初始化信号量
/*
         if(!set_semvalue())                                                               //程序第一次被调用，初始化信号量
          {  
            fprintf(stderr, "Failed to initialize semaphore\n");  
            exit(EXIT_FAILURE);  
          }  

          if(!setModbusPV())                                                                      //程序第一次被调用，初始化信号量
          {  
              fprintf(stderr, "Failed to initialize modbus_semaphore\n");  
              exit(EXIT_FAILURE); 
          } 

*/
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

/*                  if(code==180)                               //设备激活
                    {
                       ret=setOpticalProtectSegment(root,tree,code);
                       if(ret->RespondCode <0){
                          printf("</RespondMessage>");
                          mxmlDelete(tree);
                          exit(0);  
                        }
                        if (ret->RespondCode == 0)RespondMessage_OK(code);
                        else RespondMessage_Error(code,ret);
                        ree(ret);
                        break;
			}
                    }else {
                    
 */                     switch(code){


/*******************************************加入功能块begin*****************************************************/
/*回复消息*/
/**************************************************************
                Responed Code
0：成功处理命令 	1：设置测试参数非法
3: 命令无效	        2：设置门限参数非法
4：设置通信参数非法	5：成功处理命令，并等待数据 (ignore)
10：接收到测试数据文件	11：接收到RTU运行时间
12：接收到RTU网络参数	13：成功切换到保护光缆
14：数据库不同步 
**************************************************************/	
    		               case 100 :{   //设置优化参数                                   
                                    if (setDefaultTestSegment(root,tree,code)<0){
                                            printf("</RespondMessage>");
                                            mxmlDelete(tree);
			                    exit(0);   
                                        }
                                        RespondMessage_OK(code); 
                                        break;    
                                }   
                         
                               case 110 :{  //执行点名测试任务
                                     ret=setNamedTestSegment(root,tree,code);                                        
                                     if(ret->RespondCode <0){
                                        printf("</RespondMessage>");
                                        mxmlDelete(tree);
                                        exit(0);  
                                        }
                                     if (ret->RespondCode == 0);//RespondMessage_OK(code);
                                     else RespondMessage_Error(code,ret);
                                     free(ret);
                                     break; 
                                }    
           
                               case 120:{   //执行周期测试任务
                                    ret=setCycletestSegment(root,tree,code);
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

                               case 220:{   //结束周期测试任务  
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

                               case 130:{   //执行障碍告警测试任务     
                                    ret=setAlarmtestSegment(root,tree,code);
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

                               case 170:{   //设置保护光路组     
                                    ret=setOpticalProtectSegment(root,tree,code);
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



                               case 250:{   //取消光保护配对     
                                    ret=endOpticalProtectSegment(root,tree,code);
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
                               case 370:{   //请求光保护切换   
                                    ret=requestProtectSwitch(root,tree,code);
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

                               case 140:{   //设置告警信息     
                                    ret=setAlarmInformation(root,tree,code);
                                    if(ret->RespondCode <0){
                                        printf("</RespondMessage>");
                                        mxmlDelete(tree);
                                        exit(0);  
                                    }
                                    if (ret->RespondCode == 0)RespondMessage_OK(code);
                                    free(ret);
                                    break;
				    }


                               case 150:{   //设置基准时间     
                                    ret=setReferenceTime(root,tree,code);
                                    if(ret->RespondCode <0){
                                        printf("</RespondMessage>");
                                        mxmlDelete(tree);
                                        exit(0);  
                                    }
                                    if (ret->RespondCode == 0)RespondMessage_OK(code);
                                    free(ret);
                                    break;
				    }
                               case 320:{   //请求报告基准时间     
                                    ret=requestReferenceTime (root,tree,code);
                                    if(ret->RespondCode <0){
                                        printf("</RespondMessage>");
                                        mxmlDelete(tree);
                                        exit(0);  
                                    }
                                    if (ret->RespondCode == 0)RespondMessage_OK(code);
                                    free(ret);
                                    break;
				    }


                               case 160:{   //设置网络参数 
                                    ret=setNetwork(root,tree,code);
                                    if(ret->RespondCode <0){
                                        printf("</RespondMessage>");
                                        mxmlDelete(tree);
                                        exit(0);  
                                    }
                                    if (ret->RespondCode == 0)RespondMessage_OK(code);
                                    free(ret);
                                    break;
				    }

                               case 230:{   //取消障碍告警测试     
                                    ret=endAlarmtestSegment(root,tree,code);
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

                               case 240:{   //取消告警信息    
                                    ret=endAlarmInfo(root,tree,code);
                                    if(ret->RespondCode <0){
                                        printf("</RespondMessage>");
                                        mxmlDelete(tree);
                                        exit(0);  
                                    }
                                    if (ret->RespondCode == 0)RespondMessage_OK(code);
                                    free(ret);
                                    break;
				    }

                               case 360:{   //获取光功率值    
                                    ret=getOpticPowerParameter(root,tree,code);
                                    if(ret->RespondCode <0){
                                        printf("</RespondMessage>");
                                        mxmlDelete(tree);
                                        exit(0);  
                                    }
                                    if (ret->RespondCode == 0);//RespondMessage_OK(code);
                                    else RespondMessage_Error(code,ret);
                                    free(ret);
                                    break;
				    }

		              case  180:{
				    ret=setRTUMode(root,tree,code);
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
/******************************************加入功能块end****************************************************************/                                        
                               default  :  NullPossess();

                             }                                   
                    // }
            } 
          mxmlDelete(tree);		   
          printf("</RespondMessage>");
          //del_semvalue();
          //delModbusPV();
          exit(0);  
    }  










