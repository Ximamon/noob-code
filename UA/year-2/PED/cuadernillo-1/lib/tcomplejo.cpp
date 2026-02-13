//
// Created by julianhinojosagil on 1/2/26.
//

#include "../include/tcomplejo.h"
#include <iostream>
#include <cmath>


TComplejo::TComplejo() {
    this->real = 0;
    this->imaginario = 0;
}

TComplejo::TComplejo(double re) {
    this->real = re;
    this->imaginario = 0;
}

TComplejo::TComplejo(double re, double im) {
    this->real = re;
    this->imaginario = im;
}

TComplejo::TComplejo(const TComplejo &other) {
    this->real = other.real;
    this->imaginario = other.imaginario;
}

TComplejo::~TComplejo() {
    this->real = 0;
    this->imaginario = 0;
}

TComplejo& TComplejo::operator= (const TComplejo &other) {
    if (this != &other) {
        this->real = other.real;
        this->imaginario = other.imaginario;
    }
    return *this;
}

TComplejo TComplejo::operator+ (const TComplejo& other) const {
    TComplejo res(this->real + other.real, this->imaginario + other.imaginario);
    return res;
}

TComplejo TComplejo::operator- (const TComplejo& other) const {
    TComplejo res(this->real - other.real, this->imaginario - other.imaginario);
    return res;
}

TComplejo TComplejo::operator* (const TComplejo& other) const {
    // Aplicando (ac - bd) + (ad + ac)
    double realPart = (this->real * other.real) - (this->imaginario * other.imaginario);
    double imagPart = (this->real * other.imaginario) + (this->imaginario * other.real);
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

bool TComplejo::operator==(const TComplejo &other) const {
    return (this->real == other.real && this->imaginario == other.imaginario);
}

bool TComplejo::operator!=(const TComplejo &other) const {
    return (this->real != other.real || this->imaginario != other.imaginario);
}

double TComplejo::Re() const {
    return this->real;
}

double TComplejo::Im() const {
    return this->imaginario;
}

void TComplejo::Re(const double re) {
    this->real = re;
}

void TComplejo::Im(const double im) {
    this->imaginario = im;
}

double TComplejo::Arg() const {
    // atan2(y, x) calcula el arcotangente devolviendo el cuadrante correcto
    double a = atan2(this->imaginario, this->real);
    return a;
}

double TComplejo::Mod() const {
    // Almacenamos en una variable para evitar problemas de precision
    double a = sqrt(pow(this->real, 2) + pow(this->imaginario, 2));
    return a;
}

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
