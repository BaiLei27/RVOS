#include <gtkmm.h>
#include <iostream>
#include <map>
#include <exception>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <sstream>
#include <iomanip>
#include <format>
#include "Core/InstTypeFactory.hh"
#include "ISA/InstFormat.hh"
#include "Gui/RISCVInstructionWindow.hh"

RISCVInstructionWindow::RISCVInstructionWindow(): InsEntry_(Gtk::make_managed<Gtk::Entry>()),
                                                  InsButtonParse_(Gtk::make_managed<Gtk::Button>("Parse Instruction")),
                                                  uiContainer_(Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 10))
{
    set_title("RISC-V Instruction Encoder/Decoder");
    set_default_size(900, 300);

    uiContainer_->set_margin(15);

    InsEntry_->set_placeholder_text("Enter instruction");
    InsEntry_->set_hexpand(true);
    InsEntry_->signal_activate().connect(sigc::mem_fun(*this, &RISCVInstructionWindow::onInsButtonParseClicked));

    pEntryRow_= Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 4);
    pEntryRow_->append(*InsEntry_);

    pSettingsBtn_= Gtk::make_managed<Gtk::Button>("\u22EE");
    pSettingsBtn_->set_tooltip_text("ABI / ISA settings");
    setupSettingsPopover();
    pEntryRow_->append(*pSettingsBtn_);

    InsButtonParse_->signal_clicked().connect(sigc::mem_fun(*this, &RISCVInstructionWindow::onInsButtonParseClicked));

    uiContainer_->append(*pEntryRow_);
    uiContainer_->append(*InsButtonParse_);

    InsTextView_= Gtk::make_managed<Gtk::TextView>();
    InsTextView_->set_margin(10);
    InsTextView_->set_vexpand(true);
    InsTextView_->set_hexpand(true);
    InsTextView_->set_editable(false);
    InsTextView_->set_cursor_visible(false);
    loadCssFromFile(*this);

    initInstFormatUI();
    uiContainer_->append(*InsTextView_);
    set_child(*uiContainer_);
}

void RISCVInstructionWindow::initInstFormatUI()
{
    rTypeUI_= new InstFormatUI(createRTypeFormat());
    rTypeUI_->set_visible(false);
    rTypeUI_->signal_put_to_output.connect([this](const std::string &content) {
        if(InsTextView_ && InsTextView_->get_buffer()) {
            InsTextView_->get_buffer()->set_text(content);
        }
    });
    uiContainer_->append(*rTypeUI_);
}

void RISCVInstructionWindow::hideAllTypeUI()
{
    rTypeUI_->hide();
}

void RISCVInstructionWindow::onInsButtonParseClicked()
{
    int ret    = 0;
    auto hexStr= InsEntry_->get_text();
    if(hexStr.empty()) {
        showError("Please enter a valid hexadecimal instruction");
        return;
    }

    try {
        std::string cleanStr= hexStr;
        if(cleanStr.size() >= 2 && (cleanStr.substr(0, 2) == "0x" || cleanStr.substr(0, 2) == "0X")) {
            cleanStr= cleanStr.substr(2);
        }

        uint64_t value= 0;
        value         = std::stoull(cleanStr, nullptr, 16);

        if(value > 0xFFFFFFFFULL) {
            throw std::out_of_range("instruction value out of 32-bit range");
        }
        uint32_t instructionNum= static_cast<uint32_t>(value);
        pInst_                 = new Instruction(instructionNum, hasSetABI_);
        if(pInst_ == nullptr) {
            throw std::invalid_argument("Failed to create instruction instance");
        }
        ret= pInst_->Decode();
        if(ret <= 0) {
            throw std::invalid_argument("Failed to decode instruction");
        }
        showInsResult(*pInst_);
    } catch(const std::invalid_argument &e) {
        showError("Invalid input format: please enter a valid hexadecimal number");
    } catch(const std::out_of_range &e) {
        showError("Value out of range: only 32-bit instructions supported (0x00000000 ~ 0xFFFFFFFF)");
    } catch(const std::exception &e) {
        showError(std::string("Decode failed: ") + e.what());
    } catch(...) {
        showError("Unknown error");
    }

    InsEntry_->grab_focus();
}

void RISCVInstructionWindow::showInsResult(Instruction &inst)
{
    auto buffer= InsTextView_->get_buffer();
    if(!buffer) {
        std::cerr << "Error: TextView buffer is null.";
        return;
    }

    hideAllTypeUI();
    InstFormatUI *pCurrUi= nullptr;
    InstFormat fmt       = inst.GetType().GetInstFormat();
    switch(fmt) {
    case InstFormat::R:
        rTypeUI_->show();
        pCurrUi= rTypeUI_;
        break;
    default:
        showError("err inst type!");
        break;
    }

    std::ostringstream oss;
    oss << "Format          = " << std::hex << inst.GetFormat() << '\n';
    oss << "Instruction set = " << inst.GetXLEN() << "\n";
    buffer->set_text(oss.str());

    if(pCurrUi) {
        UpdateDisplay(*pCurrUi, inst);
    }
}

void RISCVInstructionWindow::UpdateDisplay(InstFormatUI &instUi, Instruction &inst)
{
    instUi.UpdateDisplay(inst);
}

void RISCVInstructionWindow::showError(const std::string &message)
{
    auto buffer= InsTextView_->get_buffer();
    if(buffer) {
        buffer->set_text("Error: " + message);
    }
    std::cerr << "Error: " << message;
}

void RISCVInstructionWindow::setupSettingsPopover()
{
    pSettingsPopover_= Gtk::make_managed<Gtk::Popover>();
    pSettingsPopover_->set_has_arrow(true);
    pSettingsPopover_->set_parent(*pSettingsBtn_);

    auto pPopoverBox= Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 12);
    pPopoverBox->set_margin(12);

    // ABI row: label + switch
    auto pAbiRow  = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    auto pAbiLabel= Gtk::make_managed<Gtk::Label>("ABI");
    pAbiLabel->set_halign(Gtk::Align::START);
    pAbiRow->append(*pAbiLabel);

    pAbiSwitch_= Gtk::make_managed<Gtk::Switch>();
    pAbiSwitch_->set_active(hasSetABI_);
    pAbiSwitch_->set_halign(Gtk::Align::END);
    pAbiSwitch_->property_active().signal_changed().connect([this] {
        hasSetABI_= pAbiSwitch_->get_active();
    });
    pAbiRow->append(*pAbiSwitch_);
    pPopoverBox->append(*pAbiRow);

    // ISA row: label + dropdown
    auto pIsaRow  = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    auto pIsaLabel= Gtk::make_managed<Gtk::Label>("ISA");
    pIsaLabel->set_halign(Gtk::Align::START);
    pIsaRow->append(*pIsaLabel);

    const std::vector<Glib::ustring> isaOpts{"AUTO", "RV32I", "RV64I", "RV128I"};
    auto isaList= Gtk::StringList::create(isaOpts);
    pIsaDropDown_= Gtk::make_managed<Gtk::DropDown>(isaList);
    pIsaDropDown_->set_selected(0);
    pIsaDropDown_->set_hexpand(true);
    pIsaDropDown_->set_halign(Gtk::Align::END);
    pIsaRow->append(*pIsaDropDown_);
    pPopoverBox->append(*pIsaRow);

    pSettingsPopover_->set_child(*pPopoverBox);
    pSettingsBtn_->signal_clicked().connect([this] {
        pSettingsPopover_->popup();
    });
}

void RISCVInstructionWindow::loadCssFromFile(Gtk::Window &window)
{
    try {
        auto cssProvider= Gtk::CssProvider::create();
        auto cssFile    = Gio::File::create_for_path(CSS_FILE_PATH);
        cssProvider->load_from_file(cssFile);

        auto display= Gdk::Display::get_default();
        if(display) {
            Gtk::StyleContext::add_provider_for_display(
                display, cssProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        }
    } catch(const Gio::ResourceError &ex) {
        std::cerr << "Resource error: " << ex.what();
    } catch(const Glib::Error &ex) {
        std::cerr << "GLib error: " << ex.what();
    } catch(const std::exception &ex) {
        std::cerr << "Error: " << ex.what();
    }
}