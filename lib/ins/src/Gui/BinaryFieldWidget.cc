#include "Gui/BinaryFieldWidget.hh"

BinaryFieldWidget::BinaryFieldWidget(std::vector<Gtk::Label *> &binaryLabelsV, const InstBinaryField &field)
    : mBox_(Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 0))
{
    mBox_->set_css_classes({ field.cssClass_ });
    mBox_->set_margin_start(0);
    mBox_->set_margin_end(0);
    mBox_->set_hexpand(false);
    for(int i= field.startBit_; i <= field.endBit_; i++) {
        auto pTmpLabel= Gtk::make_managed<Gtk::Label>("0");
        pTmpLabel->set_width_chars(1);
        pTmpLabel->set_halign(Gtk::Align::CENTER);
        pTmpLabel->set_valign(Gtk::Align::CENTER);
        pTmpLabel->set_size_request(18, 30);
        pTmpLabel->set_css_classes({ "binary-bit" });
        if(g_index > 0 && g_index % 4 == 0)
            pTmpLabel->set_margin_start(8);
        controlLabels_.push_back(pTmpLabel);
        mBox_->append(*pTmpLabel);
        g_index++;
    }
}

void BinaryFieldWidget::Highlight()
{
    for(auto &label: controlLabels_) {
        label->add_css_class("highlighted");
    }
}

void BinaryFieldWidget::Unhighlight()
{
    for(auto &label: controlLabels_) {
        label->remove_css_class("highlighted");
    }
}

void BinaryFieldWidget::HighlightInMouse()
{
    for(auto &label: controlLabels_) {
        label->add_css_class("highlighted-in-mouse");
    }
}

void BinaryFieldWidget::UnhighlightInMouse()
{
    for(auto &label: controlLabels_) {
        label->remove_css_class("highlighted-in-mouse");
    }
}

void BinaryFieldWidget::SetupController(const std::string &name,
                                        InstTypeRelationEntity &instFmt,
                                        InstCommST::BinaryFieldWidgetMap_u &binaryFieldWidgets,
                                        InstCommST::AsmMnemonicWidgetMap_u &asmFieldWidgets)
{
    pMotionController_= Gtk::EventControllerMotion::create();
    pMotionController_->signal_motion().connect([this, &name, &instFmt, &binaryFieldWidgets, &asmFieldWidgets](double x, double y) {
        HighlightInMouse();
        mBox_->set_tooltip_text(name);
        auto binaryFieldRelations= instFmt.binaryFieldRelations_;
        for(auto &relatedField: binaryFieldRelations[name]) {
            auto asmIter= asmFieldWidgets.find(relatedField);
            if(asmIter != asmFieldWidgets.end()) {
                asmIter->second->Highlight();
            }
            auto binaryIter= binaryFieldWidgets.find(relatedField);
            if(binaryIter != binaryFieldWidgets.end()) {
                binaryIter->second->Highlight();
            }
        }
    });
    pMotionController_->signal_leave().connect([this, &name, &instFmt, &binaryFieldWidgets, &asmFieldWidgets]() {
        UnhighlightInMouse();
        auto binaryFieldRelations= instFmt.binaryFieldRelations_;
        for(auto &relatedField: binaryFieldRelations[name]) {
            auto asmIter= asmFieldWidgets.find(relatedField);
            if(asmIter != asmFieldWidgets.end()) {
                asmIter->second->Unhighlight();
            }
            auto binaryIter= binaryFieldWidgets.find(relatedField);
            if(binaryIter != binaryFieldWidgets.end()) {
                binaryIter->second->Unhighlight();
            }
        }
    });
    mBox_->add_controller(pMotionController_);
}

void BinaryFieldWidget::UpdateControlLables(uint32_t instructionValue)
{
    for(std::size_t i= 0; i < controlLabels_.size(); i++) {
        int bitPosition= controlLabels_.size() - 1 - i;
        int bitValue   = (instructionValue >> bitPosition) & 0x1;
        controlLabels_[i]->set_text(std::to_string(bitValue));
    }
}
