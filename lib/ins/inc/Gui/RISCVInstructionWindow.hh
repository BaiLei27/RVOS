#ifndef RISCV_INSTRUCTION_WINDOW_HH
#define RISCV_INSTRUCTION_WINDOW_HH

#include <gtkmm.h>
#include <string>
#include <giomm.h>  // Gio::Application
#include <glibmm.h> // Glib::RefPtr
#include "Core/Instruction.hh"
#include "Gui/InstFormatUI.hh"

class RISCVInstructionWindow : public Gtk::Window {
public:
    Gtk::Box *uiContainer_      = nullptr;
    Gtk::Entry *InsEntry_       = nullptr;
    Gtk::Button *InsButtonParse_= nullptr;
    Gtk::TextView *InsTextView_ = nullptr;
    Instruction *pInst_         = nullptr;
    InstFormatUI *rTypeUI_      = nullptr;

public:
    RISCVInstructionWindow();
    ~RISCVInstructionWindow()= default;

private:
    void onInsButtonParseClicked();
    void showInsResult(Instruction &inst);
    void showError(const std::string &message);
    void loadCssFromFile(Gtk::Window &window);

    void initInstFormatUI();
    void hideAllTypeUI();
    void UpdateDisplay(InstFormatUI &instUi, Instruction &inst);
};

#endif // RISCV_INSTRUCTION_WINDOW_HH