

#ifndef BINARYFIELDWIDGET_HH
#define BINARYFIELDWIDGET_HH
#include <iostream>
#include <unordered_map>
#include <gtkmm.h>
#include <vector>
#include <gtkmm.h>
#include <glibmm.h>
#include <cstdint>
#include "Gui/InstCommST.hh"
#include "Gui/AsmMnemonicWidget.hh"

class BinaryFieldWidget {
public:
    BinaryFieldWidget(std::vector<Gtk::Label *> &binaryLabelsV, const InstBinaryField &field);
    ~BinaryFieldWidget()= default;

public:
    Gtk::Box *mBox_= nullptr;
    std::vector<Gtk::Label *> controlLabels_;
    inline static int g_index                                  = 0;
    Glib::RefPtr<Gtk::EventControllerMotion> pMotionController_= nullptr;

public:
    void Highlight();
    void Unhighlight();
    void HighlightInMouse();
    void UnhighlightInMouse();
    void SetupController(const std::string &name,
                         InstTypeRelationEntity &instFmt,
                         InstCommST::BinaryFieldWidgetMap_u &binaryFieldWidgets,
                         InstCommST::AsmMnemonicWidgetMap_u &AsmFieldWidgets_);

    void UpdateControlLables(uint32_t instructionValue);
};

#endif
