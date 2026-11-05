// SPDX-License-Identifier: GPL-3.0-only

#include "json.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "util.h"

namespace host::json {

Value Value::Bool(bool v) {
    Value r;
    r.type = Type::Bool;
    r.b = v;
    return r;
}

Value Value::Int(int64_t v) {
    Value r;
    r.type = Type::Int;
    r.i = v;
    return r;
}

Value Value::Str(std::wstring v) {
    Value r;
    r.type = Type::String;
    r.s = std::move(v);
    return r;
}

Value Value::Arr() {
    Value r;
    r.type = Type::Array;
    return r;
}

Value Value::Obj() {
    Value r;
    r.type = Type::Object;
    return r;
}

const Value& Value::Get(const std::wstring& key) const {
    static const Value kNull;
    if (type != Type::Object) return kNull;
    for (auto& [k, v] : fields)
        if (k == key) return v;
    return kNull;
}

Value& Value::Set(const std::wstring& key, Value v) {
    type = Type::Object;
    for (auto& [k, old] : fields)
        if (k == key) return old = std::move(v);
    fields.emplace_back(key, std::move(v));
    return fields.back().second;
}

int64_t Value::AsInt(int64_t fallback) const {
    if (type == Type::Int) return i;
    if (type == Type::Double) return (int64_t)d;
    if (type == Type::Bool) return b;
    return fallback;
}

bool Value::AsBool(bool fallback) const {
    if (type == Type::Bool) return b;
    if (type == Type::Int) return i != 0;
    return fallback;
}

std::wstring Value::AsString(const std::wstring& fallback) const {
    return type == Type::String ? s : fallback;
}

namespace {

struct Parser {
    const std::string& t;
    size_t p = 0;
    int depth = 0;

    void Ws() {
        while (p < t.size() && (t[p] == ' ' || t[p] == '\t' || t[p] == '\r' || t[p] == '\n')) p++;
    }

    bool Lit(const char* word) {
        size_t n = strlen(word);
        if (t.compare(p, n, word) != 0) return false;
        p += n;
        return true;
    }

    bool Hex4(unsigned& v) {
        if (p + 4 > t.size()) return false;
        v = 0;
        for (int k = 0; k < 4; k++) {
            char c = t[p++];
            v <<= 4;
            if (c >= '0' && c <= '9') v |= c - '0';
            else if (c >= 'a' && c <= 'f') v |= c - 'a' + 10;
            else if (c >= 'A' && c <= 'F') v |= c - 'A' + 10;
            else return false;
        }
        return true;
    }

    bool String(std::wstring& out) {
        if (p >= t.size() || t[p] != '"') return false;
        p++;
        std::string raw;  // UTF-8 bytes between escapes
        out.clear();
        auto flush = [&] {
            out += Widen(raw);
            raw.clear();
        };
        while (p < t.size()) {
            char c = t[p++];
            if (c == '"') {
                flush();
                return true;
            }
            if ((unsigned char)c < 0x20) return false;
            if (c != '\\') {
                raw += c;
                continue;
            }
            if (p >= t.size()) return false;
            char e = t[p++];
            switch (e) {
                case '"': raw += '"'; break;
                case '\\': raw += '\\'; break;
                case '/': raw += '/'; break;
                case 'b': raw += '\b'; break;
                case 'f': raw += '\f'; break;
                case 'n': raw += '\n'; break;
                case 'r': raw += '\r'; break;
                case 't': raw += '\t'; break;
                case 'u': {
                    unsigned u;
                    if (!Hex4(u)) return false;
                    flush();
                    out += (wchar_t)u;  // surrogate pairs stay as two UTF-16 units
                    break;
                }
                default: return false;
            }
        }
        return false;
    }

    bool Number(Value& v) {
        size_t start = p;
        bool isInt = true;
        if (p < t.size() && t[p] == '-') p++;
        while (p < t.size()) {
            char c = t[p];
            if (c >= '0' && c <= '9') {
                p++;
            } else if (c == '.' || c == 'e' || c == 'E' || c == '+' || c == '-') {
                isInt = false;
                p++;
            } else {
                break;
            }
        }
        std::string num = t.substr(start, p - start);
        if (num.empty() || num == "-") return false;
        char* end = nullptr;
        if (isInt) {
            v.type = Value::Type::Int;
            v.i = strtoll(num.c_str(), &end, 10);
        } else {
            v.type = Value::Type::Double;
            v.d = strtod(num.c_str(), &end);
        }
        return end && *end == 0;
    }

    bool Parse(Value& v) {
        if (++depth > 64) return false;
        Ws();
        if (p >= t.size()) return false;
        char c = t[p];
        bool ok;
        if (c == '{') {
            p++;
            v = Value::Obj();
            Ws();
            ok = true;
            if (p < t.size() && t[p] == '}') {
                p++;
            } else {
                while (true) {
                    std::wstring key;
                    Ws();
                    if (!String(key)) { ok = false; break; }
                    Ws();
                    if (p >= t.size() || t[p++] != ':') { ok = false; break; }
                    Value item;
                    if (!Parse(item)) { ok = false; break; }
                    v.Set(key, std::move(item));
                    Ws();
                    if (p < t.size() && t[p] == ',') { p++; continue; }
                    if (p < t.size() && t[p] == '}') { p++; break; }
                    ok = false;
                    break;
                }
            }
        } else if (c == '[') {
            p++;
            v = Value::Arr();
            Ws();
            ok = true;
            if (p < t.size() && t[p] == ']') {
                p++;
            } else {
                while (true) {
                    Value item;
                    if (!Parse(item)) { ok = false; break; }
                    v.Push(std::move(item));
                    Ws();
                    if (p < t.size() && t[p] == ',') { p++; continue; }
                    if (p < t.size() && t[p] == ']') { p++; break; }
                    ok = false;
                    break;
                }
            }
        } else if (c == '"') {
            v = Value::Str({});
            ok = String(v.s);
        } else if (Lit("true")) {
            v = Value::Bool(true);
            ok = true;
        } else if (Lit("false")) {
            v = Value::Bool(false);
            ok = true;
        } else if (Lit("null")) {
            v = Value::Null();
            ok = true;
        } else {
            ok = Number(v);
        }
        depth--;
        return ok;
    }
};

void WriteString(const std::wstring& s, std::string& out) {
    out += '"';
    std::wstring plain;
    auto flush = [&] {
        out += Narrow(plain);
        plain.clear();
    };
    for (wchar_t c : s) {
        if (c == L'"' || c == L'\\') {
            plain += L'\\';
            plain += c;
        } else if (c < 0x20) {
            flush();
            char buf[8];
            snprintf(buf, sizeof(buf), "\\u%04x", (unsigned)c);
            out += buf;
        } else {
            plain += c;
        }
    }
    flush();
    out += '"';
}

void WriteValue(const Value& v, std::string& out) {
    switch (v.type) {
        case Value::Type::Null: out += "null"; break;
        case Value::Type::Bool: out += v.b ? "true" : "false"; break;
        case Value::Type::Int: out += std::to_string(v.i); break;
        case Value::Type::Double: {
            char buf[64];
            snprintf(buf, sizeof(buf), "%.17g", v.d);
            out += buf;
            break;
        }
        case Value::Type::String: WriteString(v.s, out); break;
        case Value::Type::Array: {
            out += '[';
            for (size_t k = 0; k < v.items.size(); k++) {
                if (k) out += ", ";
                WriteValue(v.items[k], out);
            }
            out += ']';
            break;
        }
        case Value::Type::Object: {
            out += "{ ";
            for (size_t k = 0; k < v.fields.size(); k++) {
                if (k) out += ", ";
                WriteString(v.fields[k].first, out);
                out += ": ";
                WriteValue(v.fields[k].second, out);
            }
            out += v.fields.empty() ? "}" : " }";
            break;
        }
    }
}

}  // namespace

bool Parse(const std::string& utf8, Value& out) {
    std::string text = utf8;
    if (text.size() >= 3 && (unsigned char)text[0] == 0xEF && (unsigned char)text[1] == 0xBB &&
        (unsigned char)text[2] == 0xBF)
        text.erase(0, 3);
    Parser ps{text};
    if (!ps.Parse(out)) return false;
    ps.Ws();
    return ps.p == text.size();
}

std::string Write(const Value& v) {
    std::string out;
    WriteValue(v, out);
    out += '\n';
    return out;
}

}  // namespace host::json
