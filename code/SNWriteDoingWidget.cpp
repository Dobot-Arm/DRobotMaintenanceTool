#include "SNWriteDoingWidget.h"
#include "ui_SNWriteDoingWidget.h"
#include <QMovie>
#include <QTimer>
#include "Define.h"
#include "MainWidget2.h"
SNWriteDoingWidget::SNWriteDoingWidget(QWidget *parent) :
    UIBaseWidget(parent),
    ui(new Ui::SNWriteDoingWidget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");

    //加载gif图片
    QMovie *movie = new QMovie(":/image/images/loading.gif");
    ui->labelDoing->setMovie(movie);
    //延迟500ms后,执行以下事件
    QTimer::singleShot(500, this, [&]()
    {
        ControllerSnData controlData;
        controlData.ControllCabinetSNCode = m_controllCabinetSN;
        RobotArmSnData robotData;
        robotData.RobotArmSNCode = m_robotArmSN;
        m_isRobotArmWriteSuccess = PublicgGetMainWindow()->GetDobotProtocol()->postSettingsProductInfoRobotArmSn(g_strCurrentIP,robotData);
        m_isConrtolWriteSuccess = PublicgGetMainWindow()->GetDobotProtocol()->postSettingsProductInfoControllerSn(g_strCurrentIP,controlData);
        m_isAlarmClearSuccess = true;
        if(CommonData::getControllerType().originName!=g_strPropertiesCabinetTypeMagicianE6)
        {
            //查看当前有无报警
            if(PublicgGetMainWindow()->GetDobotProtocol()->postInterfaceClearAlarms(g_strCurrentIP))
            {
                PublicSleep(1000);//清除报警后，等待1s查看是否清除成功。
                DobotType::ProtocolExchangeResult info = PublicgGetMainWindow()->GetDobotProtocol()->getProtocolExchange(g_strCurrentIP);
                if(!info.alarms[0].contains(4112))
                {
                    CommonData::setSettingsProductInfoHardwareInfo(PublicgGetMainWindow()->GetDobotProtocol()->getSettingsProductInfoHardwareInfo(g_strCurrentIP));
                    m_isAlarmClearSuccess = true;
                }
                else
                {
                    m_isAlarmClearSuccess = false;
                }
            }
        }
        m_loopExit.quit();
    });
    movie->start();
    hide();
    setGeometry(0,0,parent->width(),parent->height());
}

SNWriteDoingWidget::~SNWriteDoingWidget()
{
    delete ui;
}

void SNWriteDoingWidget::setControllCabinetSN(QString controllCabinetSN)
{
    m_controllCabinetSN = controllCabinetSN;
}

void SNWriteDoingWidget::setRobotArmSN(QString robotArmSN)
{
    m_robotArmSN = robotArmSN;
}

bool SNWriteDoingWidget::isRobotArmWriteSuccess()
{
    return m_isRobotArmWriteSuccess;
}

bool SNWriteDoingWidget::isConrtolWriteSuccess()
{
    return m_isConrtolWriteSuccess;
}

bool SNWriteDoingWidget::isAlarmClearSuccess()
{
    return m_isAlarmClearSuccess;
}

void SNWriteDoingWidget::exec()
{
    raise();
    show();
    m_loopExit.exec();
    hide();
}
