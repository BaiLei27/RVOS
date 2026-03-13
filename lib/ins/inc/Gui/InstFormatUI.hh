#ifndef INSTFORMATUI_HH
#define INSTFORMATUI_HH
#include <gtkmm.h>
#include <glibmm.h>
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
    Gtk::Label *pHexLabel_ = nullptr;
    InstCommST::BinaryFieldWidgetMap_u BinaryFieldWidgets_;
    InstCommST::AsmMnemonicWidgetMap_u AsmFieldWidgets_;

    sigc::signal<void(const std::string &)> signal_put_to_output;

    explicit InstFormatUI(const InstTypeRelationEntity &format);
    void UpdateDisplay(Instruction &inst);

    std::string getAssemblyContent() const;
    std::string getBinaryContent() const;
    std::string getHexContent() const;

protected:
    void setupFieldControllers();
    void setupAssemblyDisplay();
    void setupBinaryDisplay();
    void setupHexDisplay();
    void updateAssemblyDisplay(Instruction &inst);
    void updateBinaryDisplay(Instruction &inst);
    void updateHexDisplay(Instruction &inst);

    std::vector<Gtk::Label *> BinaryLabelsV_;
    std::unordered_map<std::string, Gtk::Box *> FieldBoxes_;
    std::unordered_map<std::string, Gtk::Label *> FieldValueLabels_;

private:
    void updateRTypeDisplay(Instruction &inst);
    void updateITypeDisplay(Instruction &inst);
};

InstTypeRelationEntity createRTypeFormat();
InstTypeRelationEntity createITypeFormat();

#endif // INSTFORMATUI_HH_