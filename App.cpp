#include "App.h"
#include "MainFrame.h"

wxIMPLEMENT_APP(App);

bool App::OnInit() {
    if (!wxApp::OnInit()) return false;
    auto* f = new MainFrame();
    f->Show(true);
    return true;
}

