# RobotMaintenanceTool
1. RobotMaintenanceTool工具是用来对机器人相关设备进行更新升级。

2. DRobotMaintenanceTool使用Qt5.12.11开发，使用了Qt的相关模块如下：
   `core` `gui` `network` `widgets`

3. DRobotMaintenanceTool使用了第三方开源库[libcurl](https://curl.se/libcurl/)。

4. DRobotMaintenanceTool使用了第三方库[QSsh](https://github.com/lvklabs/QSsh)
   ```
   为了满足特定需求，对sftpchannel.h/cpp做了如下修改:
   1. 添加Internal::AbstractSftpOperation::Ptr lookupJob(SftpJobId id);
   2. 添加2个信号
      void fileDownloadProgress(QSsh::SftpJobId job, quint64 fileSize, quint64 offset);
      void fileUploadProgress(QSsh::SftpJobId job, quint64 fileSize, quint64 offset);
   同时在sftpchannel_p.h中添加了如下2个信号
      void fileDownloadProgress(QSsh::SftpJobId job, quint64 fileSize, quint64 offset);
      void fileUploadProgress(QSsh::SftpJobId job, quint64 fileSize, quint64 offset);
   ```
5. DRobotMaintenanceTool的源码遵循LGPL协议

6. DRobotMaintenanceTool代码已经开源到[github上](https://github.com/Dobot-Arm/DRobotMaintenanceTool)
