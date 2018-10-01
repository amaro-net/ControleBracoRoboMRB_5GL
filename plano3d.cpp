#include "plano3d.h"
#include "constantes.h"
#include "utils.h"

Plano3D::Plano3D()
{
    Plano3D(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
}

Plano3D::Plano3D(double x0, double y0, double z0,
                 double nx, double ny, double nz,
                 double xMin, double xMax,
                 double yMin, double yMax,
                 double zMin, double zMax)
{
    this->x0 = x0;
    this->y0 = y0;
    this->z0 = z0;
    this->nx = nx;
    this->ny = ny;
    this->nz = nz;

    this->xMin = xMin;
    this->xMax = xMax;
    this->yMin = yMin;
    this->yMax = yMax;
    this->zMin = zMin;
    this->zMax = zMax;

    calculaD();
}

Plano3D::Plano3D(QVector3D P0, QVector3D N)
{
    Plano3D(double(P0.x()),
            double(P0.y()),
            double(P0.z()),
            double(N.x()),
            double(N.y()),
            double(N.z()));
}

bool Plano3D::contemPonto(double x, double y, double z)
{
    bool resultado = EhIgual(nx * x + ny * y + nz * z + d, 0.0, CASAS_DECIMAIS_POSICAO_XYZ);

    if(resultado && std::abs(xMin) != INFINITY)
        resultado = resultado && EhMenorOuIgual(xMin, x, CASAS_DECIMAIS_POSICAO_XYZ);

    if(resultado && std::abs(xMax) != INFINITY)
        resultado = resultado && EhMenorOuIgual(x, xMax, CASAS_DECIMAIS_POSICAO_XYZ);


    if(resultado && std::abs(yMin) != INFINITY)
        resultado = resultado && EhMenorOuIgual(yMin, y, CASAS_DECIMAIS_POSICAO_XYZ);

    if(resultado && std::abs(yMax) != INFINITY)
        resultado = resultado && EhMenorOuIgual(y, yMax, CASAS_DECIMAIS_POSICAO_XYZ);


    if(resultado && std::abs(zMin) != INFINITY)
        resultado = resultado && EhMenorOuIgual(zMin, z, CASAS_DECIMAIS_POSICAO_XYZ);

    if(resultado && std::abs(zMax) != INFINITY)
        resultado = resultado && EhMenorOuIgual(z, zMax, CASAS_DECIMAIS_POSICAO_XYZ);

    return resultado;
}

void Plano3D::calculaD()
{
    d = -nx * x0 - ny * y0 - nz * z0;
}

double Plano3D::X0() const
{
    return x0;
}

void Plano3D::setX0(double value)
{
    x0 = value;
    calculaD();
}

double Plano3D::Y0() const
{
    return y0;
}

void Plano3D::setY0(double value)
{
    y0 = value;
    calculaD();
}

double Plano3D::Z0() const
{
    return z0;
}

void Plano3D::setZ0(double value)
{
    z0 = value;
    calculaD();
}

double Plano3D::Nx() const
{
    return nx;
}

void Plano3D::setNx(double value)
{
    nx = value;
    calculaD();
}

double Plano3D::Ny() const
{
    return ny;
}

void Plano3D::setNy(double value)
{
    ny = value;
    calculaD();
}

double Plano3D::Nz() const
{
    return nz;
}

void Plano3D::setNz(double value)
{
    nz = value;
    calculaD();
}





