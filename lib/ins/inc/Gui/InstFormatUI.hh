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

class InstFormatUI: public Gtk::Box {
public:
    S_InstTypeRelationEntity_t format_;
    explicit InstFormatUI(const S_InstTypeRelationEntity_t &format);
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

S_InstTypeRelationEntity_t createRTypeFormat();

#endif // INSTFORMATUI_HH_