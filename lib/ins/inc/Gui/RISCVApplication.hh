#ifndef RISCV_APPLICATION_HH
#define RISCV_APPLICATION_HH
#include <gtkmm.h>
#include "Gui/RISCVInstructionWindow.hh"

class RISCVApplication: public Gtk::Application {
public:
    RISCVApplication(): Gtk::Application("org.gtkmm.riscv.visualization", Gio::Application::Flags::NONE) { }

    static Glib::RefPtr<RISCVApplication> Create();

protected:
    void on_activate() override;
};

#endif // RISCV_APPLICATION_HH