#include "console.h"

Console::Console(QWidget *parent) : QPlainTextEdit(parent) {
}

void Console::mouseDoubleClickEvent(QMouseEvent *e) {}
void Console::mousePressEvent(QMouseEvent *e) {}
void Console::mouseMoveEvent(QMouseEvent *e) {}
void Console::mouseReleaseEvent(QMouseEvent *e) {}
void Console::keyPressEvent(QKeyEvent *e) {}
void Console::keyReleaseEvent(QKeyEvent *e) {}

void Console::addString(QString s) {
    if (s[s.length() - 1] == '\n')
        s = s.left(s.length() - 1);
    appendPlainText(s);
}
