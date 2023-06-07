#include "mesh.h"



Vector4 Triangle::normal()
{
Vector4 vec1 = _vert[0] - _vert[1];
Vector4 vec2 = _vert[2] - _vert[1];

return Vector4
(
    vec1.Y() *vec2.Z() - vec2.Y() * vec1.Z(),
    vec2.X() *vec1.Z() - vec1.X() * vec2.Z(),
    vec1.X() *vec2.Y() - vec2.X() * vec1.Y()
);
}



double Triangle::minZ() const
{
    Vector4 min = _vert[0]; //nazwa zwodzaca, bo szukamy maximum
    for(unsigned short i = 1; i < 3; ++i)
    {
        if(_vert[i].Z() > min.Z()) min = _vert[i];
    }
    
    //https://www.geeksforgeeks.org/program-to-find-the-centroid-of-the-triangle/
    Vector4 buffer;

    for(short i = 0; i < 3; ++i)
    {
        double pos = 0.;
        for(short j = 0; j < 3; ++j)
        {
            pos += _vert[j].data[i];
        }
        pos /=2.;
        buffer.data[i] = pos;
    }

//zmodyfikowany painter's algorithm tak, by bral pod uwage zarowno najdalszy punkt, jak i srodek trojkata
    double minval = /*min.Z();*//*buffer.Z();*/( min.Z() +  buffer.Z()) / 2.;


return minval;
}