#include "Gui/InstCommST.hh"
BinaryFieldWidgetPairST::BinaryFieldWidgetPairST(std::vector<Gtk::Label*> &binaryLabelsV_,const InstFieldST &field) {
    box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 0);
    box->set_css_classes({field.cssClass_});
    box->set_margin_start(0);
    box->set_margin_end(0);
    box->set_hexpand(false);
    for(int i = field.startBit_; i <= field.endBit_; i++) {
        auto tmpLabel = Gtk::make_managed<Gtk::Label>("0");
        tmpLabel->set_width_chars(1);
        tmpLabel->set_halign(Gtk::Align::CENTER);
        tmpLabel->set_valign(Gtk::Align::CENTER);
        tmpLabel->set_size_request(18, 30);
        tmpLabel->set_css_classes({"binary-bit"});
        if(index_ > 0 && index_ % 4 == 0)
            tmpLabel->set_margin_start(8);
        controlLabelsV_.push_back(tmpLabel);
        box->append(*tmpLabel);
        index_++;
    }
}
void BinaryFieldWidgetPairST::highlight() {
    for(auto &label : controlLabelsV_) {
        label->add_css_class("highlighted");
    }
}
void BinaryFieldWidgetPairST::unhighlight() {
    for(auto &label : controlLabelsV_) {
        label->remove_css_class("highlighted");
    }
}
void BinaryFieldWidgetPairST::highlight_in_mouse() {
    for(auto &label : controlLabelsV_) {
        label->add_css_class("highlighted-in-mouse");
    }
}
void BinaryFieldWidgetPairST::unhighlight_in_mouse() {
    for(auto &label : controlLabelsV_) {
        label->remove_css_class("highlighted-in-mouse");
    }
}

void BinaryFieldWidgetPairST::setupController(const std::string& name, InstFormatST& instFmt,
         BinaryFieldWidgetSTMap& binaryFieldWidgets_, AsmMnemonicWidgetSTMap& asmFieldWidgets_) {
    pMotionController = Gtk::EventControllerMotion::create();
    pMotionController->signal_motion().connect([this, name, instFmt, binaryFieldWidgets_, asmFieldWidgets_](double x, double y) {
        highlight_in_mouse();
        box->set_tooltip_text(name);
        auto binaryFieldRelations_ = instFmt.binaryFieldRelations_;
        for(auto &relatedField : binaryFieldRelations_[name]) {
            auto asmIter = asmFieldWidgets_.find(relatedField);
            if(asmIter != asmFieldWidgets_.end()) {
                asmIter->second->highlight();
            }
            auto binaryIter = binaryFieldWidgets_.find(relatedField);
            if(binaryIter != binaryFieldWidgets_.end()) {
                binaryIter->second->highlight();
            }
        }
    });
    pMotionController->signal_leave().connect([this, name, instFmt, binaryFieldWidgets_, asmFieldWidgets_]() {
        unhighlight_in_mouse();
        auto binaryFieldRelations_ = instFmt.binaryFieldRelations_;
        for(auto &relatedField : binaryFieldRelations_[name]) {
            auto asmIter = asmFieldWidgets_.find(relatedField);
            if(asmIter != asmFieldWidgets_.end()) {
                asmIter->second->unhighlight();
            }
            auto binaryIter = binaryFieldWidgets_.find(relatedField);
            if(binaryIter != binaryFieldWidgets_.end()) {
                binaryIter->second->unhighlight();
            }
        }
    });
    box->add_controller(pMotionController);
}

void BinaryFieldWidgetPairST::updateControlLables(uint32_t instructionValue) {
    for(size_t i = 0; i < controlLabelsV_.size(); i++) {
        int bitPosition = controlLabelsV_.size() - 1 - i;
        int bitValue = (instructionValue >> bitPosition) & 0x1;
        controlLabelsV_[i]->set_text(std::to_string(bitValue));
    }
}

AsmMnemonicWidgetPairST::AsmMnemonicWidgetPairST(const std::string& name, Gtk::Box* parentAsmBox) {
    if (parentAsmBox) {
        box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 0);
        box->set_css_classes({"instruction-container"});
        label = Gtk::make_managed<Gtk::Label>(name);
        if(name == "mnemonic") {
            label->set_margin_end(8);
            label->set_css_classes({"instruction-label"});
        }  else if(name == ",") {

        } else {
            label->set_css_classes({"register-label"});
        }
        box->append(*label);
        parentAsmBox->append(*box);
    }
}

void AsmMnemonicWidgetPairST::highlight() {
    label->add_css_class("highlighted");
}
void AsmMnemonicWidgetPairST::unhighlight() {
    label->remove_css_class("highlighted");
}
void AsmMnemonicWidgetPairST::highlight_in_mouse() {
    label->add_css_class("highlighted-in-mouse");
}
void AsmMnemonicWidgetPairST::unhighlight_in_mouse() {
    label->remove_css_class("highlighted-in-mouse");
}

void AsmMnemonicWidgetPairST::setupController(const std::string& name, InstFormatST& instFmt, BinaryFieldWidgetSTMap& binaryFieldWidgets_) {
    pMotionController = Gtk::EventControllerMotion::create();
    pMotionController->signal_motion().connect([this, name, instFmt, binaryFieldWidgets_](double x, double y) {
        highlight_in_mouse();
        auto asmFieldRelations_ = instFmt.asmFieldRelations_;
        for(const auto& relatedField : asmFieldRelations_[name]) {
            auto iter = binaryFieldWidgets_.find(relatedField);
            if(iter != binaryFieldWidgets_.end()) {
                iter->second->highlight();
            }
        }
    });
    pMotionController->signal_leave().connect([this, name, instFmt, binaryFieldWidgets_]() {
        unhighlight_in_mouse();
        auto asmFieldRelations_ = instFmt.asmFieldRelations_;
        for(const auto& relatedField : asmFieldRelations_[name]) {
            auto iter = binaryFieldWidgets_.find(relatedField);
            if(iter != binaryFieldWidgets_.end()) {
                iter->second->unhighlight();
            }
        }
    });
    box->add_controller(pMotionController);
}