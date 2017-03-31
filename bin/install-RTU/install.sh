#!/bin/sh
ping_count=2
:>ping_action

optarg=`getopt -o r:l:s:h --long route-ip,local-ip,server-ip,help \
     -n 'install.sh' -- "$@"`
if [ $? != 0 ] ; then 
   echo "终止执行..." >&2 ; 
   exit 1 ; 
fi

eval set -- "$optarg"

echo "<----------光缆监控系统环境自动配置--------->"
mkdir -p /web
chmod 4777 /web
mkdir -p /web/cgi-bin
chmod 4777 /web/cgi-bin
mkdir -p /etc/boa
mkdir -p /lib/pkgconfig

:>/web/cgi-bin/fiberMointor.conf

echo "配置网络 ..."
while true ; do
   case "$1" in
        -r|--route-ip)      
		RouteIP=$2       
		NIC=eth0
		MAC=`LANG=C ifconfig $NIC | awk '/HWaddr/{ print $5 }' `
		IP=`LANG=C ifconfig $NIC | awk '/inet addr:/{ print $2 }' | awk -F: '{print $2 }'`
		MASK=`LANG=C ifconfig $NIC | awk -F: '/Mask/{print $4}'`
		if [ $RouteIP = $IP ]

		then
			echo "本机IP与设置IP相同，忽略RouteIP设置"
			echo "eth0 MAC:$MAC"
			echo "eth0 IP:$IP"
			echo "eth0 MASK:$MASK" 
                        echo "eth0 $RouteIP">>/web/cgi-bin/fiberMointor.conf
                        
		else
			echo "网卡ETH0-------->开始RoutIP合法性/冲突检测:$RouteIP,次数$ping_count."
			if [ `echo $RouteIP | awk -F . '{print NF}'` -ne 4 ];then 
				echo "非法 IP!" 
                                echo "终止执行..."
				exit
			else 
				a=`echo $RouteIP | awk -F . '{print $1}'` 
				b=`echo $RouteIP | awk -F . '{print $2}'` 
				c=`echo $RouteIP | awk -F . '{print $3}'` 
				d=`echo $RouteIP | awk -F . '{print $4}'` 
				if [[ $a -gt 0 && $a -le 255 ]] && [[ $b -ge 0 && $b -le 255 ]] && [[ $c -ge 0 && $c -le 255 ]] && [[ $d -gt 0 && $d -lt 255 ]];then 
					echo "合法RouteIP:$RouteIP";
				else 
					echo "非法RouteIP [-r]!";
                                        echo "终止执行..."
				        exit 
				fi 
			fi 
			ping $RouteIP -c $ping_count  >.ping_tmp
			cat .ping_tmp  >>ping_action
			sum_ping=`tail -2 .ping_tmp |head -1 |awk -F, '{print$2}' |cut -c 2-2`
			loss_ping=`tail -2 .ping_tmp |head -1 |awk -F, '{print$4}'|cut -c 2-5`
			if [ $sum_ping -eq $ping_count ];then
			    echo "-->$RouteIP  IP冲突,不可使用,请更换IP重新设置";
                            echo "终止执行..."
			    exit
			else
			    echo "$RouteIP IP不冲突,设置eth0为此IP";
                            echo "eth0 $RouteIP">>/web/cgi-bin/fiberMointor.conf
                            ifconfig eth0 $RouteIP;
			fi
		                    
	       fi
	      shift 2;; 
        -l|--local-ip)   
		LocalIP=$2
		NIC=eth1
		MAC=`LANG=C ifconfig $NIC | awk '/HWaddr/{ print $5 }' `
		IP=`LANG=C ifconfig $NIC | awk '/inet addr:/{ print $2 }' | awk -F: '{print $2 }'`
		MASK=`LANG=C ifconfig $NIC | awk -F: '/Mask/{print $4}'`

                echo "LocalIP:$LocalIP"

                
		if [ $LocalIP = $IP ]

		then
		        echo "本机IP与设置IP相同，忽略LocalIP设置"
			echo "eth1 MAC:$MAC"
			echo "eth1 IP:$IP"
			echo "eth1 MASK:$MASK" 
                        echo "eth1 $LocalIP">>/web/cgi-bin/fiberMointor.conf
		 else
			echo "网卡ETH1-------->开始LocalIP合法性/冲突检测:$LocalIP,次数$ping_count."
			if [ `echo $LocalIP | awk -F . '{print NF}'` -ne 4 ];then 
				echo "非法 IP!" 
                                echo "终止执行..."
				exit 
			else 
				a=`echo $LocalIP | awk -F . '{print $1}'` 
				b=`echo $LocalIP | awk -F . '{print $2}'` 
				c=`echo $LocalIP | awk -F . '{print $3}'` 
				d=`echo $LocalIP | awk -F . '{print $4}'` 
				if [[ $a -gt 0 && $a -le 255 ]] && [[ $b -ge 0 && $b -le 255 ]] && [[ $c -ge 0 && $c -le 255 ]] && [[ $d -gt 0 && $d -lt 255 ]];then 
					echo "合法LocalIP is: $LocalIP"; 
                                        echo "eth1 $LocalIP">>/web/cgi-bin/fiberMointor.conf
                                        ifconfig eth1 $LocalIP;
				else 
					echo "非法 LocalIP [-l]!"; 
                                        echo "终止执行..." 
				        exit 
				fi 
			fi		        
			                 
		 fi
	     shift 2;; 
        -s|--server-ip)
             ServerIP=$2;
             NIC=eth0       
	     IP=`LANG=C ifconfig $NIC | awk '/inet addr:/{ print $2 }' | awk -F: '{print $2 }'`	     
             if [ $ServerIP = $IP ]
             then
                echo "中心网管服务器(时间同步服务器)与本机IP相同，请请重新设置..."
                echo "终止执行..." 
		exit
             else
                        echo "中心网管服务器(时间同步服务器)-------->开始进行ServerIP合法性/可达监测:$ServerIP,次数$ping_count." 
			if [ `echo $ServerIP | awk -F . '{print NF}'` -ne 4 ];then 
				echo "非法 IP!" 
                                echo "终止执行..."
				exit 
			else 
				a=`echo $ServerIP | awk -F . '{print $1}'` 
				b=`echo $ServerIP | awk -F . '{print $2}'` 
				c=`echo $ServerIP | awk -F . '{print $3}'` 
				d=`echo $ServerIP | awk -F . '{print $4}'` 
				if [[ $a -gt 0 && $a -le 255 ]] && [[ $b -ge 0 && $b -le 255 ]] && [[ $c -ge 0 && $c -le 255 ]] && [[ $d -gt 0 && $d -lt 255 ]];then 
					echo "合法ServerIP is: $ServerIP"; 
				else 
					echo "非法 ServerIP [-t]!"; 
                                        echo "终止执行..." 
				        exit 
				fi 
			fi 

                        ping $ServerIP -c $ping_count  >.ping_tmp;
			cat .ping_tmp  >>ping_action;
			sum_ping=`tail -2 .ping_tmp |head -1 |awk -F, '{print$2}'|cut -c 2-2`;
			loss_ping=`tail -2 .ping_tmp |head -1 |awk -F, '{print$4}'|cut -c 2-5`;
			if [ $sum_ping -eq $ping_count ];then
			    echo "$ServerIP  中心网管服务器(时间同步服务器)IP地址可达,";
                            echo "ServerIP $ServerIP">>/web/cgi-bin/fiberMointor.conf
			else
			    echo "-->$ServerIP IP不可达,请设置正确的服务器IP地址";
                            echo "终止执行..."
			    exit
			fi


             fi
             shift 2;;
        -h|--help)
	      echo "./install.sh -r [IP1] -l [IP2] -s [IP3]"
	      echo "    -r --route-ip :设置eth0网口的IP地址，提供局域网远程路由!";
	      echo "    -l --local-ip:设置eth1网口的IP地址，提供与OTDR本地直连!";
              echo "    -s --server-ip:设置中心网管服务器(时间同步服务器)IP地址!";
	      echo "    例如: ./inatall.sh -r 192.168.0.160 -l 192.168.1.120";
              exit;;
        --)  shift ; break ;;
         *)  echo "非法参数!"; echo "终止执行..." exit ;;  
  esac  
done 
sleep 1
echo "网络配置:完成!"

echo "步骤1:安装XML库 ..."
cp -rf ./libmxml.* /lib
sleep 1
echo "步骤1:成功!"


echo "步骤2:安装Sqlite数据库 ..."
cp -rf ./libsqlite3.* /lib
cp -rf ./bin/sqlite3 /bin
cp -rf ./web/cgi-bin/System.db /web/cgi-bin
sleep 1
echo "步骤2:成功!"


echo "步骤3:安装modbus库 ..."
cp -rf ./libmodbus.*  /lib
cp -rf ./pkgconfig/libmodbus.pc /lib/pkgconfig
sleep 1
echo "步骤3:成功!"

echo "步骤4:安装Curl库 ..."
cp -rf ./libcurl.* /lib
cp -rf ./pkgconfig/libcurl.pc /lib/pkgconfig
sleep 1
echo "步骤4:成功!"

echo "步骤5:安装Boa服务器 ..."
cp -rf ./bin/boa /bin
cp -rf ./etc/boa/boa.conf /etc/boa
cp -rf ./etc/mime.types /etc
sleep 1
echo "步骤5:成功!"

echo "步骤6:安装系统应用程序"
cp -rf ./web/cgi-bin/OtdrAllData.hex /web/cgi-bin/
cp -rf ./web/cgi-bin/back.xml   /web/cgi-bin
cp -rf ./web/cgi-bin/recv.xml   /web/cgi-bin 
cp -rf ./web/cgi-bin/send.xml   /web/cgi-bin
cp -rf ./web/cgi-bin/BoaCom.cgi /web/cgi-bin
cp -rf ./web/cgi-bin/otdrMain   /web/cgi-bin
cp -rf ./web/cgi-bin/cycMain    /web/cgi-bin
cp -rf ./web/cgi-bin/alarmMain  /web/cgi-bin
cp -rf ./web/cgi-bin/ProtectMasterMain  /web/cgi-bin
cp -rf ./web/cgi-bin/ProtectSlaveMain   /web/cgi-bin
cp -rf ./web/cgi-bin/reboot.sh          /web/cgi-bin
cp -rf ./web/cgi-bin/selfCheck          /web/cgi-bin


chmod 4777 /web/cgi-bin/recv.xml /web/cgi-bin/BoaCom.cgi /web/cgi-bin/otdrMain /web/cgi-bin/cycMain /web/cgi-bin/alarmMain /web/cgi-bin/fiberMointor.conf

read -p "是否配置OTDR测试[Y/n]:"
if [ -z "$REPLY" ]
then
   otdrMain=y;
else
   otdrMain=$REPLY;
fi
echo "otdrMain $otdrMain">>/web/cgi-bin/fiberMointor.conf

read -p "是否配置周期测试[y/N]:"
if [ -z "$REPLY" ]
then
   cycleMain=n;
else
   cycleMain=$REPLY;
fi
echo "cycleMain $cycleMain">>/web/cgi-bin/fiberMointor.conf


read -p "是否配置障碍告警测试[y/N]:"
if [ -z "$REPLY" ]
then
   alarmMain=n;
else
   alarmMain=$REPLY;
fi
echo "alarmMain $alarmMain">>/web/cgi-bin/fiberMointor.conf  

read -p "是否配置主端保护模式[y/N]"
if [ -z "$REPLY" ]
then
   ProtectMasterMain=n;
else
   ProtectMasterMain=$REPLY;
fi
echo "ProtectMasterMain $ProtectMasterMain">>/web/cgi-bin/fiberMointor.conf  


read -p "是否配置从端保护模式[y/N]"
if [ -z "$REPLY" ]
then
   ProtectSlaveMain=n;
else
   ProtectSlaveMain=$REPLY;
fi
echo "ProtectSlaveMain $ProtectSlaveMain">>/web/cgi-bin/fiberMointor.conf  

read -p "是否配置上电自启动[y/N]:"
if [ -z "$REPLY" ]
then
   autoPowerOn=n;
else
   autoPowerOn=$REPLY;
fi
echo "autoPowerOn $autoPowerOn">>/web/cgi-bin/fiberMointor.conf

cp -rf ./S110fiberMointor  /etc/init.d
cp -rf ./S40network        /etc/init.d

sleep 1
echo "步骤6:成功!"

echo "安装时间同步客户端"
cp -rf ./bin/ntpdate /bin
echo "安装完成"

echo "<----------自动配置完成---------->"

