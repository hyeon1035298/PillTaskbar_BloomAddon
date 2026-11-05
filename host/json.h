// SPDX-License-Identifier: GPL-3.0-only
//
// A small JSON reader and writer, enough for host-state.json and runtime-state.json.
// Integers keep 64 bits because host-state.json carries FILETIME values, which a
// double cannot hold exactly.

#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace host::json {

struct Value {
    enum class Type { Null, Bool, Int, Double, String, Array, Object };
    Type type = Type::Null;
    bool b = false;
    int64_t i = 0;
    double d = 0;
    std::wstring s;
    std::vector<Value> items;                            // Array
    std::vector<std::pair<std::wstring, Value>> fields;  // Object, in written order

    static Value Null() { return {}; }
    static Value Bool(bool v);
    static Value Int(int64_t v);
    static Value Str(std::wstring v);
    static Value Arr();
    static Value Obj();

    bool IsNull() const { return type == Type::Null; }
    // Object access; Get returns a null value for a missing key.
    const Value& Get(const std::wstring& key) const;
    Value& Set(const std::wstring& key, Value v);
    void Push(Value v) { items.push_back(std::move(v)); }

    int64_t AsInt(int64_t fallback = 0) const;
    bool AsBool(bool fallback = false) const;
    std::wstring AsString(const std::wstring& fallback = {}) const;
};

bool Parse(const std::string& utf8, Value& out);
std::string Write(const Value& v);

}  // namespace host::json
