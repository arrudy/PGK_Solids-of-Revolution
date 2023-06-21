#include "GUIMyFrame1.h"
#include "Frame.h"
#include <vector>
#include <fstream>
#include <queue>
#include <utility>
#include <algorithm>

#ifndef PI
#define PI (4.*atan(1.))
#endif


void GUIMyFrame1::screenshot( wxCommandEvent& event )
{
    //zapis do pliku
    wxFileDialog saveFileDialog(this, "Choose a file", "", "", "PNG Files (*.png)|*.png|BMP Files (*.bmp)|*.bmp", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (saveFileDialog.ShowModal() == wxID_CANCEL) return;

	wxImage screen(draw_target);
	screen.AddHandler(new wxPNGHandler);

	screen.SaveFile(saveFileDialog.GetPath());
}






void GUIMyFrame1::close( wxCloseEvent& event ) {d_ptr->Destroy(); Destroy();};

GUIMyFrame1::GUIMyFrame1( wxWindow* parent )
:
MyFrame1( parent )
{
 m_button_load_geometry->SetLabel(_("Wczytaj Geometri\u0119"));
 m_staticText25->SetLabel(_("Obr\u00F3t X:"));
 m_staticText27->SetLabel(_("Obr\u00F3t Y:"));
 m_staticText29->SetLabel(_("Obr\u00F3t Z:"));

 WxSB_TranslationX->SetRange(0, 200); WxSB_TranslationX->SetValue(100);
 WxSB_TranslationY->SetRange(0, 200); WxSB_TranslationY->SetValue(100);
 WxSB_TranslationZ->SetRange(0, 200); WxSB_TranslationZ->SetValue(100);

 WxSB_RotateX->SetRange(0, 360); WxSB_RotateX->SetValue(0);
 WxSB_RotateY->SetRange(0, 360); WxSB_RotateY->SetValue(0);
 WxSB_RotateZ->SetRange(0, 360); WxSB_RotateZ->SetValue(0);

 WxSB_ScaleX->SetRange(1, 200); WxSB_ScaleX->SetValue(100);
 WxSB_ScaleY->SetRange(1, 200); WxSB_ScaleY->SetValue(100);
 WxSB_ScaleZ->SetRange(1, 200); WxSB_ScaleZ->SetValue(100);
}

void GUIMyFrame1::WxPanel_Repaint( wxUpdateUIEvent& event )
{
 Repaint();
}

void GUIMyFrame1::prepareData(std::vector<Vector4> & data, size_t quality = 21)
{
if(!data.size()) return;
//zdecydowalem sie chwilowo na 2x liczenie tego samego (prev_v i curr_v) aby choc troche uproscic implementacje; na pewno da sie zoptymalizowac (czy jest po co?)
//chyba wystarczy curr_v policzyc przed petla a potem w petli podmienic z prev, tam liczac curr
//albo cos z petla do{}while pokombinowac
//gdybym jeszcze to dorzucil to ten kod bylby calkowicie nieczytelny i niedebugowalny, wiec sie powstrzymalem

//albo #pragma omp parallel for; wiecej liczenia ale pelne zrownoleglenie

/*############
##############
#### PREP ####
##############
############*/

isConvex = true;
//sprawdzam czy bryla jest wypokla
//koncept back_4
{
    double minx = data[0].X(), maxx = data[0].X();
    for(size_t row = 0; row < data.size() ; ++row)
    {
    size_t row_prev = row;
    size_t row_curr = (row+1) % (data.size());

    double x_begin = data[row_prev].X();
    double x_end = data[row_curr].X();
    if(x_begin > x_end) std::swap(x_begin,x_end);

    for(size_t another = row+1; another < data.size()-2; ++another) //koncept back_4
    {
        size_t an_prev = (another+1) % (data.size());
        size_t an_curr = (another+2) % (data.size());

        ColiderFunc::LinEq line1 (data[row_prev], data[row_curr]);
        ColiderFunc::LinEq line2 (data[an_prev], data[an_curr]);

        Vector4 PoI = ColiderFunc::intersect(line1,line2);

        if(x_begin < PoI.X() && PoI.X() < x_end ) 
        {
            double x_anbegin = data[an_prev].X();
            double x_anend = data[an_curr].X();
            if(x_anbegin > x_anend) std::swap(x_anbegin,x_anend);
            if(x_anbegin < PoI.X() && PoI.X() < x_anend ) 
            {
            isConvex = false;
            break;
            }



        }

    }

    if(x_end > maxx) maxx = x_end;
    if(x_begin < minx) minx = x_begin;

    if(maxx * minx < 0.)
    {
        isConvex = false;
        break;
    }

    }



}



if(!isConvex)
{
    m_checkBoxRender->SetValue(false);
    m_checkBoxRender->Disable();
}
else
{
    m_checkBoxRender->Enable();
    m_checkBoxRender->SetValue(simplified);
}



_data.clear();

#pragma omp parallel for
for(size_t row = 0; row < data.size() ; ++row) // krawedzi jest n, scian jest n
{
size_t prev = row;
size_t curr = (row+1) % (data.size()); //celuje w nastepny; potrzebuje dwa rozne rzedy ponktow do obliczen. Modulo, bo zapetla sie na koniec by utworzyc ostatnia powierzchnie

double delta_angle = 2.*PI / quality; //koncept back_1; to jest w radianach offset miedzy kolejnymi punktami na okregu, ktory jest tworzony

//offset procesowanego rzedu - koncept back_2 lewo gora
//istotne aby operowalo na wlasciwym ID okregu, jesli ma sie prawidlowo stykac z poczatkiem
double prev_degree_offset = (prev %2 ? 0. : delta_angle / 2.); //prev ma offset jesli jest parzysty
double curr_degree_offset = (curr %2 ? 0. : delta_angle / 2.); //curr ma offset jesli jest nieparzysty

//wlasciwe wektory, ktore przetrzymuja okregi na ktorych leza punkty
std::vector<Vector4> prev_v;
std::vector<Vector4> curr_v;


//stworzenie okregow do dalszego procesowania
for(size_t i = 0; i < quality; ++i)
{
    prev_v.push_back(matrix_Zrotate(static_cast<double>(i) * delta_angle + prev_degree_offset) * data[prev]); 
}
for(size_t i = 0; i < quality; ++i)
{
    curr_v.push_back(matrix_Zrotate(static_cast<double>(i) * delta_angle + curr_degree_offset) * data[curr]);
}






//koncept back_2
//musi byc wskaznik; dla referencji calkowicie sie psuje
std::vector<std::pair<std::vector<Vector4> *, std::vector<Vector4>::iterator>> queue;

//istotne jest w jaki sposob preparujemy kolejke; w ten sposob narzucamy czesciowo ksztalt trojkatow
//wymuszamy tez kierunek normalnej dla pierwszego trojkata (koncept back_3)
if(!(curr % 2)) //curr ma offset
{
    queue.push_back(std::pair<std::vector<Vector4> *, std::vector<Vector4>::iterator>(&prev_v,prev_v.begin()));
    queue.push_back(std::pair<std::vector<Vector4> *, std::vector<Vector4>::iterator>(&curr_v,curr_v.begin()));
    queue.push_back(std::pair<std::vector<Vector4> *, std::vector<Vector4>::iterator>(&prev_v,prev_v.begin()+1));
}
else //prev ma offset
{
    queue.push_back(std::pair<std::vector<Vector4> *, std::vector<Vector4>::iterator>(&curr_v,curr_v.begin()));
    queue.push_back(std::pair<std::vector<Vector4> *, std::vector<Vector4>::iterator>(&prev_v,prev_v.begin()));
    queue.push_back(std::pair<std::vector<Vector4> *, std::vector<Vector4>::iterator>(&curr_v,curr_v.begin()+1));
}


//funkcja lambda iterujaca wektor w petli; po przeskoczeniu za koniec wraca na poczatek
//zwraca to co ma byc nastepne w iteracji
//operuje na parze (vektor, iterator) w celu prawidlowego zapetlania
auto loop_iterate = []( const std::pair<std::vector<Vector4> *, std::vector<Vector4>::iterator> & tup) 
{ return tup.second + 1 < tup.first->end() 
    ?      
    std::pair<std::vector<Vector4> *, std::vector<Vector4>::iterator>{tup.first,tup.second + 1}    
    :      
    std::pair<std::vector<Vector4> *, std::vector<Vector4>::iterator>{tup.first,tup.first->begin()}      ; };



/*############
##############
#### CORE ####
##############
############*/

//koncept back_2
for(size_t iter = 0; iter < 2*quality; ++iter)
{
    //rdzen generatora; generowanie trojkatow w oparciu o kolejke FIFO z uwzglednieniem kierunku normalnej i przesuniecia rzedu (pierwszy if-else w petli for) (koncept back_3)
    //if((iter%2)) //w wariancie uproszczonym, gdy nie robimy przesuniecia rzedow
    if((iter%2) ^ (curr%2)) //warianty pelny
    {
    #pragma omp critical(_data)
    _data.push_back(Triangle{{*(queue[2].second),*(queue[1].second),*(queue[0].second)}});
    }
    else
    {
    #pragma omp critical(_data)
    _data.push_back(Triangle{{*(queue[0].second),*(queue[1].second),*(queue[2].second)}});
    }
    std::pair<std::vector<Vector4> *, std::vector<Vector4>::iterator> temp = loop_iterate(queue[1]); //iteruje
    
    /*
    for(auto & obj : queue)
    {
        std::cout << (*std::get<1>(obj)) << " ";
    }
    std::cout << "\n";
    for(auto & obj : _data.back()._vert) std::cout << obj << " ";

    std::cout << "\n" << *std::get<1>(temp) << "\n";
    */

   
    //i modyfikuje kolejke aby celowala na nastepny trojkat
    queue.insert(queue.end(),temp);
    queue.erase(queue.begin());



}
//std::cout << "\n";


}

std::cout << "Size of _data: " << _data.size() << "\n";

}


void GUIMyFrame1::m_button_load_geometry_click( wxCommandEvent& event )
{
 wxFileDialog WxOpenFileDialog(this, wxT("Choose a file"), wxT(""), wxT(""), wxT("Geometry file (*.geo)|*.geo"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);



 if (WxOpenFileDialog.ShowModal() == wxID_OK)
 {
  double x1, y1, z1;

  std::ifstream in(WxOpenFileDialog.GetPath().ToAscii());
  if (in.is_open())
  {
   _raw_data.clear();
   while (!in.eof())
   {
    in >> x1 >> y1 >> z1;
    Vector4 point(x1, y1, z1);
    std::cout << point << "\n";
    _raw_data.push_back(point);
   }
   in.close();
  }


{
bool isPlanar = true;
double angle = atan2(_raw_data.front().Y(),_raw_data.front().X());
for(auto & obj : _raw_data)
{
    if( fabs(atan2(obj.Y(),obj.X()) - angle) > 0.01) 
    {
        isPlanar = false;
        break;
    }
}

if(isPlanar)
{
    for(auto & obj : _raw_data)
    {
        double len = sqrt(pow(obj.X(),2) + pow(obj.Y(),2));
        obj.data[0] = len * cos(angle);
    }

}


}









int width = d_ptr->DrawingPanel->GetSize().GetX();
int height = d_ptr->DrawingPanel->GetSize().GetY();

std::vector<wxPoint> update;

for(auto & obj : _raw_data)
{
    update.push_back(wxPoint( obj.X()/2. * width, obj.Z()/2. * height ));
}
d_ptr->setValues(update);
d_ptr->Refresh();


prepareData(_raw_data,pow(WxSB_Quality->GetValue(),2) + 5);
 }





}




void GUIMyFrame1::Scrolls_Updated( wxScrollEvent& event )
{
WxST_TranslationX->SetLabel(wxString::Format(wxT("%g"), (WxSB_TranslationX->GetValue() - 100) / 50.0));
WxST_TranslationY->SetLabel(wxString::Format(wxT("%g"), (WxSB_TranslationY->GetValue() - 100) / 50.0));
WxST_TranslationZ->SetLabel(wxString::Format(wxT("%g"), (WxSB_TranslationZ->GetValue() - 100) / 50.0));

WxST_RotateX->SetLabel(wxString::Format(wxT("%d"), WxSB_RotateX->GetValue()));
WxST_RotateY->SetLabel(wxString::Format(wxT("%d"), WxSB_RotateY->GetValue()));
WxST_RotateZ->SetLabel(wxString::Format(wxT("%d"), WxSB_RotateZ->GetValue()));

WxST_ScaleX->SetLabel(wxString::Format(wxT("%g"), WxSB_ScaleX->GetValue() / 100.0));
WxST_ScaleY->SetLabel(wxString::Format(wxT("%g"), WxSB_ScaleY->GetValue() / 100.0));
WxST_ScaleZ->SetLabel(wxString::Format(wxT("%g"), WxSB_ScaleZ->GetValue() / 100.0));
WxST_Quality->SetLabel(wxString::Format(wxT("%d"), static_cast<int>(pow(WxSB_Quality->GetValue(),2)) + 5));

if(event.GetEventObject() == WxSB_Quality)
{
    prepareData(_raw_data,pow(WxSB_Quality->GetValue(),2) + 5);
}

Repaint();
}




void GUIMyFrame1::selectRender( wxCommandEvent& event ){
    simplified = event.GetInt();
    Repaint();
}

void GUIMyFrame1::selectPersp( wxCommandEvent& event )
{
    perspective = event.GetInt();
    Repaint();
}




Matrix4 matrix_perspv1(double w, double h, double n, double f)
{
    Matrix4 matr;
    matr(0,0) = 2*n/w;
    matr(1,1) = 2*n/h;
    matr(2,2) = -(f+n)/(f-n);
    matr(3,3) = 0.;

    matr(0,3) = -n / w; //
    matr(1,3) = -n / h; //

    matr(2,3) = -2.*f*n / (f - n);
    matr(3,2) = -1.;


return matr;
}


Matrix4 matrix_perspangular(double w, double h, double n, double f)
{


double aspectRatio = w/h;


    Matrix4 matr;
    matr(0,0) = 1 / (tan(PI/4.)*aspectRatio);
    matr(1,1) = 1 / tan(PI/4.);
    //matr(2,2) = (n+f)/(n-f);
    matr(2,2) = -f / (f-n);
    matr(3,3) = 0.;

    //matr(2,3) = 2 * n* f / (n - f);
    matr(2,3) = -f*n / (f-n);
    matr(3,2) = -1.;


return matr;
}


//naklada transformacje: rotacji, skali, translacji na trojkat
bool GUIMyFrame1::transformator(Triangle & triangle, int w, int h)
{
    bool flag = true;
    for(auto & point : triangle._vert)
    {
    point = matrix_scale(
        Vector4(
            WxSB_ScaleX->GetValue() /100.,
            WxSB_ScaleY->GetValue() /100.,
            WxSB_ScaleZ->GetValue() /100.)
        ) * point; //scale*/
        
    point = 
    matrix_Zrotate(WxSB_RotateZ->GetValue()*PI/180.) * //rotate
    matrix_Yrotate(WxSB_RotateY->GetValue()*PI/180.) *
    matrix_Xrotate(WxSB_RotateX->GetValue()*PI/180. + PI/2.) * 
    point;



    //translate
    point = matrix_translate(
        Vector4(
            -(WxSB_TranslationX->GetValue() - 100.) /100.,
            (WxSB_TranslationY->GetValue() - 100.) /100.,
            1.1 + (WxSB_TranslationZ->GetValue() - 100.) /100.
            //0.2
            )) * point;



    if(point.Z() < 0.1) flag = false;

    }



return flag;
}




std::vector<Vector4> bresenham(Vector4 v0, Vector4 v1)
{

struct
{
    std::vector<Vector4> operator()(const Vector4 & begin, const Vector4 & end)
    {
        double a_z = (end.Z() - begin.Z()) / (end.X() - begin.X());

//prep
        int dx = end.X() - begin.X();
        int dy = end.Y() - begin.Y();
        int yi = 1;
        if(dy < 0)
        {
            yi = -1;
            dy = -dy;
        }
        int D = 2*dy - dx;
        int y = begin.Y();
        std::vector<Vector4> line;
//core
        for(int x = begin.X(); x <  end.X(); ++x)
        {
            line.push_back(Vector4(x,y,a_z*(static_cast<double>(x) - begin.X()) + begin.Z()));
            if(D > 0)
            {
                y+=yi;
                D+=2*(dy - dx);                
            }
            else
            D+=2*dy;
        }


        return line;
    };
} lineLow;

struct
{
    std::vector<Vector4> operator()(const Vector4 & begin, const Vector4 & end)
    {
        double a_z = (end.Z() - begin.Z()) / (end.X() - begin.X());

//prep
        int dx = end.X() - begin.X();
        int dy = end.Y() - begin.Y();
        int xi = 1;
        if(dx < 0)
        {
            xi = -1;
            dx = -dx;
        }
        int D = 2*dx - dy;
        int x = begin.X();
        std::vector<Vector4> line;
//core
        for(int y = begin.Y(); y < end.Y(); ++y)
        {
            line.push_back(Vector4(x,y,a_z*(static_cast<double>(x) - begin.X()) + begin.Z()));
            if(D > 0)
            {
                x+=xi;
                D+=2*(dx - dy);                
            }
            else
            D+=2*dx;
        }


        return line;
    };
} lineHigh;






if(fabs(v1.Y() - v0.Y()) < fabs(v1.X() - v0.X()))
{
    if(v0.X() > v1.X())
    {
        return lineLow(v1,v0);
    }
    else
    {
        return lineLow(v0,v1);
    }

}
else
{
    if(v0.Y() > v1.Y())
    {
        return lineHigh(v1,v0);
    }
    else
    {
        return lineHigh(v0,v1);
    }

}

    
}




class ImgDataHandler{
    public:
ImgDataHandler(wxImage & image) : _data{static_cast<unsigned char*>(image.GetData())}, _width{static_cast<size_t>(image.GetWidth())}, _height{static_cast<size_t>(image.GetHeight())} {};
ImgDataHandler(ImgDataHandler & cp) : _width(cp._width), _height{cp._height}
{
_data = static_cast<unsigned char *>(malloc(_height*_width*3));
std::copy(cp._data,cp._data+_height*_width*3,_data);
};




unsigned char & operator()(size_t row, size_t col, unsigned char color = 0)
{
    return _data[row*_width*3 + col*3 + color];
};

unsigned char * _data;

size_t _width;
size_t _height;
};






                                    //kopia trojkata!
void drawTriangle( wxImage & dc, Triangle tr, std::vector<std::vector<double>> & z_buff)
{
struct
{
    bool operator()(const Vector4 & obj1, const Vector4 & obj2){return obj1.Y() < obj2.Y();};
} TrianglePolicy;
std::stable_sort(tr._vert,tr._vert+3,TrianglePolicy); //sortuje tak, aby miec y0 <= y1 <= y2

struct
{
    //wytyczenie granicy dla kazdego y
    std::vector<Vector4> operator()(Vector4 begin, Vector4 end)
    {
        double a = (end.X() - begin.X()) / (end.Y() - begin.Y());
        double a_z = (end.Z() - begin.Z()) / (end.Y() - begin.Y());
        
        long y0 = begin.Y();
        std::vector<Vector4> result;
        //optymistycznie zakladam ze powierzchnia nie ma grubosci
        //krotkowzroczne, bo rowniez aplikuje sie do linii :(
        //ostatecznie, do wytyczenia linii wykorzystuje bresenham()
        if(a != a || a_z != a_z) return result; 
        else
        {
        //if(a == INFINITY) a = 0.;
        //if( a_z == INFINITY) a_z = 0.;
        result.resize((static_cast<long>(end.Y()) - y0 + 1));

        #pragma omp parallel for
        for(long pos = y0; pos <= static_cast<long>(end.Y()); ++pos)
        {
            result[pos - y0] = Vector4(
                a * (static_cast<double>(pos) - y0) + begin.X(),
                pos,
                a_z * (static_cast<double>(pos) - y0) + begin.Z()) ;
        }
        return result;
        }
    }

} marginY;







std::vector<Vector4> x012 = marginY(tr._vert[0], tr._vert[1]);
if(x012.size()) x012.pop_back();
std::vector<Vector4> x12 = marginY(tr._vert[1], tr._vert[2]);
std::vector<Vector4> x02 = marginY(tr._vert[0], tr._vert[2]);
x012.insert(x012.end(),x12.begin(),x12.end());

std::vector<Vector4> & left = x012;
std::vector<Vector4> & right = x02;

if(x02.size() && x012.size())
{
if(x02[x02.size()/2].X() < x012[x012.size()/2].X())
{
    std::swap(left,right);
}





ImgDataHandler handle(dc);

//inner color
//doing all the heavy lifting

//dc.SetPen(wxPen(*wxRED,2));
//dc.SetPen(*wxRED_PEN);
#pragma omp parallel for
for(size_t i = 0; i < std::min(x012.size(),x02.size()); ++i) //iteracja po pietrach
{
    double a_z = (right[i].Z() - left[i].Z()) / (right[i].X() - left[i].X()); //interpolacja
    size_t y = right[i].Y();



    for(size_t x = left[i].X(); x <= right[i].X();++x) //wlasciwa iteracja po x; kolorujemy rzad na danej wysokosci y
    {
        if(0<= y && y < z_buff.size() && 0 <= x && x < z_buff[y].size())
        {
            double z_height = a_z*(static_cast<double>(x) - left[i].X()) + left[i].Z();

            #pragma omp critical (render)
            if(z_height < z_buff[y][x] && z_height > 0.1)
            {
                //std::cout << x << ", " << y << "\n";
                z_buff[y][x] = z_height;
                //dc.DrawPoint(x,y);
                handle(y,x,0) = 200;
                handle(y,x,1) = 0;
                handle(y,x,2) = 0;
            }
            //else
            //{
                //std::cout << z_height << " " <<z_buff[y][x] << "\n";
            //}
        }

    }
//std::cout << "\n";

}

//outline
//dc.SetPen(*wxBLACK_PEN);
#pragma omp parallel for
for(short l = 0; l < 3; ++l)
{

std::vector<Vector4> line = bresenham(tr._vert[l], tr._vert[ (l+1)%3 ]);

for(auto & obj : line)
{
    size_t x = (obj.X()), y = (obj.Y());
    if(0<= y && y < z_buff.size() && 0 <= x && x < z_buff[y].size() )//fabs(obj.Z() - z_buff[y][x]) < 0.01
    {
        #pragma omp critical (render)
        if(obj.Z() > 0.1 && obj.Z() <= z_buff[y][x] + 0.001)
        {
        z_buff[y][x] = obj.Z();
        //dc.DrawPoint(x,y);
        handle(y,x,0) = 0;
        handle(y,x,1) = 0;
        handle(y,x,2) = 0;
        }
    }
}


}



}

//dc.SetPen(*wxBLACK_PEN);
}


void GUIMyFrame1::Repaint()
{
wxClientDC dc1(WxPanel);
draw_target = wxImage(WxPanel->GetSize()); //do tego rysujemy sposobem domowym


wxBitmap render_target(draw_target); //do tego rysujemy dc.draw
 wxBufferedDC dc(&dc1,render_target);
 int w, h;
 WxPanel->GetSize(&w, &h);

dc.SetBackground(wxBrush(wxColor(255, 255, 255)));
dc.Clear();

std::vector<Triangle> data;


std::vector<std::vector<double>> z_buffer(h);

for(size_t i = 0; i < h; ++i)
{
    std::vector<double> row(w);
    for(size_t j = 0; j < w; ++j)
    {
        row[j] = 9999.9999;
    }
    z_buffer[i] = row;
    //z_buffer.push_back(row);
}


//preprocessing

for(std::vector<Triangle>::iterator obj = _data.begin(); obj < _data.end(); ++obj)
{

    Triangle copy = *obj;

    //s1 sprawdza czy jest przed kamera
    bool s1 = transformator(copy,w,h);

    //warunek s1 aplikuje sie tylko przy uproszczonym renderze
    //warunek wektor normalny aplikuje sie tylko wtedy, gdy bryla nie jest wklesla —> USUNIĘTE
    if((simplified & !s1) continue;
    else
    {
        data.push_back(copy);
    }
}

//painter's algorithm

if(simplified && isConvex) //gdy rozszerzony renderer z buforem Z to wtedy sortowanie jest zbedne
{
struct
{
    bool operator()(const Triangle & obj1, const Triangle & obj2){return obj1.minZ() > obj2.minZ();};
} TrianglePolicy;
std::stable_sort(data.begin(),data.end(),TrianglePolicy);
}

dc.SetBrush(*wxRED_BRUSH);

//drawing

for(std::vector<Triangle>::iterator obj = data.begin(); obj < data.end(); ++obj)
{
    for(auto & point : obj->_vert)
        {
            //clipping!
            if(perspective) point = matrix_perspgl(0.15, h *0.15 / w, -0.1 , -100.) * point;
            //point = matrix_perspangular(w,h,0.1,100.) * point;
            if(point.data[3]) for(int i = 0 ;i < 4; ++i) point.data[i] /= point.data[3];
            point = matrix_viewport(w,h) * point;
            
        }



    

//wlasciwe rysowanie trojkata
    if(!simplified || !isConvex)
    {
    drawTriangle(draw_target,*obj,z_buffer);
    }
    else
    {
        wxPoint arr[3] = {
        wxPoint(obj->_vert[0].X(),obj->_vert[0].Y()),
        wxPoint(obj->_vert[1].X(),obj->_vert[1].Y()),
        wxPoint(obj->_vert[2].X(),obj->_vert[2].Y())};
        dc.DrawPolygon(3,arr);
    }


}
if(!simplified || !isConvex) dc.DrawBitmap(wxBitmap(draw_target),wxPoint(0,0));
else draw_target = render_target.ConvertToImage();


// tu rysowac
}
