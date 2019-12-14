#pragma once
#include <QOpenGLWidget>
#include "EasyScreenCapturer/EasyScreenCapturer.h"
#include <atomic>

class ShowPreviewWindow : public QOpenGLWidget
{
    Q_OBJECT
public:
    ShowPreviewWindow(QWidget *parent);
    ~ShowPreviewWindow();

    void UpdateFrame(std::shared_ptr<media::CaptureBmpData> frame);
    void StartPlay() { m_bPlay = true; }
    void StopPlay() { m_bPlay = false; }

protected:
    //对3个纯虚函数的重定义
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void paintEvent(QPaintEvent *event);

    void keyPressEvent(QKeyEvent *event);//处理键盘按下事件
    void mouseDoubleClickEvent(QMouseEvent *event); //鼠标双击事件
private:
    std::shared_ptr<media::CaptureBmpData> m_pFrame = nullptr;
    std::atomic_bool m_bPlay = false;
    std::atomic_bool m_bFullScreen = false;
};

