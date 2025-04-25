#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <optional>
#include "ast.hpp"


struct SymbolInfo {
    SymbolInfo(const std::set<Modifier>& mods, const std::string& type)
        : mods(mods), type(type){}

    std::set<Modifier> mods;
    std::string type;
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

    std::optional<SymbolInfo> lookup(const std::string& name) const {
        if (auto it = _table.find(name); it != _table.end()) {
            return it->second;
        }
        return std::nullopt; // Сигнал, что переменная не найдена
    }

private:
    std::unordered_map<std::string, SymbolInfo> _table;

};