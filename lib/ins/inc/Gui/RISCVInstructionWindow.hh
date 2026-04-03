#ifndef RISCV_INSTRUCTION_WINDOW_HH
#define RISCV_INSTRUCTION_WINDOW_HH

#include <gtkmm.h>
#include <string>
#include <giomm.h>  // Gio::Application
#include <glibmm.h> // Glib::RefPtr
#include "Core/Instruction.hh"
#include "Gui/InstFormatUI.hh"

class RISCVInstructionWindow: public Gtk::Window {
public:
    Gtk::Box *uiContainer_ {};
    Gtk::Box *pEntryRow_ {};
    Gtk::Entry *InsEntry_ {};
    Gtk::Button *InsButtonParse_ {};
    Gtk::TextView *InsTextView_ {};
    Gtk::Button *pSettingsBtn_ {};
    Gtk::Popover *pSettingsPopover_ {};
    Gtk::Switch *pAbiSwitch_ {};
    Gtk::MenuButton *pIsaMenuBtn_ {};
    Instruction *pInst_ {};
    InstFormatUI *rTypeUI_ {};
    InstFormatUI *iTypeUI_ {};
    InstFormatUI *jTypeUI_ {};
    InstFormatUI *uTypeUI_ {};
    InstFormatUI *sTypeUI_ {};

    bool hasSetABI_              = false;
    int selectedIsaIndex_       = 0;  // 0=AUTO, 1=RV32I, 2=RV64I, 3=RV128I

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
    void setupSettingsPopover();
    void refreshAssemblyForAbiChange();
};

#endif // RISCV_INSTRUCTION_WINDOW_HH