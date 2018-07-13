#include "solucaocinematicainversa.h"
//#include <stddef.h>
SolucaoCinematicaInversa::SolucaoCinematicaInversa()
{
    peso = 0;
}

SolucaoCinematicaInversa::~SolucaoCinematicaInversa()
{
    //if(subSolucao != (SubSolucaoCinematicaInversa*)0)
    if(subSolucao != nullptr)
        delete subSolucao;
}
