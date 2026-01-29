@echo off
echo "start fileRecovery"
cd ./tool/fileRecovery
echo %1
set server_ip=%1
set tarName=%2
@echo off
pscp.exe -pw dobot -r ./project/* root@%server_ip%:/dobot/userdata/project/


(
  echo cd /dobot/userdata/project
  echo tar -xvf %tarName%.tar.gz
  echo rm -rf %tarName%.tar.gz
  echo exit 
) | plink.exe -ssh -l root -pw dobot %server_ip%



echo "fileRecovery start successfully."