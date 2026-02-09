#ifndef ASMMNEMONICWIDGET_HH
#define ASMMNEMONICWIDGET_HH
#include <iostream>
#include <unordered_map>
#include <gtkmm.h>
#include <string>   // std::string
#include <gtkmm.h>  // Gtk::Box, Gtk::Label, Gtk::EventControllerMotion
#include <glibmm.h> // Glib::RefPtr
#include "Gui/InstCommST.hh"
#include "Gui/BinaryFieldWidget.hh"

class AsmMnemonicWidget {
public:
    AsmMnemonicWidget(const std::string &mnemonicName, Gtk::Box *pParentAsmBox);
    ~AsmMnemonicWidget()= default;

public:
    Gtk::Box *mBox_                                            = nullptr;
    Gtk::Label *mLabel_                                        = nullptr;
    Glib::RefPtr<Gtk::EventControllerMotion> pMotionController_= nullptr;

public:
    void Highlight();
    void Unhighlight();
    void HighlightInMouse();
    void UnhighlightInMouse();

    void SetupController(const std::string &name, InstTypeRelationEntity &instFmt, InstCommST::BinaryFieldWidgetMap_u &binaryFieldWidgets);
};

#endif
