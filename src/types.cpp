#include "types.hpp"
#include <iostream>
#include <memory>
#include <string>


std::shared_ptr<Type> make_type(const std::string& type_name, bool is_const){
    if (type_name == "short") return std::make_shared<ShortIntType>(is_const);
    if (type_name == "int") return std::make_shared<IntType>(is_const);
    if (type_name == "long") return std::make_shared<LongIntType>(is_const);
    if (type_name == "char") return std::make_shared<CharType>(is_const);
    if (type_name == "bool") return std::make_shared<BoolType>(is_const);
    if (type_name == "double") return std::make_shared<DoubleType>(is_const);
    if (type_name == "ldouble") return std::make_shared<LongDoubleType>(is_const);
    if (type_name == "float") return std::make_shared<FloatType>(is_const);
    if (type_name == "void") return std::make_shared<VoidType>();
    return std::make_shared<StructType>(type_name, is_const);
}

std::shared_ptr<Type> make_type_arr(const std::string& type_name, bool is_const){
    return std::make_shared<ArrayType>(make_type(type_name, is_const));
}

std::shared_ptr<Type> make_type_struct(const std::string& type_name, const std::vector<std::pair<std::pair<Modifiers, TypePtr>, std::string>>& members){
    return std::make_shared<StructType>(type_name, members);
}

bool VoidType::is_compatible_with(const TypePtr other) const {
    return std::dynamic_pointer_cast<VoidType>(other) != nullptr;
}
bool ArithmeticType::is_compatible_with(const TypePtr other) const {
    return std::dynamic_pointer_cast<ArithmeticType>(other) != nullptr;
}

bool StructType::is_compatible_with(const TypePtr other) const {
    auto other_ptr = std::dynamic_pointer_cast<StructType>(other);
    if (other_ptr) {
        return this->name == other_ptr->name;
    }
    return false;
}

bool ArrayType::is_compatible_with(const TypePtr other) const {
    auto other_ptr = std::dynamic_pointer_cast<ArrayType>(other);
    if(other_ptr != nullptr){
        return this->element_type->is_compatible_with(other_ptr->element_type);
    }
}


std::optional<std::pair<std::pair<Modifiers, TypePtr>, std::string>> StructType::lookup_member(const std::string& member_name) const {
    for (const auto& member : members) {
        if (member.second == member_name) {
            return member; // Возвращаем тип члена
        }
    }
    return std::nullopt; // Член не найден
}


