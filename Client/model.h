#ifndef MODEL_H
#define MODEL_H

#include <QString>
#ifdef Q_OS_MAC
    #include <OpenGL/gl.h>
#else
    #include <GL/gl.h>
#endif
#include <stdio.h>
#include <QDebug>
#include <assert.h>

class Model {
    public:
        Model(QString);
        void draw(double k, double x, double y, double z);

    private:
        int n;
        int **r;


};

#endif // MODEL_H
