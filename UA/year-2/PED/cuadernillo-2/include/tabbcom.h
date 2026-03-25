//
// Created by julianhinojosagil on 25/3/26.
//

#ifndef TABBCOM_H
#define TABBCOM_H
#include "tcomplejo.h"
#include "tvectorcom.h"

class TNodoABB {
public:
    TNodoABB();
    TNodoABB(const TNodoABB &);
    ~TNodoABB();
    TNodoABB &operator=(const TNodoABB &);

private:
    TComplejo item;
    TABBCom iz, de;
};

class TABBCom {
public:
    TABBCom();
    TABBCom(const TABBCom &);
    ~TABBCom();
    TABBCom &operator=(const TABBCom &);

    bool operator==(const TABBCom &);
    bool EsVacio() const;
    bool Insertar(const TComplejo &);
    bool Borrar(const TComplejo &);
    bool Buscar(const TComplejo &) const;

    TComplejo raiz();
    int Altura() const;
    int Nodos() const;
    int NodosHoja() const;

    TVectorCom Inorden() const;
    TVectorCom Preorden() const;
    TVectorCom Postorden() const;
    TVectorCom Niveles() const;

    friend std::ostream &operator<<(std::ostream &, const TABBCom &);
};

#endif //TABBCOM_H