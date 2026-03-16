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
public:
    TListaPos();
    TListaPos(const TListaPos &);
    ~TListaPos();
    TListaPos &operator=(const TListaPos &);

    bool operator==(const TListaPos &);
    bool operator!=(const TListaPos &);

    TListaPos Anterior();
    TListaPos Siguiente();

    bool EsVacia();
private:
    TListaNodo *pos;
};

class TListaCom {
    friend std::ostream & operator<<(std::ostream &, TListaCom &);
public:
    TListaCom();
    TListaCom(const TListaCom&);
    ~TListaCom();
    TListaCom& operator=(const TListaCom&);

    bool operator==(const TListaCom&);
    bool operator!=(const TListaCom&);

    TListaCom operator+(const TListaCom&);
    TListaCom operator-(const TListaCom&);

    bool EsVacia();
    bool InsCabeza(const TComplejo&);
    bool InsertarI(TComplejo&, const TListaCom&);
    bool InsertarD(TComplejo&, const TListaCom&);
    bool Borrar(TComplejo&);
    bool BorrarTodos(const TComplejo&);
    bool Borrar(const TListaPos&);
    TComplejo Obtener(const TListaPos&);
    bool Buscar(TComplejo&);
    int Longitud();
    TListaPos Primera();
    TListaPos Ultima();
private:
    TListaNodo *primero;
    TListaNodo *ultimo;
};

#endif //TLISTACOM_H