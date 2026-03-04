//
// Created by julianhinojosagil on 4/3/26.
//

#ifndef TLISTANODO_H
#define TLISTANODO_H

#include <iostream>
#include "tcomplejo.h"
#include "tvectorcom.h"

class TListaNodo {
public:
    TListaNodo();
    TListaNodo(const TListaNodo &);
    ~TListaNodo();
    TListaNodo &operator=(const TListaNodo &);
private:
    TComplejo e;
    TListaNodo *anterior;
    TListaNodo *siguiente;
};

#endif //TLISTANODO_H