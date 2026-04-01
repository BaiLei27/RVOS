#include <functional>
#include <gdkmm.h>
#include "Gui/InstFormatUI.hh"

static void appendCopyAndToViewButtons(Gtk::Box &row, InstFormatUI *pUI,
                                       const std::function<std::string()> &getContent)
{
    auto pSpacer = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 0);
    pSpacer->set_hexpand(true);

    auto pCopyBtn  = Gtk::make_managed<Gtk::Button>("Copy");
    auto pToViewBtn= Gtk::make_managed<Gtk::Button>("To View");
    pCopyBtn->set_margin_start(8);
    pToViewBtn->set_margin_start(4);

    pCopyBtn->signal_clicked().connect([getContent] {
        auto content = getContent();
        if(!content.empty()) {
            if(auto display = Gdk::Display::get_default()) {
                if(auto clipboard = display->get_clipboard()) {
                    clipboard->set_text(Glib::ustring(content));
                }
            }
        }
    });

    pToViewBtn->signal_clicked().connect([pUI, getContent] {
        auto content = getContent();
        if(!content.empty()) {
            pUI->signal_put_to_output.emit(content);
        }
    });

    row.append(*pSpacer);
    row.append(*pCopyBtn);
    row.append(*pToViewBtn);
}

InstFormatUI::InstFormatUI(const InstTypeRelationEntity &format)
    : Gtk::Box(Gtk::Orientation::VERTICAL, 10), format_(format)
{
    setupAssemblyDisplay();
    setupBinaryDisplay();
    setupHexDisplay();
    setupFieldControllers();
}

void InstFormatUI::setupAssemblyDisplay()
{
    auto pAsmArea = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 0);
    pAsmArea->set_hexpand(true);
    auto pAsmTitle= Gtk::make_managed<Gtk::Label>("Assembly =");
    pAsmTitle->set_margin_end(8);
    pAsmArea->append(*pAsmTitle);

    for(const std::string &iStr: format_.instTypeV_) {
        AsmMnemonicWidget *pAsmMnemonic= new AsmMnemonicWidget(iStr, pAsmArea);
        if(iStr.compare(",") == 0)
            continue;
        AsmFieldWidgets_[iStr]= pAsmMnemonic;
    }

    appendCopyAndToViewButtons(*pAsmArea, this, [this] { return getAssemblyContent(); });
    append(*pAsmArea);
}

void InstFormatUI::setupBinaryDisplay()
{
    auto pBinaryArea = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 0);
    auto pBinaryTitle= Gtk::make_managed<Gtk::Label>("Binary =");
    pBinaryTitle->set_margin_end(8);
    pBinaryArea->append(*pBinaryTitle);
    pBinaryArea->set_halign(Gtk::Align::FILL);
    pBinaryArea->set_valign(Gtk::Align::CENTER);
    pBinaryArea->set_hexpand(true);

    for(const auto &binaryElem: format_.binaryV_) {
        BinaryFieldWidget *pTmpBinaryField   = new BinaryFieldWidget(BinaryLabelsV_, binaryElem);
        BinaryFieldWidgets_[binaryElem.name_]= pTmpBinaryField;
        pBinaryArea->append(*pTmpBinaryField->mBox_);
    }

    appendCopyAndToViewButtons(*pBinaryArea, this, [this] { return getBinaryContent(); });
    append(*pBinaryArea);
}

void InstFormatUI::setupHexDisplay()
{
    auto pHexArea = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 0);
    pHexArea->set_hexpand(true);
    auto pHexTitle= Gtk::make_managed<Gtk::Label>("Hexadecimal =");
    pHexTitle->set_margin_end(8);
    pHexArea->append(*pHexTitle);

    pHexLabel_ = Gtk::make_managed<Gtk::Label>("0x00000000");
    pHexLabel_->set_margin_end(8);
    pHexArea->append(*pHexLabel_);

    appendCopyAndToViewButtons(*pHexArea, this, [this] { return getHexContent(); });
    append(*pHexArea);
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
    updateHexDisplay(inst);
}

void InstFormatUI::updateRTypeDisplay(Instruction &inst)
{
    if(inst.GetTypePtr() == nullptr) {
        return;
    }
    int size                                    = 0;
    const std::vector<std::string> &instAssembly= inst.GetTypePtr()->GetInstAssembly();
    size                                        = format_.instTypeV_.size();
    for(int i= 0, j= 0; i < size; ++i) {
        if(format_.instTypeV_[i] == ",")
            continue;
        if(instAssembly.at(j) == " " || instAssembly.at(j) == ",") {
            j++;
        }
        AsmFieldWidgets_[format_.instTypeV_[i]]->mLabel_->set_text(instAssembly.at(j++));
    }
}

void InstFormatUI::updateITypeDisplay(Instruction &inst)
{
    if(inst.GetTypePtr() == nullptr) {
        return;
    }
    int size                                    = 0;
    const std::vector<std::string> &instAssembly= inst.GetTypePtr()->GetInstAssembly();
    size                                        = format_.instTypeV_.size();
    for(int i= 0, j= 0; i < size; ++i) {
        if(format_.instTypeV_[i] == ",")
            continue;
        if(j < static_cast<int>(instAssembly.size()) && (instAssembly.at(j) == " " || instAssembly.at(j) == ",")) {
            j++;
        }
        if(j < static_cast<int>(instAssembly.size())) {
            AsmFieldWidgets_[format_.instTypeV_[i]]->mLabel_->set_text(instAssembly.at(j++));
        }
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
    switch(pInstType->GetInstFormat()) {
    case InstFormat::R:
        updateRTypeDisplay(inst);
        break;
    case InstFormat::I:
    case InstFormat::J:
        updateITypeDisplay(inst);
        break;
    default:
        break;
    }
}

inline std::string getHexStr(uint32_t val)
{
    return std::format("{:08x}", val);
}

void InstFormatUI::updateHexDisplay(Instruction &inst)
{
    if(inst.GetTypePtr() == nullptr) {
        return;
    }

    uint32_t val= static_cast<uint32_t>(inst);
    pHexLabel_->set_text("0x" + getHexStr(val));
}

std::string InstFormatUI::getAssemblyContent() const
{
    std::string result;
    for(const std::string &key: format_.instTypeV_) {
        if(key == ",") {
            result += ", ";
            continue;
        }
        auto it = AsmFieldWidgets_.find(key);
        if(it != AsmFieldWidgets_.end() && it->second->mLabel_) {
            result += it->second->mLabel_->get_text().raw();
            if(key == "mnemonic") {
                result += " ";
            }
        }
    }
    return result;
}

std::string InstFormatUI::getBinaryContent() const
{
    std::string result;
    for(const auto &elem: format_.binaryV_) {
        auto it = BinaryFieldWidgets_.find(elem.name_);
        if(it != BinaryFieldWidgets_.end()) {
            for(auto *label: it->second->controlLabels_) {
                if(label) {
                    result += label->get_text().raw();
                }
            }
        }
    }
    return result;
}

std::string InstFormatUI::getHexContent() const
{
    if(pHexLabel_) {
        return std::string(pHexLabel_->get_text().raw());
    }
    return {};
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

InstTypeRelationEntity createITypeFormat()
{
    InstTypeRelationEntity iFormat;

    iFormat.typeName_ = "I-Type";
    iFormat.fmt_      = InstFormat::I;
    iFormat.instTypeV_= { "mnemonic", "rd", ",", "rs1", ",", "imm" };
    iFormat.binaryV_  = {
        { "imm",    20, 31, "imm[11:0]" },
        { "rs1",    15, 19, "rs1"       },
        { "funct3", 12, 14, "funct3"    },
        { "rd",     7,  11, "rd"        },
        { "opcode", 0,  6,  "opcode"    },
    };

    iFormat.binaryFieldRelations_= {
        { "opcode", { "mnemonic", "funct3", "imm" } },
        { "funct3", { "mnemonic", "opcode", "imm" } },
        { "imm",    { "mnemonic", "opcode", "funct3" } },
        { "rd",     { "rd" }                           },
        { "rs1",    { "rs1" }                          },
    };
    iFormat.asmFieldRelations= {
        { "mnemonic", { "opcode", "funct3", "imm" } },
        { "rd",       { "rd" }                     },
        { "rs1",      { "rs1" }                    },
        { "imm",      { "imm" }                    },
    };

    return iFormat;
}

InstTypeRelationEntity createJTypeFormat()
{
    InstTypeRelationEntity jFormat;

    jFormat.typeName_ = "J-Type";
    jFormat.fmt_      = InstFormat::J;
    jFormat.instTypeV_= { "mnemonic", "rd", ",", "imm" };
    jFormat.binaryV_  = {
        { "imm20",    31, 31, "imm[20]"   },
        { "imm10_1",  21, 30, "imm[10:1]" },
        { "imm11",    20, 20, "imm[11]"   },
        { "imm19_12", 12, 19, "imm[19:12]" },
        { "rd",       7,  11, "rd"        },
        { "opcode",   0,  6,  "opcode"    },
    };

    jFormat.binaryFieldRelations_= {
        { "opcode",   { "mnemonic" } },
        { "imm20",    { "imm" } },
        { "imm10_1",  { "imm" } },
        { "imm11",    { "imm" } },
        { "imm19_12", { "imm" } },
        { "rd",       { "rd" } },
    };
    jFormat.asmFieldRelations= {
        { "mnemonic", { "opcode" } },
        { "rd",       { "rd" } },
        { "imm",      { "imm20", "imm10_1", "imm11", "imm19_12" } },
    };

    return jFormat;
}
