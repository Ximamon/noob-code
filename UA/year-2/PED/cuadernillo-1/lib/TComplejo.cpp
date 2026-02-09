//
// Created by julianhinojosagil on 1/2/26.
//

#include "../include/TComplejo.h"
#include <iostream>
#include <iosfwd>


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
        (*this).~TComplejo();
        this->real = other.real;
        this->imaginario = other.imaginario;
    }
    return *this;
}

bool TComplejo::operator==(const TComplejo &other) const {
    bool temp = (real == other.real && imaginario == other.imaginario) ? true : false;
    return temp;
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

std::ostream &operator<<(std::ostream &os, const TComplejo &complejo) {
    os << "(" << complejo.Re() << " " << complejo.Im() << ")" << std::endl;
    return os;
}
