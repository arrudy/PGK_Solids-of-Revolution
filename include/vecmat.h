#pragma once

#include <stdio.h>
#include <math.h>
#include <iostream>

#ifndef PI
#define PI (atan(1.) * 4.)
#endif


class Vector4
{
 public:
 double data[4];
 Vector4(double x = 0., double y = 0., double z = 0.) : data{x,y,z,1.} {}; //tworzy wektor zerowy
 void Set(double x,double y,double z);
 double X() const;
 double Y() const;
 double Z() const;
 //operacje dodawania i odejmowania wektorow
    Vector4 operator+(const Vector4 &);
    Vector4 operator-(const Vector4 &);

//operator dostepu do elementow wektora
 double & operator[](int row) {return data[row];};



//operacja mnozenia wektora przez skalar a*V
 friend Vector4 operator*(const Vector4 &,double);


friend std::ostream & operator<<(std::ostream & stream, const Vector4 & obj)
{
    return stream << "(" << obj.X() << ", " << obj.Y() << ", " << obj.Z() << ")";
}

};

class Matrix4
{
 public:
 double data[4][4];
 Matrix4(); //tworzy macierz jednostkowa

//operator dostepu do elementow macierzy
 double & operator()(int row, int col) {return data[row][col];};

//operator mnozenia 2 macierzy M*M
 Matrix4 operator*(const Matrix4);

//operator przeksztalcania wektora macierza M*V
 friend Vector4 operator*(const Matrix4,const Vector4);
};


//implementacja macierzy przeksztalcen


Matrix4 matrix_translate(Vector4 vec);
Matrix4 matrix_scale(Vector4 vec);
Matrix4 matrix_Xrotate(double angle); //przyjmuje kat w radianach!
Matrix4 matrix_Yrotate(double angle); //przyjmuje kat w radianach!
Matrix4 matrix_Zrotate(double angle); //przyjmuje kat w radianach!
Matrix4 matrix_perspgl(double w, double h, double n, double f);
Matrix4 matrix_viewport(double w, double h);


namespace ColiderFunc{

struct LinEq{
    double a;
    double b;


    LinEq(Vector4 v1, Vector4 v2)
    {
        if(v1.X() > v2.X()) std::swap(v1,v2);
        a = (v2.Z()-v1.Z()) / (v2.X()-v1.X());
        b = v1.Z() - a*v1.X();
    };

    LinEq(Vector4 v, double a) : a{a}, b{v.Z()-a*v.X()} {};

};

//returns point of intersection between two lines
Vector4 intersect(LinEq line, LinEq side);
}