#include <gtkmm.h>
#include <iostream>
#include <vector>
#include <map>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <sstream>
#include <iomanip>
#include <format>
#include "Core/InstTypeFactory.hh"
#include "ISA/InstFormat.hh"
#include "Core/Instruction.hh"
#include "Gui/InstFormatUI.hh"

class RISCVInstructionWindow: public Gtk::Window {
public:
    Gtk::Box *uiContainer_      = nullptr;
    Gtk::Entry *InsEntry_       = nullptr;
    Gtk::Button *InsButtonParse_= nullptr;
    Gtk::TextView *InsTextView_ = nullptr;
    Instruction *pInst_         = nullptr;
    InstFormatUI *rTypeUI_      = nullptr;
    InstFormatUI *iTypeUI_      = nullptr;
    InstFormatUI *sTypeUI_      = nullptr;
    InstFormatUI *bTypeUI_      = nullptr;
    InstFormatUI *uTypeUI_      = nullptr;
    InstFormatUI *jTypeUI_      = nullptr;

public:
    RISCVInstructionWindow();
    ~RISCVInstructionWindow()= default;

private:
    void initInstFormatUI();
    void onInsButtonParseClicked();
    void showInsResult(Instruction &inst);
    void updateDisplay(InstFormatUI &instUi, Instruction &inst);
    void showError(const std::string &message);
    void loadCssFromFile(Gtk::Window &window);
    void hideAllTypeUI();
};

RISCVInstructionWindow::RISCVInstructionWindow()
{
    set_title("RISC-V Instruction Encoder/Decoder");
    set_default_size(900, 300);

    uiContainer_= Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 10);
    uiContainer_->set_margin(15);

    InsEntry_= Gtk::make_managed<Gtk::Entry>();
    InsEntry_->set_placeholder_text("请输入指令");
    InsEntry_->signal_activate().connect(sigc::mem_fun(*this, &RISCVInstructionWindow::onInsButtonParseClicked));

    InsButtonParse_= Gtk::make_managed<Gtk::Button>("解析指令");
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
    set_child(*uiContainer_);
}

void RISCVInstructionWindow::hideAllTypeUI()
{
    rTypeUI_->hide();
    iTypeUI_->hide();
}

void RISCVInstructionWindow::initInstFormatUI()
{
    rTypeUI_= new InstFormatUI(createRTypeFormat());
    uiContainer_->append(*rTypeUI_);
    rTypeUI_->set_visible(false);
    iTypeUI_= new InstFormatUI(createITypeFormat());
    uiContainer_->append(*iTypeUI_);
    iTypeUI_->set_visible(false);
    sTypeUI_= new InstFormatUI(createSTypeFormat());
    uiContainer_->append(*sTypeUI_);
    sTypeUI_->set_visible(false);
    bTypeUI_= new InstFormatUI(createBTypeFormat());
    uiContainer_->append(*bTypeUI_);
    bTypeUI_->set_visible(false);
    uTypeUI_= new InstFormatUI(createUTypeFormat());
    uiContainer_->append(*uTypeUI_);
    uTypeUI_->set_visible(false);
    jTypeUI_= new InstFormatUI(createJTypeFormat());
    uiContainer_->append(*jTypeUI_);
    jTypeUI_->set_visible(false);

    uiContainer_->append(*InsTextView_);
}

void RISCVInstructionWindow::onInsButtonParseClicked()
{
    int ret    = 0;
    auto hexStr= InsEntry_->get_text();
    if(hexStr.empty()) {
        showError("请输入有效的十六进制指令");
        return;
    }
    std::cout << "Parsing hex: " << hexStr << std::endl;

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
        std::cerr << "Error: TextView buffer is null." << std::endl;
        return;
    }
    hideAllTypeUI();
    InstFormatUI *currUi= nullptr;
    InstFormat fmt_     = inst.GetType().GetInstFormat();
    switch(fmt_) {
    case InstFormat::R:
        rTypeUI_->show();
        currUi= rTypeUI_;
        break;
    case InstFormat::I:
        iTypeUI_->show();
        currUi= iTypeUI_;
        break;
    case InstFormat::S:
        sTypeUI_->show();
        currUi= sTypeUI_;
        break;
    case InstFormat::B:
        bTypeUI_->show();
        currUi= bTypeUI_;
        break;
    case InstFormat::U:
        uTypeUI_->show();
        currUi= uTypeUI_;
        break;
    case InstFormat::J:
        jTypeUI_->show();
        currUi= jTypeUI_;
        break;
    default:
        showError("err inst type!");
        break;
    }
    uint32_t val= static_cast<uint32_t>(inst);
    std::ostringstream oss;
    // oss << "Hexadecimal   = 0x"<< std::hex << std::setfill('0') << std::setw(8) << static_cast<uint32_t>(inst) << "\n";
    oss << "Hexadecimal   = 0x" << getHexStr(val) << "\n";
    oss << "Format          = " << std::hex << inst.GetFormat() << '\n';
    oss << "Instruction set = " << inst.GetXLEN() << "\n";
    buffer->set_text(oss.str());

    if(currUi) {
        updateDisplay(*currUi, inst);
    }
}

void RISCVInstructionWindow::updateDisplay(InstFormatUI &instUi, Instruction &inst)
{
    instUi.updateDisplay(inst);
}

void RISCVInstructionWindow::showError(const std::string &message)
{
    hideAllTypeUI();
    auto buffer= InsTextView_->get_buffer();
    if(buffer) {
        buffer->set_text("错误: " + message);
    }
    std::cerr << "Error: " << message << std::endl;
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
        std::cerr << "Resource error: " << ex.what() << std::endl;
    } catch(const Glib::Error &ex) {
        std::cerr << "GLib error: " << ex.what() << std::endl;
    } catch(const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
}