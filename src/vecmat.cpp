#include "vecmat.h"



void Vector4::Set(double x,double y,double z)
{
 data[0]=x; data[1]=y; data[2]=z;
}

double Vector4::X() const
{
	return data[0];
}

double Vector4::Y() const
{
	return data[1];
}

double Vector4::Z() const
{
	return data[2];
}



Vector4 Vector4::operator+ (const Vector4 &gVector)
{

 Vector4 Result(0,0,0);
 Result.data[3] = 0.;
 for (int i=0;i<4;i++) Result.data[i]=data[i]+gVector.data[i];
 return Result;
}

Vector4 Vector4::operator- (const Vector4 &gVector)
{

 Vector4 Result(0,0,0);
 Result.data[3] = 0.;
 for (int i=0;i<4;i++) Result.data[i]=data[i]-gVector.data[i];
 return Result;
}

Vector4 operator* (const Vector4 &gVector,double val)
{

 Vector4 Result(0,0,0);
 Result.data[3] = 0.;
 for (int i=0;i<4;i++) Result.data[i]=gVector.data[i]*val;
 return Result;
}

Matrix4::Matrix4()
{
 data[0][0]=1.0; data[0][1]=0.0; data[0][2]=0.0; data[0][3]=0.0;
 data[1][0]=0.0; data[1][1]=1.0; data[1][2]=0.0; data[1][3]=0.0;
 data[2][0]=0.0; data[2][1]=0.0; data[2][2]=1.0; data[2][3]=0.0;
 data[3][0]=0.0; data[3][1]=0.0; data[3][2]=0.0; data[3][3]=1.0;
}

Matrix4 Matrix4::operator* (const Matrix4 gMatrix)
{
 
 Matrix4 tmp;
 for(int i =0; i < 4; ++i) tmp(i,i) = 0;

#pragma omp parallel for
 for (int i=0;i<4;i++)
    for (int j=0;j<4;j++)
     {
      tmp.data[i][j]=0.0;
      for (int k=0;k<4;k++)
       tmp.data[i][j]=tmp.data[i][j]+(data[i][k]*gMatrix.data[k][j]);
     }
 return tmp;
}

Vector4 operator* (const Matrix4 gMatrix,const Vector4 gVector)
{

 Vector4 tmp(0,0,0);
 tmp.data[3] = 0.;

 for (int i=0;i<4;i++)
  {
   tmp.data[i]=0.0;
   for (int j=0;j<4;j++) tmp.data[i]=tmp.data[i]+(gMatrix.data[i][j]*gVector.data[j]);
  }
 return tmp;
}





Matrix4 matrix_translate(Vector4 vec)
{
Matrix4 matr;
for(int i = 0; i < 3; ++i) matr(i,3) = vec.data[i];

return matr;
}


Matrix4 matrix_scale(Vector4 vec)
{
Matrix4 matr;
for(int i = 0; i < 3; ++i) matr(i,i) = vec.data[i];
return matr;
}

Matrix4 matrix_Xrotate(double angle)
{
Matrix4 matr;
double cosd = cos(angle);
double sind = sin(angle);

matr.data[1][1] = matr.data[2][2] = cosd;
matr.data[2][1] = sind;
matr.data[1][2] = -sind;

return matr;
}

Matrix4 matrix_Yrotate(double angle)
{
Matrix4 matr;
double cosd = cos(angle);
double sind = sin(angle);

matr.data[0][0] = matr.data[2][2] = cosd;
matr.data[0][2] = sind;
matr.data[2][0] = -sind;

return matr;
}


Matrix4 matrix_Zrotate(double angle)
{
Matrix4 matr;
double cosd = cos(angle);
double sind = sin(angle);

matr.data[0][0] = matr.data[1][1] = cosd;
matr.data[1][0] = sind;
matr.data[0][1] = -sind;

return matr;
}

Matrix4 matrix_perspgl(double w, double h, double n, double f)
{
    Matrix4 matr;
    matr(0,0) = 2*n/w;
    matr(1,1) = 2*n/h;
    matr(2,2) = -(f+n)/(f-n);
    matr(3,3) = 0.;

    matr(0,2) = 0.; //
    matr(1,2) = 0.; //

    matr(2,3) = -2.*f*n / (f - n);
    matr(3,2) = -1.;


return matr;
}

/*Matrix4 matrix_viewport(double w, double h)
{

    Matrix4 matr;
    matr(0,0) = w/2;
    matr(1,1) = -h/2;
    matr(2,2) = 2.;

    matr(0,3) = w/2.;
    matr(1,3) = h/2.;

    matr(2,3) = 0.1; //z translation
    return matr;
}*/
Matrix4 matrix_viewport(double w, double h)
{
double dim = std::min(w,h);


    Matrix4 matr;
    matr(0,0) = dim/2;
    matr(1,1) = -dim/2;
    matr(2,2) = 2.;

    matr(0,3) = w/2.;
    matr(1,3) = h/2.;

    matr(2,3) = 0.1; //z translation
    return matr;
}

namespace ColiderFunc
{
Vector4 intersect(LinEq line, LinEq side)
    {
        double x_inter = (side.b-line.b)/(line.a-side.a);
        return Vector4(x_inter,0., line.a*x_inter + line.b);
    }
}