#include "FormMacDetail.h"
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

FormMacDetail::FormMacDetail(QWidget *parent,QList<QPair<QString,QString>> allCard) : UIBaseWidget(parent)
{
    QWidget::setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |Qt::WindowSystemMenuHint);   //设置无边框,置顶
    setWindowModality(Qt::ApplicationModal);      //禁用主窗口
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");
    setGeometry(0,0,parent->width(),parent->height());

    if (this->objectName().isEmpty())
        this->setObjectName("FormMacDetail");
    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setObjectName("gridLayout");
    QWidget *widget = new QWidget(this);
    widget->setObjectName("widget");
    widget->setFixedSize(QSize(500, 350));
    widget->setStyleSheet("background-color: rgb(250, 253, 255);border-radius: 4px;");
    gridLayout->addWidget(widget, 0, 0, 1, 1);

    QVBoxLayout *verticalLayout = new QVBoxLayout(widget);
    verticalLayout->setSpacing(1);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    verticalLayout->setContentsMargins(14, -1, 14, 20);

    int iWndHeightTotal = 0;
    //标题***************************************************************
    QLabel *labelTitle = new QLabel(tr("Mac地址"),widget);
    labelTitle->setObjectName("labelTitle");
    labelTitle->setMinimumSize(QSize(0, 60));
    labelTitle->setMaximumSize(QSize(16777215, 60));
    labelTitle->setStyleSheet("opacity:1;font-size:22px;font-weight:500;line-height: normal;color: #2D3440;");
    labelTitle->setAlignment(Qt::AlignCenter);
    verticalLayout->addWidget(labelTitle);
    iWndHeightTotal += labelTitle->height();

    //表头************************************************************
    QHBoxLayout* horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing(6);
    horizontalLayout->setObjectName("horizontalLayout");
    QLabel *labelTitleName = new QLabel("Name", widget);
    labelTitleName->setObjectName("labelTitleName");
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(labelTitleName->sizePolicy().hasHeightForWidth());
    labelTitleName->setSizePolicy(sizePolicy);
    labelTitleName->setFixedSize(QSize(180, 44));
    labelTitleName->setStyleSheet("background: #EEF1F5;");
    labelTitleName->setAlignment(Qt::AlignCenter);
    horizontalLayout->addWidget(labelTitleName);
    QLabel *labelTitleMac = new QLabel("Mac", widget);
    labelTitleMac->setObjectName("labelTitleMac");
    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(labelTitleMac->sizePolicy().hasHeightForWidth());
    labelTitleMac->setSizePolicy(sizePolicy1);
    labelTitleMac->setMinimumSize(QSize(270, 44));
    labelTitleMac->setMaximumSize(QSize(16777215, 44));
    labelTitleMac->setStyleSheet("background: #EEF1F5;");
    labelTitleMac->setAlignment(Qt::AlignCenter);
    horizontalLayout->addWidget(labelTitleMac);
    verticalLayout->addLayout(horizontalLayout);
    iWndHeightTotal += qMax(labelTitleName->height(),labelTitleMac->height());

    for (int i=0; i<allCard.size(); ++i){
        auto hLayout = new QHBoxLayout();
        hLayout->setObjectName(QString::asprintf("horizontalLayout%d",i+1));
        verticalLayout->addLayout(hLayout);

        auto labelName = new QLabel(allCard[i].first, widget);
        labelName->setObjectName(QString::asprintf("labelName%d",i+1));
        sizePolicy.setHeightForWidth(labelName->sizePolicy().hasHeightForWidth());
        labelName->setSizePolicy(sizePolicy);
        labelName->setFixedSize(QSize(180, 44));
        labelName->setStyleSheet("background-color: rgb(250, 253, 255);");
        labelName->setAlignment(Qt::AlignCenter);
        hLayout->addWidget(labelName, 0, Qt::AlignHCenter);

        auto labelMac = new QLabel(allCard[i].second, widget);
        labelMac->setObjectName(QString::asprintf("labelMac%d",i+1));
        sizePolicy.setHeightForWidth(labelMac->sizePolicy().hasHeightForWidth());
        labelMac->setSizePolicy(sizePolicy);
        labelMac->setMinimumSize(QSize(270, 44));
        labelMac->setMaximumSize(QSize(16777215, 44));
        labelMac->setStyleSheet("background-color: rgb(250, 253, 255);");
        labelMac->setAlignment(Qt::AlignCenter);
        hLayout->addWidget(labelMac);

        iWndHeightTotal += qMax(labelName->height(),labelMac->height());

        auto line = new QFrame(widget);
        line->setObjectName(QString::asprintf("line%d",i+1));
        line->setMinimumSize(QSize(100, 1));
        line->setMaximumSize(QSize(16777215, 1));
        line->setStyleSheet(QString::fromUtf8("background-color: #DFE3E5;border:none;"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        verticalLayout->addWidget(line);
        iWndHeightTotal += line->height();
    }

    auto verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verticalLayout->addItem(verticalSpacer);

    auto btnClose = new QPushButton(tr("关闭"),widget);
    connect(btnClose,&QPushButton::clicked,this,&FormMacDetail::close);
    sizePolicy.setHeightForWidth(btnClose->sizePolicy().hasHeightForWidth());
    btnClose->setSizePolicy(sizePolicy);
    btnClose->setMinimumSize(QSize(140, 44));
    btnClose->setMaximumSize(QSize(140, 44));
    btnClose->setStyleSheet("border-radius: 3px;opacity: 1;\n"
                            "border:none;background: #0047BB;color: #FAFDFF;\n"
                            "font-size: 16px;font-weight: 350;\n");
    verticalLayout->addWidget(btnClose, 0, Qt::AlignHCenter);
    iWndHeightTotal += btnClose->height();
    widget->setFixedSize(QSize(500, iWndHeightTotal+120));
}

void FormMacDetail::show()
{
    raise();
    QWidget::show();
}
