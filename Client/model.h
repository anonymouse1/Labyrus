#ifndef MODEL_H
#define MODEL_H

#include <QString>
#include <GL/gl.h>
#include <stdio.h>
#include <QDebug>

class Model {
    public:
        Model(QString);
        void draw(double k, double x, double y, double z);

    private:
        int n;
        int **r;


};

#endif // MODEL_H
