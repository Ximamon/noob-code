//
// Created by julianhinojosagil on 4/3/26.
//

#ifndef TLISTACOM_H
#define TLISTACOM_H

#include <iostream>
#include "tcomplejo.h"
#include "tvectorcom.h"

class TListaNodo {
    // Declaramos las clases amigas para que puedan acceder a la parte privada
    friend class TListaPos;
    friend class TListaCom;
public:
    TListaNodo();
    TListaNodo(const TListaNodo &);
    ~TListaNodo();
    TListaNodo &operator=(const TListaNodo &);
private:
    TComplejo e;
    TListaNodo *anterior;
    TListaNodo *siguiente;
};

class TListaPos {
    friend class TListaCom;
public:
    TListaPos();
    TListaPos(const TListaPos &);
    ~TListaPos();
    TListaPos &operator=(const TListaPos &);

    bool operator==(const TListaPos &) const;
    bool operator!=(const TListaPos &) const;

    TListaPos Anterior() const;
    TListaPos Siguiente() const;

    bool EsVacia() const;
private:
    TListaNodo *pos;
};

class TListaCom {
    friend std::ostream & operator<<(std::ostream &, const TListaCom &);
public:
    TListaCom();
    TListaCom(const TListaCom&);
    ~TListaCom();
    TListaCom& operator=(const TListaCom&);

    bool operator==(const TListaCom&) const;
    bool operator!=(const TListaCom&) const;

    TListaCom operator+(const TListaCom&) const;
    TListaCom operator-(const TListaCom&) const;

    bool EsVacia() const;
    bool InsCabeza(const TComplejo&);
    bool InsertarI(const TComplejo&, const TListaPos&);
    bool InsertarD(const TComplejo&, const TListaPos&);
    bool Borrar(const TComplejo&);
    bool BorrarTodos(const TComplejo&);
    bool Borrar(TListaPos&);

    static TComplejo Obtener(const TListaPos&);
    bool Buscar(const TComplejo&) const;
    int Longitud() const;
    TListaPos Primera() const;
    TListaPos Ultima() const;
private:
    TListaNodo *primero;
    TListaNodo *ultimo;
};

#endif //TLISTACOM_H