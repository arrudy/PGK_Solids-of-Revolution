#pragma once
#include <wx/wxprec.h>
#include <wx/mousestate.h>
#include <wx/colordlg.h>
#include <wx/dcbuffer.h>
#include <wx/filefn.h> 
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/panel.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/slider.h>
#include <wx/sizer.h>
#include <wx/frame.h>
#include <wx/choice.h>
#include <wx/textctrl.h>
#include <wx/dcclient.h>
#include <wx/dcbuffer.h>
#include <wx/log.h>


#include <memory>
#include <vector>
#include<string>

class GUIMyFrame1;

class MyFrame : public wxFrame
{
public:
	MyFrame(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Drawing Panel"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(1200, 1200), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
	~MyFrame();
	//metoda zwracajaca wartosci punktow
	std::vector<wxPoint>& getValues() {
		return values;
	}
	void setValues(std::vector<wxPoint> vec) {values = vec; editPoint = nullptr; m_choice2->SetSelection(vec.size()-1);};

	void set_m_ptr(GUIMyFrame1 * ptr) {m_ptr = ptr;};

		//panel do rysowania
	wxPanel* DrawingPanel;
private:
	//metoda resetuj�ca przy zmianie ilo�ci punkt�w
	void newChoice(wxCommandEvent& a);
	void render(wxCommandEvent& a);
	void Paint(wxPaintEvent& a);
	void UpdateForm(wxUpdateUIEvent& a);
	void Draw();
	//metoda do przechowania w wektorze values punkt�w z myszki
	void drawPoints(wxMouseEvent& a);
	//metoda resetuj�ca wszystkie pola
	void reset(wxCommandEvent& a);
	enum {
		ID_RESETBUTTON = 1001,
		ID_RENDERBUTTON = 1002
	};
	GUIMyFrame1 * m_ptr;

protected:
	//tekst pierwszy
	wxStaticText* m_staticText1;
	//pierwszy pasek wyboru (ilosc punktow)
	wxChoice* m_choice2;
	//tekst drugi
	wxStaticText* m_staticText2;
	//drugi pasek wyboru
	wxChoice* m_choice1;
	//przycisk do resetowania
	wxButton* Reset;
	//przycisk do renderowania
	wxButton* Render;
	//wektor przechowujacy pola tekstowe na wpisywanie punkt�w
	std::vector< wxTextCtrl*> v;
	//wektor przechowujacy wartosci puntkow
	std::vector<wxPoint> values;
	// buffer u�ywany do rysowania
	wxBitmap Buffer;
	
	wxPoint * editPoint = nullptr;
};

