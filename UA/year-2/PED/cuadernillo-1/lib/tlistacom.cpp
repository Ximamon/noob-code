//
// Created by julianhinojosagil on 4/3/26.
//

#include "tlistacom.h"
#include <new>

// ------------------- TListaNodo -------------------

TListaNodo::TListaNodo() {
    this->anterior = nullptr;
    this->siguiente = nullptr;
}

TListaNodo::TListaNodo(const TListaNodo &otro) {
    this->e = otro.e;
    this->anterior = otro.anterior;
    this->siguiente = otro.siguiente;
}

TListaNodo::~TListaNodo() {
    this->anterior = nullptr;
    this->siguiente = nullptr;
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

// ------------------- TListaPos -------------------
TListaPos::TListaPos() {
    this->pos = nullptr;
}

TListaPos::TListaPos(const TListaPos &otra) {
    this->pos = otra.pos;
}

TListaPos::~TListaPos() {
    this->pos = nullptr;
}

TListaPos &TListaPos::operator=(const TListaPos &otra) {
    if (this->pos != otra.pos) {
        this->pos = otra.pos;
    }
    return *this;
}

bool TListaPos::operator==(const TListaPos &otra) const {
    return this->pos == otra.pos;
}

bool TListaPos::operator!=(const TListaPos &otra) const {
    return this->pos != otra.pos;
}

TListaPos TListaPos::Anterior() const {
    TListaPos anterior;
    if (this->pos != nullptr && this->pos->anterior != nullptr) {
        anterior.pos = this->pos->anterior;
    }
    return anterior;
}

TListaPos TListaPos::Siguiente() const {
    TListaPos siguiente;
    if (this->pos != nullptr && this->pos->siguiente != nullptr) {
        siguiente.pos = this->pos->siguiente;
    }
    return siguiente;
}

bool TListaPos::EsVacia() const {
    return this->pos == nullptr;
}

// ------------------------------------------------------

// ------------------- TListaCom -------------------

TListaCom::TListaCom() {
    this->primero = nullptr;
    this->ultimo = nullptr;
}

TListaCom::TListaCom(const TListaCom & otra) {
    this->primero = nullptr;
    this->ultimo = nullptr;

    TListaNodo *nodo = otra.primero;
    while (nodo != nullptr) {
        TListaNodo *nodoNuevo = new TListaNodo();
        nodoNuevo->e = nodo->e;

        if (this->primero == nullptr) {
            this->primero = nodoNuevo;
            this->ultimo = nodoNuevo;
        } else {
            this->ultimo->siguiente = nodoNuevo;
            nodoNuevo->siguiente = this->ultimo;
            this->ultimo = nodoNuevo;
        }
        nodo = nodo->siguiente;
    }
}

TListaCom::~TListaCom() {
    TListaNodo *nodo = this->primero;
    while (nodo != nullptr) {
        this->primero = this->primero->siguiente;
        delete nodo;
        nodo = this->primero;
    }
    this->ultimo = nullptr;
}

TListaCom &TListaCom::operator=(const TListaCom &otra) {
    if (this != &otra) {
        this->~TListaCom();

        TListaNodo *nodo = otra.primero;
        while (nodo != nullptr) {
            TListaNodo *nodoNuevo = new TListaNodo();
            nodoNuevo->e = nodo->e;

            if (this->primero == nullptr) {
                this->primero = nodoNuevo;
                this->ultimo = nodoNuevo;
            } else {
                this->ultimo->siguiente = nodoNuevo;
                nodoNuevo->anterior = this->ultimo;
                this->ultimo = nodoNuevo;
            }
            nodo = nodo->siguiente;
        }
    }
    return *this;
}


bool TListaCom::operator==(const TListaCom &otra) const {
    TListaNodo *nodo1 = this->primero;
    TListaNodo *nodo2 = otra.primero;

    while (nodo1 != nullptr && nodo2 != nullptr) {
        if (nodo1->e != nodo2->e) {
            return false;
        }
        nodo1 = nodo1->siguiente;
        nodo2 = nodo2->siguiente;
    }
    return nodo1 == nullptr && nodo2 == nullptr;
}

bool TListaCom::operator!=(const TListaCom &otra) const {
    return !(*this == otra);
}

TListaCom TListaCom::operator+(const TListaCom &otra) const {
    TListaCom res;

    TListaNodo *nodo = this->primero;
    while (nodo != nullptr) {
        TListaNodo *nodoNuevo = new TListaNodo();
        nodoNuevo->e = nodo->e;

        if (res.primero == nullptr) {
            res.primero = nodoNuevo;
            res.ultimo = nodoNuevo;
        } else {
            res.ultimo->siguiente = nodoNuevo;
            nodoNuevo->anterior = res.ultimo;
            res.ultimo = nodoNuevo;
        }
        nodo = nodo->siguiente;
    }

    nodo = otra.primero;
    while (nodo != nullptr) {
        TListaNodo *nodoNuevo = new TListaNodo();
        nodoNuevo->e = nodo->e;

        if (res.primero == nullptr) {
            res.primero = nodoNuevo;
            res.ultimo = nodoNuevo;
        } else {
            res.ultimo->siguiente = nodoNuevo;
            nodoNuevo->anterior = res.ultimo;
            res.ultimo = nodoNuevo;
        }
        nodo = nodo->siguiente;
    }
    return res;
}

TListaCom TListaCom::operator-(const TListaCom &otra) const {
    TListaCom res;

    TListaNodo *nodo1 = this->primero;
    while (nodo1 != nullptr) {
        bool encontrado = false;

        TListaNodo *nodo2 = otra.primero;
        while (nodo2 != nullptr) {
            if (nodo1->e == nodo2->e) {
                encontrado = true;
                break;
            }
            nodo2 = nodo2->siguiente;
        }

        if (!encontrado) {
            TListaNodo *nodoNuevo = new TListaNodo();
            nodoNuevo->e = nodo1->e;

            if (res.primero == nullptr) {
                res.primero = nodoNuevo;
                res.ultimo = nodoNuevo;
            } else {
                res.ultimo->siguiente = nodoNuevo;
                nodoNuevo->anterior = res.ultimo;
                res.ultimo = nodoNuevo;
            }
        }
        nodo1 = nodo1->siguiente;
    }
    return res;
}

bool TListaCom::EsVacia() const {
    return this->primero == nullptr;
}

bool TListaCom::InsCabeza(const TComplejo &e) {
    TListaNodo *nodo = new(std::nothrow) TListaNodo();

    if (nodo == nullptr) {
        return false;
    }

    nodo->e = e;

    if (this->EsVacia()) {
        this->primero = nodo;
        this->ultimo = nodo;
    } else {
        nodo->siguiente = this->primero;
        this->primero->anterior = nodo;
        this->primero = nodo;
    }
    return true;
}

// Inserta el elemento a la izquierda de la posición indicada
bool TListaCom::InsertarI(const TComplejo &e, const TListaPos &p) {
    // Hay que comprobar que el objeto TListaPos no es vacío
    if (p.pos == nullptr) {
        return false;
    }

    // Intentamos reservar memoria para el nuevo nodo
    TListaNodo *nuevo = new(std::nothrow) TListaNodo();
    if (nuevo == nullptr) {
        return false;
    }
    nuevo->e = e;

    // Enlazamos el nuevo nodo por la izquierda (antes de p.pos)
    nuevo->siguiente = p.pos;
    nuevo->anterior = p.pos->anterior;

    if (p.pos->anterior != nullptr) {
        // Si NO estábamos en el primer nodo, el anterior apunta al nuevo
        p.pos->anterior->siguiente = nuevo;
    } else {
        // Si estábamos en el primer nodo, la cabeza de la lista cambia
        this->primero = nuevo;
    }

    // El nodo actual ahora tiene al nuevo justo a su izquierda
    p.pos->anterior = nuevo;

    return true;
}

// Inserta el elemento a la derecha de la posición indicada
bool TListaCom::InsertarD(const TComplejo &e, const TListaPos &p) {
    // Hay que comprobar que el objeto TListaPos no es vacío
    if (p.pos == nullptr) {
        return false;
    }

    // Intentamos reservar memoria para el nuevo nodo
    TListaNodo *nuevo = new(std::nothrow) TListaNodo();
    if (nuevo == nullptr) {
        return false;
    }
    nuevo->e = e;

    // Enlazamos el nuevo nodo por la derecha (después de p.pos)
    nuevo->anterior = p.pos;
    nuevo->siguiente = p.pos->siguiente;

    if (p.pos->siguiente != nullptr) {
        // Si NO estábamos en el último nodo, el siguiente apunta al nuevo hacia atrás
        p.pos->siguiente->anterior = nuevo;
    } else {
        // Si estábamos en el último nodo, la cola de la lista cambia
        this->ultimo = nuevo;
    }

    // El nodo actual ahora tiene al nuevo justo a su derecha
    p.pos->siguiente = nuevo;

    return true;
}

// Busca y borra la primera ocurrencia del elemento
bool TListaCom::Borrar(const TComplejo &e) {
    TListaNodo *nodo = this->primero;

    while (nodo != nullptr) {
        if (nodo->e == e) { // Si encontramos el elemento
            // 1. Desenlazamos el nodo por la izquierda
            if (nodo->anterior != nullptr) {
                nodo->anterior->siguiente = nodo->siguiente;
            } else {
                this->primero = nodo->siguiente; // Si era el primero, actualizamos la cabeza
            }

            // 2. Desenlazamos el nodo por la derecha
            if (nodo->siguiente != nullptr) {
                nodo->siguiente->anterior = nodo->anterior;
            } else {
                this->ultimo = nodo->anterior; // Si era el último, actualizamos la cola
            }

            // 3. Borramos el nodo de la memoria
            delete nodo;
            return true; // Hemos borrado la primera ocurrencia [cite: 264, 315]
        }
        nodo = nodo->siguiente;
    }
    return false; // No existía en la lista [cite: 315]
}

// Busca y borra todas las ocurrencias del elemento
bool TListaCom::BorrarTodos(const TComplejo &e) {
    bool borrado = false; // Bandera para saber si borramos al menos uno
    TListaNodo *nodo = this->primero;

    while (nodo != nullptr) {
        if (nodo->e == e) { // Si encontramos una ocurrencia
            TListaNodo *aBorrar = nodo;
            nodo = nodo->siguiente; // Avanzamos ANTES de borrar el nodo actual

            if (aBorrar->anterior != nullptr) {
                aBorrar->anterior->siguiente = aBorrar->siguiente;
            } else {
                this->primero = aBorrar->siguiente;
            }

            if (aBorrar->siguiente != nullptr) {
                aBorrar->siguiente->anterior = aBorrar->anterior;
            } else {
                this->ultimo = aBorrar->anterior;
            }

            delete aBorrar;
            borrado = true; // Confirmamos que hemos borrado al menos uno
        } else {
            // Si no era el elemento, simplemente avanzamos
            nodo = nodo->siguiente;
        }
    }
    return borrado; // Devuelve true si se pudo borrar al menos uno [cite: 265, 316]
}

// Borra el elemento que ocupa la posición indicada
bool TListaCom::Borrar(TListaPos &p) {
    // Hay que comprobar que el objeto TListaPos no es vacío [cite: 320]
    if (p.pos == nullptr) {
        return false; // [cite: 318]
    }

    TListaNodo *aBorrar = p.pos;

    if (aBorrar->anterior != nullptr) {
        aBorrar->anterior->siguiente = aBorrar->siguiente;
    } else {
        this->primero = aBorrar->siguiente;
    }

    if (aBorrar->siguiente != nullptr) {
        aBorrar->siguiente->anterior = aBorrar->anterior;
    } else {
        this->ultimo = aBorrar->anterior;
    }

    delete aBorrar;

    // Una vez eliminado el elemento, la posición tiene que pasar a estar vacía [cite: 317]
    p.pos = nullptr;

    return true; // [cite: 318]
}

// Obtiene el elemento que ocupa la posición indicada
TComplejo TListaCom::Obtener(const TListaPos &p) {
    // Si la posición es vacía, se devuelve un TComplejo creado con el constructor por defecto [cite: 321]
    if (p.pos == nullptr) {
        TComplejo vacio;
        return vacio;
    }
    // Si no es vacía, devolvemos el elemento
    return p.pos->e;
}

// Devuelve true si el elemento está en la lista, false en caso contrario
bool TListaCom::Buscar(const TComplejo &e) const {
    // Se realiza una búsqueda lineal desde el primer elemento hasta el último [cite: 322]
    TListaNodo *nodo = this->primero;
    while (nodo != nullptr) {
        if (nodo->e == e) {
            return true; // Lo encontramos
        }
        nodo = nodo->siguiente;
    }
    return false; // Llegamos al final y no estaba
}

// Devuelve la longitud de la lista
int TListaCom::Longitud() const {
    int contador = 0;
    TListaNodo *nodo = this->primero;
    while (nodo != nullptr) {
        contador++;
        nodo = nodo->siguiente;
    }
    return contador;
}

// Devuelve la primera posición en la lista
TListaPos TListaCom::Primera() const {
    TListaPos p;
    p.pos = this->primero;
    return p;
}

// Devuelve la última posición en la lista
TListaPos TListaCom::Ultima() const {
    TListaPos p;
    p.pos = this->ultimo;
    return p;
}

std::ostream & operator<<(std::ostream &os, const TListaCom &lista) {
    os << "{";

    TListaPos p = lista.Primera();
    while (!p.EsVacia()) {
        os << lista.Obtener(p); // Imprimimos el elemento

        p = p.Siguiente();      // Avanzamos a la siguiente posición

        if (!p.EsVacia()) {
            os << " ";          // Si no hemos llegado al final, ponemos el espacio
        }
    }

    os << "}";
    return os;
}

// ------------------------------------------------------