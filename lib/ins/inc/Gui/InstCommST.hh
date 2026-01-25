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
    std::string cssClass_ = "bit-field-container";
} S_InstBinaryField_t;

typedef std::unordered_map<std::string, std::vector<std::string>> binaryRelationMap;
typedef std::unordered_map<std::string, std::vector<std::string>> asmRelationMap;

typedef struct S_InstTypeRelationEntity_t {
    std::string typeName_;
    InstFormat fmt_;
    std::vector<std::string> instTypeV_;
    std::vector<S_InstBinaryField_t> binaryV_;
    binaryRelationMap binaryFieldRelations_;
    asmRelationMap asmFieldRelations_;
}S_InstTypeRelationEntity_t;

#endif