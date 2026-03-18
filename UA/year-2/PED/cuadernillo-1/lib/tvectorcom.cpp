//
// Created by julianhinojosagil on 18/2/26.
//

// ------------------- Forma canonica -------------------

#include "tvectorcom.h"

TVectorCom::TVectorCom() {
    this->tamano = 0;
    this->c = NULL;
}

TVectorCom::TVectorCom(int t) {
    if (t > 0) {
        this->tamano = t;
        this->c = new TComplejo[this->tamano];
    } else {
        this->tamano = 0;
        this->c = NULL;
    }

}

TVectorCom::TVectorCom(const TVectorCom &v) {
    copia(v);
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
        if (this->c != NULL) {
            delete [] this->c;
        }
        copia(v);
    }
    return *this;
}

// ------------------------------------------------------

// ------------------- Operadores (clase) -------------------

TComplejo &TVectorCom::operator[](int i) {
    if (i >= 1 && i <= this->tamano) {
        return this->c[i-1];
    }

    error = TComplejo();
    return error;
}

TComplejo TVectorCom::operator[](int i) const {
    if (i >= 1 && i <= this->tamano) {
        return this->c[i-1];
    }
    return TComplejo();
}

bool TVectorCom::operator==(const TVectorCom &v) const {
    if (this->tamano != v.tamano) {
        return false;
    }
    for (int i = 0; i < this->tamano; i++) {
        if (this->c[i] != v.c[i]) {
            return false;
        }
    }
    return true;
}

bool TVectorCom::operator!=(const TVectorCom &v) const {
    return !(*this == v);
}

// ------------------------------------------------------

// ------------------- Metodos de Clase -------------------

int TVectorCom::Tamano() const {
    return this->tamano;
}

int TVectorCom::Ocupadas() const {
    int count = 0;
    for (int i = 0; i < this->tamano; i++) {
        TComplejo vacio;
        if (this->c[i] != vacio) {
            count++;
        }
    }
    return count;
}

bool TVectorCom::ExisteCom(TComplejo &comp) {
    for (int i = 0; i < this->tamano; i++) {
        if (this->c[i] == comp) {
            return true;
        }
    }
    return false;
}

void TVectorCom::MostrarComplejos(double d) const {
    bool first = true;
    std::cout << "[";

    for (int i = 0; i < this->tamano; i++) {
        if (this->c[i].Re() >= d) {
            if (!first) {
                std::cout << ", ";
            }
            std::cout << this->c[i];
            first = false;
        }
    }
    std::cout << "]";
}

bool TVectorCom::Redimensionar(int newSize) {
    if (newSize <= 0 || newSize == this->tamano) {
        return false;
    }

    TComplejo *newC = new TComplejo[newSize];

    if (newSize > this->tamano) {
        for (int i = 0; i < this->tamano; i++) {
            newC[i] = this->c[i];
        }
    } else {
        for (int i = 0; i < newSize; i++) {
            newC[i] = this->c[i];
        }
    }

    if (this->c != NULL) {
        delete [] this->c;
    }

    this->c = newC;
    this->tamano = newSize;

    return true;
}

std::ostream & operator<<(std::ostream &os, const TVectorCom &v) {
    os << "[";
    for (int i = 0; i < v.tamano; i++) {
        os << "(" << i + 1 << ") " << v.c[i];

        if (i < v.tamano - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

void TVectorCom::copia(const TVectorCom &v) {
    this->tamano = v.tamano;
    if (this->tamano > 0) {
        this->c = new TComplejo[this->tamano];
        for (int i = 0; i < this->tamano; i++) {
            this->c[i] = v.c[i];
        }
    } else {
        this->c = NULL;
    }
}