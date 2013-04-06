#include "console.h"

Console::Console(QWidget *parent) : QPlainTextEdit(parent) {
}

void Console::mouseDoubleClickEvent(QMouseEvent *e) {e->accept();}
void Console::mousePressEvent(QMouseEvent *e) {e->accept();}
void Console::mouseMoveEvent(QMouseEvent *e) {e->accept();}
void Console::mouseReleaseEvent(QMouseEvent *e) {e->accept();}
void Console::keyPressEvent(QKeyEvent *e) {e->accept();}
void Console::keyReleaseEvent(QKeyEvent *e) {e->accept();}

void Console::addString(QString s) {
    if (s[s.length() - 1] == '\n')
        s = s.left(s.length() - 1);
    appendPlainText(s);
}
