#include "ServerMainWindow.h"
#include "glog/logging.h"

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

    connect(ui->serverFpsComboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(onFpsSelectedChanged(const QString &)));

    connect(&m_captureThread, &CaptureScreenThread::onGetNextScreenFrame, this, &ServerMainWindow::onGetNewFrame, Qt::QueuedConnection);
}


ServerMainWindow::~ServerMainWindow()
{
    m_captureThread.StopCaptureScreen();
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
    CaptureStatusChange();
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
    CaptureStatusChange();
}

void ServerMainWindow::onFpsSelectedChanged(const QString& fps)
{
    m_captureThread.SetFps(fps.toInt());
}

void ServerMainWindow::CaptureStatusChange() {
    if ((m_bPreviewFlag || m_bSentFlag))
    {
        if (!m_captureThread.IsRunning()) {
            m_captureThread.StartCaptureScreen(m_pUi->serverFpsComboBox->currentText().toInt());
        }
    }
    else {
        if (m_captureThread.IsRunning()) {
            m_captureThread.StopCaptureScreen();
        }
    }
}

void ServerMainWindow::onGetNewFrame()
{
    m_pUi->serverPreviewWindow->UpdateFrame(m_captureThread.GetFrame());
}
