#pragma once
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include "modifiers.hpp"

enum class TypeRank {
    Bool,
    Char,
    Short,
    Int,
    Long,
    Float,
    Double,
    LongDouble
};

class Type {
    public:
    Type() = default;
    Type(bool is_const) : _is_const(is_const) {}
    // Метод для получения имени типа (строка)
    virtual std::string get_name() const = 0;
    
    // Метод для проверки совместимости типов
    virtual bool is_compatible_with(const std::shared_ptr<Type> other) const = 0;
    
    bool is_const() const { return _is_const; }

    virtual ~Type() = default;

protected:
    bool _is_const;
};

typedef std::shared_ptr<Type> TypePtr;

std::shared_ptr<Type> make_type(const std::string& type_name, bool is_const = false);
std::shared_ptr<Type> make_type_arr(const std::string& type_name, bool is_const = false);
std::shared_ptr<Type> make_type_struct(const std::string& type_name, std::vector<std::pair<std::pair<Modifiers, TypePtr>, std::string>> members, bool is_const = false);

class FundamentalType : public virtual Type {
public:
    virtual ~FundamentalType() = default; 
};

class VoidType : public FundamentalType {
public:
    std::string get_name() const override { return "void"; }
    bool is_compatible_with(const TypePtr other) const override;
};

// class NullptrType : public FundamentalType {
// public:
//     std::string get_name() const override { return "nullptr"; }
//     bool is_compatible_with(const Type& other) const override;
// };

class ArithmeticType : public FundamentalType {
public:
    bool is_compatible_with(TypePtr other) const override;
    
    virtual ~ArithmeticType() = default; 
protected:
    TypeRank rank;
};

class IntegralType : public ArithmeticType {
public:

    virtual ~IntegralType() = default; 
};

class BoolType : public IntegralType {
public:
    BoolType(bool is_const) : Type(is_const) {
        rank = TypeRank::Bool;
    }

    std::string get_name() const override { return "bool"; }
};

class CharType : public IntegralType {
public:
    CharType(bool is_const) : Type(is_const) {
        rank = TypeRank::Char;
    }

    std::string get_name() const override { return "char"; }
};

class IntType : public IntegralType {
public:
    IntType(bool is_const) : Type(is_const) {
        rank = TypeRank::Int;
    }
    std::string get_name() const override { return "int"; }
};

class ShortIntType : public IntegralType {
public:
    ShortIntType(bool is_const) : Type(is_const) {
        rank = TypeRank::Short;
    }
    std::string get_name() const override { return "short"; }
};

class LongIntType : public IntegralType {
    public:
        LongIntType(bool is_const) : Type(is_const) {
            rank = TypeRank::Long;
        }
        std::string get_name() const override { return "long"; }
    };


class FloatingType : public ArithmeticType {
public:

    virtual ~FloatingType() = default; 
};

class FloatType : public FloatingType {
public:
    FloatType(bool is_const) : Type(is_const) {
        rank = TypeRank::Float;
    }
    std::string get_name() const override { return "float"; }
};

class DoubleType : public FloatingType {
public:
    DoubleType(bool is_const) : Type(is_const) {
        rank = TypeRank::Double;
    }
    std::string get_name() const override { return "double"; }
};

class LongDoubleType : public FloatingType {
public:
    LongDoubleType(bool is_const) : Type(is_const) {
        rank = TypeRank::LongDouble;
    }
    std::string get_name() const override { return "ldouble"; }
};

class CompositeType : public virtual Type {
public:
    virtual ~CompositeType() = default; 
};

// class FunctionType : public CompositeType {
// public:
//     std::string get_name() const override { return "function"; }
//     bool is_compatible_with(const TypePtr other) const override;
// };

class ArrayType : public CompositeType {
    public:
        ArrayType(TypePtr element_type, bool is_const) : element_type(element_type), Type(is_const) {}

        TypePtr element_type;

        std::string get_name() const override { return "array"; }
        bool is_compatible_with(const TypePtr other) const override;
};

// Record Types (struct, class, union)
class RecordType : public CompositeType {
    public:
    virtual ~RecordType() = default; 
};

class StructType : public RecordType {
    public:
        StructType(const std::string& name, bool is_const) : name(name) , Type(is_const) {}
        StructType(const std::string& name, const std::vector<std::pair<std::pair<Modifiers, TypePtr>, std::string>>& members) :
            name(name), members(members) {}
        
        std::string name;
        std::vector<std::pair<std::pair<Modifiers, TypePtr>, std::string>> members; // name, type
        
        std::string get_name() const override { return "struct"; }

        std::optional<std::pair<std::pair<Modifiers, TypePtr>, std::string>> lookup_member(const std::string& member_name) const;

        bool is_compatible_with(const TypePtr other) const override;

};

// class ClassType : public RecordType {
// public:
//     std::string get_name() const override { return "class"; }
//     bool is_compatible_with(const Type& other) const override {
//         return dynamic_cast<const ClassType*>(&other) != nullptr;
//     }
// };

// class UnionType : public RecordType {
// public:
//     std::string get_name() const override { return "union"; }
//     bool is_compatible_with(const Type& other) const override {
//         return dynamic_cast<const UnionType*>(&other) != nullptr;
//     }
// };

// class EnumType : public CompositeType {
// public:
//     virtual ~EnumType() = default; 
// };

// class UnscopedEnumType : public EnumType {
// public:
//     std::string get_name() const override { return "unscoped enum"; }
//     bool is_compatible_with(const Type& other) const override {
//         return dynamic_cast<const UnscopedEnumType*>(&other) != nullptr;
//     }
// };

// class ScopedEnumType : public EnumType {
// public:
//     std::string get_name() const override { return "scoped enum"; }
//     bool is_compatible_with(const Type& other) const override {
//         return dynamic_cast<const ScopedEnumType*>(&other) != nullptr;
//     }
// };

// class PointerType : public CompositeType {
// public:
//     std::string get_name() const override { return "pointer"; }
//     bool is_compatible_with(const Type& other) const override {
//         return dynamic_cast<const PointerType*>(&other) != nullptr;
//     }
// };

// class ReferenceType : public CompositeType {
// public:
//     virtual ~ReferenceType() = default; 
// };

// class LvalueReferenceType : public ReferenceType {
// public:
//     std::string get_name() const override { return "lvalue reference"; }
//     bool is_compatible_with(const Type& other) const override {
//         return dynamic_cast<const LvalueReferenceType*>(&other) != nullptr;
//     }
// };

// class RvalueReferenceType : public ReferenceType {
// public:
//     std::string get_name() const override { return "rvalue reference"; }
//     bool is_compatible_with(const Type& other) const override {
//         return dynamic_cast<const RvalueReferenceType*>(&other) != nullptr;
//     }
// };

