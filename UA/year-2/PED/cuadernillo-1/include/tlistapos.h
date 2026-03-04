//
// Created by julianhinojosagil on 4/3/26.
//

#ifndef TLISTAPOS_H
#define TLISTAPOS_H

#include <iostream>
#include "tcomplejo.h"
#include "tvectorcom.h"
#include "tlistanodo.h"

class TListaPos {
public:
    TListaPos();
    TListaPos(const TListaPos &);
    ~TListaPos();
    TListaPos &operator=(const TListaPos &);

    bool operator==(const TListaPos &);
    bool operator!=(const TListaPos &);

    TListaPos Anterior();
    TListaPos Siguiente();

    bool EsVacia();
private:
    TListaNodo *pos;
};

#endif //TLISTAPOS_H