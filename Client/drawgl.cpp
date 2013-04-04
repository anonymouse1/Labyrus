#include "drawgl.h"
#include <QtOpenGL/QGLWidget>
#include <GL/glu.h>

DrawGl::DrawGl(QApplication *app, QString skin, double mouse, QWidget *parent) :
    QGLWidget(parent)
{
    application = app;
    skinPath = skin;
    enteringText = false;
    xRot = -90;
    yRot = 0;
    zRot = 0;
    perspective = 45;

    animZRot = 0;
    fps = 0;
    oldFps = 0;

    nSca = 1;

    xtra = ytra = 0;
    ztra = -wallHeight / 2;
    startingGame = false;

    hudFont = QFont("Monospace", 15, 20, true);
    menuFont = QFont("Monospace", 30, 40, true);

    timeFPS = new QTimer;
    timeFPS->setInterval(1000);
    startAfter = 666;

    QObject::connect(timeFPS, SIGNAL(timeout()), this, SLOT(drawFPS()));
    timeFPS->start();


    firstMouseMove = true;
    botActive = false;
    mousePressed = false;

    compass = new QPixmap(skinPath + "/compass.png");
    needRefreshCursor = true;
    mouseSensitivity = mouse;
    activePoint = 0;

    QSettings s(settingsFile, QSettings::IniFormat);
    restoreGeometry(s.value("widgetGeometry").toByteArray());
    QCursor::setPos(width() / 2, height() / 2);
    setCursor(QCursor(Qt::BlankCursor));
    setMouseTracking(true);
}

void DrawGl::initializeGL() {
    qglClearColor(Qt::gray);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    textures[0] = bindTexture(QPixmap(skinPath + "/defaultWall.jpg"), GL_TEXTURE_2D);
    textures[1] = bindTexture(QPixmap(skinPath + "/shortWall.jpg"), GL_TEXTURE_2D);
    textures[2] = bindTexture(QPixmap(skinPath + "/horizontalSlice.jpg"), GL_TEXTURE_2D);
    textures[3] = bindTexture(QPixmap(skinPath + "/floor.jpg"), GL_TEXTURE_2D);
    textures[4] = bindTexture(QPixmap(skinPath + "/compass.png"), GL_TEXTURE_2D);
    textures[5] = bindTexture(QPixmap(skinPath + "/sky.jpg"), GL_TEXTURE_2D);
    textures[6] = bindTexture(QPixmap(skinPath + "/model.jpg"), GL_TEXTURE_2D);
    textures[7] = bindTexture(QPixmap(skinPath + "/roof.jpg"), GL_TEXTURE_2D);
    textures[8] = bindTexture(QPixmap(skinPath + "/icon.png"), GL_TEXTURE_2D);
    textures[9] = bindTexture(QPixmap(skinPath + "/blackout.png"), GL_TEXTURE_2D);
    textures[10] = bindTexture(QPixmap(skinPath + "/hudbackground.png"), GL_TEXTURE_2D);
    textures[11] = bindTexture(QPixmap(skinPath + "/okular.png"), GL_TEXTURE_2D);
    textures[12] = bindTexture(QPixmap(skinPath + "/verticalSlice.jpg"), GL_TEXTURE_2D);

    I = new Model(skinPath + "/simple.s3d");

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_2D);

    glFrontFace(GL_CCW);
    glPointSize(10);
}

void DrawGl::resizeGL(int w, int h) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, (GLint)w, (GLint)h);
    gluPerspective(perspective, w / (double)h, 0.001, 1000000.0);
    if (isFullScreen())
        QCursor::setPos(w / 2, h / 2);

    menuFont = QFont("FreeSans", min(30, min(h, w) / 30), 40, true);
    k = 1.0 / sizeView;
    f = k / 10;
    needRefreshCursor = true;
}

void DrawGl::paintGL() {
    if (a->radiation) {
        a->radiation = false;
        if (perspective < 150)
            perspective++;
           this->resizeGL(this->width(), this->height());
    }
    fps++;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(a->yAngle, 1.0f, 0.0f, 0.0f);
    glRotatef(yRot, 0.0f, 1.0f, 0.0f);
    glRotatef(a->angle, 0.0f, 0.0f, 1.0f);

    nSca = a->n / 2.0;
    glScalef(nSca, nSca, nSca);
    glTranslatef(-a->coord.x / sizeView, -a->coord.y / sizeView, ztra - a->coord.h / sizeView);

//    drawAxis();
    enableLight();
    drawSkyBox();
    drawMaze();
    drawHeroes();
    if (!legacy->ctrlPressed) {
        drawCompass();
        drawHUD();
    }

    if (a->escapeMode)
        drawMenu();
    if (legacy->ctrlPressed)
        drawOkular();

    if (botActive)
        drawBotLast();
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

    renderText(0.0, 0.0, 1.0, QString::number(1));
    renderText(0.0, 1.0, 0.0, QString::number(1));
    renderText(1.0, 0.0, 0.0, QString::number(1));
}

void DrawGl::enableLight() {
        float dir[3] = {0, 0, -1};
//    GLfloat pos[4] = {(float)(a->coord.x()), (float)(a->coord.y()), -wallHeight / 2, 1.0f};
        GLfloat pos[4] = {0.5, 0.5, 0.01, 1.0f};
        GLfloat color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
        GLfloat mat_specular[4] = {1.0f, 1.0f, 1.0f, 1.0f};
        GLfloat ambientLight[] = {0.1f, 0.1f, 0.1f, 0.8f};


        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

        glLightfv(GL_LIGHT0, GL_POSITION, pos);
        glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);
        glLightfv(GL_LIGHT0, GL_AMBIENT_AND_DIFFUSE, color);
        glEnable(GL_LIGHT0);



        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_specular);
        glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

void DrawGl::drawSkyBox() {
    loadTexture(textures[5]);
    glBegin(GL_QUADS);
   /*     for (int i = 0; i < a->n; i++)
            for (int j = 0; j < a->n; j++) {
                glVertex3f((i + 1) / sizeView, j / sizeView, wallHeight);
                glTexCoord2d(1, 1);
                glVertex3f(i / sizeView, j / sizeView, wallHeight);
                glTexCoord2d(0, 1);
                glVertex3f(i / sizeView, (j + 1) / sizeView, wallHeight);
                glTexCoord2d(0, 0);
                glVertex3f((i + 1) / sizeView, (j + 1) / sizeView, wallHeight);
                glTexCoord2d(1, 0);
            }*/
        glVertex3f(50, -50, 10);
        glTexCoord2d(1, 1);
        glVertex3f(-50, -50, 10);
        glTexCoord2d(0, 1);
        glVertex3f(-50, 50, 10);
        glTexCoord2d(0, 0);
        glVertex3f(50, 50, 10);
        glTexCoord2d(1, 0);
    glEnd();
}

void DrawGl::drawQuad(double x1, double y1, double x2, double y2, double h, double height) {
    glVertex3f(x1, y1, h + height);
    glTexCoord2d(0, 0);
    glVertex3f(x1, y1, h);
    glTexCoord2d(1, 0);
    glVertex3f(x2, y2, h);
    glTexCoord2d(1, 1);
    glVertex3f(x2, y2, h + height);
    glTexCoord2d(0, 1);
}

void DrawGl::drawRoofPart(double x, double y, double h, int type, bool b) {
    if (type == 1) {
        if (b) {
            glVertex3f(x - f, y + k, h + eps);
            glTexCoord2d(1, 0);
            glVertex3f(x - f, y, h + eps);
            glTexCoord2d(1, 1);
            glVertex3f(x + f, y, h + eps);
            glTexCoord2d(0, 1);
            glVertex3f(x + f, y + k, h + eps);
            glTexCoord2d(0, 0);
        } else {
            glVertex3f(x + f, y, h - eps);
            glTexCoord2d(0, 1);
            glVertex3f(x - f, y, h - eps);
            glTexCoord2d(1, 1);
            glVertex3f(x - f, y + k, h - eps);
            glTexCoord2d(1, 0);
            glVertex3f(x + f, y + k, h - eps);
            glTexCoord2d(0, 0);
        }
    } else if (type == 0) {
        if (b) {
            glVertex3f(x + k, y + f, h + 2 * eps);
            glTexCoord2d(1, 0);
            glVertex3f(x, y + f, h + 2 * eps);
            glTexCoord2d(1, 1);
            glVertex3f(x, y - f, h + 2 * eps);
            glTexCoord2d(0, 1);
            glVertex3f(x + k, y - f, h + 2 * eps);
            glTexCoord2d(0, 0);
        } else {
            glVertex3f(x, y + f, h - 2 * eps);
            glTexCoord2d(1, 0);
            glVertex3f(x + k, y + f, h - 2 * eps);
            glTexCoord2d(1, 1);
            glVertex3f(x + k, y - f, h - 2 * eps);
            glTexCoord2d(0, 1);
            glVertex3f(x, y - f, h - 2 * eps);
            glTexCoord2d(0, 0);
        }
    }
}

void DrawGl::drawMaze() {
    qglColor(Qt::white);
    loadTexture(textures[defaultWall]);
    glBegin(GL_QUADS);
        for (int i = 0; i < a->m; i++) {
            double x = a->walls[i][0] * k;
            double y = a->walls[i][1] * k;
            double h = a->walls[i][2] * wallHeight;
            if (a->walls[i][3] == 0) {
                drawQuad(x, y - f, x + k, y - f, h, wallHeight);
                drawQuad(x + k, y + f, x, y + f, h, wallHeight);
            } else if (a->walls[i][3] == 1) {
                drawQuad(x + f, y, x + f, y + k, h, wallHeight);
                drawQuad(x - f, y + k, x - f, y, h, wallHeight);
            }
        }
    glEnd();

    loadTexture(textures[verticalSlice]);
    glBegin(GL_QUADS);
        for (int i = 0; i < a->m; i++) {
            double x = a->walls[i][0] * k;
            double y = a->walls[i][1] * k;
            double h = a->walls[i][2] * wallHeight;
            if ((a->walls[i][3] == 2) && (a->h != 1)) {
                drawQuad(x, y - eps, x + k, y - eps, h - f, f * 2);
                drawQuad(x - eps, y + k, x - eps, y, h - f, f * 2);
                drawQuad(x + k + eps, y, x + k + eps, y + k, h - f, f * 2);
                drawQuad(x + k, y + k + eps, x, y + k + eps, h - f, f * 2);
            }
        }
    glEnd();

    loadTexture(textures[shortWall]);
    glBegin(GL_QUADS);
        for (int i = 0; i < a->m; i++) {
            double x = a->walls[i][0] * k;
            double y = a->walls[i][1] * k;
            double h = a->walls[i][2] * wallHeight;
            if (a->walls[i][3] == 0) {
                drawQuad(x, y + f, x, y - f, h, wallHeight);
                drawQuad(x + k, y - f, x + k, y + f, h, wallHeight);
            } else if (a->walls[i][3] == 1) {
                drawQuad(x - f, y, x + f, y, h, wallHeight);
                drawQuad(x + f, y + k, x - f, y + k, h, wallHeight);
            }
        }
    glEnd();

    loadTexture(textures[horizontalSlice]);
    glBegin(GL_QUADS);
        for (int i = 0; i < a->m; i++) {
            double x = a->walls[i][0] * k;
            double y = a->walls[i][1] * k;
            double h = a->walls[i][2] * wallHeight;
            drawRoofPart(x, y, h, a->walls[i][3], false);
            drawRoofPart(x, y, h + wallHeight, a->walls[i][3], true);
        }
    glEnd();

    loadTexture(textures[3]);
    glBegin(GL_QUADS);
        for (int i = 0; i < a->n; i++)
            for (int j = 0; j < a->n; j++) {
                glVertex3f(i * k, (j + 1) * k, -eps);
                glTexCoord2d(1, 1);
                glVertex3f(i * k, j * k, -eps);
                glTexCoord2d(0, 1);
                glVertex3f((i + 1) * k, j * k, -eps);
                glTexCoord2d(0, 0);
                glVertex3f((i + 1) * k, (j + 1) * k, -eps);
                glTexCoord2d(1, 0);
            }

    glEnd();

    loadTexture(textures[roof]);
    glBegin(GL_QUADS);
        for (int i = 0; i < a->m; i++)
            if ((a->walls[i][3] == 2) && (a->h != 1))
                drawFloorPoint(a->walls[i][0], a->walls[i][1], a->walls[i][2], false);

    glEnd();

    loadTexture(textures[Floor]);
    glBegin(GL_QUADS);
        for (int i = 0; i < a->m; i++)
            if ((a->walls[i][3] == 2) && (a->h != 1))
                drawFloorPoint(a->walls[i][0], a->walls[i][1], a->walls[i][2], true);
    glEnd();


    qglColor(Qt::blue);

    glLineWidth(1);
    drawText(f + eps, f + eps, wallHeight / 2, false, true, QString::fromLocal8Bit("Добро Пыжаловать!!!"));
//    drawText(k - 2 * f, 2 * f, wallHeight / 2, true, false, QString("Welcome to SuperMaze on x"));

    loadTexture(textures[model]);
    if (a->h == 1)
        if (a->coord.h > 1)
            I->draw(1 / sizeView / 10, a->coord.x * k, a->coord.y * k, wallHeight / 3);
    if (startingGame)
        renderText(this->width() / 2 - 100, this->height() / 2, tr("Starting after ") + QString::number((3000 - startAfter) / 1000) + QString(" seconds"), hudFont);
}

void DrawGl::onx() {
//    xRot += 1;
    zRot += 1;
    repaint();
}

void DrawGl::drawFPS() {
    oldFps = fps;
    fps = 0;
}

void DrawGl::keyPressEvent(QKeyEvent *event) {
    if (startingGame && (event->key() == Qt::Key_Escape))
        return;

    int key = event->key();
    if (a->escapeMode) {
        if ((activePoint == 0) && ((key == Qt::Key_Return) || (key == Qt::Key_Right)))
            a->escapeMode = false;
        if ((activePoint == 2) && ((key == Qt::Key_Return) || (key == Qt::Key_Right) )) {
            if (botActive)
                legacy->stopBot = true;
            else
                legacy->startBot();
        }

        if ((activePoint == 1) && ((key == Qt::Key_Return) || (key == Qt::Key_Right))) {
            if (isFullScreen())
                showNormal();
            else
                showFullScreen();
        }

        if ((activePoint == 4) && ((key == Qt::Key_Return) || (key == Qt::Key_Right)))
            legacy->legalStop();
        if ((activePoint == 3) && ((key == Qt::Key_Plus) || (key == Qt::Key_Right)))
            mouseSensitivity += 0.02;
        if ((activePoint == 3) && ((key == Qt::Key_Minus) || (key == Qt::Key_Left)))
            mouseSensitivity -= 0.02;

        if (mouseSensitivity < eps)
            mouseSensitivity = 0;

        if (key == Qt::Key_Up)
            activePoint = (activePoint - 1 + 5) % 5;

        if (key == Qt::Key_Down)
            activePoint = (activePoint + 1) % 5;

        if (key == Qt::Key_Escape)
            a->escapeMode = false;

        return;
    }

    if (event->key() == Qt::Key_Return) {
        enteringText = !enteringText;
        if (!enteringText)
            processText();

        currentText = "";
        return;
    } else if (enteringText) {
        if (event->key() == Qt::Key_Backspace)
            currentText = currentText.left(currentText.length() - 1);
        else if (event->key() == Qt::Key_Escape) {
            currentText = "";
            enteringText = false;
        } else if (currentText.length() < 30) {
            currentText += event->text();
        }
    } else {
        legacy->keyPressEvent(event);
    }

    event->accept();
}

void DrawGl::keyReleaseEvent(QKeyEvent *event) {
    legacy->keyReleaseEvent(event);
}

void DrawGl::mousePressEvent(QMouseEvent *event) {
    mousePressed = true;
    event->accept();
}

void DrawGl::mouseReleaseEvent(QMouseEvent *event) {
    mousePressed = false;
    event->accept();
}

void DrawGl::mouseMoveEvent(QMouseEvent *event) {
    if (startingGame)
        return;

    if (botActive || (!this->isFullScreen()))
        return;

    double x = (event->x() - width() / 2) * mouseSensitivity;
    double y = (event->y() - height() / 2)  * mouseSensitivity;

    QCursor::setPos(width() / 2, height() / 2);

    a->angle += x;
    a->yAngle += y;
    a->checkAngles();
    event->accept();
}


void DrawGl::loadTexture(GLuint a) {
    glBindTexture(GL_TEXTURE_2D, a);
}

void DrawGl::drawText(double x, double y, double z, bool xForwarding, bool yForwarding, QString s) {
    QPainterPath a;
    a.addText(0, 20, QFont("FreeSans", 10), s);
    QList<QPolygonF> l = a.toSubpathPolygons();
        for (QList<QPolygonF>::iterator i = l.begin(); i != l.end(); i++) {
        glBegin(GL_LINE_STRIP);
            for (QPolygonF::iterator j = (*i).begin(); j != (*i).end(); j++) {
                qglColor(Qt::red);
                glVertex3f(j->rx() * 0.0005f * xForwarding + x, j->rx() * 0.0005f * yForwarding + y, -j->ry() * 0.0005f + z);
            }
        glEnd();
    }
}

void DrawGl::processText() {
    if (currentText == "")
        return;
    emit runCommand("I\n" + currentText);
    currentText = currentText.toUpper();
    if (currentText == "EXIT") {
        legacy->legalStop();
    } else if (currentText == "BOT") {
        legacy->startBot();
    } else if (currentText == "STOP") {
        legacy->stopBot = true;
    } else if (currentText == "PING") {
        emit runCommand("p\n");
        a->pingTime = new QTime;
        a->pingTime->start();
    } else if (currentText == "HELP") {
        a->messages->addMessage("Possible commands: help bot stop ping exit...");
    }

    qDebug() << currentText << "processed";
}

void DrawGl::begin2d() {
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, this->width(), 0, this->height());
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
}

void DrawGl::end2d() {
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
}

QPixmap DrawGl::generateCompass(double angle) {
    QPixmap res(400, 400);
    res.fill(Qt::transparent);

    QPainter p;
    p.begin(&res);

    QTransform transform;
    transform.translate(199, 199);
    transform.rotate(angle);
    p.setTransform(transform);

    p.drawPixmap(-200, -200, QPixmap(skinPath + "/compass.png"));
    p.end();
    return res;
}

void DrawGl::drawFloorPoint(double x, double y, double h, bool b) {
    if (b) {
        glVertex3f((x + 1) * k, (y + 1) * k, h * k + k / 10);
        glTexCoord2d(1, 1);
        glVertex3f(x * k, (y + 1) * k, h * k + k / 10);
        glTexCoord2d(0, 1);
        glVertex3f(x * k, y * k, h * k + k / 10);
        glTexCoord2d(0, 0);
        glVertex3f((x + 1) * k, y * k, h * k + k / 10);
        glTexCoord2d(1, 0);
    } else {
        glVertex3f((x + 1) * k, (y + 1) * k, h * k - k / 10);
        glTexCoord2d(1, 1);
        glVertex3f((x + 1) * k, y * k, h * k - k / 10);
        glTexCoord2d(1, 0);
        glVertex3f(x * k, y * k, h * k - k / 10);
        glTexCoord2d(0, 0);
        glVertex3f(x * k, (y + 1) * k, h * k - k / 10);
        glTexCoord2d(0, 1);
    }
}

void DrawGl::drawHeroes() {
    qglColor(Qt::black);
    loadTexture(textures[model]);
    for (int i = 0; i < a->otherHeroes; i++)
        if ((a->heroes[i].x != -1) || (a->heroes[i].y) != -1) {
            qglColor(QColor(0, a->otherAlive[i] * 200, 0));

            renderText(a->heroes[i].x * k, a->heroes[i].y * k, a->heroes[i].h * k + 3 * f, a->heroNames[i], hudFont);
            I->draw(1 / sizeView / 10, a->heroes[i].x * k, a->heroes[i].y * k, a->heroes[i].h * k);
        }
}

void DrawGl::drawCompass() {
    deleteTexture(textures[4]);
    textures[4] = bindTexture(generateCompass(a->angle), GL_TEXTURE_2D);
    begin2d();
    loadTexture(textures[4]);
    glBegin(GL_QUADS);
        glVertex2d(this->width() - 200, 0);
        glTexCoord2d(0, 0);
        glVertex2d(this->width(), 0);
        glTexCoord2d(0, 1);
        glVertex2d(this->width(), 200);
        glTexCoord2d(1, 1);
        glVertex2d(this->width() - 200, 200);
        glTexCoord2d(1, 0);
    glEnd();
    end2d();
}

void DrawGl::drawHUD() {
    begin2d();
    loadTexture(textures[blackout]);
    glBegin(GL_QUADS);
        glVertex2d(-100, -156);
        glTexCoord2d(0, 0);
        glVertex2d(156, -156);
        glTexCoord2d(1, 0);
        glVertex2d(156, 156);
        glTexCoord2d(1, 1);
        glVertex2d(-100, 156);
        glTexCoord2d(0, 1);

        glVertex2d(this->width() - 64, this->height() - 32);
        glTexCoord2d(0, 0);
        glVertex2d(this->width(), this->height() - 32);
        glTexCoord2d(1, 0);
        glVertex2d(this->width(), this->height() + 32);
        glTexCoord2d(1, 1);
        glVertex2d(this->width() - 64, this->height() + 32);
        glTexCoord2d(0, 1);

        glVertex2d(-80, this->height() - 56);
        glTexCoord2d(0, 0);
        glVertex2d(196, this->height() - 56);
        glTexCoord2d(1, 0);
        glVertex2d(196, this->height() + 200);
        glTexCoord2d(1, 1);
        glVertex2d(-80, this->height() + 200);
        glTexCoord2d(0, 1);
    glEnd();
    end2d();
    qglColor(Qt::green);
    renderText(5, 15, tr("Elapsed: ") + QString::number(legacy->thread->fromStartOfGame.elapsed() / 1000) + QString("s"), hudFont);
    renderText(5, this->height() - 20, tr("Alive: ") + QString::number(a->alive), hudFont);
    renderText(5, this->height() - 40, tr("patrons: ") + QString::number(a->patrons), hudFont);
    renderText(5, this->height() - 60, tr("walls: ") + QString::number(a->wall), hudFont);
    renderText(5, this->height() - 80, tr("destroy: ") + QString::number(a->destroy), hudFont);
    renderText(5, this->height() - 100, tr("Floor №") + QString::number(a->getFloor()), hudFont);
    renderText(this->width() - 60, 10, QString("FPS: ") + QString::number(oldFps));

    qglColor(Qt::red);

    QList<QString> list = a->messages->getMessages();
    begin2d();
    loadTexture(textures[hudbackground]);
    glBegin(GL_QUADS);
        if (enteringText) {
            glVertex2d(-50 - currentText.length() * 13, this->height() / 2 - 85 - 6);
            glTexCoord2d(0, 0);
            glVertex2d(50 + currentText.length() * 13, this->height() / 2 - 85 - 6);
            glTexCoord2d(1, 0);
            glVertex2d(50 + currentText.length() * 13, this->height() / 2 - 85 + 15);
            glTexCoord2d(1, 1);
            glVertex2d(-50 - currentText.length() * 13, this->height() / 2 - 85 + 15);
            glTexCoord2d(0, 1);
        }
        for (int i = 0; i < list.size(); i++) {
            glVertex2d(-50 - list[i].length() * 13, this->height() / 2 + (list.size() - i) * 20 - 6 - 85);
            glTexCoord2d(0, 0);
            glVertex2d(50 + list[i].length() * 13, this->height() / 2 + (list.size() - i) * 20 - 6 - 85);
            glTexCoord2d(1, 0);
            glVertex2d(50 + list[i].length() * 13, this->height() / 2 + (list.size() - i) * 20 + 15 - 85);
            glTexCoord2d(1, 1);
            glVertex2d(-50 - list[i].length() * 13, this->height() / 2 + (list.size() - i) * 20 + 15 - 85);
            glTexCoord2d(0, 1);
        }
    glEnd();
    end2d();
    if (enteringText)
        renderText(5, this->height() / 2 + 85, "-" + currentText, hudFont);

    loadTexture(textures[2]);
    for (int i = 0; i < list.size(); i++)
        renderText(5, this->height() / 2 + 85 - 20 * (list.size() - i), list[i], hudFont);
}

void DrawGl::drawMenu() {
    begin2d();
    loadTexture(textures[hudbackground]);
    glBegin(GL_QUADS);
    for (int i = 0; i < 2; i++) {
        glVertex2d(this->width() / 2 - max(320, this->width() / 5 * 2), this->height() / 2 - 240);
        glTexCoord2d(0, 0);
        glVertex2d(this->width() / 2 + max(320, this->width() / 5 * 2), this->height() / 2 - 240);
        glTexCoord2d(1, 0);
        glVertex2d(this->width() / 2 + max(320, this->width() / 5 * 2), this->height() / 2 + 240);
        glTexCoord2d(1, 1);
        glVertex2d(this->width() / 2 - max(320, this->width() / 5 * 2), this->height() / 2 + 240);
        glTexCoord2d(0, 1);
    }
    glEnd();
    loadTexture(textures[icon]);
    glBegin(GL_QUADS);
        glVertex2d(this->width() / 2 - 240, this->height() / 2 - activePoint * 50 - 16 + 100);
        glTexCoord2d(0, 0);
        glVertex2d(this->width() / 2 - 240 + 64, this->height() / 2 - activePoint * 50 - 16 + 100);
        glTexCoord2d(1, 0);
        glVertex2d(this->width() / 2 - 240 + 64, this->height() / 2 - activePoint * 50 + 48 + 100);
        glTexCoord2d(1, 1);
        glVertex2d(this->width() / 2 - 240, this->height() / 2 - activePoint * 50 + 48 + 100);
        glTexCoord2d(0, 1);
    glEnd();
    end2d();

    renderText(this->width() / 2 - 175, this->height() / 2 - 100, tr("Return"), menuFont);
    QString fullscreenActive;
    if (isFullScreen())
        fullscreenActive = tr("on");
    else
        fullscreenActive = tr("off");

    renderText(this->width() / 2 - 175, this->height() / 2 - 50, tr("FullScreen: ") + fullscreenActive, menuFont);

    if (botActive)
        renderText(this->width() / 2 - 175, this->height() / 2, tr("Stop"), menuFont);
    else
        renderText(this->width() / 2 - 175, this->height() / 2, tr("BOT"), menuFont);

    renderText(this->width() / 2 - 175, this->height() / 2 + 50, tr("Mouse Sensitivity: ") + QString::number(mouseSensitivity), menuFont);
    renderText(this->width() / 2 - 175, this->height() / 2 + 100, tr("Exit?"), menuFont);
}

void DrawGl::drawOkular() {
    int r = min(width() / 2, height() / 2) * 2;
    loadTexture(textures[okular]);
    begin2d();
    glBegin(GL_QUADS);
        glVertex2d(this->width() / 2 - r, this->height() / 2 - r);
        glTexCoord2d(0, 0);
        glVertex2d(this->width() / 2 + r, this->height() / 2 - r);
        glTexCoord2d(1, 0);
        glVertex2d(this->width() / 2 + r, this->height() / 2 + r);
        glTexCoord2d(1, 1);
        glVertex2d(this->width() / 2 - r, this->height() / 2 + r);
        glTexCoord2d(0, 1);

        glVertex2d(this->width() / 2 - 10 * r, this->height() / 2 - r);
        glTexCoord2d(0, 0);
        glVertex2d(this->width() / 2 - r, this->height() / 2 - r);
        glTexCoord2d(1, 0);
        glVertex2d(this->width() / 2 - r, this->height() / 2 + r);
        glTexCoord2d(1, 1);
        glVertex2d(this->width() / 2 - 10 * r, this->height() / 2 + r);
        glTexCoord2d(0, 1);


        glVertex2d(this->width() / 2 + r, this->height() / 2 - r);
        glTexCoord2d(0, 0);
        glVertex2d(this->width() / 2 + 10 * r, this->height() / 2 - r);
        glTexCoord2d(1, 0);
        glVertex2d(this->width() / 2 + 10 * r, this->height() / 2 + r);
        glTexCoord2d(1, 1);
        glVertex2d(this->width() / 2 + r, this->height() / 2 + r);
        glTexCoord2d(0, 1);
    glEnd();
    end2d();
}

void DrawGl::drawBotLast() {
    loadTexture(textures[hudbackground]);
    begin2d();
    glBegin(GL_QUADS);
        glVertex2d(this->width() - 100, this->height() / 2 - 20);
        glTexCoord2d(0, 0);
        glVertex2d(this->width() + 100, this->height() / 2 - 20);
        glTexCoord2d(1, 0);
        glVertex2d(this->width() + 100, this->height() / 2 + 20);
        glTexCoord2d(1, 1);
        glVertex2d(this->width() - 100, this->height() / 2 + 20);
        glTexCoord2d(0, 1);
    glEnd();
    end2d();
    renderText(this->width() - 94, this->height() / 2 + 5, tr("BotLast: ") + QString::number(botLast, 'g', 0) + "%");
}

DrawGl::~DrawGl() {
    QSettings s(settingsFile, QSettings::IniFormat);
    s.setValue("mouseSensitivity", QVariant(mouseSensitivity));
    if (!isFullScreen())
        s.setValue("widgetGeometry", QVariant(saveGeometry()));
}
