// NOTLINTBEGIN
#include <gtkmm.h>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include "Gui/InstCommST.hh"
#include "ISA/InstFormat.hh"
#include "Core/Instruction.hh"
#include "Gui/BinaryFieldWidget.hh"
#include "Gui/AsmMnemonicWidget.hh"

class InstFormatUI: public Gtk::Box {
public:
    using BinaryFieldWidgetMap= std::unordered_map<std::string, BinaryFieldWidget *>;
    using AsmMnemonicWidgetMap= std::unordered_map<std::string, AsmMnemonicWidget *>;
    S_InstTypeRelationEntity_t format_;
    InstFormatUI(const S_InstTypeRelationEntity_t &format);
    void updateInstructionValue(uint32_t instructionValue);
    void updateDisplay(Instruction &inst);

protected:
    void setupFieldControllers();
    void setupAssemblyDisplay();
    void setupBinaryDisplay();
    void updateAssemblyDisplay(Instruction &inst);
    void updateBinaryDisplay(Instruction &inst);

    std::vector<Gtk::Label *> binaryLabelsV_;
    std::unordered_map<std::string, Gtk::Box *> fieldBoxes_;
    std::unordered_map<std::string, Gtk::Label *> fieldValueLabels_;
    BinaryFieldWidgetMap binaryFieldWidgets_;
    AsmMnemonicWidgetMap asmFieldWidgets_;

    uint32_t currentInstruction_;

private:
    void updateRTypeDisplay(Instruction &inst);
    void updateITypeDisplay(Instruction &inst);
    void updateJTypeDisplay(Instruction &inst);
    void updateSTypeDisplay(Instruction &inst);
    void updateBTypeDisplay(Instruction &inst);
    void updateUTypeDisplay(Instruction &inst);
};

S_InstTypeRelationEntity_t createRTypeFormat();
S_InstTypeRelationEntity_t createITypeFormat();
S_InstTypeRelationEntity_t createJTypeFormat();
S_InstTypeRelationEntity_t createSTypeFormat();
S_InstTypeRelationEntity_t createBTypeFormat();
S_InstTypeRelationEntity_t createUTypeFormat();

// NOTLINTEND