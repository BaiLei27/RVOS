

#ifndef BINARYFIELDWIDGET_HH
#define BINARYFIELDWIDGET_HH
#include <iostream>
#include <unordered_map>
#include <gtkmm.h>

#include "Gui/InstCommST.hh"
#include "Gui/AsmMnemonicWidget.hh"
class AsmMnemonicWidget;

class BinaryFieldWidget {
public:
    using BinaryFieldWidgetMap= std::unordered_map<std::string, BinaryFieldWidget *>;
    using AsmMnemonicWidgetMap= std::unordered_map<std::string, AsmMnemonicWidget *>;
    BinaryFieldWidget(std::vector<Gtk::Label *> &binaryLabelsV_, const S_InstBinaryField_t &field);
    ~BinaryFieldWidget()= default;

public:
    Gtk::Box *MBox_= nullptr;
    std::vector<Gtk::Label *> ControlLabels_;
    inline static int g_index                                  = 0;
    Glib::RefPtr<Gtk::EventControllerMotion> PMotionController_= nullptr;

public:
    void highlight();
    void unhighlight();
    void highlightInMouse();
    void unhighlightInMouse();
    void setupController(const std::string &name, S_InstTypeRelationEntity_t &instFmt, BinaryFieldWidgetMap &binaryFieldWidgets_, AsmMnemonicWidgetMap &asmFieldWidgets_);
    void updateControlLables(uint32_t instructionValue);
};

#endif