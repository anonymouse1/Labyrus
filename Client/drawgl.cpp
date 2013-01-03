#include "drawgl.h"
//#include <Qt/qgl.h>
#include <QtOpenGL/QGLWidget>
#include <GL/glu.h>

DrawGl::DrawGl(QApplication *app, QString skin, QWidget *parent) :
    QGLWidget(parent)
{
    application = app;
    skinPath = "../skins/" + skin;
    enteringText = false;
    xRot = -90;
    yRot = 0;
    zRot = 0;
//    yRot = 1;
//    zRot = 1;

    animZRot = 0;
    fps = 0;
    oldFps = 0;

    nSca = 1;

    xtra = ytra = 0;
    ztra = -0.1;
    startingGame = false;
    plMouseXRot = 0;
    plMouseYRot = 0;

    hudFont = QFont("FreeSans", 15, 20, true);


//    t = new QTimer;
    timeFPS = new QTimer;
//    t->setInterval(3);
    timeFPS->setInterval(1000);
    startAfter = 666;

//    QObject::connect(t, SIGNAL(timeout()), this, SLOT(onx()));
    QObject::connect(timeFPS, SIGNAL(timeout()), this, SLOT(drawFPS()));
    this->setFocus();
//    t->start();
    timeFPS->start();


    qDebug("%d", defaultWall.load(skinPath + "/defaultWall.png"));
    qDebug("%d", shortWall.load(skinPath + "/shortWall.png"));

    defaultWall = QGLWidget::convertToGLFormat(defaultWall);
    shortWall = QGLWidget::convertToGLFormat(shortWall);

//    showFullScreen();
//    setMouseTracking(true);
    firstMouseMove = true;
    botActive = false;
}

void DrawGl::initializeGL() {
    qglClearColor(Qt::white);
    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_CULL_FACE);
    glEnable(GL_POINT_SMOOTH);

    textures[0] = bindTexture(QPixmap(skinPath + "/defaultWall.png"), GL_TEXTURE_2D);
    textures[1] = bindTexture(QPixmap(skinPath + "/shortWall.png"), GL_TEXTURE_2D);

    glEnable(GL_TEXTURE_2D);


//    loadTexture(textures[0]);


//    glGenTextures(2, textures);

    /*glBindTexture(GL_TEXTURE_2D, 0);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, (GLsizei)defaultWall.width(), (GLsizei)defaultWall.height(), GL_RGBA, GL_UNSIGNED_BYTE,
                 defaultWall.bits());


    glBindTexture(GL_TEXTURE_2D, 1);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, (GLsizei)shortWall.width(), (GLsizei)shortWall.height(), GL_RGBA, GL_UNSIGNED_BYTE,
                 shortWall.bits());
*/


    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    float dir[3] = {0, 0, -1};
    float pos[4] = {0.5, 0.5, 10, 200};
    float color[4] = {200, 200, 200, 5};
    float mat_specular[4] = {10, 10, 10, 10};
    GLfloat ambientLight[] = {0.5f, 0.5f, 0.5f, 1.0f};


    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, color);

    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_specular);
    glPointSize(50);
}

void DrawGl::resizeGL(int w, int h) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, (GLint)w, (GLint)h);
    gluPerspective(45.0, w / (double)h, 0.001, 100.0);
}

void DrawGl::paintGL() {
//    if (startAfter == 666)
//        return;

//    application->processEvents();

//    qDebug() << "paint" << rand();
    fps++;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(yRot, 0.0f, 1.0f, 0.0f);
    glRotatef(a->angle, 0.0f, 0.0f, 1.0f);

    nSca = a->n / 2.0;
    glScalef(nSca, nSca, nSca);
    glTranslatef((-a->coord.x() - animX) / sizeView, (-a->coord.y() - animY) / sizeView, ztra);

    glEnable(GL_LIGHTING);
//    drawAxis();
    drawMaze();
}

void DrawGl::drawAxis() {
    glLineWidth(3.0f);

    glBegin(GL_LINES);
        qglColor(Qt::red);

        glVertex3f(1.0f, 0.0f, 0.0f);
        glVertex3f(-1.0f, 0.0f, 0.0f);

        qglColor(Qt::green);

        glVertex3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, -1.0f, 0.0f);

        qglColor(Qt::blue);

        glVertex3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, -1.0f);
    glEnd();

    renderText(0, 0, 1, QString::number(1));
    renderText(0, 1, 0, QString::number(1));
    renderText(1, 0, 0, QString::number(1));
}

void DrawGl::drawQuad(double x1, double y1, double x2, double y2, bool shortWall) {
    glVertex3f(x1, y1, wallHeight);
    glTexCoord2d(0, 0);
    glVertex3f(x1, y1, 0);
    glTexCoord2d(1, 0);
    glVertex3f(x2, y2, 0);
    glTexCoord2d(1, 1);
    glVertex3f(x2, y2, wallHeight);
    glTexCoord2d(0, 1);
}

void DrawGl::drawMaze() {
    double k = 1.0 / a->n;
    k = 1.0 / sizeView;
    double f = k / 10;
    qglColor(Qt::gray);
//    float dir[3] = {1, 1, 1};
//    float pos[4] = {(a->coord.x() + animX) * k, (a->coord.y() + animY) * k, 0.1, 1};

    /*glBegin(GL_TRIANGLES);
        qglColor(Qt::gray);
        for (int i = 0; i < a->m; i++) {
            glColor3ub((a->walls[i][0] + 1) * 10, (a->walls[i][1] + 1) * 10, (a->walls[i][2] + 1) * 100);
            if (a->walls[i][2] == 0) {
                glVertex3f(a->walls[i][0] * k, a->walls[i][1] * k - f, 0.0);
                glVertex3f((a->walls[i][0] + 1) * k, a->walls[i][1] * k - f, 0.0);
                glVertex3f((a->walls[i][0] + 1) * k, a->walls[i][1] * k - f, wallHeight);

                glVertex3f((a->walls[i][0] + 1) * k, a->walls[i][1] * k + f, 0.0);
                glVertex3f(a->walls[i][0] * k, a->walls[i][1] * k + f, 0.0);
                glVertex3f((a->walls[i][0] + 1) * k, a->walls[i][1] * k + f, wallHeight);
                //-----------------------------------------------
                glVertex3f(a->walls[i][0] * k, a->walls[i][1] * k - f, 0.0);
                glVertex3f(a->walls[i][0] * k, a->walls[i][1] * k + f, 0.0);
                glVertex3f(a->walls[i][0] * k, a->walls[i][1] * k + f, wallHeight);

                glVertex3f(a->walls[i][0] * k, a->walls[i][1] * k + f, wallHeight);
                glVertex3f(a->walls[i][0] * k, a->walls[i][1] * k - f, wallHeight);
                glVertex3f(a->walls[i][0] * k, a->walls[i][1] * k - f, 0.0);
                //-----------------------------------------------
                glVertex3f((a->walls[i][0] + 1) * k, a->walls[i][1] * k - f, wallHeight);
                glVertex3f(a->walls[i][0] * k, a->walls[i][1] * k - f, 0.0);
                glVertex3f(a->walls[i][0] * k, a->walls[i][1] * k - f, wallHeight);

                glVertex3f(a->walls[i][0] * k, a->walls[i][1] * k + f, 0.0);
                glVertex3f((a->walls[i][0] + 1) * k, a->walls[i][1] * k + f, wallHeight);
                glVertex3f(a->walls[i][0] * k, a->walls[i][1] * k + f, wallHeight);
                //------------------------------------------------
                glVertex3f((a->walls[i][0] + 1) * k, a->walls[i][1] * k - f, 0.0);
                glVertex3f((a->walls[i][0] + 1) * k, a->walls[i][1] * k + f, 0.0);
                glVertex3f((a->walls[i][0] + 1) * k, a->walls[i][1] * k + f, wallHeight);

                glVertex3f((a->walls[i][0] + 1) * k, a->walls[i][1] * k + f, wallHeight);
                glVertex3f((a->walls[i][0] + 1) * k, a->walls[i][1] * k - f, wallHeight);
                glVertex3f((a->walls[i][0] + 1) * k, a->walls[i][1] * k - f, 0.0);
            } else {
                glVertex3f(a->walls[i][0] * k - f, a->walls[i][1] * k, 0.0);
                glVertex3f(a->walls[i][0] * k - f, (a->walls[i][1] + 1) * k, 0.0);
                glVertex3f(a->walls[i][0] * k - f, (a->walls[i][1] + 1) * k, wallHeight);

                glVertex3f(a->walls[i][0] * k + f, a->walls[i][1] * k, 0.0);
                glVertex3f(a->walls[i][0] * k + f, (a->walls[i][1] + 1) * k, 0.0);
                glVertex3f(a->walls[i][0] * k + f, (a->walls[i][1] + 1) * k, wallHeight);
                //-------------------------------------------------
                glVertex3f(a->walls[i][0] * k + f, a->walls[i][1] * k, 0.0);
                glVertex3f(a->walls[i][0] * k + f, a->walls[i][1] * k, wallHeight);
                glVertex3f(a->walls[i][0] * k - f, a->walls[i][1] * k, wallHeight);

                glVertex3f(a->walls[i][0] * k - f, a->walls[i][1] * k, wallHeight);
                glVertex3f(a->walls[i][0] * k + f, a->walls[i][1] * k, 0.0);
                glVertex3f(a->walls[i][0] * k - f, a->walls[i][1] * k, 0.0);
                //-------------------------------------------------
                glVertex3f(a->walls[i][0] * k - f, (a->walls[i][1] + 1) * k, wallHeight);
                glVertex3f(a->walls[i][0] * k - f, a->walls[i][1] * k, 0.0);
                glVertex3f(a->walls[i][0] * k - f, a->walls[i][1] * k, wallHeight);

                glVertex3f(a->walls[i][0] * k + f, (a->walls[i][1] + 1) * k, wallHeight);
                glVertex3f(a->walls[i][0] * k + f, a->walls[i][1] * k, 0.0);
                glVertex3f(a->walls[i][0] * k + f, a->walls[i][1] * k, wallHeight);
                //---------------------------------------------------
                glVertex3f(a->walls[i][0] * k + f, (a->walls[i][1] + 1) * k, 0.0);
                glVertex3f(a->walls[i][0] * k + f, (a->walls[i][1] + 1) * k, wallHeight);
                glVertex3f(a->walls[i][0] * k - f, (a->walls[i][1] + 1) * k, wallHeight);

                glVertex3f(a->walls[i][0] * k - f, (a->walls[i][1] + 1) * k, wallHeight);
                glVertex3f(a->walls[i][0] * k + f, (a->walls[i][1] + 1) * k, 0.0);
                glVertex3f(a->walls[i][0] * k - f, (a->walls[i][1] + 1) * k, 0.0);
            }
        }

        qglColor(Qt::black);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 1, 0);
        glVertex3f(1, 0, 0);

        glVertex3f(0, 1, 0);
        glVertex3f(1, 0, 0);
        glVertex3f(1, 1, 0);
    glEnd();
*/


    loadTexture(textures[0]);
    glBegin(GL_QUADS);
        for (int i = 0; i < a->m; i++) {
            double x = a->walls[i][0] * k;
            double y = a->walls[i][1] * k;
//            glColor3ub((a->walls[i][0] + 1) * 10, (a->walls[i][1] + 1) * 10, (a->walls[i][2] + 1) * 100);

            if (a->walls[i][2] == 0) {
                drawQuad(x, y - f, x + k, y - f, false);
                drawQuad(x + k, y + f, x, y + f, false);
            } else {
                drawQuad(x + f, y, x + f, y + k, false);
                drawQuad(x - f, y + k, x - f, y, false);
            }
        }
        glVertex3f(0, 0 ,0);
        glVertex3f(k * a->n, 0, 0);
        glVertex3f(k * a->n, k * a->n, 0);
        glVertex3f(0, k * a->n, 0);
    glEnd();

    loadTexture(textures[1]);

    glBegin(GL_QUADS);
        for (int i = 0; i < a->m; i++) {
            double x = a->walls[i][0] * k;
            double y = a->walls[i][1] * k;
//            glColor3ub((a->walls[i][0] + 1) * 10, (a->walls[i][1] + 1) * 10, (a->walls[i][2] + 1) * 100);

            if (a->walls[i][2] == 0) {
                drawQuad(x, y + f, x, y - f, true);
                drawQuad(x + k, y - f, x + k, y + f, true);
            } else {
                drawQuad(x - f, y, x + f, y, true);
                drawQuad(x + f, y + k, x - f, y + k, true);
            }
        }
    glEnd();


  /*  glBegin(GL_POINTS);
        glColor3f(0.0f, 1.0f, 0.0f);
        for (int i = 0; i < a->numberArsenals; i++)
            glVertex3f(a->arsenal[i].x() * k + k / 2, a->arsenal[i].y() * k + k / 2, wallHeight / 2);


    glEnd();*/

    qglColor(QColor(0, 0, 250));
    for (int i = 0; i < a->numberArsenals; i++)
        renderText(a->arsenal[i].x() * k + k / 2, a->arsenal[i].y() * k + k / 2, wallHeight / 2, "Arsenal");

    renderText(a->hospital.x() * k + k / 2, a->hospital.y() * k + k / 2, wallHeight / 2, "Hospital");
    for (int i = 0; i < a->otherHeroes; i++)
        if ((a->heroes[i].x() != -1) || (a->heroes[i].y()) != -1) {
//            glVertex3f(a->heroes[i].x() * k + k / 2, a->heroes[i].y() * k + k / 2, wallHeight / 2);
            qglColor(QColor(0, a->otherAlive[i] * 200, 0));
            renderText(a->heroes[i].x() * k + k / 2, a->heroes[i].y() * k + k / 2, wallHeight / 2, a->heroNames[i], hudFont);
        }
//    paintEngine()->drawEllipse(QRect(0, 0, 10, 10));
//    paintEngine()->setActive(true);
//    qDebug() << paintEngine()->isActive();
//    paintEngine()->drawEllipse(QRect(1, 1, 10, 10));

    qglColor(QColor(200, 150, 0));

    renderText(5, this->height() - 20, QString("Alive status: " + QString::number(a->alive)), hudFont);
    renderText(5, this->height() - 40, QString("patrons: ") + QString::number(a->patrons), hudFont);
    renderText(5, this->height() - 60, QString("walls: ") + QString::number(a->wall), hudFont);
    renderText(5, this->height() - 80, QString("destroy: ") + QString::number(a->destroy), hudFont);
    renderText(5, this->height() - 100, QString("debug: ") + QString::number(a->coord.x()) + " " + QString::number(a->coord.y()));
    renderText(this->width() - 60, 10, QString("FPS: ") + QString::number(oldFps));

    qglColor(Qt::red);
    if (enteringText)
        renderText(5, this->height() - 120, "-" + currentText, hudFont);

    glLineWidth(1);

    drawText(f + eps, f + eps, wallHeight / 2, false, true, QString::fromLocal8Bit("Добро Пыжаловать!!!"));
//    drawText(k - 2 * f, 2 * f, wallHeight / 2, true, false, QString("Welcome to SuperMaze on x"));

    if (startingGame)
        renderText(this->width() / 2 - 100, this->height() / 2, QString("Starting after ") + QString::number((3000 - startAfter) / 1000) + QString(" seconds"), hudFont);
}

void DrawGl::onx() {
//    xRot += 1;
    zRot += 1;
    repaint();
}

void DrawGl::drawFPS() {
    if (rand() % 100 == 0) {
        qDebug() << "current fps:" << fps;
        qDebug() << "current opengl error:" << glGetError();
    }
    oldFps = fps;
    fps = 0;
}

void DrawGl::keyPressEvent(QKeyEvent *event) {
    if (event->key() == (Qt::Key_Enter xor 1)) {
        enteringText = !enteringText;
        if (!enteringText)
            processText();

        currentText = "";
        return;
    } else if (enteringText)
        if (event->key() == Qt::Key_Backspace)
            currentText = currentText.left(currentText.length() - 1);
        else
            currentText += event->text();
    else {
        a->keyPressEvent(event);
    }

    event->accept();
}

void DrawGl::keyReleaseEvent(QKeyEvent *event) {
    a->keyReleaseEvent(event);
}

/*void DrawGl::mousePressEvent(QMouseEvent *event) {
//    a->taskKill();
    event->accept();
}

void DrawGl::mouseReleaseEvent(QMouseEvent *event) {
    event->accept();
}*/

/*void DrawGl::wheelEvent(QWheelEvent *event) {
    if (event->delta() > 0)
        a->fgup();
    else
        a->fgdown();
}*/

/*void DrawGl::mouseMoveEvent(QMouseEvent *event) {
    if (botActive)
        return;

    if (firstMouseMove) {
        lastClickX = event->x();
        lastClickY = event->y();
        firstMouseMove = false;
        return;
    }

    plMouseXRot = (event->x() - lastClickX);

    while (plMouseXRot > 45) {
        plMouseXRot -= 90;
        zRot += 90;
        lastClickX += 90;
        a->nap = a->lefter();
    }

    while (plMouseXRot < -45) {
        plMouseXRot += 90;
        zRot -= 90;
        lastClickX -= 90;
        a->nap = a->righter();
    }
}*/


void DrawGl::loadTexture(GLuint a) {
    glBindTexture(GL_TEXTURE_2D, a);

 /*   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // задана линейная фильтрация вблизи
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // задана линейная фильтрация вдали
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // при фильтрации игнорируются тексели, выходящие за границу текстуры для s координаты
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // при фильтрации игнорируются тексели, выходящие за границу текстуры для t координаты
*/}

void DrawGl::drawText(double x, double y, double z, bool xForwarding, bool yForwarding, QString s) {
    QPainterPath a;
    a.addText(0, 20, QFont("FreeSans", 10), s);
//    qDebug() << s;
    QList<QPolygonF> l = a.toSubpathPolygons();
    int k = 0;
    for (QList<QPolygonF>::iterator i = l.begin(); i != l.end(); i++) {
        glBegin(GL_LINE_STRIP);
            for (QPolygonF::iterator j = (*i).end(); j != (*i).begin();) {
                --j;
                glVertex3f(j->rx() * 0.0005f * xForwarding + x, j->rx() * 0.0005f * yForwarding + y, -j->ry() * 0.0005f + z);
                k++;
            }
        glEnd();
    }
//    qDebug() << k << "points";
}

void DrawGl::processText() {
    currentText = currentText.toUpper();
    if (currentText == "EXIT") {
        a->deleteLater();
        this->deleteLater();
    } else/* if (currentText == "BOT") {
        a->startBot();
    }*/

    qDebug() << currentText << "processed";
}
