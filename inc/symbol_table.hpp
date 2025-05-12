#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <optional>
#include "ast.hpp"

struct TypedefInfo {
    TypedefInfo(const Modifiers& mods, const TypePtr type)
        : mods(mods), type(type){}
    Modifiers mods;
    TypePtr type; 
};

struct SymbolInfo {
    SymbolInfo(const Modifiers& mods, const TypePtr type)
        : mods(mods), type(type){}

    Modifiers mods;
    TypePtr type;
};

struct FunctionInfo {
    FunctionInfo(const Modifiers& return_mods, const TypePtr return_type, const std::vector<std::pair<std::pair<Modifiers, TypePtr>, std::string>>& params) :
        return_mods(return_mods), return_type(return_type), params(params) {}
    Modifiers return_mods;
    TypePtr return_type; // Тип возвращаемого значения
    std::vector<std::pair<std::pair<Modifiers, TypePtr>, std::string>> params; // Параметры функции (модификаторы,тип и имя)
};

class SymbolTable {
public:
    SymbolTable() = default;

    void declare(const std::string& name, const SymbolInfo& info) {
        if (_table.find(name) != _table.end()) {
            std::cout << "Symbol '" + name + "' already declared." << std::endl;
            //throw std::runtime_error("Symbol '" + name + "' already declared.");
        }
        _table[name] = info;
    }

    void declare(const std::string& name, const FunctionInfo& info) {
        if (_functions.find(name) != _functions.end()) {
            std::cout << "Function '" + name + "' is already declared." << std::endl;
            //throw std::runtime_error("Function '" + name + "' is already declared.");
        }
        _functions[name] = info;
    }

    void declare(const std::string& name, const TypedefInfo& info) {
        if (_typedefs.find(name) != _typedefs.end()) {
            std::cout << "Typedef '" + name + "' is already declared." << std::endl;
            //throw std::runtime_error("Function '" + name + "' is already declared.");
        }
        _typedefs[name] = info;
    }

    std::optional<SymbolInfo> lookup_var(const std::string& name) const {
        if (auto it = _table.find(name); it != _table.end()) {
            return it->second;
        }
        return std::nullopt; // Сигнал, что переменная не найдена
    }

    std::optional<FunctionInfo> lookup_func(const std::string& name) const {
        if (auto it = _functions.find(name); it != _functions.end()) {
            return it->second;
        }
        return std::nullopt; // Функция не найдена
    }

    std::optional<TypedefInfo> lookup_typedef(const std::string& name) const {
        if (auto it = _typedefs.find(name); it != _typedefs.end()) {
            return it->second;
        }
        return std::nullopt; // Тип не найден
    }
 

private:
    std::unordered_map<std::string, SymbolInfo> _table;
    std::unordered_map<std::string, TypedefInfo> _typedefs;
    std::unordered_map<std::string, FunctionInfo> _functions;
};

