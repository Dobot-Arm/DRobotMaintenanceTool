#include "FileCheckTipWidget.h"
#include "ui_FileCheckTipWidget.h"
#include "Define.h"
#include "CommonData.h"
#include "MainWidget2.h"
#include <QMovie>
FileCheckTipWidget::FileCheckTipWidget(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::FileCheckTipWidget)
{
    ui->setupUi(this);
    SET_STYLE_SHEET(FileCheckTipWidget);
    //加载gif图片
    QMovie *movie = new QMovie(":/image/images/loading.gif");
    ui->labelDoing->setMovie(movie);
    movie->start();
    connect(ui->btnConfirm,&QPushButton::clicked,this,&FileCheckTipWidget::slotClose);
}

FileCheckTipWidget::~FileCheckTipWidget()
{
    delete ui;
}

void FileCheckTipWidget::setStatus(FileCheckStatus status)
{
    ui->textErrorFiles->hide();
    ui->btnConfirm->hide();
    ui->labelDoing->hide();
    ui->labelStatus->hide();
    ui->labelStatusIcon->hide();
    if(status == FileCheckStatus::FILE_CHECK_DOING)
    {
        ui->labelDoing->show();
        FaultCheckData data;
        data.operationType = "files";
        data.control = CommonData::getCurrentSettingsVersion().control;
        FaultCheckResult fileCheckResult = PublicgGetMainWindow()->GetDobotProtocol()->postFaultCheck(g_strCurrentIP,data);
        if(fileCheckResult.result == "success")
        {
            setStatus(FileCheckStatus::FILE_CHECK_SUCCESS);
        }
        else
        {
            m_missingFiles = fileCheckResult.missingFiles;
            setStatus(FileCheckStatus::FILE_CHECK_FAILED);
        }
    }
    else if(status == FileCheckStatus::FILE_CHECK_FAILED)
    {
        ui->labelStatus->setText(tr("缺失文件"));
        QString textErrorFiles;
        for(int i = 0; i < m_missingFiles.count();i++)
        {
            textErrorFiles.append(tr("缺失文件%1: ").arg(i+1)+m_missingFiles[i]+"\n");
        }
        ui->textErrorFiles->setText(textErrorFiles);
        ui->textErrorFiles->show();

        ui->labelStatusIcon->setProperty("isSuccess","false");
        updateStyleSheet(ui->labelStatusIcon);
        ui->labelStatus->show();
        ui->labelStatusIcon->show();
        ui->btnConfirm->show();
    }
    else if(status == FileCheckStatus::FILE_CHECK_SUCCESS)
    {
        ui->labelStatus->setText(tr("无缺失文件"));
        ui->labelStatusIcon->setProperty("isSuccess","true");
        updateStyleSheet(ui->labelStatusIcon);
        ui->labelStatus->show();
        ui->labelStatusIcon->show();
        ui->btnConfirm->show();
    }

}

void FileCheckTipWidget::slotClose()
{
    close();
}
