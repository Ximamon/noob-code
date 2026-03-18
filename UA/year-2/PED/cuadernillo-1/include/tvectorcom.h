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
    TVectorCom(const TVectorCom &);
    ~TVectorCom();
    TVectorCom& operator=(const TVectorCom &);

    TComplejo& operator[](int);
    TComplejo operator[](int) const;

    bool operator==(const TVectorCom &) const;
    bool operator!=(const TVectorCom &) const;

    int Tamano() const;
    int Ocupadas() const;
    bool ExisteCom(TComplejo &);
    void MostrarComplejos(double) const;
    bool Redimensionar(int);
private:
    TComplejo *c;
    int tamano;
    TComplejo error; // Necesario para devolver referencias cuando el índice falla

    void copia(const TVectorCom &);

};

#endif // TVECTORCOM_H