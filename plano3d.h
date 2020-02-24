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
#ifndef PLANO3D_H
#define PLANO3D_H

#include <QVector3D>
#include "utils.h"

class Plano3D
{
    public:
        double xMin;
        double xMax;
        double yMin;
        double yMax;
        double zMin;
        double zMax;
        double d;

        QVector3D N;
        QVector3D P0;



        Plano3D();
        Plano3D(double x0, double y0, double z0,
                double nx, double ny, double nz,
                double xMin = -static_cast<double>(INFINITY), double xMax = +static_cast<double>(INFINITY),
                double yMin = -static_cast<double>(INFINITY), double yMax = +static_cast<double>(INFINITY),
                double zMin = -static_cast<double>(INFINITY), double zMax = +static_cast<double>(INFINITY));
        Plano3D(QVector3D P0, QVector3D N,
                double xMin = -static_cast<double>(INFINITY), double xMax = +static_cast<double>(INFINITY),
                double yMin = -static_cast<double>(INFINITY), double yMax = +static_cast<double>(INFINITY),
                double zMin = -static_cast<double>(INFINITY), double zMax = +static_cast<double>(INFINITY));

        bool contemPonto(double x, double y, double z);
        bool contemPonto(QVector3D Pc);

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
