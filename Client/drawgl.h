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
#include <QSettings>
#include <QDir>
#include <algorithm>

using std::min;
using std::max;

class MainWindow;

#ifdef PORTABLE
    const QString skinPath = "../skins/";
    const QString settingsFile = "../labyrus.ini";
#else
    const QString skinPath = "/usr/share/labyrus/skins/";
    const QString settingsFile = QDir::homePath() + "/.labyrus/labyrus.ini";
#endif

const double wallHeight = 0.1;
const double eps = 1e-5 * 5;
const double sizeView = 10;

enum {
    defaultWall = 0,
    shortWall = 1,
    horizontalSlice = 2,
    Floor = 3,
    compass = 4,
    sky = 5,
    model = 6,
    roof = 7,
    icon = 8,
    blackout = 9,
    hudbackground = 10,
    okular = 11,
    verticalSlice = 12,
    winners = 13
};

class DrawGl : public QGLWidget
{
    Q_OBJECT
public:
    explicit DrawGl(QApplication *app, QString skin, double mouse, QWidget *parent = 0);
    ~DrawGl();
    NetworkClass *a;
    MainWindow *legacy;
    QApplication *application;
    int xRot, yRot, zRot;
    int animZRot;
    int oldFps;
    GLfloat animX, animY;
    GLfloat nSca;
    GLfloat xtra, ytra, ztra;
    QFont hudFont, menuFont;
    bool startingGame;
    bool started;
    bool firstStart;
    qreal startAfter;
    bool botActive;
    int perspective;
    bool needRefreshCursor;
    double botLast;
    double k;
    double f;
    double mouseSensitivity;
    int progress;
    void updatePerspective();

private:
    void drawAxis();
    void drawMaze();
    void drawSkyBox();
    void drawHeroes();
    void drawCompass();
    void drawHUD();
    void drawMenu();
    void drawOkular();
    void drawBotLast();
    void drawWinners();
    void drawPreview();
    void drawChat();
    void drawStarting();

    void flushBug();

    void drawQuad(double x1, double y1, double x2, double y2, double h, double height);
    void drawFloorPoint(double x1, double y1, double h, bool b);
    void drawRoofPart(double x, double y, double h, int type, bool b);
    void loadTexture(GLuint);

    void drawText(double x, double y, double z, bool xForwarding, bool yForwarding, QString s);

    QTimer *t, *timeFPS, *cursorSetTimer;
    int fps;
    GLuint textures[14];
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
    int activePoint;

    void enableLight();
    void begin2d();
    void end2d();
protected:
    void initializeGL();
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
private slots:
    void cursorSet();
};

#endif // DRAWGL_H
