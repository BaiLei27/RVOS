#include "Gui/InstFormatUI.hh"

InstFormatUI::InstFormatUI(const InstTypeRelationEntity &format)
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

    for(const std::string &iStr: format_.instTypeV_) {
        AsmMnemonicWidget *pAsmMnemonic= new AsmMnemonicWidget(iStr, pAsmArea);
        if(iStr.compare(",") == 0)
            continue;
        AsmFieldWidgets_[iStr]= pAsmMnemonic;
    }

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

    for(const auto &binaryElem: format_.binaryV_) {
        BinaryFieldWidget *pTmpBinaryField   = new BinaryFieldWidget(BinaryLabelsV_, binaryElem);
        BinaryFieldWidgets_[binaryElem.name_]= pTmpBinaryField;
        pBinaryArea->append(*pTmpBinaryField->mBox_);
    }

    append(*pBinaryArea);
}

void InstFormatUI::setupFieldControllers()
{
    for(const auto &asmElem: AsmFieldWidgets_) {
        asmElem.second->SetupController(asmElem.first, format_, BinaryFieldWidgets_);
    }

    for(const auto &binaryElem: BinaryFieldWidgets_) {
        binaryElem.second->SetupController(binaryElem.first, format_, BinaryFieldWidgets_, AsmFieldWidgets_);
    }
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
    for(int i= 0, j= 0; i < size; i++) {
        if(format_.instTypeV_[i] == ",")
            continue;
        if(instAssembly.at(j) == " " || instAssembly.at(j) == ",") {
            j++;
        }
        AsmFieldWidgets_[format_.instTypeV_[i]]->mLabel_->set_text(instAssembly.at(j++));
    }
}

void InstFormatUI::updateBinaryDisplay(Instruction &inst)
{
    if(inst.GetTypePtr() == nullptr) {
        return;
    }
    int size                                  = 0;
    const std::vector<uint32_t> &instBitsField= inst.GetTypePtr()->GetInstBitsField();
    size                                      = format_.binaryV_.size();

    for(int i= size - 1, j= 0; i >= 0; i--) {
        BinaryFieldWidgets_[format_.binaryV_[i].name_]->UpdateControlLables(instBitsField.at(j++));
    }
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

InstTypeRelationEntity createRTypeFormat()
{
    InstTypeRelationEntity rFormat;

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
