#ifndef ASMMNEMONICWIDGET_HH
#define ASMMNEMONICWIDGET_HH
#include <iostream>
#include <unordered_map>
#include <gtkmm.h>

#include "Gui/InstCommST.hh"
#include "Gui/BinaryFieldWidget.hh"
class BinaryFieldWidget;

class AsmMnemonicWidget {
public:
    using BinaryFieldWidgetMap= std::unordered_map<std::string, BinaryFieldWidget *>;
    AsmMnemonicWidget(const std::string &mnemonicName, Gtk::Box *parentAsmBox);
    ~AsmMnemonicWidget()= default;

public:
    Gtk::Box *MBox_                                            = nullptr;
    Gtk::Label *MLabel_                                        = nullptr;
    Glib::RefPtr<Gtk::EventControllerMotion> PMotionController_= nullptr;

public:
    void highlight();
    void unhighlight();
    void highlightInMouse();
    void unhighlightInMouse();

    void setupController(const std::string &name, S_InstTypeRelationEntity_t &instFmt, BinaryFieldWidgetMap &binaryFieldWidgets_);
};

#endif