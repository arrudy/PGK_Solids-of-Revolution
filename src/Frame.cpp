#include "Frame.h"
#include "GUIMyFrame1.h"
#include "vecmat.h"

void MyFrame::close( wxCloseEvent& event ) {m_ptr->Destroy(); Destroy();};

MyFrame::~MyFrame() {
	delete m_staticText1;
	delete m_choice2;
	delete m_staticText2;
	delete m_choice1;
	delete DrawingPanel;
	delete Reset;
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( MyFrame::close ) );
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
	Render = new wxButton(this, ID_RENDERBUTTON, wxT("Render"), wxDefaultPosition, wxDefaultSize, 0);
	MenuSize->Add(Render, 0, wxALIGN_CENTER | wxALL, 5);

	m_staticText1 = new wxStaticText(this, wxID_ANY, wxT("Liczba punkt\u00F3w"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1->Wrap(-1);
	MenuSize->Add(m_staticText1, 0, wxALIGN_CENTER | wxALL, 5);

	wxString m_choice2Choices[] = { wxT("2"), wxT("3"), wxT("4"), wxT("5"), wxT("6"), wxT("7"), wxT("8"), wxT("9"),wxT("10") };
	int m_choice2NChoices = sizeof(m_choice2Choices) / sizeof(wxString);
	m_choice2 = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice2NChoices, m_choice2Choices, 0);
	m_choice2->SetSelection(8);
	MenuSize->Add(m_choice2, 0, wxALIGN_CENTER | wxALL, 0);

	m_staticText2 = new wxStaticText(this, wxID_ANY, wxT("Metoda dodawania punkt\u00F3w"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText2->Wrap(-1);
	MenuSize->Add(m_staticText2, 0, wxALIGN_CENTER | wxALL, 5);

	wxString m_choice1Choices[] = { wxT("Rysuj"), wxT("Wpisz") };
	int m_choice1NChoices = sizeof(m_choice1Choices) / sizeof(wxString);
	m_choice1 = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice1NChoices, m_choice1Choices, 0);
	m_choice1->SetSelection(0);
	MenuSize->Add(m_choice1, 0, wxALIGN_CENTER | wxALL, 5);


	//tutaj stworzenie ilo�ci miejsc na wpisywanie punktow mia�o by� dynamicznioe dlatego wektor ale ostatecznie zosta�o statyczne 10 punktow osobo wartosc X osobno Y
	int n = m_choice2->GetSelection() + 2;
	for (auto i = 0; i < 2 * n; ++i) {
		if (!(i % 2)) {
			v.push_back(new wxTextCtrl(this, wxID_ANY, std::string("Punkt x nr " + std::to_string(i / 2 + 1)), wxDefaultPosition, wxDefaultSize, 0));
		}
		else {
			v.push_back(new wxTextCtrl(this, wxID_ANY, std::string("Punkt y nr " + std::to_string((i - 1) / 2 + 1)), wxDefaultPosition, wxDefaultSize, 0));
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
	Bind(wxEVT_BUTTON, &MyFrame::render, this, ID_RENDERBUTTON);
	DrawingPanel->Bind(wxEVT_LEFT_DOWN, &MyFrame::drawPoints, this);

	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( MyFrame::close ) );
}

void MyFrame::drawPoints(wxMouseEvent& a) {
	if (values.size() < m_choice2->GetSelection() + 2) {
		wxPoint help(a.GetPosition());
		values.push_back(wxPoint(help.x - DrawingPanel->GetSize().x / 2, help.y - DrawingPanel->GetSize().y / 2));
		/*wxString message = wxString::Format("Mouse detected x=%d y =%d", help.x - DrawingPanel->GetSize().x / 2, help.y - DrawingPanel->GetSize().y / 2);
		wxLogStatus(message);*/
	}
	else{ //edycja istniajacych punktow
		wxPoint mousePos(a.GetPosition() - wxPoint(DrawingPanel->GetSize().x/2,DrawingPanel->GetSize().y/2));
		if(!editPoint)
		{
		for(auto & obj : values)
		{
			if(fabs(mousePos.x - obj.x) < 3 && fabs(mousePos.y - obj.y) < 3)
			{
				editPoint = &obj;
				break;
			}
		}

		}
		else
		{
			*editPoint = mousePos;
			editPoint = nullptr;
		}

	}
}
void MyFrame::reset(wxCommandEvent& WXUNUSED(a)) {
	values.clear();
	for (auto i = 0; i < v.size(); i += 2) {
		v[i]->SetValue(std::string("Punkt x nr " + std::to_string(i / 2 + 1)));
		v[i + 1]->SetValue(std::string("Punkt y nr " + std::to_string(i / 2 + 1)));
	}
	editPoint = nullptr;
}

void MyFrame::render(wxCommandEvent& WXUNUSED(a)) {

	std::vector<Vector4> data;
	double ref_width = DrawingPanel->GetSize().GetX() / 2.;
	double ref_height = DrawingPanel->GetSize().GetY() / 2.;

	for (auto& obj : values)
	{
		data.push_back(Vector4(static_cast<double>(obj.x) / ref_width, 0, static_cast<double>(obj.y) / ref_height));
	}
	m_ptr->Show(true);
	m_ptr->setData(data);
}




void MyFrame::newChoice(wxCommandEvent& WXUNUSED(a)) {
	values.clear();
	for (auto i = 0; i < v.size(); i += 2) {
		v[i]->SetValue(std::string("Punkt x nr " + std::to_string(i / 2 + 1)));
		v[i + 1]->SetValue(std::string("Punkt y nr " + std::to_string(i / 2 + 1)));
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

	Drawing->DrawLine(0, -DrawingPanel->GetSize().y / 2+5, 0, DrawingPanel->GetSize().y / 2-5);
	Drawing->DrawLine(0, -DrawingPanel->GetSize().y / 2 + 5, 5, -DrawingPanel->GetSize().y / 2 + 10);
	Drawing->DrawLine(0, -DrawingPanel->GetSize().y / 2 + 5, -5, -DrawingPanel->GetSize().y / 2 + 10);
	Drawing->DrawLine(-DrawingPanel->GetSize().x / 2 +5, 0, DrawingPanel->GetSize().x / 2-5, 0);
	Drawing->DrawLine(DrawingPanel->GetSize().x / 2 -5, 0, DrawingPanel->GetSize().x / 2 -10 ,-5);
	Drawing->DrawLine(DrawingPanel->GetSize().x / 2 -5, 0, DrawingPanel->GetSize().x / 2 -10, 5);
	if ((m_choice1->GetSelection())) {
		values.clear();
		values.resize(m_choice2->GetSelection() + 2);


		int it = 0;
		for (auto i = 0; i < (2 * m_choice2->GetSelection() + 4); i += 2) {
			int x = wxAtoi(v[i]->GetValue());
			int y = wxAtoi(v[i + 1]->GetValue());
			Drawing->DrawCircle(wxPoint(x, y), 2);
			values[it] = wxPoint(x, y);
			if (i > 1) {
				Drawing->DrawLine(wxPoint(wxAtoi(v[i - 2]->GetValue()), wxAtoi(v[i - 1]->GetValue())), wxPoint(x, y));
			}
			++it;
		}
		wxString message = wxString::Format("Mouse detected x=%d y =%d", values[0].x, values[0].y);
		wxLogStatus(message);
		Drawing->DrawLine(wxPoint(wxAtoi(v[0]->GetValue()), wxAtoi(v[1]->GetValue())), wxPoint(wxAtoi(v[(2 * m_choice2->GetSelection() + 4) - 2]->GetValue()), wxAtoi(v[(2 * m_choice2->GetSelection() + 4) - 1]->GetValue())));
	}
	else {

		for (auto i = 0; i < values.size(); ++i) {

			Drawing->DrawCircle(values[i], 2);
			v[2 * i]->SetValue(wxString(std::to_string(values[i].x)));
			v[2 * i + 1]->SetValue(wxString(std::to_string(values[i].y)));
			if (i > 0) {
				Drawing->DrawLine(values[i], values[i - 1]);

			}

		}
		if (values.size() > 2) {
			Drawing->DrawLine(values[values.size() - 1], values[0]);
		}
		if(editPoint)
		{
			Drawing->SetPen(*wxRED_PEN);
			Drawing->DrawCircle(*editPoint, 4);
			Drawing->SetPen(*wxBLACK_PEN);
		}
		

	}

}