//
// Created by julianhinojosagil on 4/3/26.
//

#include "tlistapos.h"

// ------------------- Forma canonica -------------------

TListaNodo::TListaNodo() {
    this->anterior = NULL;
    this->siguiente = NULL;
}

TListaNodo::TListaNodo(const TListaNodo &otro) {
    this->e = otro.e;
    this->anterior = otro.anterior;
    this->siguiente = otro.siguiente;
}

TListaNodo::~TListaNodo() {
    this->anterior = NULL;
    this->siguiente = NULL;
}

TListaNodo & TListaNodo::operator= (const TListaNodo &otro) {
    if (this != &otro) {
        this->e = otro.e;
        this->anterior = otro.anterior;
        this->siguiente = otro.siguiente;
    }
    return *this;
}

// ------------------------------------------------------
