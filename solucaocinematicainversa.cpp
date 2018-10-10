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

bool SolucaoCinematicaInversa::possuiAlgumAnguloImpossivel(int *idxAngulo)
{
    bool ehPossivel[5];

    ehPossivel[0] = this->teta1possivel;
    ehPossivel[1] = this->teta2possivel;
    ehPossivel[2] = this->teta3possivel;
    ehPossivel[3] = this->teta4possivel;
    ehPossivel[4] = this->teta5possivel;

    for(int i = 0; i < 5; i++)
    {
        if(!ehPossivel[i])
        {
            if(idxAngulo != nullptr)
                *idxAngulo = i + 1;
            return true;
        }
    }

    return false;
}
