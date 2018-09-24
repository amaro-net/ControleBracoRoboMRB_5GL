#include "solucaocinematicainversa.h"

SolucaoCinematicaInversa::SolucaoCinematicaInversa()
{
    peso = 0;
}

SolucaoCinematicaInversa::~SolucaoCinematicaInversa()
{    
    if(subSolucao != nullptr)
        delete subSolucao;
}
