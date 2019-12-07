#pragma once
#include <qobject.h>

#include "ui_EasyRemoteDesktop.h"
#include "CaptureScreenThread.h"

class ServerMainWindow :
    public QObject
{
    Q_OBJECT
public slots:
    void onGetNewFrame();

private slots:
    void onPreviewBtnClicked();
    void onSentBtnClicked();
    void onFpsSelectedChanged(const QString& fps);

public:
    ServerMainWindow(Ui::EasyRemoteDesktopClass *ui);
    ~ServerMainWindow();

private:
    void CaptureStatusChange();
    Ui::EasyRemoteDesktopClass *m_pUi;

    bool m_bPreviewFlag = false;
    bool m_bSentFlag = false;

    CaptureScreenThread m_captureThread;
};

