@echo off
chcp 65001
echo "start UpdateTool"
cd ./tool/logExport
cd /d %~dp0
set server_ip=%1
set projectPath=%2
set zipName=%3
rd /s /q cache
md cache


call :runServerShellAsh "cd /dobot/userdata/project;tar czvf %zipName%.tar.gz logs/ project/%projectPath%/"


pscp.exe -pw dobot -r root@%server_ip%:/dobot/userdata/project/%zipName%.tar.gz ./cache


(
  echo cd /dobot/userdata/project
  echo rm -rf %zipName%.tar.gz
  echo exit 
) | plink.exe -ssh -l root -pw dobot %server_ip%

echo "logExport successfully."

exit

:runServerShellAsh
    REM 创建临时的shell脚本文件
    echo #/bin/ash > temp_script.sh
    echo %~1 >> temp_script.sh
    pscp.exe -pw dobot -r temp_script.sh root@%server_ip%:/tmp
    REM 运行shell脚本并保存输出
    plink.exe -ssh -l root -pw dobot %server_ip% "cd /tmp;chmod +x temp_script.sh;./temp_script.sh"
    exit /B 0
