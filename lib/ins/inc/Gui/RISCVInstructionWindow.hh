#pragma once
#include <gtkmm.h>
#include <string>
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
    InstFormatUI *iTypeUI_      = nullptr;
    InstFormatUI *sTypeUI_      = nullptr;
    InstFormatUI *bTypeUI_      = nullptr;
    InstFormatUI *uTypeUI_      = nullptr;
    InstFormatUI *jTypeUI_      = nullptr;

public:
    RISCVInstructionWindow();
    ~RISCVInstructionWindow()= default;

private:
    void initInstFormatUI();
    void onInsButtonParseClicked();
    void showInsResult(Instruction &inst);
    void updateDisplay(InstFormatUI &instUi, Instruction &inst);
    void showError(const std::string &message);
    void loadCssFromFile(Gtk::Window &window);
    void hideAllTypeUI();
};
