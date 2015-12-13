#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H

#include <QObject>
#include <QWindow>
#include <QOpenGLFunctions>
#include <QOpenGLPaintDevice>

namespace tron {
namespace rendering {

class OpenGlWindow : public QWindow, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit OpenGlWindow(QWindow *parent = 0);

    virtual void render(QPainter *painter);
    virtual void render();

    virtual void initialize();

    void setAnimating(bool animating);

    public slots:
        void renderLater();
        void renderNow();

    protected:
        bool event(QEvent *event) Q_DECL_OVERRIDE;

        void exposeEvent(QExposeEvent *event) Q_DECL_OVERRIDE;

    private:
        bool m_update_pending;
        bool m_animating;

        QOpenGLContext *m_context;
        QOpenGLPaintDevice *m_device;
};


}
}

#endif // OPENGLWINDOW_H
