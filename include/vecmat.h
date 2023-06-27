#pragma once

#include <stdio.h>
#include <math.h>
#include <iostream>

#ifndef PI
#define PI (atan(1.) * 4.)
#endif

/**
 * @brief Klasa reprezentująca 4-wymiarowy wektor (X,Y,Z,L)
 * 
 */
class Vector4
{
 public:
 double data[4];

 /**
  * @brief Konstruktor domyślny wektora.
  * 
  * @param x Współrzędna X.
  * @param y Współrzędna Y.
  * @param z Współrzędna Z.
  */
 Vector4(double x = 0., double y = 0., double z = 0.) : data{x,y,z,1.} {}; //tworzy wektor zerowy
 
 
 void Set(double x,double y,double z);

 /// @brief Operator dostępu do zmiennej.
 double X() const;
  /// @brief Operator dostępu do zmiennej.
 double Y() const;
  /// @brief Operator dostępu do zmiennej.
 double Z() const;
    /// @brief operacja dodawania wektorów
    Vector4 operator+(const Vector4 &);
    /// @brief operacja odejmowania wektorów
    Vector4 operator-(const Vector4 &);

///operator dostepu do elementow wektora
 double & operator[](int row) {return data[row];};



///Operacja mnożenia wektora przez skalar a*V
 friend Vector4 operator*(const Vector4 &,double);

/// @brief Awaryjny operator wypisania.
friend std::ostream & operator<<(std::ostream & stream, const Vector4 & obj)
{
    return stream << "(" << obj.X() << ", " << obj.Y() << ", " << obj.Z() << ")";
}

};

class Matrix4
{
 public:
 double data[4][4];
 Matrix4(); ///Tworzy macierz jednostkową

///Operator dostępu do elementów macierzy
 double & operator()(int row, int col) {return data[row][col];};

///Operator mnożenia 2 macierzy (M*M)
 Matrix4 operator*(const Matrix4);

///Operator przeksztalcania wektora macierzą (M*V)
 friend Vector4 operator*(const Matrix4,const Vector4);
};


//implementacja macierzy przeksztalcen

/**
 * @brief Generator macierzy translacji o wektor.
 * 
 * @param vec Wektor translacji docelowej.
 * @return Matrix4 
 */
Matrix4 matrix_translate(Vector4 vec);
/**
 * @brief Generator macierzy skalowania wektora.
 * 
 * @param vec Wektor skali.
 * @return Matrix4 
 */
Matrix4 matrix_scale(Vector4 vec);
/**
 * @brief Generator macierzy rotacji w osi X.
 * 
 * @param angle Kąt w radianach.
 * @return Matrix4 
 */
Matrix4 matrix_Xrotate(double angle); 
/**
 * @brief Generator macierzy rotacji w osi Y.
 * 
 * @param angle Kąt w radianach.
 * @return Matrix4 
 */
Matrix4 matrix_Yrotate(double angle);
/**
 * @brief Generator macierzy rotacji w osi Z.
 * 
 * @param angle Kąt w radianach.
 * @return Matrix4 
 */
Matrix4 matrix_Zrotate(double angle); 
/**
 * @brief Generator macierzy rzutu perspektywicznego.
 * 
 * @param w Szerokość docelowa pola widzenia na dystansie f.
 * @param h Wysokość docelowa pola widzenia na dystansie f.
 * @param n Dystans near pola widzenia.
 * @param f Dystans far pola widzenia.
 * @return Matrix4 
 */
Matrix4 matrix_perspgl(double w, double h, double n, double f);
/**
 * @brief Generator macierzy translującej i skalującej obraz do rozmiarów okna.
 * 
 * @param w 
 * @param h 
 * @return Matrix4 
 */
Matrix4 matrix_viewport(double w, double h);


namespace ColiderFunc{


/**
 * @brief Struktura reprezentująca równanie prostej w postaci z = ax + b
 * 
 */
struct LinEq{
    /// @brief Współczynnik liniowy prostej.
    long double a;
    /// @brief Offset prostej od osi.
    long double b;

/**
 * @brief Wyznacza równanie prostej postaci z = ax+b
 * 
 * @param v1 Pierwszy wektor.
 * @param v2 Drugi wektor.
 */
    LinEq(Vector4 v1, Vector4 v2)
    {
        if(v1.X() > v2.X()) std::swap(v1,v2);
        a = (v2.Z()-v1.Z()) / (v2.X()-v1.X());
        b = v1.Z() - a*v1.X();
    };

/**
 * @brief Wyznacza równanie prostej postaci z = ax+b
 * 
 * @param v Wektor początkowy.
 * @param a Współczynnik kierunkowy.
 */
    LinEq(Vector4 v, double a) : a{a}, b{v.Z()-a*v.X()} {};

};

/**
 * @brief Zwraca punkt przecięcia dwóch prostych
 * 
 * @param line Pierwsza linia.
 * @param side Druga linia.
 * @return Vector4 
 */
Vector4 intersect(LinEq line, LinEq side);
}