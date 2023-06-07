#include "Frame.h"

MyFrame::~MyFrame() {
	delete m_staticText1;
	delete m_choice2;
	delete m_staticText2;
	delete m_choice1;
	delete DrawingPanel;
	delete Reset;
}

MyFrame::MyFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style) {
	//this->SetSizeHints(wxDefaultSize, wxDefaultSize);
this->SetClientSize(this->FromDIP(wxSize(600, 800)));
	
	wxBoxSizer* WindowSize;
	WindowSize = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* DrawingPanelSize;
	DrawingPanelSize = new wxBoxSizer(wxVERTICAL);

	DrawingPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	DrawingPanel->SetBackgroundColour(wxColour(255, 255, 255));

	DrawingPanelSize->Add(DrawingPanel, 1, wxEXPAND | wxALL, 5);


	WindowSize->Add(DrawingPanelSize, 1, wxEXPAND, 5);

	wxBoxSizer* MenuSize;
	MenuSize = new wxBoxSizer(wxVERTICAL);
	
	Reset = new wxButton(this, ID_RESETBUTTON, wxT("Reset"), wxDefaultPosition, wxDefaultSize, 0);
	MenuSize->Add(Reset, 0, wxALIGN_CENTER | wxALL, 5);

	m_staticText1 = new wxStaticText(this, wxID_ANY, wxT("Number of points"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1->Wrap(-1);
	MenuSize->Add(m_staticText1, 0, wxALIGN_CENTER | wxALL, 5);

	wxString m_choice2Choices[] = { wxT("1"), wxT("2"), wxT("3"), wxT("4"), wxT("5"), wxT("6"), wxT("7"), wxT("8"), wxT("9"),wxT("10") };
	int m_choice2NChoices = sizeof(m_choice2Choices) / sizeof(wxString);
	m_choice2 = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice2NChoices, m_choice2Choices, 0);
	m_choice2->SetSelection(9);
	MenuSize->Add(m_choice2, 0, wxALIGN_CENTER | wxALL, 0);

	m_staticText2 = new wxStaticText(this, wxID_ANY, wxT("Method of adding points"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText2->Wrap(-1);
	MenuSize->Add(m_staticText2, 0, wxALIGN_CENTER | wxALL, 5);

	wxString m_choice1Choices[] = { wxT("Draw"), wxT("Write") };
	int m_choice1NChoices = sizeof(m_choice1Choices) / sizeof(wxString);
	m_choice1 = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice1NChoices, m_choice1Choices, 0);
	m_choice1->SetSelection(0);
	MenuSize->Add(m_choice1, 0, wxALIGN_CENTER | wxALL, 5);


	//tutaj stworzenie ilo�ci miejsc na wpisywanie punktow mia�o by� dynamicznioe dlatego wektor ale ostatecznie zosta�o statyczne 10 punktow osobo wartosc X osobno Y
	int n = m_choice2->GetSelection()+1;
	for (auto i = 0; i < 2*n; ++i) {
		if (!(i % 2)) {
			v.push_back(new wxTextCtrl(this, wxID_ANY, std::string("point x number " + std::to_string(i/2+1)), wxDefaultPosition, wxDefaultSize, 0));
		}
		else {
			v.push_back(new wxTextCtrl(this, wxID_ANY, std::string("point y number " + std::to_string((i-1)/2+1)), wxDefaultPosition, wxDefaultSize, 0));
		}
		MenuSize->Add(v[i], 0, wxALIGN_CENTER | wxALL, 5);
	}

	WindowSize->Add(MenuSize, 0, wxEXPAND, 5);
	
	

	this->SetSizer(WindowSize);
	this->Layout();

	this->Centre(wxBOTH);
	CreateStatusBar();
	Bind(wxEVT_PAINT, &MyFrame::Paint, this);
	Bind(wxEVT_UPDATE_UI, &MyFrame::UpdateForm, this);
	Bind(wxEVT_BUTTON, &MyFrame::reset, this, ID_RESETBUTTON);
	Bind(wxEVT_CHOICE, &MyFrame::newChoice, this);
	DrawingPanel->Bind(wxEVT_LEFT_DOWN, &MyFrame::drawPoints, this);
	
}
void MyFrame::drawPoints(wxMouseEvent& a) {
	if (values.size()< m_choice2->GetSelection()+1) {
		wxPoint help(a.GetPosition());
		values.push_back(wxPoint(help.x - DrawingPanel->GetSize().x / 2, help.y - DrawingPanel->GetSize().y / 2));
		/*wxString message = wxString::Format("Mouse detected x=%d y =%d", help.x - DrawingPanel->GetSize().x / 2, help.y - DrawingPanel->GetSize().y / 2);
		wxLogStatus(message);*/
	}
}
void MyFrame::reset(wxCommandEvent& WXUNUSED(a)) {
	values.clear();
	for (auto i = 0; i < v.size(); i+=2) {
		v[i]->SetValue(std::string("point x number " + std::to_string(i/2 + 1)));
		v[i + 1]->SetValue(std::string("point y number " + std::to_string(i/2 + 1)));
	}
}
void MyFrame::newChoice(wxCommandEvent& WXUNUSED(a)) {
	values.clear();
	for (auto i = 0; i < v.size(); i += 2) {
		v[i]->SetValue(std::string("point x number " + std::to_string(i / 2 + 1)));
		v[i + 1]->SetValue(std::string("point y number " + std::to_string(i / 2 + 1)));
	}
}
void MyFrame::Paint(wxPaintEvent& WXUNUSED(a)) {
	Draw();
}

void MyFrame::UpdateForm(wxUpdateUIEvent& WXUNUSED(a)) {
	Draw();
}

void MyFrame::Draw() {
	std::unique_ptr<wxClientDC> clientDC(new wxClientDC(DrawingPanel));
	Buffer = wxBitmap(DrawingPanel->GetSize()); 
	std::unique_ptr<wxBufferedDC> Drawing(new wxBufferedDC(clientDC.get(), Buffer));
	
	Drawing->SetDeviceOrigin(DrawingPanel->GetSize().x / 2, DrawingPanel->GetSize().y / 2);

	Drawing->SetBackground(*wxWHITE_BRUSH);
	Drawing->Clear();

	
	if ((m_choice1->GetSelection())) {
		values.clear();

		values.reserve(m_choice2->GetSelection() + 1);
		int it = 0;
		for (auto i = 0; i < (2 * m_choice2->GetSelection()+2) ; i+=2) {
			int x = wxAtoi(v[i]->GetValue());
			int y = wxAtoi(v[i+1]->GetValue());
			Drawing->DrawCircle(wxPoint(x, y),2);
			values[it] = wxPoint(x, y);
			if (i > 1) {
				Drawing->DrawLine(wxPoint(wxAtoi(v[i - 2]->GetValue()), wxAtoi(v[i - 1]->GetValue())), wxPoint(x, y));
			}
			++it;
		}
		wxString message = wxString::Format("Mouse detected x=%d y =%d", values[0].x, values[0].y);
		wxLogStatus(message);
		Drawing->DrawLine(wxPoint(wxAtoi(v[0]->GetValue()), wxAtoi(v[1]->GetValue())), wxPoint(wxAtoi(v[(2 * m_choice2->GetSelection() + 2) - 2]->GetValue()), wxAtoi(v[(2 * m_choice2->GetSelection() + 2)-1]->GetValue())));
	}
	else {
		
		for (auto i = 0; i < values.size(); ++i) {
	
			Drawing->DrawCircle(values[i], 2);
			v[2 * i]->SetValue(wxString(std::to_string(values[i].x)));
			v[2 * i + 1]->SetValue(wxString(std::to_string(values[i].y)));
			if (i > 0) {
				Drawing->DrawLine(values[i], values[i-1]);
				
			}

		}
		if (values.size() > 2) {
			Drawing->DrawLine(values[values.size() - 1], values[0]);
		}
		
	}
	
}