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

RISCVInstructionWindow::RISCVInstructionWindow() : 
    InsEntry_(Gtk::make_managed<Gtk::Entry>()) , 
    InsButtonParse_(Gtk::make_managed<Gtk::Button>("解析指令")) ,
    uiContainer_(Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 10))
{
    set_title("RISC-V Instruction Encoder/Decoder");
    set_default_size(900, 300);

    uiContainer_->set_margin(15);

    InsEntry_->set_placeholder_text("请输入指令");
    InsEntry_->signal_activate().connect(sigc::mem_fun(*this, &RISCVInstructionWindow::onInsButtonParseClicked));

    InsButtonParse_->signal_clicked().connect(sigc::mem_fun(*this, &RISCVInstructionWindow::onInsButtonParseClicked));

    uiContainer_->append(*InsEntry_);
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
        showError("请输入有效的十六进制指令");
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
        pInst_                 = new Instruction(instructionNum);
        if(pInst_ == nullptr) {
            throw std::invalid_argument("无法创建指令实例");
        }
        ret= pInst_->Decode();
        if(ret <= 0) {
            throw std::invalid_argument("无法解码指令");
        }
        showInsResult(*pInst_);
    } catch(const std::invalid_argument &e) {
        showError("输入格式错误：请输入有效的十六进制数");
    } catch(const std::out_of_range &e) {
        showError("数值超出范围：仅支持32位指令（0x00000000 ~ 0xFFFFFFFF）");
    } catch(const std::exception &e) {
        showError(std::string("解码失败：") + e.what());
    } catch(...) {
        showError("未知错误");
    }

    InsEntry_->grab_focus();
}

inline std::string getHexStr(uint32_t val)
{
    return std::format("{:08x}", val);
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
    uint32_t val = static_cast<uint32_t>(inst);
    std::ostringstream oss;
    oss << "Hexadecimal   = 0x" << getHexStr(val) << "\n";
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
        buffer->set_text("错误: " + message);
    }
    std::cerr << "Error: " << message;
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