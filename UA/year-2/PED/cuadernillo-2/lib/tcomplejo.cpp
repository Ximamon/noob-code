//
// Created by julianhinojosagil on 1/2/26.
//

#include "tcomplejo.h"
#include <iostream>
#include <cmath>


// ------------------- Forma canonica -------------------

// Constructor unico (cubre constructor por defecto, con 1 arg y 2 arg)
TComplejo::TComplejo(double re, double im) {
    this->re = re;
    this->im = im;
}

void TComplejo::copia(const TComplejo &other) {
    this->re = other.re;
    this->im = other.im;
}

// Constructor de copia
TComplejo::TComplejo(const TComplejo &other) {
    copia(other);
}

// Destructor
TComplejo::~TComplejo() {
    this->re = 0;
    this->im = 0;
}

// Operardor de Asignacion
TComplejo& TComplejo::operator= (const TComplejo &other) {
    if (this != &other) {
        this->re = other.re;
        this->im = other.im;
    }
    return *this;
}

// ------------------------------------------------------

// ------------------- Operadores Aritmeticos (clase) -------------------

TComplejo TComplejo::operator+ (const TComplejo& other) const {
    // Suma: (a+c) + (b+d)i
    TComplejo res(this->re + other.re, this->im + other.im);
    return res;
}

TComplejo TComplejo::operator- (const TComplejo& other) const {
    // Resta: (a-c) + (b-d)i
    TComplejo res(this->re - other.re, this->im - other.im);
    return res;
}

TComplejo TComplejo::operator* (const TComplejo& other) const {
    // Multiplicación: (ac - bd) + (ad + bc)i
    double realPart = (this->re * other.re) - (this->im * other.im);
    double imagPart = (this->re * other.im) + (this->im * other.re);
    TComplejo res(realPart, imagPart);
    return res;
}

TComplejo TComplejo::operator+ (double d) const {
    TComplejo temp(d);
    return (*this + temp);
}

TComplejo TComplejo::operator- (double d) const {
    TComplejo temp(d);
    return (*this - temp);
}

TComplejo TComplejo::operator* (double d) const {
    TComplejo temp(d);
    return (*this * temp);
}

// -----------------------------------------------------------------------

// ------------------- Comparacion -------------------

bool TComplejo::operator== (const TComplejo &other) const {
    return (this->re == other.re && this->im == other.im);
}

bool TComplejo::operator!= (const TComplejo &other) const {
    return !(*this == other);
}

// ---------------------------------------------------

// ------------------- Getter / Setters -------------------

double TComplejo::Re() const {
    return this->re;
}

double TComplejo::Im() const {
    return this->im;
}

void TComplejo::Re(const double re) {
    this->re = re;
}

void TComplejo::Im(const double im) {
    this->im = im;
}

// --------------------------------------------------------

// ------------------- Calculo (Arg y Mod) -------------------

double TComplejo::Arg() const {
    // atan2(y, x) calcula el arcotangente devolviendo el cuadrante correcto
    double a = atan2(this->im, this->re);
    return a;
}

double TComplejo::Mod() const {
    // Almacenamos en una variable para evitar problemas de precision
    double a = sqrt(pow(this->re, 2) + pow(this->im, 2));
    return a;
}

// -----------------------------------------------------------

// ------------------- Funciones amigas -------------------

std::ostream &operator<<(std::ostream &os, const TComplejo &complejo) {
    os << "(" << complejo.Re() << " " << complejo.Im() << ")";
    return os;
}

TComplejo operator+(double d, const TComplejo &other) {
    TComplejo temp(d);
    return temp + other;
}

TComplejo operator-(double d, const TComplejo &other) {
    TComplejo temp(d);
    return temp - other;
}

TComplejo operator*(double d, const TComplejo &other) {
    TComplejo temp(d);
    return temp * other;
}

// --------------------------------------------------------
