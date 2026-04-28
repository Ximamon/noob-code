//
// Created by julianhinojosagil on 25/3/26.
//

#ifndef TABBCOM_H
#define TABBCOM_H
#include "tcomplejo.h"
#include "tvectorcom.h"
#include <iostream>

class TNodoABB;

class TABBCom {
private:
    // Puntero al nodo
    TNodoABB *nodo;

    // Métodos auxiliares (les pongo const al final porque los métodos que los llaman son const)
    void InordenAux(TVectorCom &, int &) const;
    void PreordenAux(TVectorCom &, int &) const;
    void PostordenAux(TVectorCom &, int &) const;
    bool EsMenor(const TComplejo &c1, const TComplejo &c2) const;
    bool EsMayor(const TComplejo &c1, const TComplejo &c2) const;
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

    TComplejo Raiz() const;
    int Altura() const;
    int Nodos() const;
    int NodosHoja() const;

    TVectorCom Inorden() const;
    TVectorCom Preorden() const;
    TVectorCom Postorden() const;
    TVectorCom Niveles() const;

    friend std::ostream &operator<<(std::ostream &, const TABBCom &);
};

class TNodoABB {
    // Permitimos que TABBCom acceda a la parte privada de TNodoABB
    friend class TABBCom;
private:
    TComplejo item;
    TABBCom iz, de;

    TNodoABB();
    TNodoABB(const TNodoABB &);
    ~TNodoABB();
    TNodoABB &operator=(const TNodoABB &);
};

#endif //TABBCOM_H