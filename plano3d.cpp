/*
    Controle do Braço Robô MRB-5GL - controls and configures MRB-5GL, a 5 DOF Robot Arm prototype

    Copyright (C) 2019  Amaro Duarte de Paula Neto

    This file is part of Controle do Braço Robô MRB-5GL.

    Controle do Braço Robô MRB-5GL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Controle do Braço Robô MRB-5GL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Controle do Braço Robô MRB-5GL.  If not, see <https://www.gnu.org/licenses/>.

    contact e-mail: amaro.net80@gmail.com


    Este arquivo é parte do programa Controle do Braço Robô MRB-5GL

    Controle do Braço Robô MRB-5GL é um software livre; você pode redistribuí-lo e/ou
    modificá-lo dentro dos termos da Licença Pública Geral GNU como
    publicada pela Free Software Foundation (FSF); na versão 3 da
    Licença, ou (a seu critério) qualquer versão posterior.

    Controle do Braço Robô MRB-5GL é distribuído na esperança de que possa ser útil,
    mas SEM NENHUMA GARANTIA; sem uma garantia implícita de ADEQUAÇÃO
    a qualquer MERCADO ou APLICAÇÃO EM PARTICULAR. Veja a
    Licença Pública Geral GNU para maiores detalhes.

    Você deve ter recebido uma cópia da Licença Pública Geral GNU junto
    com este programa, Se não, veja <http://www.gnu.org/licenses/>.
*/
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





