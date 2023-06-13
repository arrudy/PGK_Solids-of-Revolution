#ifndef __GUIMyFrame1__
#define __GUIMyFrame1__

#include "vecmat.h"
/**
@file
Subclass of MyFrame1, which is generated by wxFormBuilder.
*/

#include "GUI.h"

//// end generated include

#include <wx/filedlg.h>
#include <wx/dcmemory.h>
#include <wx/dcclient.h>
#include <wx/dcbuffer.h>
#include <wx/colourdata.h>
#include <wx/colordlg.h>
#include "mesh.h"

class MyFrame;

/** Implementing MyFrame1 */
class GUIMyFrame1 : public MyFrame1
{
	private:
		MyFrame * d_ptr;



	protected:
		

		// Handlers for MyFrame1 events.
		void WxPanel_Repaint( wxUpdateUIEvent& event );
		void m_button_load_geometry_click( wxCommandEvent& event );
		void Scrolls_Updated( wxScrollEvent& event );
		void selectRender( wxCommandEvent& event );
		void selectPersp( wxCommandEvent& event );

	public:
		/** Constructor */
		GUIMyFrame1( wxWindow* parent );
		void close( wxCloseEvent& event );
	//// end generated class members

	void setData(std::vector<Vector4> vec) {_raw_data = vec;prepareData(vec,pow(WxSB_Quality->GetValue(),2) + 5); Refresh();};
	void set_d_ptr(MyFrame * ptr) {d_ptr = ptr;};



  void Repaint();
  bool transformator(Triangle & triangle, int w, int h);

//data musi byc uporzadkowane pod katem kolejnosci linii tworzacych obrys przekroju bryly
//quality odpowiada za to, ile lacznie punktow bedzie odpowiadac za przyblizenie okregu
	void prepareData(std::vector<Vector4> & data, size_t quality);

	wxImage draw_target;
	std::vector<Triangle> _data; //przenosi mesh
	std::vector<Vector4> _raw_data; //przenosi punkty wyznaczajace ksztalt przekroju

	bool perspective = false;
	bool simplified = true;
	bool isConvex = false;


};

#endif // __GUIMyFrame1__
