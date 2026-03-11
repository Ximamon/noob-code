//
// Created by julianhinojosagil on 4/3/26.
//

#include "tlistapos.h"

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

