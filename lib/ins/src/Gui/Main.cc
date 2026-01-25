#include <gtkmm.h>
#include "RISCVInstructionWindow.cc"

class RISCVApplication : public Gtk::Application {
public:
    RISCVApplication() : Gtk::Application("org.gtkmm.riscv.visualization", Gio::Application::Flags::NONE) {}

    static Glib::RefPtr<RISCVApplication> Create() {
        return Glib::RefPtr<RISCVApplication>(new RISCVApplication());
    }

protected:
    void on_activate() override {
        auto pWindow = new RISCVInstructionWindow();
        add_window(*pWindow);

        pWindow->signal_close_request().connect([pWindow]() -> bool {
            delete pWindow;
            return true;
        }, false);

        pWindow->show();
    }
};
int main(int argc, char* argv[]) {
    auto app = RISCVApplication::Create();
    return app->run(argc, argv);
}