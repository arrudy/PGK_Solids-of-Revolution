#include <wx/wx.h>
#include "GUIMyFrame1.h"
#include "Frame.h"

class MyApp : public wxApp {

public:

 virtual bool OnInit();
 virtual int OnExit() { return 0; }

};

IMPLEMENT_APP(MyApp);

bool MyApp::OnInit() 
{
 SetProcessDPIAware();
 wxFrame *mainFrame = new GUIMyFrame1(NULL);
 wxFrame *drawFrame = new MyFrame(NULL);

reinterpret_cast<GUIMyFrame1*>(mainFrame)->set_d_ptr(reinterpret_cast<MyFrame*>(drawFrame));

reinterpret_cast<MyFrame*>(drawFrame)->set_m_ptr(reinterpret_cast<GUIMyFrame1*>(mainFrame));

 drawFrame->Show(true);
 mainFrame->Show(true);
 SetTopWindow(mainFrame);


 return true;
}