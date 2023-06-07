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
 mainFrame->Show(true);
 drawFrame->Show(true);
 SetTopWindow(mainFrame);

 return true;
}