//
// Created by julianhinojosagil on 25/3/26.
//

#include "../include/tabbcom.h"
#include <queue>
#include <cmath>

// ------------------- Forma canonica -------------------
// ---------------------  TNodoABB  ---------------------

// Constructor por defecto, inicializa el item con un TComplejo por defecto y los subárboles izquierdo y derecho como vacíos
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

// ------------------------------------------------------

// ------------------- Forma canonica -------------------
// ---------------------  TABBCom  ----------------------

// Constructor por defecto, inicializa el nodo como un puntero nulo (árbol vacío)
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

// ------------------------------------------------------

// ----------------- Metodos operadores -----------------
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

// Método de búsqueda: Devuelve true si el complejo c se encuentra en el árbol, false en caso contrario
bool TABBCom::Buscar(const TComplejo &c) const {
    if (EsVacio()) return false;

    // Si el complejo es igual al del nodo actual, lo hemos encontrado
    if (c == this->nodo->item)
        return true;
    // Si es menor, buscamos en el subárbol izquierdo
    else if (EsMenor(c, this->nodo->item))
        return this->nodo->iz.Buscar(c);
    // Si es mayor, buscamos en el subárbol derecho
    else 
        return this->nodo->de.Buscar(c);
}

// Insértalo en el árbol. Devuelve true si se ha insertado correctamente, false si el complejo ya existía en el árbol
bool TABBCom::Insertar(const TComplejo &c) {
    // Si el árbol está vacío, creamos un nuevo nodo con el complejo c y lo asignamos a nodo
    if (EsVacio()) {
        this->nodo = new TNodoABB();
        this->nodo->item = c;
        return true;
    }

    if (c == this->nodo->item)
        return false;
    // Si el complejo es menor que el del nodo actual, lo insertamos en el subárbol izquierdo
    else if (EsMenor(c, this->nodo->item))
        return this->nodo->iz.Insertar(c);
    // Si no, lo insertamos en el subárbol derecho
    else
        return this->nodo->de.Insertar(c);
}

// Borra el complejo c del árbol. Devuelve true si se ha borrado correctamente, false si el complejo no existía en el árbol
bool TABBCom::Borrar(const TComplejo &c) {
    if (EsVacio()) return false;
    

    if (EsMenor(c, this->nodo->item)) {
        return nodo->iz.Borrar(c);
    } else if (EsMayor(c, this->nodo->item)) {
        return nodo->de.Borrar(c);
    // Si el complejo es igual al del nodo actual, lo hemos encontrado y debemos borrarlo
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

// Dos árboles son iguales si sus recorridos inorden son iguales (porque el orden de los elementos en el recorrido inorden es único para cada árbol)
bool TABBCom::operator==(const TABBCom &otro) {
    return this->Inorden() == otro.Inorden();
}

// ------------------------------------------------------

// ----------------- Metodos de consulta ----------------

// Devuelve el complejo almacenado en la raíz del árbol. Si el árbol está vacío, devuelve un complejo por defecto (0 + 0i)
TComplejo TABBCom::Raiz() const {
    if (EsVacio()) return TComplejo();

    return this->nodo->item;
}

// Devuelve la altura del árbol. La altura de un árbol vacío es 0, la altura de un árbol con solo un nodo es 1, etc
int TABBCom::Altura() const {
    if (EsVacio()) return 0;

    // La altura de un nodo es 1 + la altura del subárbol más alto entre el izquierdo y el derecho
    int alturaIzquierda = this->nodo->iz.Altura();
    int alturaDerecha = this->nodo->de.Altura();

    if (alturaIzquierda > alturaDerecha)
        return 1 + alturaIzquierda;
    else
        return 1 + alturaDerecha;
}

// Devuelve el número total de nodos en el árbol. Un árbol vacío tiene 0 nodos
int TABBCom::Nodos() const {
    if (EsVacio()) return 0;

    return 1 + this->nodo->iz.Nodos() + this->nodo->de.Nodos();
}

// Devuelve el número de nodos hoja en el árbol
// Un nodo hoja es aquel que no tiene hijos (es decir, ambos subárboles izquierdo y derecho son vacíos)
int TABBCom::NodosHoja() const {
    if (EsVacio()) return 0;

    if (this->nodo->iz.EsVacio() && this->nodo->de.EsVacio()) return 1;

    return this->nodo->iz.NodosHoja() + this->nodo->de.NodosHoja();
}

// ------------------------------------------------------

// --------------------- Recorridos ---------------------

// Método auxiliar privado para el recorrido inorden
// Rellena el vector v con los elementos del árbol en orden, utilizando pos como índice de inserción
void TABBCom::InordenAux(TVectorCom &v, int &pos) const {
    if (!EsVacio()) {
        this->nodo->iz.InordenAux(v, pos);

        v[pos] = this->nodo->item;
        pos++;

        this->nodo->de.InordenAux(v, pos);
    }
} 

// El recorrido inorden visita primero el subárbol izquierdo, luego el nodo actual, y finalmente el subárbol derecho
TVectorCom TABBCom::Inorden() const {
    int pos = 1;
    TVectorCom v(Nodos());
    InordenAux(v, pos);
    return v;
}

// Método auxiliar privado para el recorrido preorden
void TABBCom::PreordenAux(TVectorCom &v, int &pos) const {
    if (!EsVacio()) {
        v[pos] = nodo->item;
        pos++;
        
        this->nodo->iz.PreordenAux(v, pos);
        this->nodo->de.PreordenAux(v, pos);
    }
}

// El recorrido preorden visita primero el nodo actual, luego el subárbol izquierdo, y finalmente el subárbol derecho
TVectorCom TABBCom::Preorden() const {
    int pos = 1;
    TVectorCom v(Nodos());
    PreordenAux(v, pos);
    return v;
}

// Método auxiliar privado para el recorrido postorden
void TABBCom::PostordenAux(TVectorCom &v, int &pos) const {
    if (!EsVacio()) {
        this->nodo->iz.PostordenAux(v, pos);
        this->nodo->de.PostordenAux(v, pos);
        
        v[pos] = nodo->item;
        pos++;
    }
}

// El recorrido postorden visita primero el subárbol izquierdo, luego el subárbol derecho, y finalmente el nodo actual
TVectorCom TABBCom::Postorden() const {
    int pos = 1;
    TVectorCom v(Nodos());
    PostordenAux(v, pos);
    return v;
}

// El recorrido por niveles visita los nodos del árbol nivel por nivel, de izquierda a derecha
TVectorCom TABBCom::Niveles() const {
    TVectorCom v(Nodos());

    if (EsVacio()) return v;

    // Utilizamos una cola para realizar el recorrido por niveles (BFS)
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

// ------------------------------------------------------

std::ostream &operator<<(std::ostream &os, const TABBCom &arbol) {
    os << arbol.Niveles();
    return os;
}