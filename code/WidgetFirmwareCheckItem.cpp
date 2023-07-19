#include "WidgetFirmwareCheckItem.h"
#include "ui_WidgetFirmwareCheckItem.h"

WidgetFirmwareCheckItem::WidgetFirmwareCheckItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetFirmwareCheckItem)
{
    ui->setupUi(this);
    ui->labelFailed->hide();
    ui->labelSuccess->hide();
    ui->labelRepairing->hide();
    ui->btnRepair->hide();
    ui->labelMessage->hide();
    ui->labelWarningIcon->hide();
    connect(ui->btnRepair,&QPushButton::clicked,this,[&]{
        setLabelStatus(1);
        emit signal_firmwareCheckRepair(m_theme);
    });
    QImage *img_mainicon;//主图标显示在右上角lable中
    img_mainicon =new QImage;//新建一个image对象
    img_mainicon->load(":/image/images2/icon_warning.png"); //载入图片到img对象中
    img_mainicon->scaled(ui->labelWarningIcon->size(),Qt::KeepAspectRatio);//把图片
    ui->labelWarningIcon->setScaledContents(true);
    ui->labelWarningIcon->setPixmap(QPixmap::fromImage(*img_mainicon)); //将图片放入label，使用setPixmap,注意指针*img


    //ui->label_mainicon->setAlignment(Qt::AlignCenter); //将图片放在label中心，用缩放了就不需要了

}


WidgetFirmwareCheckItem::~WidgetFirmwareCheckItem()
{
    delete ui;
}

void WidgetFirmwareCheckItem::setTheme(QString theme)
{
    m_theme = theme;
    ui->labelTheme->setText(m_theme);
}

QString WidgetFirmwareCheckItem::theme()
{
    return ui->labelTheme->text();
}

void WidgetFirmwareCheckItem::setLabelStatus(int status)
{
    ui->labelFailed->hide();
    ui->labelSuccess->hide();
    ui->labelRepairing->hide();
    ui->btnRepair->hide();
    ui->labelMessage->hide();
    ui->labelWarningIcon->hide();
    if(status == 10){
        ui->btnRepair->show();
    }
    if(status == -1){
        ui->labelFailed->show();
    }
    if(status == 0){

    }
    if(status == 1){
        ui->labelRepairing->show();
    }
    if(status == 2){
        if(m_theme.contains("Mac",Qt::CaseInsensitive)){
            ui->labelMessage->setText("请重启控制柜生效");
            ui->labelMessage->show();
            ui->labelWarningIcon->show();
        }
        ui->labelSuccess->show();
    }
}

void WidgetFirmwareCheckItem::setThemeStatus(int status)
{
    if(status == -1){
        ui->labelTheme->setText(m_theme+"异常");
    }
    if(status == 0){
        ui->labelTheme->setText(m_theme);
    }
    if(status == 1){
        ui->labelTheme->setText("扫描"+m_theme+"...");
    }
    if(status == 2){
        ui->labelTheme->setText(m_theme+"正常");
    }
}

void WidgetFirmwareCheckItem::setMainThemeStatus(int status)
{

    if(status == 1){
        ui->labelTheme->setText(m_theme+"中");
    }
    if(status == 2){
        ui->labelTheme->setText(m_theme+"完成");
    }

}

void WidgetFirmwareCheckItem::setBigFont()
{
    QFont font;
    font.setPixelSize(18);
    font.setBold(true);
    ui->labelTheme->setFont(font);
    isBigTheme = true;
}
