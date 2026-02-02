//
// Created by julianhinojosagil on 1/2/26.
//

#include "../include/TComplejo.h"

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
