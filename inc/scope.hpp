#pragma once
#include <memory>
#include "symbol_table.hpp"


class Scope {
    public:
    Scope(std::shared_ptr<Scope> parent = nullptr) : _parent(parent) {}

    std::shared_ptr<Scope> enterScope() {
        return std::make_shared<Scope>(std::make_shared<Scope>(*this));
    }

    std::shared_ptr<Scope> exitScope() {
        return _parent ? _parent : nullptr;
    }

    void declare(const std::string& name, const SymbolInfo& info) {
        _symbol_table.declare(name, info);
    }

    void declare(const std::string& name, const FunctionInfo& info) {
        _symbol_table.declare(name, info);
    }

    void declare(const std::string& name, const TypedefInfo& info) {
        _symbol_table.declare(name, info);
    }

    std::optional<SymbolInfo> lookup_var(const std::string& name) const {
        return _symbol_table.lookup_var(name);
    }

    std::optional<SymbolInfo> exists_var(const std::string& name) const {
        if (auto info = _symbol_table.lookup_var(name)) {
            return info;
        }
        // Если не найдено, ищем в родительской области
        return _parent ? _parent->exists_var(name) : std::nullopt;
    }

    std::optional<FunctionInfo> lookup_func(const std::string& name) const {
        return _symbol_table.lookup_func(name);
    }
    
    std::optional<FunctionInfo> exists_func(const std::string& name) const {
        if (auto info = _symbol_table.lookup_func(name)) {
            return info;
        }
        // Если не найдено, ищем в родительской области
        return _parent ? _parent->exists_func(name) : std::nullopt;
    }

    std::optional<TypedefInfo> exists_type(const std::string& name) const {
        if (auto info = _symbol_table.lookup_typedef(name)) {
            return info;
        }
        // Если не найдено, ищем в родительской области
        return _parent ? _parent->exists_type(name) : std::nullopt;
    }

    private:

    SymbolTable _symbol_table;    
    std::shared_ptr<Scope> _parent;
};