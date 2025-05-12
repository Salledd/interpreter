#pragma once
#include <set>
#include <string>

enum class Modifier {
    Const,
    Static,
    Unsigned,
};

struct Modifiers {
    std::set<Modifier> mods;

    bool empty() const {
        return mods.empty(); // Проверка на пустоту
    }

    void add(Modifier mod) {
        mods.insert(mod); // Вставка элемента
    }

    bool has(Modifier mod) const {
        return mods.count(mod) != 0; // Проверка наличия элемента
    }

    std::string get_str(){
        std::string str = "";
        for (const auto& mod : mods) {
            switch (mod) {
                case Modifier::Const: str += "Const"; break;
                case Modifier::Static: str += "Static"; break;
                case Modifier::Unsigned: str += "Unsigned"; break;
            }
            str += " ";
        }
    }

};