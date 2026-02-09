#ifndef INSTCOMMST_HH
#define INSTCOMMST_HH
#include <gtkmm.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>
#include "ISA/InstFormat.hh"

typedef struct S_InstBinaryField_t {
    std::string name_;
    int startBit_;
    int endBit_;
    std::string tooltip_;
    std::string cssClass_= "bit-field-container";
} S_InstBinaryField_t;

class BinaryFieldWidget;
class AsmMnemonicWidget;

namespace InstCommST {
using binaryRelationMap= std::unordered_map<std::string, std::vector<std::string>>;
using asmRelationMap   = std::unordered_map<std::string, std::vector<std::string>>;

using BinaryFieldWidgetMap_u= std::unordered_map<std::string, BinaryFieldWidget *>;
using AsmMnemonicWidgetMap_u= std::unordered_map<std::string, AsmMnemonicWidget *>;
} // namespace InstCommST

typedef struct S_InstTypeRelationEntity_t {
    std::string typeName_;
    InstFormat fmt_;
    std::vector<std::string> instTypeV_;
    std::vector<S_InstBinaryField_t> binaryV_;
    InstCommST::binaryRelationMap binaryFieldRelations_;
    InstCommST::asmRelationMap asmFieldRelations;
} S_InstTypeRelationEntity_t;

#endif