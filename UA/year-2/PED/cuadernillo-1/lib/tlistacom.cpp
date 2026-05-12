//
// Created by julianhinojosagil on 4/3/26.
//

#include "tlistacom.h"
#include <new>

// ------------------- TListaNodo -------------------

// Constructor por defecto: inicializa los punteros de enlace a nullptr
TListaNodo::TListaNodo() {
    this->anterior = nullptr;
    this->siguiente = nullptr;
}

// Constructor de copia: copia el elemento y las referencias a los vecinos
TListaNodo::TListaNodo(const TListaNodo &otro) {
    this->e = otro.e;
    this->anterior = otro.anterior;
    this->siguiente = otro.siguiente;
}

// Destructor: solo limpia las referencias (no debe eliminar nodos vecinos)
TListaNodo::~TListaNodo() {
    this->anterior = nullptr;
    this->siguiente = nullptr;
}

// Operador de asignación: copia el contenido y los punteros
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

// Objeto ligero que encapsula una posición (puntero a TListaNodo)
TListaPos::TListaPos() {
    this->pos = nullptr;
}

// Copia la referencia a la posición (no clona nodos)
TListaPos::TListaPos(const TListaPos &otra) {
    this->pos = otra.pos;
}

// Destructor: solo limpia la referencia
TListaPos::~TListaPos() {
    this->pos = nullptr;
}

// Operador de asignación: actualizar la referencia si es distinto
TListaPos &TListaPos::operator=(const TListaPos &otra) {
    if (this->pos != otra.pos) {
        this->pos = otra.pos;
    }
    return *this;
}

// Comparadores: igualdad por puntero
bool TListaPos::operator==(const TListaPos &otra) const {
    return this->pos == otra.pos;
}

bool TListaPos::operator!=(const TListaPos &otra) const {
    return this->pos != otra.pos;
}

// Devuelve la posición anterior (si existe), o una posición vacía
TListaPos TListaPos::Anterior() const {
    TListaPos anterior;
    if (this->pos != nullptr && this->pos->anterior != nullptr) {
        anterior.pos = this->pos->anterior;
    }
    return anterior;
}

// Devuelve la posición siguiente (si existe), o una posición vacía
TListaPos TListaPos::Siguiente() const {
    TListaPos siguiente;
    if (this->pos != nullptr && this->pos->siguiente != nullptr) {
        siguiente.pos = this->pos->siguiente;
    }
    return siguiente;
}

// Indica si la posición está vacía (nullptr)
bool TListaPos::EsVacia() const {
    return this->pos == nullptr;
}

// ------------------------------------------------------

// ------------------- TListaCom -------------------

// Constructor por defecto: lista inicialmente vacía (sin nodos)
TListaCom::TListaCom() {
    this->primero = nullptr;
    this->ultimo = nullptr;
}

// Constructor de copia: realiza una copia profunda de los nodos en orden
TListaCom::TListaCom(const TListaCom & otra) {
    // Inicializamos la lista vacía
    this->primero = nullptr;
    this->ultimo = nullptr;

    // Recorremos la lista fuente y clonamos cada nodo al final de la nueva lista
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

// Destructor: libera todos los nodos reservados
TListaCom::~TListaCom() {
    TListaNodo *nodo = this->primero;
    while (nodo != nullptr) {
        this->primero = this->primero->siguiente;
        delete nodo;
        nodo = this->primero;
    }
    this->ultimo = nullptr;
}

// Operador de asignación: limpia la lista actual y copia la otra (deep copy)
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

// Comparación: dos listas son iguales si tienen la misma secuencia de elementos
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

// Concatenación: devuelve una nueva lista con los elementos de ambas listas en orden
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

// Resta de listas: elementos de la primera que no aparecen en la segunda
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

// Comprueba si la lista está vacía
bool TListaCom::EsVacia() const {
    return this->primero == nullptr;
}

// Inserta un elemento al principio de la lista. Devuelve false si falla la reserva.
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

// Inserta a la izquierda de la posición indicada (antes de p). Requiere p válida.
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

// Inserta a la derecha de la posición indicada (después de p). Requiere p válida.
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

// Busca y borra la primera ocurrencia del elemento. Mantiene enlaces correctamente.
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
            return true; // Hemos borrado la primera ocurrencia
        }
        nodo = nodo->siguiente;
    }
    return false; // No existía en la lista
}

// Busca y borra todas las ocurrencias del elemento. Devuelve true si borró al menos una.
bool TListaCom::BorrarTodos(const TComplejo &e) {
    bool borrado = false;
    TListaNodo *nodo = this->primero;

    while (nodo != nullptr) {
        if (nodo->e == e) {
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
    return borrado; // Devuelve true si se pudo borrar al menos uno
}

// Borra el nodo apuntado por la posición p y deja p vacía. Requiere p válida.
bool TListaCom::Borrar(TListaPos &p) {
    if (p.pos == nullptr) return false;

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

    return true;
}

// Devuelve el elemento en la posición p; si p es vacía, devuelve un TComplejo por defecto
TComplejo TListaCom::Obtener(const TListaPos &p) {
    if (p.pos == nullptr) {
        TComplejo vacio;
        return vacio;
    }
    return p.pos->e;
}

// Búsqueda lineal: devuelve true si el elemento está en la lista
bool TListaCom::Buscar(const TComplejo &e) const {
    // Se realiza una búsqueda lineal desde el primer elemento hasta el último
    TListaNodo *nodo = this->primero;
    while (nodo != nullptr) {
        if (nodo->e == e) {
            return true; // Lo encontramos
        }
        nodo = nodo->siguiente;
    }
    return false; // Llegamos al final y no estaba
}

// Longitud: recorre la lista y cuenta nodos
int TListaCom::Longitud() const {
    int contador = 0;
    TListaNodo *nodo = this->primero;
    while (nodo != nullptr) {
        contador++;
        nodo = nodo->siguiente;
    }
    return contador;
}

// Devuelve la primera posición (cabecera)
TListaPos TListaCom::Primera() const {
    TListaPos p;
    p.pos = this->primero;
    return p;
}

// Devuelve la última posición (cola)
TListaPos TListaCom::Ultima() const {
    TListaPos p;
    p.pos = this->ultimo;
    return p;
}

// Salida por flujo: imprime la lista entre llaves, elementos separados por espacio
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