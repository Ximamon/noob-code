//
// Created by julianhinojosagil on 18/2/26.
//

#ifndef TVECTORCOM_H
#define TVECTORCOM_H

#include <iostream>
#include "tcomplejo.h"

class TVectorCom {
    friend std::ostream & operator<<(std::ostream &, const TVectorCom &);

    public:
        TVectorCom();
        TVectorCom(int);
        TVectorCom(TVectorCom &);
        TVectorCom& operator=(const TVectorCom &);

        TComplejo& operator[](int);
        TComplejo operator[](int) const;

        bool operator==(const TVectorCom &);
        bool operator!=(const TVectorCom &);

        int Tamano();
        int Ocupadas();
        bool ExisteCom();
        void MostrarComplejos(double);
        bool Redimensionar(int);
};

#endif // TVECTORCOM_H