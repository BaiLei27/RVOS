#include <gtkmm.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>
#include "ISA/InstFormat.hh"

typedef struct InstFieldST {
    std::string name_;
    int startBit_;
    int endBit_;
    std::string tooltip_;
    std::string cssClass_ = "bit-field-container";
}InstFieldST;

typedef std::unordered_map<std::string, std::vector<std::string>> binaryRelationMap;
typedef std::unordered_map<std::string, std::vector<std::string>> asmRelationMap;

typedef struct InstFormatST {
    std::string typeName_;
    InstFormat fmt_;
    std::vector<std::string> instTypeV_;
    std::vector<InstFieldST> binaryV_;
    binaryRelationMap binaryFieldRelations_;
    asmRelationMap asmFieldRelations_;
}InstFormatST;

struct BinaryFieldWidgetPairST;
struct AsmMnemonicWidgetPairST;
typedef std::unordered_map<std::string, BinaryFieldWidgetPairST*> BinaryFieldWidgetSTMap;
typedef std::unordered_map<std::string, AsmMnemonicWidgetPairST*> AsmMnemonicWidgetSTMap;

typedef struct BinaryFieldWidgetPairST {
    Gtk::Box* box = nullptr;
    std::vector<Gtk::Label*> controlLabelsV_; 
    inline static int index_ = 0;
    Glib::RefPtr<Gtk::EventControllerMotion> pMotionController = nullptr;
    BinaryFieldWidgetPairST(std::vector<Gtk::Label*> &binaryLabelsV_,const InstFieldST &field);
    void highlight();
    void unhighlight();
    void highlight_in_mouse();
    void unhighlight_in_mouse();
    void setupController(const std::string& name, InstFormatST& instFmt,
         BinaryFieldWidgetSTMap& binaryFieldWidgets_, AsmMnemonicWidgetSTMap& asmFieldWidgets_);
    void updateControlLables(uint32_t instructionValue);
}BinaryFieldWidgetPairST;


typedef struct AsmMnemonicWidgetPairST {
    Gtk::Box* box = nullptr;
    Gtk::Label* label = nullptr; 
    Glib::RefPtr<Gtk::EventControllerMotion> pMotionController = nullptr;

    AsmMnemonicWidgetPairST(const std::string& name, Gtk::Box* parentAsmBox);
    void highlight();
    void unhighlight();
    void highlight_in_mouse();
    void unhighlight_in_mouse();

    void setupController(const std::string& name, InstFormatST& instFmt, BinaryFieldWidgetSTMap& binaryFieldWidgets_);

}AsmMnemonicWidgetPairST;

