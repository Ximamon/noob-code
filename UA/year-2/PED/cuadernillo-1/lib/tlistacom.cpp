//
// Created by julianhinojosagil on 4/3/26.
//

#include "tlistacom.h"

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

TListaPos::TListaPos() {
    this->pos = NULL;
}

TListaPos::TListaPos(const TListaPos &otra) {
    this->pos = otra.pos;
}

TListaPos::~TListaPos() {
    this->pos = NULL;
}

TListaPos &TListaPos::operator=(const TListaPos &otra) {
    if (this->pos != otra.pos) {
        this->pos = otra.pos;
    }
    return *this;
}

bool TListaPos::operator==(const TListaPos &otra) {
    return this->pos == otra.pos;
}

bool TListaPos::operator!=(const TListaPos &otra) {
    return this->pos != otra.pos;
}

TListaPos TListaPos::Anterior() {
    TListaPos anterior;
    if (this->pos != NULL && this->pos->anterior != NULL) {
        anterior.pos = this->pos->anterior;
    }
    return anterior;
}

TListaPos TListaPos::Siguiente() {
    TListaPos siguiente;
    if (this->pos != NULL && this->pos->siguiente != NULL) {
        siguiente.pos = this->pos->siguiente;
    }
    return siguiente;
}

bool TListaPos::EsVacia() {
    return this->pos == NULL;
}