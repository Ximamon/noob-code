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
    TComplejo (const TComplejo&);             //Constructor copia
    ~TComplejo();                       //Destructor
    TComplejo& operator= (TComplejo&);  // Sobrecarga del operador asignación

    TComplejo operator+ (TComplejo&);
    TComplejo operator- (TComplejo&);
    TComplejo operator* (TComplejo&);
    TComplejo operator+ (double);
    TComplejo operator- (double);
    TComplejo operator* (double);

    bool operator== (TComplejo&);       // IGUALDAD de números complejos
    bool operator!= (TComplejo&);       // DESIGUALDAD de números complejos
    double Re();                        // Devuelve PARTE REAL
    double Im();                        // Devuelve PARTE IMAGINARIA
    void Re(double);                    // Modifica PARTE REAL
    void Im(double);                    // Modifica PARTE IMAGINARIA
    double Arg(void);                   // Calcula el Argumento (en Radianes)
    double Mod(void);                   // Calcula el Módulo

    // Sobrecarga del operador SALIDA
    friend std::ostream & operator<<(std::ostream &, TComplejo &);
    friend TComplejo operator+ (double , TComplejo&);
    friend TComplejo operator- (double , TComplejo&);
    friend TComplejo operator* (double , TComplejo&);
private:
    double real;
    double imaginario;
};

#endif //TCOMPLEJO_H