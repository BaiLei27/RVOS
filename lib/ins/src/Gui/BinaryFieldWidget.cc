#include "Gui/BinaryFieldWidget.hh"

BinaryFieldWidget::BinaryFieldWidget(std::vector<Gtk::Label *> &binaryLabelsV_, const S_InstBinaryField_t &field)
{
    MBox_= Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 0);
    MBox_->set_css_classes({ field.cssClass_ });
    MBox_->set_margin_start(0);
    MBox_->set_margin_end(0);
    MBox_->set_hexpand(false);
    for(int i= field.startBit_; i <= field.endBit_; i++) {
        auto tmpLabel= Gtk::make_managed<Gtk::Label>("0");
        tmpLabel->set_width_chars(1);
        tmpLabel->set_halign(Gtk::Align::CENTER);
        tmpLabel->set_valign(Gtk::Align::CENTER);
        tmpLabel->set_size_request(18, 30);
        tmpLabel->set_css_classes({ "binary-bit" });
        if(g_index > 0 && g_index % 4 == 0)
            tmpLabel->set_margin_start(8);
        ControlLabels_.push_back(tmpLabel);
        MBox_->append(*tmpLabel);
        g_index++;
    }
}

void BinaryFieldWidget::highlight()
{
    for(auto &label: ControlLabels_) {
        label->add_css_class("highlighted");
    }
}

void BinaryFieldWidget::unhighlight()
{
    for(auto &label: ControlLabels_) {
        label->remove_css_class("highlighted");
    }
}

void BinaryFieldWidget::highlightInMouse()
{
    for(auto &label: ControlLabels_) {
        label->add_css_class("highlighted-in-mouse");
    }
}

void BinaryFieldWidget::unhighlightInMouse()
{
    for(auto &label: ControlLabels_) {
        label->remove_css_class("highlighted-in-mouse");
    }
}

void BinaryFieldWidget::setupController(const std::string &name, S_InstTypeRelationEntity_t &instFmt, BinaryFieldWidgetMap &binaryFieldWidgets_, AsmMnemonicWidgetMap &asmFieldWidgets_)
{
    PMotionController_= Gtk::EventControllerMotion::create();
    PMotionController_->signal_motion().connect([this, &name, &instFmt, &binaryFieldWidgets_, &asmFieldWidgets_](double x, double y) {
        highlightInMouse();
        MBox_->set_tooltip_text(name);
        auto binaryFieldRelations_= instFmt.binaryFieldRelations_;
        for(auto &relatedField: binaryFieldRelations_[name]) {
            auto asmIter= asmFieldWidgets_.find(relatedField);
            if(asmIter != asmFieldWidgets_.end()) {
                asmIter->second->highlight();
            }
            auto binaryIter= binaryFieldWidgets_.find(relatedField);
            if(binaryIter != binaryFieldWidgets_.end()) {
                binaryIter->second->highlight();
            }
        }
    });
    PMotionController_->signal_leave().connect([this, &name, &instFmt, &binaryFieldWidgets_, &asmFieldWidgets_]() {
        unhighlightInMouse();
        auto binaryFieldRelations_= instFmt.binaryFieldRelations_;
        for(auto &relatedField: binaryFieldRelations_[name]) {
            auto asmIter= asmFieldWidgets_.find(relatedField);
            if(asmIter != asmFieldWidgets_.end()) {
                asmIter->second->unhighlight();
            }
            auto binaryIter= binaryFieldWidgets_.find(relatedField);
            if(binaryIter != binaryFieldWidgets_.end()) {
                binaryIter->second->unhighlight();
            }
        }
    });
    MBox_->add_controller(PMotionController_);
}

void BinaryFieldWidget::updateControlLables(uint32_t instructionValue)
{
    for(size_t i= 0; i < ControlLabels_.size(); i++) {
        int bitPosition= ControlLabels_.size() - 1 - i;
        int bitValue   = (instructionValue >> bitPosition) & 0x1;
        ControlLabels_[i]->set_text(std::to_string(bitValue));
    }
}