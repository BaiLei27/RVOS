#include <gtkmm.h>
#include <iostream>
#include <vector>
#include <map>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <sstream>
#include <iomanip> 
#include "Core/InstTypeFactory.hh"
#include "ISA/InstFormat.hh"
#include "Core/Instruction.hh"

class RISCVInstructionWindow : public Gtk::Window {
public:
    Gtk::Box* uiContainer_ = nullptr;
    Gtk::Entry* InsEntry_ = nullptr;
    Gtk::Button* InsButtonParse_ = nullptr;
    Gtk::TextView* InsTextView_ = nullptr;
    Instruction *pInst_ = nullptr;
    RISCVInstructionWindow();
private:
    void onInsButtonParseClicked();
    void showInsResult(Instruction& inst);
    void showError(const std::string& message);
};

RISCVInstructionWindow::RISCVInstructionWindow() {
    set_title("RISC-V Instruction Encoder/Decoder");
    set_default_size(900, 300);
    
    uiContainer_ = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 10);
    uiContainer_->set_margin(15);
    
    InsEntry_ = Gtk::make_managed<Gtk::Entry>();
    InsEntry_->set_placeholder_text("请输入指令");
    InsEntry_->signal_activate().connect(sigc::mem_fun(*this, &RISCVInstructionWindow::onInsButtonParseClicked));
    
    InsButtonParse_ = Gtk::make_managed<Gtk::Button>("解析指令");
    InsButtonParse_->signal_clicked().connect(sigc::mem_fun(*this, &RISCVInstructionWindow::onInsButtonParseClicked));

    uiContainer_->append(*InsEntry_);
    uiContainer_->append(*InsButtonParse_);
    
    InsTextView_ = Gtk::make_managed<Gtk::TextView>();
    InsTextView_->set_margin(10);
    InsTextView_->set_vexpand(true);
    InsTextView_->set_hexpand(true);
    InsTextView_->set_editable(false);
    InsTextView_->set_cursor_visible(false);

    set_child(*uiContainer_);
}

void RISCVInstructionWindow::onInsButtonParseClicked() {
    auto hexStr = InsEntry_->get_text();
    if (hexStr.empty()) {
        showError("请输入有效的十六进制指令");
        return;
    }
    std::cout << "Parsing hex: " << hexStr << std::endl;

    try {
        std::string cleanStr = hexStr;
        if (cleanStr.size() >= 2 && 
            (cleanStr.substr(0, 2) == "0x" || cleanStr.substr(0, 2) == "0X")) {
            cleanStr = cleanStr.substr(2);
        }

        uint32_t instructionNum = std::stoul(cleanStr, nullptr, 16);
        pInst_ = new Instruction(instructionNum);
        if(pInst_ == nullptr) {
            throw std::invalid_argument("无法创建指令实例");
        }
        pInst_->Decode();

        showInsResult(*pInst_);

    } catch (const std::invalid_argument& e) {
        showError("输入格式错误：请输入有效的十六进制数");
    } catch (const std::out_of_range& e) {
        showError("数值超出范围：仅支持32位指令（0x00000000 ~ 0xFFFFFFFF）");
    } catch (const std::exception& e) {
        showError(std::string("解码失败：") + e.what());
    } catch (...) {
        showError("未知错误");
    }

    InsEntry_->grab_focus();
}

void RISCVInstructionWindow::showInsResult(Instruction& inst) {
    auto buffer = InsTextView_->get_buffer();
    if (!buffer) {
        std::cerr << "Error: TextView buffer is null." << std::endl;
        return;
    }
    InstFormat fmt_ = inst.GetType().GetInstFormat();

    std::ostringstream oss;
    oss << "Hexadecimal   = 0x"<< std::hex << std::setfill('0') << std::setw(8) << static_cast<uint32_t>(inst) << "\n";
    oss << "Format          = "<< std::hex << inst.GetFormat() << '\n';
    oss << "Instruction set = " << inst.GetXLEN() << "\n";
    buffer->set_text(oss.str());


    uiContainer_->append(*InsTextView_);
}

void RISCVInstructionWindow::showError(const std::string& message) {
    auto buffer = InsTextView_->get_buffer();
    if (buffer) {
        buffer->set_text("错误: " + message);
    }
    std::cerr << "Error: " << message << std::endl;
}
