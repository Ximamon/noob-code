//
// Created by julianhinojosagil on 18/2/26.
//

// ------------------- Forma canonica -------------------

#include "tvectorcom.h"

TVectorCom::TVectorCom(int t) {
    this->tamano = 0;
    this->c = NULL;

    if (t > 0) {
        this->tamano = t;
        this->c = new TComplejo(this->tamano);
    }
}

TVectorCom::TVectorCom(const TVectorCom &v) {
    this->tamano = 0;
    this->c = NULL;
    *this = v;
}

TVectorCom::~TVectorCom() {
    if (this->c != NULL) {
        delete [] this->c;
        this->c = NULL;
        this->tamano = 0;
    }
}

TVectorCom &TVectorCom::operator=(const TVectorCom &v) {
    if (this != &v) {
        this->tamano = v.tamano;
        this->c = new TComplejo(this->tamano);
    }
    return *this;
}

// ------------------------------------------------------

// ------------------- Operadores (clase) -------------------

TComplejo &TVectorCom::operator[](int) {

}

