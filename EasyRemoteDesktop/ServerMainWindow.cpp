#include "ServerMainWindow.h"

ServerMainWindow::ServerMainWindow(Ui::EasyRemoteDesktopClass *ui) : m_pUi(ui)
{
    if (!ui) {
        return;
    }
    
    m_pUi->serverPreviewButton->setText(QStringLiteral("开始预览"));
    m_pUi->serverSentButton->setText(QStringLiteral("开始发送"));

    //端口号范围
    ui->serverPortEdit->setValidator(new QIntValidator(5001, 65535, this));

    //设置信号槽
    connect(ui->serverPreviewButton, SIGNAL(clicked()), this, SLOT(onPreviewBtnClicked()));
    connect(ui->serverSentButton, SIGNAL(clicked()), this, SLOT(onSentBtnClicked()));
}


ServerMainWindow::~ServerMainWindow()
{
}


void ServerMainWindow::onPreviewBtnClicked()
{
    if (m_bPreviewFlag)
    {
        m_pUi->serverPreviewButton->setText(QStringLiteral("开始预览"));
        //TODO
    }
    else {
        m_pUi->serverPreviewButton->setText(QStringLiteral("停止预览"));
        //TODO
    }

    m_bPreviewFlag = !m_bPreviewFlag;
}

void ServerMainWindow::onSentBtnClicked()
{
    if (m_bSentFlag)
    {
        m_pUi->serverSentButton->setText(QStringLiteral("开始发送"));
        //TODO
    }
    else {
        m_pUi->serverSentButton->setText(QStringLiteral("停止发送"));
        //TODO
    }

    m_bSentFlag = !m_bSentFlag;
}
