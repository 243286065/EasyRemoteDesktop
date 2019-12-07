#pragma once
#include <QOpenGLWidget>
#include "EasyScreenCapturer/EasyScreenCapturer.h"

class ShowPreviewWindow : public QOpenGLWidget
{
    Q_OBJECT
public:
    ShowPreviewWindow(QWidget *parent);
    ~ShowPreviewWindow();

    void UpdateFrame(std::shared_ptr<media::CaptureBmpData> frame);
protected:
    //对3个纯虚函数的重定义
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void paintEvent(QPaintEvent *event);

    void keyPressEvent(QKeyEvent *event);//处理键盘按下事件

    std::shared_ptr<media::CaptureBmpData> m_pFrame = nullptr;
};

