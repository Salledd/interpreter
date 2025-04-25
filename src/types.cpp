#include "types.hpp"
#include <iostream>
#include <memory>
#include <string>


std::shared_ptr<Type> make_type(const std::string& type_name){
    if (type_name == "short") return std::make_shared<ShortIntType>();
    else if (type_name == "int") return std::make_shared<IntType>();
    else if (type_name == "long") return std::make_shared<LongIntType>();
    else if (type_name == "char") return std::make_shared<CharType>();
    else if (type_name == "bool") return std::make_shared<BoolType>();
    else if (type_name == "double") return std::make_shared<DoubleType>();
    else if (type_name == "ldouble") return std::make_shared<LongDoubleType>();
    else if (type_name == "float") return std::make_shared<FloatType>();
    return std::make_shared<StructType>(type_name);
}

bool VoidType::is_compatible_with(const Type& other) const {
    return 0;
}
bool CharType::is_compatible_with(const Type& other) const {
    return dynamic_cast<const CharType*>(&other) != nullptr || dynamic_cast<const IntType*>(&other) != nullptr;
}
bool BoolType::is_compatible_with(const Type& other) const {
    return dynamic_cast<const BoolType*>(&other) != nullptr || dynamic_cast<const IntType*>(&other) != nullptr;
}
bool IntType::is_compatible_with(const Type& other) const {
    return dynamic_cast<const IntType*>(&other) != nullptr || dynamic_cast<const ShortIntType*>(&other) != nullptr ||
           dynamic_cast<const LongIntType*>(&other) != nullptr || dynamic_cast<const CharType*>(&other) != nullptr ||
           dynamic_cast<const BoolType*>(&other) != nullptr;
}
bool ShortIntType::is_compatible_with(const Type& other) const {
    return dynamic_cast<const ShortIntType*>(&other) != nullptr || dynamic_cast<const IntType*>(&other) != nullptr ||
           dynamic_cast<const LongIntType*>(&other) != nullptr || dynamic_cast<const CharType*>(&other) != nullptr ||
           dynamic_cast<const BoolType*>(&other) != nullptr;
}
bool LongIntType::is_compatible_with(const Type& other) const {
    return dynamic_cast<const LongIntType*>(&other) != nullptr || dynamic_cast<const IntType*>(&other) != nullptr ||
           dynamic_cast<const ShortIntType*>(&other) != nullptr || dynamic_cast<const CharType*>(&other) != nullptr ||
           dynamic_cast<const BoolType*>(&other) != nullptr;
}
bool DoubleType::is_compatible_with(const Type& other) const {
    return dynamic_cast<const DoubleType*>(&other) != nullptr || dynamic_cast<const LongDoubleType*>(&other) != nullptr ||
           dynamic_cast<const FloatType*>(&other) != nullptr || dynamic_cast<const IntType*>(&other) != nullptr ||
           dynamic_cast<const ShortIntType*>(&other) != nullptr || dynamic_cast<const CharType*>(&other) != nullptr ||
           dynamic_cast<const BoolType*>(&other) != nullptr;
}
bool LongDoubleType::is_compatible_with(const Type& other) const {
    return dynamic_cast<const LongDoubleType*>(&other) != nullptr || dynamic_cast<const DoubleType*>(&other) != nullptr ||
           dynamic_cast<const FloatType*>(&other) != nullptr || dynamic_cast<const IntType*>(&other) != nullptr ||
           dynamic_cast<const ShortIntType*>(&other) != nullptr || dynamic_cast<const CharType*>(&other) != nullptr ||
           dynamic_cast<const BoolType*>(&other) != nullptr;
}
bool FloatType::is_compatible_with(const Type& other) const {
    return dynamic_cast<const FloatType*>(&other) != nullptr || dynamic_cast<const DoubleType*>(&other) != nullptr ||
           dynamic_cast<const LongDoubleType*>(&other) != nullptr || dynamic_cast<const IntType*>(&other) != nullptr ||
           dynamic_cast<const ShortIntType*>(&other) != nullptr || dynamic_cast<const CharType*>(&other) != nullptr ||
           dynamic_cast<const BoolType*>(&other) != nullptr;
}
bool StructType::is_compatible_with(const Type& other) const {
    return dynamic_cast<const StructType*>(&other) != nullptr;
}
bool ArrayType::is_compatible_with(const Type& other) const {
    return dynamic_cast<const ArrayType*>(&other) != nullptr;
}
bool FunctionType::is_compatible_with(const Type& other) const {
    return 0;
}


