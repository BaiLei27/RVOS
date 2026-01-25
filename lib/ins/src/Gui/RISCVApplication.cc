#include <gtkmm.h>
#include "Gui/RISCVInstructionWindow.hh"
#include "Gui/RISCVApplication.hh"

Glib::RefPtr<RISCVApplication> RISCVApplication::Create()
{
    return Glib::RefPtr<RISCVApplication>(new RISCVApplication());
}

void RISCVApplication::on_activate()
{
    auto *pWindow = new RISCVInstructionWindow();
    add_window(*pWindow);

    pWindow->signal_close_request().connect([pWindow]() -> bool {
        delete pWindow;
        return true;
    },
                                            false);

    pWindow->show();
}