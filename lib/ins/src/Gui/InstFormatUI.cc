#include "Gui/InstFormatUI.hh"

InstFormatUI::InstFormatUI(const S_InstTypeRelationEntity_t &format)
    : Gtk::Box(Gtk::Orientation::VERTICAL, 10), format_(format)
{
    setupAssemblyDisplay();
    setupBinaryDisplay();
    setupFieldControllers();
}

void InstFormatUI::setupAssemblyDisplay()
{
    auto pAsmArea = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 0);
    auto pAsmTitle= Gtk::make_managed<Gtk::Label>("Assembly =");
    pAsmTitle->set_margin_end(8);
    pAsmArea->append(*pAsmTitle);

    append(*pAsmArea);
}

void InstFormatUI::setupBinaryDisplay()
{
    auto pBinaryArea = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 0);
    auto pBinaryTitle= Gtk::make_managed<Gtk::Label>("Binary =");
    pBinaryTitle->set_margin_end(8);
    pBinaryArea->append(*pBinaryTitle);
    pBinaryArea->set_halign(Gtk::Align::START);
    pBinaryArea->set_valign(Gtk::Align::CENTER);
    pBinaryArea->set_hexpand(false);

    append(*pBinaryArea);
}

void InstFormatUI::setupFieldControllers()
{

}

void InstFormatUI::UpdateDisplay(Instruction &inst)
{
    updateAssemblyDisplay(inst);
    updateBinaryDisplay(inst);
}

void InstFormatUI::updateRTypeDisplay(Instruction &inst)
{
    if(inst.GetTypePtr() == nullptr) {
        return;
    }
    int size                                    = 0;
    const std::vector<std::string> &instAssembly= inst.GetTypePtr()->GetInstAssembly();
    size                                        = format_.instTypeV_.size();
}

void InstFormatUI::updateBinaryDisplay(Instruction &inst)
{
    if(inst.GetTypePtr() == nullptr) {
        return;
    }
    int size                                    = 0;
    const std::vector<uint32_t> &instBitsField  = inst.GetTypePtr()->GetInstBitsField();
    size                                        = format_.instTypeV_.size();
}

void InstFormatUI::updateAssemblyDisplay(Instruction &inst)
{
    const auto *pInstType= inst.GetTypePtr();
    if(pInstType == nullptr) {
        return;
    }
    if(pInstType->GetInstFormat() == InstFormat::R) {
        updateRTypeDisplay(inst);
    }
}

S_InstTypeRelationEntity_t createRTypeFormat()
{
    S_InstTypeRelationEntity_t rFormat;

    rFormat.typeName_ = "R-Type";
    rFormat.fmt_      = InstFormat::R;
    rFormat.instTypeV_= { "mnemonic", "rd", ",", "rs1", ",", "rs2" };
    rFormat.binaryV_  = {
        { "funct7", 25, 31, "funct7" },
        { "rs2",    20, 24, "rs2"    },
        { "rs1",    15, 19, "rs1"    },
        { "funct3", 12, 14, "funct3" },
        { "rd",     7,  11, "rd"     },
        { "opcode", 0,  6,  "opcode" },
    };

    rFormat.binaryFieldRelations_= {
        { "opcode", { "mnemonic", "funct3", "funct7" } },
        { "funct3", { "mnemonic", "opcode", "funct7" } },
        { "funct7", { "mnemonic", "opcode", "funct3" } },
        { "rd",     { "rd" }                           },
        { "rs1",    { "rs1" }                          },
        { "rs2",    { "rs2" }                          },
    };
    rFormat.asmFieldRelations= {
        { "mnemonic", { "opcode", "funct3", "funct7" } },
        { "rd",       { "rd" }                         },
        { "rs1",      { "rs1" }                        },
        { "rs2",      { "rs2" }                        },
    };

    return rFormat;
}
