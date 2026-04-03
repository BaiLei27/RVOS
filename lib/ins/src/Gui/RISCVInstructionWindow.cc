#include <gtkmm.h>
#include <cctype>
#include <exception>
#include <format>
#include <iomanip>
#include <iostream>
#include <map>
#include <ranges>
#include <sstream>
#include <string_view>
#include <cassert>
#include <cstdlib>
#include <ctime>
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

    InsEntry_->set_placeholder_text("Hex (0x33), binary (0b110011 or 32 bits), or assembly (add x0,x0,x0)");
    InsEntry_->set_hexpand(true);
    InsEntry_->signal_activate().connect(sigc::mem_fun(*this, &RISCVInstructionWindow::onInsButtonParseClicked));

    pEntryRow_= Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 4);
    pEntryRow_->append(*InsEntry_);

    pSettingsBtn_= Gtk::make_managed<Gtk::Button>();
    pSettingsBtn_->set_icon_name("view-more-symbolic");
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
        if(InsEntry_ && InsEntry_->get_buffer()) {
            InsEntry_->get_buffer()->set_text(content);
        }
    });
    uiContainer_->append(*rTypeUI_);

    iTypeUI_= new InstFormatUI(createITypeFormat());
    iTypeUI_->set_visible(false);
    iTypeUI_->signal_put_to_output.connect([this](const std::string &content) {
        if(InsEntry_ && InsEntry_->get_buffer()) {
            InsEntry_->get_buffer()->set_text(content);
        }
    });
    uiContainer_->append(*iTypeUI_);

    jTypeUI_= new InstFormatUI(createJTypeFormat());
    jTypeUI_->set_visible(false);
    jTypeUI_->signal_put_to_output.connect([this](const std::string &content) {
        if(InsEntry_ && InsEntry_->get_buffer()) {
            InsEntry_->get_buffer()->set_text(content);
        }
    });
    uiContainer_->append(*jTypeUI_);

    uTypeUI_= new InstFormatUI(createUTypeFormat());
    uTypeUI_->set_visible(false);
    uTypeUI_->signal_put_to_output.connect([this](const std::string &content) {
        if(InsEntry_ && InsEntry_->get_buffer()) {
            InsEntry_->get_buffer()->set_text(content);
        }
    });
    uiContainer_->append(*uTypeUI_);

    sTypeUI_= new InstFormatUI(createSTypeFormat());
    sTypeUI_->set_visible(false);
    sTypeUI_->signal_put_to_output.connect([this](const std::string &content) {
        if(InsEntry_ && InsEntry_->get_buffer()) {
            InsEntry_->get_buffer()->set_text(content);
        }
    });
    uiContainer_->append(*sTypeUI_);

    bTypeUI_= new InstFormatUI(createBTypeFormat());
    bTypeUI_->set_visible(false);
    bTypeUI_->signal_put_to_output.connect([this](const std::string &content) {
        if(InsEntry_ && InsEntry_->get_buffer()) {
            InsEntry_->get_buffer()->set_text(content);
        }
    });
    uiContainer_->append(*bTypeUI_);
}

void RISCVInstructionWindow::hideAllTypeUI()
{
    rTypeUI_->hide();
    iTypeUI_->hide();
    jTypeUI_->hide();
    uTypeUI_->hide();
    sTypeUI_->hide();
    bTypeUI_->hide();
}

static bool looksLikeHex(std::string_view s)
{
    if(s.empty()) return false;
    size_t start= 0;
    if(s.size() >= 2 && (s.substr(0, 2) == "0x" || s.substr(0, 2) == "0X")) {
        start= 2;
    }
    if(start >= s.size()) return false;
    return std::ranges::all_of(s.substr(start), [](unsigned char c) { return std::isxdigit(c); });
}

static bool looksLikeBinary(std::string_view s)
{
    if(s.empty()) return false;
    std::string_view digits;
    if(s.size() >= 2 && (s.substr(0, 2) == "0b" || s.substr(0, 2) == "0B")) {
        digits= s.substr(2);
    } else {
        // No prefix: treat as binary only if all 0/1 and length >= 1 (avoids conflict with short hex like "ff", "33")
        digits= s;
        if(digits.size() < 2) return false;
    }
    if(digits.empty() || digits.size() > 32) return false;
    return std::ranges::all_of(digits, [](unsigned char c) { return c == '0' || c == '1'; });
}

static std::string_view getBinaryDigits(std::string_view s)
{
    if(s.size() >= 2 && (s.substr(0, 2) == "0b" || s.substr(0, 2) == "0B")) {
        return s.substr(2);
    }
    return s;
}

void RISCVInstructionWindow::onInsButtonParseClicked()
{
    int ret   = 0;
    auto text= InsEntry_->get_text();
    if(text.empty()) {
        showError("Please enter hex (0x33), binary (0b110011), or assembly (e.g. add x0,x0,x0)");
        return;
    }

    std::string inputStr(text);

    try {
        // Check binary before hex: "000...00110011" (all 0/1, len>=8) is binary; "0x33" stays hex
        if(looksLikeBinary(inputStr)) {
            std::string cleanStr(getBinaryDigits(inputStr));
            uint64_t value= std::stoull(cleanStr, nullptr, 2);
            if(value > 0xFFFFFFFFULL) {
                throw std::out_of_range("instruction value out of 32-bit range");
            }
            uint32_t instructionNum= static_cast<uint32_t>(value);
            pInst_                 = new Instruction(instructionNum, hasSetABI_);
        } else if(looksLikeHex(inputStr)) {
            std::string cleanStr= inputStr;
            if(cleanStr.size() >= 2 && (cleanStr.substr(0, 2) == "0x" || cleanStr.substr(0, 2) == "0X")) {
                cleanStr= cleanStr.substr(2);
            }
            uint64_t value= std::stoull(cleanStr, nullptr, 16);
            if(value > 0xFFFFFFFFULL) {
                throw std::out_of_range("instruction value out of 32-bit range");
            }
            uint32_t instructionNum= static_cast<uint32_t>(value);
            pInst_                 = new Instruction(instructionNum, hasSetABI_);
        } else {
            pInst_= new Instruction(inputStr, hasSetABI_);
        }

        if(pInst_ == nullptr) {
            throw std::invalid_argument("Failed to create instruction instance");
        }
        ret= pInst_->Decode();
        if(ret <= 0) {
            throw std::invalid_argument("Failed to decode instruction");
        }
        showInsResult(*pInst_);
    } catch(const std::invalid_argument &e) {
        showError(std::string("Invalid input: ") + e.what());
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
    case InstFormat::I:
        iTypeUI_->show();
        pCurrUi= iTypeUI_;
        break;
    case InstFormat::J:
        jTypeUI_->show();
        pCurrUi= jTypeUI_;
        break;
    case InstFormat::U:
        uTypeUI_->show();
        pCurrUi= uTypeUI_;
        break;
    case InstFormat::S:
        sTypeUI_->show();
        pCurrUi= sTypeUI_;
        break;
    case InstFormat::B:
        bTypeUI_->show();
        pCurrUi= bTypeUI_;
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

void RISCVInstructionWindow::refreshAssemblyForAbiChange()
{
    if(!pInst_) {
        return;
    }
    uint32_t val= static_cast<uint32_t>(*pInst_);
    delete pInst_;
    pInst_= new Instruction(val, hasSetABI_);
    if(pInst_->Decode()) {
        showInsResult(*pInst_);
    }
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
        refreshAssemblyForAbiChange();
    });
    pAbiRow->append(*pAbiSwitch_);
    pPopoverBox->append(*pAbiRow);

    // ISA row: label + dropdown
    auto pIsaRow  = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    auto pIsaLabel= Gtk::make_managed<Gtk::Label>("ISA");
    pIsaLabel->set_halign(Gtk::Align::START);
    pIsaRow->append(*pIsaLabel);

    // MenuButton + buttons: each click closes popovers (Gtk::DropDown does not
    // emit selection change when re-picking the same row, so its list can stay open).
    pIsaMenuBtn_= Gtk::make_managed<Gtk::MenuButton>();
    pIsaMenuBtn_->set_label("AUTO");
    pIsaMenuBtn_->set_hexpand(true);
    pIsaMenuBtn_->set_halign(Gtk::Align::END);

    auto pIsaPopover= Gtk::make_managed<Gtk::Popover>();
    auto pIsaChoices= Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 0);
    pIsaChoices->set_margin(6);

    const std::vector<std::pair<Glib::ustring, int>> isaOpts{
        {"AUTO",  0},
        {"RV32I", 1},
        {"RV64I", 2},
        {"RV128I", 3},
    };

    for(const auto &[label, idx]: isaOpts) {
        auto pOptBtn= Gtk::make_managed<Gtk::Button>(label);
        pOptBtn->signal_clicked().connect([this, pIsaPopover, label, idx] {
            selectedIsaIndex_= idx;
            if(pIsaMenuBtn_) {
                pIsaMenuBtn_->set_label(label);
            }
            pIsaPopover->popdown();
            if(pSettingsPopover_) {
                pSettingsPopover_->popdown();
            }
        });
        pIsaChoices->append(*pOptBtn);
    }

    pIsaPopover->set_child(*pIsaChoices);
    pIsaMenuBtn_->set_popover(*pIsaPopover);
    pIsaRow->append(*pIsaMenuBtn_);
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