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
int main(int argc,char **argv)  
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

          sem_id = semget((key_t)1234, 1, 4777 | IPC_CREAT);                                //创建数据库信号量 :每一个需要用到信号量的进程,在第一次启动的时候需要初始化信号量
          modbus_sem_id = semget((key_t)5678, 1, 4777 | IPC_CREAT);                         //创建ModBus信号量 :每一个需要用到信号量的进程,在第一次启动的时候需要初始化信号量



          printf("Content-type:text/xml\n\n");                                              //This is very important 
          printf("<?xml version='1.0' encoding='UTF-8'?>\n"); 
          printf("<RespondMessage>\n"); 
                                                                                        //Save data  to file
          fp = fopen(RCV_FILE, "r");
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
		    else{
                    //code=getCommendCode(root,tree);
                    code =atoi(argv[1]);
                    switch(code){

/*******************************************加入功能块begin*****************************************************/
/*回复消息*/
/*
	(1) "0"   指令处理成功
        (2) "1"   测试参数非法: SN,P01,P02,P03,P04,P05,P06,P07
        (3) "4"   通信参数非法: 起始时间、时间间隔格式错误,回传IP地址错误
        (4) "14"  数据库不同步: 设置时间表时，检查优化参数是否存在，如果不存在报告数据库不同步错误，并回传未同步的光路号(SNo)。
*/     		               case 100 :{   //设置优化参数                                   
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
                                    //if (ret->RespondCode == 0)RespondMessage_OK(code);
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


		              case  180:{  //设置RTU模式
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


		              case  260:{  //清除RTU模式
				    ret=cancelRTUMode(root,tree,code);
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

		              case  190:{  //设置RTU端口占用
				    ret=setRTUPort(root,tree,code);
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

		              case  270:{  //清除RTU端口占用
				    ret=cancelRTUPort(root,tree,code);
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

		              case  330:{  //请求报告网络参数
				    ret=requestNetwork(root,tree,code);
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
               }                               
          mxmlDelete(tree);		   
          printf("</RespondMessage>\n");
          exit(0);  
}  










