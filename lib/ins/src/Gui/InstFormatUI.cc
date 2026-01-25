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
    auto asmArea = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 0);
    auto asmTitle= Gtk::make_managed<Gtk::Label>("Assembly =");
    asmTitle->set_margin_end(8);
    asmArea->append(*asmTitle);

    for(const std::string &iStr: format_.instTypeV_) {
        AsmMnemonicWidget *asmMnemonic_= new AsmMnemonicWidget(iStr, asmArea);
        if(iStr.compare(",") == 0)
            continue;
        asmFieldWidgets_[iStr]= asmMnemonic_;
    }

    append(*asmArea);
}

void InstFormatUI::setupBinaryDisplay()
{
    auto binaryArea = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 0);
    auto binaryTitle= Gtk::make_managed<Gtk::Label>("Binary =");
    binaryTitle->set_margin_end(8);
    binaryArea->append(*binaryTitle);
    binaryArea->set_halign(Gtk::Align::START);
    binaryArea->set_valign(Gtk::Align::CENTER);
    binaryArea->set_hexpand(false);

    for(const auto &binaryElem: format_.binaryV_) {
        BinaryFieldWidget *tmpBinaryField_   = new BinaryFieldWidget(binaryLabelsV_, binaryElem);
        binaryFieldWidgets_[binaryElem.name_]= tmpBinaryField_;
        binaryArea->append(*tmpBinaryField_->MBox_);
    }

    append(*binaryArea);
}

void InstFormatUI::setupFieldControllers()
{

    for(const auto &asmElem: asmFieldWidgets_) {
        asmElem.second->setupController(asmElem.first, format_, binaryFieldWidgets_);
    }

    for(const auto &binaryElem: binaryFieldWidgets_) {
        binaryElem.second->setupController(binaryElem.first, format_, binaryFieldWidgets_, asmFieldWidgets_);
    }
}

void InstFormatUI::updateDisplay(Instruction &inst)
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
    const std::vector<uint32_t> &instBitsField  = inst.GetTypePtr()->GetInstBitsField();
    size                                        = format_.instTypeV_.size();
    for(int i= 0, j= 0; i < size; i++) {
        if(format_.instTypeV_[i] == ",")
            continue;
        asmFieldWidgets_[format_.instTypeV_[i]]->MLabel_->set_text(instAssembly.at(j++));
    }

    for(int i= format_.binaryV_.size() - 1, j= 0; i >= 0; i--) {
        binaryFieldWidgets_[format_.binaryV_[i].name_]->updateControlLables(instBitsField.at(j++));
    }
}

void InstFormatUI::updateITypeDisplay(Instruction &inst)
{
    if(inst.GetTypePtr() == nullptr) {
        return;
    }
    const std::vector<std::string> &instAssembly= inst.GetTypePtr()->GetInstAssembly();
    const std::vector<uint32_t> &instBitsField  = inst.GetTypePtr()->GetInstBitsField();
    for(int i= 0, j= 0; i < format_.instTypeV_.size(); i++) {
        if(format_.instTypeV_[i] == ",")
            continue;
        asmFieldWidgets_[format_.instTypeV_[i]]->MLabel_->set_text(instAssembly.at(j++));
    }

    for(int i= format_.binaryV_.size() - 1, j= 0; i >= 0; i--) {
        binaryFieldWidgets_[format_.binaryV_[i].name_]->updateControlLables(instBitsField.at(j++));
    }
}

void InstFormatUI::updateJTypeDisplay(Instruction &inst)
{
}

void InstFormatUI::updateSTypeDisplay(Instruction &inst)
{
}

void InstFormatUI::updateBTypeDisplay(Instruction &inst)
{
}

void InstFormatUI::updateUTypeDisplay(Instruction &inst)
{
}

void InstFormatUI::updateAssemblyDisplay(Instruction &inst)
{

    auto instType= inst.GetTypePtr();
    if(instType == nullptr) {
        return;
    }
    if(instType->GetInstFormat() == InstFormat::R) {
        updateRTypeDisplay(inst);
    } else if(instType->GetInstFormat() == InstFormat::I) {
        updateITypeDisplay(inst);
    } else if(instType->GetInstFormat() == InstFormat::J) {
        updateJTypeDisplay(inst);
    } else if(instType->GetInstFormat() == InstFormat::S) {
        updateSTypeDisplay(inst);
    } else if(instType->GetInstFormat() == InstFormat::B) {
        updateBTypeDisplay(inst);
    } else if(instType->GetInstFormat() == InstFormat::U) {
        updateUTypeDisplay(inst);
    }
}

void InstFormatUI::updateBinaryDisplay(Instruction &inst)
{
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
    rFormat.asmFieldRelations_= {
        { "mnemonic", { "opcode", "funct3", "funct7" } },
        { "rd",       { "rd" }                         },
        { "rs1",      { "rs1" }                        },
        { "rs2",      { "rs2" }                        },
    };

    return rFormat;
}

S_InstTypeRelationEntity_t createITypeFormat()
{
    S_InstTypeRelationEntity_t iFormat;

    iFormat.typeName_ = "I-Type";
    iFormat.fmt_      = InstFormat::I;
    iFormat.instTypeV_= { "mnemonic", "rd", ",", "rs1", ",", "imm0tB" };
    iFormat.binaryV_  = {
        { "imm0tB", 20, 31, "imm[11:0]" },
        { "rs1",    15, 19, "rs1"       },
        { "funct3", 12, 14, "funct3"    },
        { "rd",     7,  11, "rd"        },
        { "opcode", 0,  6,  "opcode"    },
    };

    iFormat.binaryFieldRelations_= {
        { "opcode", { "mnemonic", "funct3" } },
        { "funct3", { "mnemonic", "opcode" } },
        { "rd",     { "rd" }                 },
        { "rs1",    { "rs1" }                },
        { "imm0tB", { "imm0tB" }             },
    };

    iFormat.asmFieldRelations_= {
        { "mnemonic", { "opcode", "funct3" } },
        { "rd",       { "rd" }               },
        { "rs1",      { "rs1" }              },
        { "imm0tB",   { "imm0tB" }           },
    };

    return iFormat;
}

S_InstTypeRelationEntity_t createSTypeFormat()
{
    S_InstTypeRelationEntity_t sFormat;

    sFormat.typeName_ = "S-Type";
    sFormat.fmt_      = InstFormat::S;
    sFormat.instTypeV_= { "mnemonic", "rs2", ",", "imm", "(rs1)" };
    sFormat.binaryV_  = {
        { "imm5tB", 25, 31, "imm[11:5]" },
        { "rs2",    20, 24, "rs2"       },
        { "rs1",    15, 19, "rs1"       },
        { "funct3", 12, 14, "funct3"    },
        { "imm0t4", 7,  11, "imm[4:0]"  },
        { "opcode", 0,  6,  "opcode"    },
    };

    sFormat.binaryFieldRelations_= {
        { "opcode", { "mnemonic", "funct3" } },
        { "funct3", { "mnemonic", "opcode" } },
        { "rs1",    { "(rs1)" }              },
        { "rs2",    { "rs2" }                },
        { "imm0t4", { "imm", "imm5tB" }      },
        { "imm5tB", { "imm", "imm0t4" }      },
    };

    sFormat.asmFieldRelations_= {
        { "mnemonic", { "opcode", "funct3" } },
        { "(rs1)",    { "rs1" }              },
        { "rs2",      { "rs2" }              },
        { "imm",      { "imm0t4", "imm5tB" } },
    };

    return sFormat;
}

S_InstTypeRelationEntity_t createBTypeFormat()
{
    S_InstTypeRelationEntity_t bFormat;

    bFormat.typeName_ = "B-Type";
    bFormat.fmt_      = InstFormat::B;
    bFormat.instTypeV_= { "mnemonic", "rs1", ",", "rs2", ",", "imm" };
    bFormat.binaryV_  = {
        { "immC",   31, 31, "imm[12]"   },
        { "imm5tA", 25, 30, "imm[10:5]" },
        { "rs2",    20, 24, "rs2"       },
        { "rs1",    15, 19, "rs1"       },
        { "funct3", 12, 14, "funct3"    },
        { "imm1t4", 8,  11, "imm[4:1]"  },
        { "immB",   7,  7,  "imm[11]"   },
        { "opcode", 0,  6,  "opcode"    },
    };

    bFormat.binaryFieldRelations_= {
        { "opcode", { "mnemonic", "funct3" }              },
        { "funct3", { "mnemonic", "opcode" }              },
        { "rs1",    { "rs1" }                             },
        { "rs2",    { "rs2" }                             },
        { "immB",   { "imm", "imm1t4", "imm5tA", "immC" } },
        { "imm1t4", { "imm", "immB", "imm5tA", "immC" }   },
        { "imm5tA", { "imm", "immB", "imm1t4", "immC" }   },
        { "immC",   { "imm", "immB", "imm1t4", "imm5tA" } },
    };

    bFormat.asmFieldRelations_= {
        { "mnemonic", { "opcode", "funct3" }                 },
        { "rs1",      { "rs1" }                              },
        { "rs2",      { "rs2" }                              },
        { "imm",      { "immB", "imm1t4", "imm5tA", "immC" } },
    };

    return bFormat;
}

S_InstTypeRelationEntity_t createUTypeFormat()
{
    S_InstTypeRelationEntity_t uFormat;

    uFormat.typeName_ = "U-Type";
    uFormat.fmt_      = InstFormat::U;
    uFormat.instTypeV_= { "mnemonic", "rd", ",", "imm" };

    uFormat.binaryV_= {
        { "imm",    12, 31, "imm[31:12]" },
        { "rd",     7,  11, "rd"         },
        { "opcode", 0,  6,  "opcode"     },
    };

    uFormat.binaryFieldRelations_= {
        { "opcode", { "mnemonic" } },
        { "rd",     { "rd" }       },
        { "imm",    { "imm" }      },
    };

    uFormat.asmFieldRelations_= {
        { "mnemonic", { "opcode" } },
        { "rd",       { "rd" }     },
        { "imm",      { "imm" }    },
    };

    return uFormat;
}

S_InstTypeRelationEntity_t createJTypeFormat()
{
    S_InstTypeRelationEntity_t jFormat;

    jFormat.typeName_ = "J-Type";
    jFormat.fmt_      = InstFormat::J;
    jFormat.instTypeV_= { "mnemonic", "rd", ",", "imm" };

    jFormat.binaryV_= {
        { "imm14",   31, 31, "imm[20]"    },
        { "imm1tA",  21, 30, "imm[10:1]"  },
        { "immB",    20, 20, "imm[11]"    },
        { "immCt13", 12, 19, "imm[19:12]" },
        { "rd",      7,  11, "rd"         },
        { "opcode",  0,  6,  "opcode"     },
    };

    jFormat.binaryFieldRelations_= {
        { "opcode",  { "mnemonic" }                          },
        { "rd",      { "rd" }                                },
        { "immCt13", { "imm", "immB", "imm1tA", "imm14" }    },
        { "immB",    { "imm", "immCt13", "imm1tA", "imm14" } },
        { "imm1tA",  { "imm", "immCt13", "immB", "imm14" }   },
        { "imm14",   { "imm", "immCt13", "immB", "imm1tA" }  },
    };

    jFormat.asmFieldRelations_= {
        { "mnemonic", { "opcode" }                             },
        { "rd",       { "rd" }                                 },
        { "imm",      { "immCt13", "immB", "imm1tA", "imm14" } },
    };

    return jFormat;
}