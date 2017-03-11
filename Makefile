arm-linux-gnueabihf-gcc -o BoaCom.cgi  ComWithHttpClient.c common.c Communicate.c namedtest.c otdr.c defaulttest.c cycletest.c alarmtest.c opticalprotect.c opticalpower.c responed.c sql.c process.c myModbus.c rtuIformation.c  checkip.c -lsqlite3 -lmxml -lmodbus -lpthread -D_GNU_SOURCE -D__USE_XOPEN

arm-linux-gnueabihf-gcc -o cycMain  cycleMain.c sql.c common.c uploadCycTestData.c otdr.c process.c  -lsqlite3 -lcurl -D_GNU_SOURCE -D__USE_XOPEN


arm-linux-gnueabihf-gcc -o otdrMain otdrMain.c common.c sql.c otdr.c uploadCycTestData.c process.c myModbus.c checkip.c -lsqlite3 -lcurl -lmodbus -D_GNU_SOURCE -D__USE_XOPEN

arm-linux-gnueabihf-gcc -o alarmMain alarmMain.c common.c sql.c otdr.c uploadCycTestData.c process.c myModbus.c  -lsqlite3 -lcurl -lmodbus -D_GNU_SOURCE -D__USE_XOPEN

arm-linux-gnueabihf-gcc -o ProtectMasterMain ProtectMasterMain.c sql.c  myModbus.c common.c process.c otdr.c uploadCycTestData.c -lmodbus -lsqlite3 -lcurl -lpthread

arm-linux-gnueabihf-gcc -o ProtectSlaveMain ProtectSlaveMain.c myModbus.c common.c process.c sql.c -lmodbus -lsqlite3 -lpthread
