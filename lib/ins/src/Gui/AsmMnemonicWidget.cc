#include "Gui/AsmMnemonicWidget.hh"

AsmMnemonicWidget::AsmMnemonicWidget(const std::string &name, Gtk::Box *parentAsmBox)
{
    if(parentAsmBox) {
        MBox_= Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 0);
        MBox_->set_css_classes({ "instruction-container" });
        MLabel_= Gtk::make_managed<Gtk::Label>(name);
        if(name == "mnemonic") {
            MLabel_->set_margin_end(8);
            MLabel_->set_css_classes({ "instruction-label" });
        } else if(name == ",") {

        } else {
            MLabel_->set_css_classes({ "register-label" });
        }
        MBox_->append(*MLabel_);
        parentAsmBox->append(*MBox_);
    }
}

void AsmMnemonicWidget::highlight()
{
    MLabel_->add_css_class("highlighted");
}

void AsmMnemonicWidget::unhighlight()
{
    MLabel_->remove_css_class("highlighted");
}

void AsmMnemonicWidget::highlightInMouse()
{
    MLabel_->add_css_class("highlighted-in-mouse");
}

void AsmMnemonicWidget::unhighlightInMouse()
{
    MLabel_->remove_css_class("highlighted-in-mouse");
}

void AsmMnemonicWidget::setupController(const std::string &name, S_InstTypeRelationEntity_t &instFmt, BinaryFieldWidgetMap &binaryFieldWidgets_)
{
    PMotionController_= Gtk::EventControllerMotion::create();
    PMotionController_->signal_motion().connect([this, &name, &instFmt, &binaryFieldWidgets_](double x, double y) {
        highlightInMouse();
        auto asmFieldRelations_= instFmt.asmFieldRelations_;
        for(const auto &relatedField: asmFieldRelations_[name]) {
            auto iter= binaryFieldWidgets_.find(relatedField);
            if(iter != binaryFieldWidgets_.end()) {
                iter->second->highlight();
            }
        }
    });
    PMotionController_->signal_leave().connect([this, &name, &instFmt, &binaryFieldWidgets_]() {
        unhighlightInMouse();
        auto asmFieldRelations_= instFmt.asmFieldRelations_;
        for(const auto &relatedField: asmFieldRelations_[name]) {
            auto iter= binaryFieldWidgets_.find(relatedField);
            if(iter != binaryFieldWidgets_.end()) {
                iter->second->unhighlight();
            }
        }
    });
    MBox_->add_controller(PMotionController_);
}