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
    this->P0.setX(static_cast<float>(x0));
    this->P0.setY(static_cast<float>(y0));
    this->P0.setZ(static_cast<float>(z0));

    this->N.setX(static_cast<float>(nx));
    this->N.setY(static_cast<float>(ny));
    this->N.setZ(static_cast<float>(nz));

    this->xMin = xMin;
    this->xMax = xMax;
    this->yMin = yMin;
    this->yMax = yMax;
    this->zMin = zMin;
    this->zMax = zMax;

    calculaD();
}

Plano3D::Plano3D(QVector3D P0, QVector3D N,
                 double xMin, double xMax,
                 double yMin, double yMax,
                 double zMin, double zMax)
{
    this->P0.setX(P0.x());
    this->P0.setY(P0.y());
    this->P0.setZ(P0.z());

    this->N.setX(N.x());
    this->N.setY(N.y());
    this->N.setZ(N.z());

    this->xMin = xMin;
    this->xMax = xMax;
    this->yMin = yMin;
    this->yMax = yMax;
    this->zMin = zMin;
    this->zMax = zMax;

    calculaD();
}

bool Plano3D::contemPonto(double x, double y, double z)
{
    double nx = static_cast<double>(N.x());
    double ny = static_cast<double>(N.y());
    double nz = static_cast<double>(N.z());

    bool resultado = EhIgual(nx * x + ny * y + nz * z + d, 0.0, CASAS_DECIMAIS_POSICAO_XYZ);

    if(resultado && std::abs(xMin) < static_cast<double>(INFINITY))
        resultado = resultado && EhMenorOuIgual(xMin, x, CASAS_DECIMAIS_POSICAO_XYZ);

    if(resultado && std::abs(xMax) < static_cast<double>(INFINITY))
        resultado = resultado && EhMenorOuIgual(x, xMax, CASAS_DECIMAIS_POSICAO_XYZ);


    if(resultado && std::abs(yMin) < static_cast<double>(INFINITY))
        resultado = resultado && EhMenorOuIgual(yMin, y, CASAS_DECIMAIS_POSICAO_XYZ);

    if(resultado && std::abs(yMax) < static_cast<double>(INFINITY))
        resultado = resultado && EhMenorOuIgual(y, yMax, CASAS_DECIMAIS_POSICAO_XYZ);


    if(resultado && std::abs(zMin) < static_cast<double>(INFINITY))
        resultado = resultado && EhMenorOuIgual(zMin, z, CASAS_DECIMAIS_POSICAO_XYZ);

    if(resultado && std::abs(zMax) < static_cast<double>(INFINITY))
        resultado = resultado && EhMenorOuIgual(z, zMax, CASAS_DECIMAIS_POSICAO_XYZ);

    return resultado;
}

bool Plano3D::contemPonto(QVector3D Pc)
{
    return contemPonto(static_cast<double>(Pc.x()), static_cast<double>(Pc.y()), static_cast<double>(Pc.z()));
}

void Plano3D::calculaD()
{
    d = double(-N.x() * P0.x() - N.y() * P0.y() - N.z() * P0.z());
}

double Plano3D::X0() const
{
    return double(P0.x());
}

void Plano3D::setX0(double value)
{
    P0.setX(static_cast<float>(value));
    calculaD();
}

double Plano3D::Y0() const
{
    return double(P0.y());
}

void Plano3D::setY0(double value)
{
    P0.setY(static_cast<float>(value));
    calculaD();
}

double Plano3D::Z0() const
{
    return double(P0.z());
}

void Plano3D::setZ0(double value)
{
    P0.setZ(static_cast<float>(value));
    calculaD();
}

double Plano3D::Nx() const
{
    return double(N.x());
}

void Plano3D::setNx(double value)
{
    N.setX(static_cast<float>(value));
    calculaD();
}

double Plano3D::Ny() const
{
    return double(N.y());
}

void Plano3D::setNy(double value)
{
    N.setY(static_cast<float>(value));
    calculaD();
}

double Plano3D::Nz() const
{
    return double(N.z());
}

void Plano3D::setNz(double value)
{
    N.setZ(static_cast<float>(value));
    calculaD();
}





