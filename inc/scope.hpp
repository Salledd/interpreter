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

    std::optional<SymbolInfo> lookup(const std::string& name) const {
        return _symbol_table.lookup(name);
    }

    std::optional<SymbolInfo> exists(const std::string& name) const {
        if (auto info = _symbol_table.lookup(name)) {
            return info;
        }
        // Если не найдено, ищем в родительской области
        return _parent ? _parent->exists(name) : std::nullopt;
    }

    private:

    SymbolTable _symbol_table;    
    std::shared_ptr<Scope> _parent;
};