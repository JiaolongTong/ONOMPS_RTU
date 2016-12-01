
			       uint32tostring(alarmpar->levelGroup[i].portGroup[j].ASNo,strSNo);
			       mysql->tableName    = "ProtectGroupTable";
			       mysql->filedsValue  =  strSNo;
			       mysql->filedsName   = "SNoA";
			       resPN=SQL_findPNo(mysql,resultPNo);                     
			       if(resPN==1){
			               mysql->filedsName   = "Status";
				       mysql->mainKeyValue = resultPNo[0];             
				       SQL_lookupPar(mysql,&result,&rednum);
				       intStatus =atoi(result[0]); 
		                       SQL_freeResult(&result,&rednum);
				       if((intStatus==1) || (intStatus ==0)){                                           
                                           existProtect=1;
                                       }
                                       else{
				               existProtect =0;
					       resp->RespondCode = 14 ;                                                
					       if(resp->SNorPN!=TYPE_SNo){
						       resp->SNorPN              = TYPE_PNo;
						       resp->Group[ErrorSNo].PNo = atoi(resultPNo[0])+1; 
						       resp->Group[ErrorSNo].SNo = alarmpar->levelGroup[i].portGroup[j].ASNo; 
						       resp->Group[ErrorSNo].Main_inform  = "保护模式:未设置配对组";
						       resp->Group[ErrorSNo].Error_inform = "Error: Sqlite don't match -->Don't have protect group[保护模式:未设置配对组]";
						       ErrorSNo++; 
					       }
                                       }
			       }else if(resPN==0){
		                       mysql->filedsName   = "SNoB";
				       resPN=SQL_findPNo(mysql,resultPNo);              
				       if(resPN==1){
					       mysql->filedsName   =  "Status";
					       mysql->mainKeyValue = resultPNo[0];     
					       SQL_lookupPar(mysql,&result,&rednum);
					       intStatus =atoi(result[0]);	
					       SQL_freeResult(&result,&rednum);	                     
					       if((intStatus==1) || (intStatus ==0))existProtect=1;
		                               else{
		                                    existProtect =0;
					            resp->RespondCode = 14 ;                                                
						    if(resp->SNorPN!=TYPE_SNo){
							    resp->SNorPN              = TYPE_PNo;
						            resp->Group[ErrorSNo].PNo = atoi(resultPNo[0])+1; 
							    resp->Group[ErrorSNo].SNo = alarmpar->levelGroup[i].portGroup[j].ASNo; 
							    resp->Group[ErrorSNo].Main_inform  = "保护模式:未设置保护组";
							    resp->Group[ErrorSNo].Error_inform = "Error: Sqlite don't match -->Don't have protect group[保护模式:未设置保护组]";
							    ErrorSNo++; 
						    }
                                               }		                             
		                       }else if(resPN==0){
				                existProtect =0;
						resp->RespondCode = 14 ;                                                
						if(resp->SNorPN!=TYPE_SNo){
							resp->SNorPN              = TYPE_PNo;
							resp->Group[ErrorSNo].PNo = atoi(resultPNo[0])+1; 
							resp->Group[ErrorSNo].SNo = alarmpar->levelGroup[i].portGroup[j].ASNo; 
							resp->Group[ErrorSNo].Main_inform  = "保护模式:未设置保护组";
							resp->Group[ErrorSNo].Error_inform = "Error: Sqlite don't match -->Don't have protect group[保护模式:未设置保护组]";
							ErrorSNo++; 
		                                }
		                        }else{
		                          	resp->SNorPN              = TYPE_PNo;
						resp->Group[ErrorSNo].PNo = atoi(resultPNo[0])+1; 
						resp->Group[ErrorSNo].SNo = alarmpar->levelGroup[i].portGroup[j].ASNo; 
						resp->Group[ErrorSNo].Main_inform  = "保护模式:该光路存在多个保护组";
						resp->Group[ErrorSNo].Error_inform = "Error: Sqlite don't match -->Don't have mutli-group[保护模式:该光路存在多个保护组]";
						ErrorSNo++;       
		                       }
                                }else{
                                      existProtect =0;
				      resp->RespondCode = 14 ;                                                
				      if(resp->SNorPN!=TYPE_SNo){
					    resp->SNorPN              = TYPE_PNo;
					    resp->Group[ErrorSNo].PNo = atoi(resultPNo[0])+1; 
					    resp->Group[ErrorSNo].SNo = alarmpar->levelGroup[i].portGroup[j].ASNo; 
					    resp->Group[ErrorSNo].Main_inform  = "保护模式:该光路存在多个保护组";
					    resp->Group[ErrorSNo].Error_inform = "Error: Sqlite don't match -->Don't have mutli-group[该光路存在多个保护组]";
					    ErrorSNo++; 
				      }
                                }

