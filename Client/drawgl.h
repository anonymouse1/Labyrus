#ifndef DRAWGL_H
#define DRAWGL_H

#include <QGLWidget>
#include <mainwindow.h>
#include <GL/glu.h>
#include <QImage>
#include <QPixmap>
#include <messagesstack.h>
#include <QPicture>
#include <model.h>

class MainWindow;

const double wallHeight = 0.1;
const double eps = 0.00001;
const double sizeView = 10;

enum {
    defaultWall = 0,
    shortWall = 1,
    roof = 2,
    Floor = 3,
    compass = 4,
    realRoof = 5,
    model = 6
};

class DrawGl : public QGLWidget
{
    Q_OBJECT
public:
    explicit DrawGl(QApplication *app, QString skin, QWidget *parent = 0);
    NetworkClass *a;
    MainWindow *legacy;
    QApplication *application;
    int xRot, yRot, zRot;
    int animZRot;
    int oldFps;
    GLfloat animX, animY;
    GLfloat nSca;
    GLfloat xtra, ytra, ztra;
    QFont hudFont;
    bool startingGame;
    qreal startAfter;
    bool botActive;
    int perspective;
    bool needRefreshCursor;
    double k;
    double f;
    void initializeGL();

private:
    void drawAxis();
    void drawMaze();
    void drawSkyBox();

    void drawQuad(double x1, double y1, double x2, double y2, double h);
    void drawFloorPoint(double x1, double y1, double h);
    void drawRoofPart(double x, double y, double h, int type);
    void loadTexture(GLuint);

    void drawText(double x, double y, double z, bool xForwarding, bool yForwarding, QString s);

    QTimer *t, *timeFPS;
    int fps;
    GLuint textures[7];
    GLuint superFontList;
    QString currentText;
    bool enteringText;
    QString skinPath;
    QPixmap generateCompass(double angle);
    QPixmap *compass;
    Model *I;

    bool firstMouseMove;
    int lastClickX;
    int lastClickY;
    double plMouseXRot;
    double plMouseYRot;
    bool mousePressed;

    void enableLight();
    void begin2d();
    void end2d();
protected:
    void resizeGL(int w, int h);
    void paintGL();

    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
//    void wheelEvent(QWheelEvent *);

signals:
    void runCommand(QString);

public slots:
    void onx();
    void drawFPS();
    void processText();
};

#endif // DRAWGL_H
