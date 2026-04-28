//
// Created by julianhinojosagil on 25/3/26.
//

#include "../include/tabbcom.h"
#include <queue>
#include <cmath>

TNodoABB::TNodoABB() {
    this->item = TComplejo();
    this->iz = TABBCom();
    this->de = TABBCom();
}

TNodoABB::TNodoABB(const TNodoABB &other) {
    this->item = other.item;
    this->iz = other.iz;
    this->de = other.de;
}

TNodoABB::~TNodoABB() {

}

TNodoABB &TNodoABB::operator=(const TNodoABB &other) {
    if (this != &other) {
        this->item = other.item;
        this->iz = other.iz;
        this->de = other.de;
    }
    return *this;
}

TABBCom::TABBCom() {
    this->nodo = nullptr;
}

TABBCom::TABBCom(const TABBCom &other) {
    if (other.nodo != nullptr) {
        this->nodo = new TNodoABB(*other.nodo);
    } else {
        this->nodo = nullptr;
    }
}

TABBCom::~TABBCom() {
    if (this->nodo != nullptr) {
        delete this->nodo;
        this->nodo = nullptr;
    }
}

TABBCom &TABBCom::operator=(const TABBCom &other) {
    if (this != &other) {
        this->~TABBCom();

        if (other.nodo != NULL)
            this->nodo = new TNodoABB(*other.nodo);
        else
            this->nodo = nullptr;
    }
    return *this;
}

bool TABBCom::EsVacio() const {
    return this->nodo == nullptr;
}

// Método auxiliar privado: Comprueba si c1 es MENOR que c2
bool TABBCom::EsMenor(const TComplejo &c1, const TComplejo &c2) const {
    double mod1 = sqrt(c1.Re() * c1.Re() + c1.Im() * c1.Im());
    double mod2 = sqrt(c2.Re() * c2.Re() + c2.Im() * c2.Im());

    if (mod1 != mod2) {
        return mod1 < mod2;
    }
    if (c1.Re() != c2.Re()) {
        return c1.Re() < c2.Re();
    }
    return c1.Im() < c2.Im();
}

// Método auxiliar privado: Comprueba si c1 es MAYOR que c2
bool TABBCom::EsMayor(const TComplejo &c1, const TComplejo &c2) const {
    double mod1 = sqrt(c1.Re() * c1.Re() + c1.Im() * c1.Im());
    double mod2 = sqrt(c2.Re() * c2.Re() + c2.Im() * c2.Im());

    if (mod1 != mod2) {
        return mod1 > mod2;
    }
    if (c1.Re() != c2.Re()) {
        return c1.Re() > c2.Re();
    }
    return c1.Im() > c2.Im();
}

bool TABBCom::Buscar(const TComplejo &c) const {
    if (EsVacio()) return false;

    if (c == this->nodo->item)
        return true;
    else if (EsMenor(c, this->nodo->item))
        return this->nodo->iz.Buscar(c);
    else 
        return this->nodo->de.Buscar(c);
}

bool TABBCom::Insertar(const TComplejo &c) {
    if (EsVacio()) {
        this->nodo = new TNodoABB();
        this->nodo->item = c;
        return true;
    }

    if (c == this->nodo->item)
        return false;
    else if (EsMenor(c, this->nodo->item))
        return this->nodo->iz.Insertar(c);
    else
        return this->nodo->de.Insertar(c);
}

bool TABBCom::Borrar(const TComplejo &c) {
    if (EsVacio()) return false;
    

    if (EsMenor(c, this->nodo->item)) {
        return nodo->iz.Borrar(c);
    } else if (EsMayor(c, this->nodo->item)) {
        return nodo->de.Borrar(c);
    } else {
        
        // Caso 1: Es un nodo hoja (sin hijos)
        if (nodo->iz.EsVacio() && nodo->de.EsVacio()) {
            delete nodo;
            nodo = NULL;
        } 
        // Caso 2: Solo tiene hijo derecho
        else if (nodo->iz.EsVacio()) {
            TNodoABB *aux = nodo;
            nodo = nodo->de.nodo;
            aux->de.nodo = nullptr; // Evitar que el destructor de aux borre el subárbol
            delete aux;
        } 
        // Caso 3: Solo tiene hijo izquierdo
        else if (nodo->de.EsVacio()) {
            TNodoABB *aux = nodo;
            nodo = nodo->iz.nodo;
            aux->iz.nodo = nullptr; // Evitar que el destructor de aux borre el subárbol
            delete aux;
        } 
        // Caso 4: Tiene dos hijos. Criterio: sustituir por el mayor de la izquierda.
        else {
            // Buscamos el mayor del subárbol izquierdo (bajar siempre por la derecha)
            TABBCom *aux = &nodo->iz;
            while (!aux->nodo->de.EsVacio()) {
                aux = &aux->nodo->de;
            }
            
            // Copiamos el elemento mayor al nodo actual
            TComplejo mayorIzquierda = aux->nodo->item;
            nodo->item = mayorIzquierda;
            
            // Borramos el nodo que contenía originalmente ese elemento mayor
            nodo->iz.Borrar(mayorIzquierda);
        }
        return true;
    }
}

bool TABBCom::operator==(const TABBCom &otro) {
    return this->Inorden() == otro.Inorden();
}

TComplejo TABBCom::Raiz() const {
    if (EsVacio()) return TComplejo();

    return this->nodo->item;
}

int TABBCom::Altura() const {
    if (EsVacio()) return 0;

    int alturaIzquierda = this->nodo->iz.Altura();
    int alturaDerecha = this->nodo->de.Altura();

    if (alturaIzquierda > alturaDerecha)
        return 1 + alturaIzquierda;
    else
        return 1 + alturaDerecha;
}

int TABBCom::Nodos() const {
    if (EsVacio()) return 0;

    return 1 + this->nodo->iz.Nodos() + this->nodo->de.Nodos();
}

int TABBCom::NodosHoja() const {
    if (EsVacio()) return 0;

    if (this->nodo->iz.EsVacio() && this->nodo->de.EsVacio()) return 1;

    return this->nodo->iz.NodosHoja() + this->nodo->de.NodosHoja();
}

void TABBCom::InordenAux(TVectorCom &v, int &pos) const {
    if (!EsVacio()) {
        this->nodo->iz.InordenAux(v, pos);

        v[pos] = this->nodo->item;
        pos++;

        this->nodo->de.InordenAux(v, pos);
    }
} 

TVectorCom TABBCom::Inorden() const {
    int pos = 1;
    TVectorCom v(Nodos());
    InordenAux(v, pos);
    return v;
}

void TABBCom::PreordenAux(TVectorCom &v, int &pos) const {
    if (!EsVacio()) {
        v[pos] = nodo->item;
        pos++;
        
        this->nodo->iz.PreordenAux(v, pos);
        this->nodo->de.PreordenAux(v, pos);
    }
}

TVectorCom TABBCom::Preorden() const {
    int pos = 1;
    TVectorCom v(Nodos());
    PreordenAux(v, pos);
    return v;
}


void TABBCom::PostordenAux(TVectorCom &v, int &pos) const {
    if (!EsVacio()) {
        this->nodo->iz.PostordenAux(v, pos);
        this->nodo->de.PostordenAux(v, pos);
        
        v[pos] = nodo->item;
        pos++;
    }
}

TVectorCom TABBCom::Postorden() const {
    int pos = 1;
    TVectorCom v(Nodos());
    PostordenAux(v, pos);
    return v;
}

TVectorCom TABBCom::Niveles() const {
    TVectorCom v(Nodos());

    if (EsVacio()) return v;

    std::queue<const TABBCom *> cola;
    int pos = 1;

    cola.push(this);

    while (!cola.empty()) {
        const TABBCom *actual = cola.front();
        cola.pop();

        if (!actual->EsVacio()) {
            v[pos] = actual->nodo->item;
            pos++;

            cola.push(&(actual->nodo->iz));
            cola.push(&(actual->nodo->de));
        }
    }

    return v;
}

std::ostream &operator<<(std::ostream &os, const TABBCom &arbol) {
    os << arbol.Niveles();
    return os;
}