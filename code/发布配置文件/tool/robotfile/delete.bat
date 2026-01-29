@echo off
set server_ip=%~1
cd %~dp0

echo only for V3,V4 robot of 6 axis

::显示准备删除的信息
echo Preparing to delete subdirectories, please wait...

:: 使用 find 命令删除 project目录下的所有子目录
plink.exe -ssh -l root -pw dobot %server_ip% "find /dobot/userdata/project/project -mindepth 1 -maxdepth 1 -type d -print0 | xargs -0 rm -rf"

::检查logs/user 目录是否存在，如果存在则删除内容
plink.exe -ssh -l root -pw dobot %server_ip% "[[ -d '/dobot/userdata/project/logs/user' ]] && rm -rf /dobot/userdata/user_project/logs/user/*"

::执行 sync 命令，确保所有数据已写入磁盘
plink.exe -ssh -l root -pw dobot %server_ip% "sync"

echo Subdirectories deleted successfully.
::echo Waiting for 2 seconds before final prompt...
timeout /t 2 

::软重启，此处不使用
::plink.exe -ssh -l root -pw dobot %server_ip% reboot
