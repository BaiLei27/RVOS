#ifndef INSTFORMATUI_HH
#define INSTFORMATUI_HH
#include <gtkmm.h>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <cstdint> // uint32_t
#include "Gui/InstCommST.hh"
#include "ISA/InstFormat.hh"
#include "Core/Instruction.hh"
#include "Gui/BinaryFieldWidget.hh"
#include "Gui/AsmMnemonicWidget.hh"
#include "Gui/InstCommST.hh"

class InstFormatUI: public Gtk::Box {
public:
    InstTypeRelationEntity format_;
    InstCommST::BinaryFieldWidgetMap_u BinaryFieldWidgets_;
    InstCommST::AsmMnemonicWidgetMap_u AsmFieldWidgets_;
    explicit InstFormatUI(const InstTypeRelationEntity &format);
    void UpdateDisplay(Instruction &inst);

protected:
    void setupFieldControllers();
    void setupAssemblyDisplay();
    void setupBinaryDisplay();
    void updateAssemblyDisplay(Instruction &inst);
    void updateBinaryDisplay(Instruction &inst);

    std::vector<Gtk::Label *> BinaryLabelsV_;
    std::unordered_map<std::string, Gtk::Box *> FieldBoxes_;
    std::unordered_map<std::string, Gtk::Label *> FieldValueLabels_;

private:
    void updateRTypeDisplay(Instruction &inst);
};

InstTypeRelationEntity createRTypeFormat();

#endif // INSTFORMATUI_HH_