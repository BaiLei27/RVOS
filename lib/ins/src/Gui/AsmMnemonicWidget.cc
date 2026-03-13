#include "Gui/AsmMnemonicWidget.hh"

AsmMnemonicWidget::AsmMnemonicWidget(const std::string &name, Gtk::Box *pParentAsmBox)
{
    if(pParentAsmBox) {
        mBox_= Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 0);
        mBox_->set_css_classes({ "instruction-container" });
        mLabel_= Gtk::make_managed<Gtk::Label>(name);
        if(name == "mnemonic") {
            mLabel_->set_margin_end(8);
            mLabel_->set_css_classes({ "instruction-label" });
        } else if(name == ",") {

        } else {
            mLabel_->set_css_classes({ "register-label" });
        }
        mBox_->append(*mLabel_);
        pParentAsmBox->append(*mBox_);
    }
}

void AsmMnemonicWidget::Highlight()
{
    mLabel_->add_css_class("highlighted");
}

void AsmMnemonicWidget::Unhighlight()
{
    mLabel_->remove_css_class("highlighted");
}

void AsmMnemonicWidget::HighlightInMouse()
{
    mLabel_->add_css_class("highlighted-in-mouse");
}

void AsmMnemonicWidget::UnhighlightInMouse()
{
    mLabel_->remove_css_class("highlighted-in-mouse");
}

void AsmMnemonicWidget::SetupController(const std::string &name,
                                        InstTypeRelationEntity &instFmt,
                                        InstCommST::BinaryFieldWidgetMap_u &binaryFieldWidgets)
{
    pMotionController_= Gtk::EventControllerMotion::create();
    pMotionController_->signal_motion().connect([this, &name, &instFmt, &binaryFieldWidgets](double x, double y) {
        HighlightInMouse();
        auto asmFieldRelations= instFmt.asmFieldRelations;
        for(const auto &relatedField: asmFieldRelations[name]) {
            auto iter= binaryFieldWidgets.find(relatedField);
            if(iter != binaryFieldWidgets.end()) {
                iter->second->Highlight();
            }
        }
    });
    pMotionController_->signal_leave().connect([this, &name, &instFmt, &binaryFieldWidgets]() {
        UnhighlightInMouse();
        auto asmFieldRelations= instFmt.asmFieldRelations;
        for(const auto &relatedField: asmFieldRelations[name]) {
            auto iter= binaryFieldWidgets.find(relatedField);
            if(iter != binaryFieldWidgets.end()) {
                iter->second->Unhighlight();
            }
        }
    });
    mBox_->add_controller(pMotionController_);
}