//
// Created by julianhinojosagil on 1/2/26.
//

#ifndef TCOMPLEJO_H
#define TCOMPLEJO_H
#include <iosfwd>

class TComplejo {
public:
    TComplejo ();                       //Constructor por defecto : PARTE REAL e IMAGINARIA inicializadas a 0
    TComplejo (double );                //Constructor a partir de la PARTE REAL
    TComplejo (double , double);        //Constructor a partir de la PARTE REAL e IMAGINARIA
    TComplejo (const TComplejo&);       //Constructor copia
    ~TComplejo();                       //Destructor
    TComplejo& operator= (const TComplejo&);  // Sobrecarga del operador asignación

    TComplejo operator+ (const TComplejo&) const;
    TComplejo operator- (const TComplejo&) const;
    TComplejo operator* (const TComplejo&) const;
    TComplejo operator+ (double) const;
    TComplejo operator- (double) const;
    TComplejo operator* (double) const;

    bool operator== (const TComplejo&) const;   // IGUALDAD de números complejos
    bool operator!= (const TComplejo&) const;   // DESIGUALDAD de números complejos
    double Re() const;                          // Devuelve PARTE REAL
    double Im() const;                          // Devuelve PARTE IMAGINARIA
    void Re(double);                            // Modifica PARTE REAL
    void Im(double);                            // Modifica PARTE IMAGINARIA
    double Arg() const;                         // Calcula el Argumento (en Radianes)
    double Mod() const;                         // Calcula el Módulo

    // Sobrecarga del operador SALIDA
    friend std::ostream & operator<<(std::ostream &, const  TComplejo &);
    friend TComplejo operator+ (double , const TComplejo&);
    friend TComplejo operator- (double , const TComplejo&);
    friend TComplejo operator* (double , const TComplejo&);
private:
    double real;
    double imaginario;
};

#endif //TCOMPLEJO_H