#ifndef PLANO3D_H
#define PLANO3D_H

#include <QVector3D>
#include "utils.h"

class Plano3D
{
    private:
        double x0;
        double y0;
        double z0;
        double nx;
        double ny;
        double nz;

    public:
        double xMin;
        double xMax;
        double yMin;
        double yMax;
        double zMin;
        double zMax;
        double d;

        Plano3D();
        Plano3D(double x0, double y0, double z0,
                double nx, double ny, double nz,
                double xMin = -static_cast<double>(INFINITY), double xMax = +static_cast<double>(INFINITY),
                double yMin = -static_cast<double>(INFINITY), double yMax = +static_cast<double>(INFINITY),
                double zMin = -static_cast<double>(INFINITY), double zMax = +static_cast<double>(INFINITY));
        Plano3D(QVector3D P0, QVector3D N);

        bool contemPonto(double x, double y, double z);

        void calculaD();

        double X0() const;
        void setX0(double value);
        double Y0() const;
        void setY0(double value);
        double Z0() const;
        void setZ0(double value);
        double Nx() const;
        void setNx(double value);
        double Ny() const;
        void setNy(double value);
        double Nz() const;
        void setNz(double value);
};

#endif // PLANO3D_H
