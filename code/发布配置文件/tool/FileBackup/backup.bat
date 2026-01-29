@echo off
echo "start backup"
cd ./tool/FileBackup
echo %1
echo %2
set server_ip=%1
set tarName=%2
rd /s /q backup
md backup
md backup\project
@echo off

(
  echo cd /dobot/userdata/project
  echo tar -zcvf %tarName%.tar.gz --exclude=logs *
  echo exit 
) | plink.exe -ssh -l root -pw dobot %server_ip%

pscp.exe -pw dobot -r root@%server_ip%:/dobot/userdata/project/%tarName%.tar.gz ./backup/project

(
  echo cd /dobot/userdata/project
  echo rm -rf %tarName%.tar.gz
  echo exit 
) | plink.exe -ssh -l root -pw dobot %server_ip%


echo "backup start successfully."