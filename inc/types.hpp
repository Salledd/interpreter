#pragma once
#include <string>
#include <memory>

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
    
    // Метод для получения имени типа (строка)
    virtual std::string get_name() const = 0;
    
    // Метод для проверки совместимости типов
    virtual bool is_compatible_with(const Type& other) const = 0;
    
    virtual ~Type() = default;
};

std::shared_ptr<Type> make_type(const std::string& type_name);

class FundamentalType : public Type {
public:
    virtual ~FundamentalType() = default; 
};

class VoidType : public FundamentalType {
public:
    std::string get_name() const override { return "void"; }
    bool is_compatible_with(const Type& other) const override;
};

// class NullptrType : public FundamentalType {
// public:
//     std::string get_name() const override { return "nullptr"; }
//     bool is_compatible_with(const Type& other) const override;
// };

class ArithmeticType : public FundamentalType {
public:
    
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
    BoolType(){
        rank = TypeRank::Bool;
    }

    std::string get_name() const override { return "bool"; }
    bool is_compatible_with(const Type& other) const override;
};

class CharType : public IntegralType {
public:
    CharType(){
        rank = TypeRank::Char;
    }

    std::string get_name() const override { return "char"; }
    bool is_compatible_with(const Type& other) const override;
};

class IntType : public IntegralType {
public:
    IntType(){
        rank = TypeRank::Int;
    }
    std::string get_name() const override { return "int"; }
    bool is_compatible_with(const Type& other) const override;
};

class ShortIntType : public IntegralType {
public:
    ShortIntType(){
        rank = TypeRank::Short;
    }
    std::string get_name() const override { return "short"; }
    bool is_compatible_with(const Type& other) const override;
};

class LongIntType : public IntegralType {
    public:
        LongIntType(){
            rank = TypeRank::Long;
        }
        std::string get_name() const override { return "long"; }
        bool is_compatible_with(const Type& other) const override;
    };


class FloatingType : public ArithmeticType {
public:

    virtual ~FloatingType() = default; 
};

class FloatType : public FloatingType {
public:
    FloatType(){
        rank = TypeRank::Float;
    }
    std::string get_name() const override { return "float"; }
    bool is_compatible_with(const Type& other) const override;
};

class DoubleType : public FloatingType {
public:
    DoubleType(){
        rank = TypeRank::Double;
    }
    std::string get_name() const override { return "double"; }
    bool is_compatible_with(const Type& other) const override;
};

class LongDoubleType : public FloatingType {
public:
    LongDoubleType(){
        rank = TypeRank::LongDouble;
    }
    std::string get_name() const override { return "long double"; }
    bool is_compatible_with(const Type& other) const override;
};

class CompositeType : public Type {
public:
    virtual ~CompositeType() = default; 
};

class FunctionType : public CompositeType {
public:
    std::string get_name() const override { return "function"; }
    bool is_compatible_with(const Type& other) const override;
};

class ArrayType : public CompositeType {
    public:
        std::string get_name() const override { return "array"; }
        bool is_compatible_with(const Type& other) const override;
};

// Record Types (struct, class, union)
class RecordType : public CompositeType {
    public:
    virtual ~RecordType() = default; 
};

class StructType : public RecordType {
    public:
        std::string name;
        StructType(std::string name) : name(name) {}
        std::string get_name() const override { return "struct"; }
        bool is_compatible_with(const Type& other) const override;
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

