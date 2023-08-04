#pragma once
#include <string>
#include <vector>
#include <vector.h>
#include <cassert>
#include <charconv>
#include <optional>

inline double ConvertToDouble(std::string_view s) {
    double result;
    std::from_chars(s.data(), s.data() + s.size(), result);
    return result;
}

// trim from start
static inline std::string &Ltrim(std::string &s) {
    s.erase(s.begin(),
            std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string &Rtrim(std::string &s) {
    s.erase(
        std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(),
        s.end());
    return s;
}

std::vector<std::string> Split(const std::string &s, char delim = ' ', bool without_empty = true) {
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> elems;
    while (std::getline(ss, item, delim)) {
        if (without_empty) {
            if (!item.empty()) {
                elems.push_back(item);
            }
        } else {
            elems.push_back(item);
        }
    }
    return elems;
}

class Reader {
protected:
    std::string s_;

public:
    Reader(std::string s) : s_(s) {
    }
    void Trim() {
        Ltrim(Rtrim(s_));
    }
    Vector GetVector() {
        Trim();
        auto v = Split(s_);
        assert(v.size() == 3);
        double x = ConvertToDouble(v[0]), y = ConvertToDouble(v[1]), z = ConvertToDouble(v[2]);
        return {x, y, z};
    }
    double GetDouble() {
        Trim();
        return ConvertToDouble(s_);
    }
};

class ReaderMtl : Reader {

public:
    ReaderMtl(std::string s) : Reader(s) {
        Trim();
    }

    bool Newmtl() {
        return s_.starts_with("newmtl") || s_.starts_with("Newmtl");
    }
    bool Ka() {
        return s_.starts_with("Ka");
    }
    bool Kd() {
        return s_.starts_with("Kd");
    }
    bool Ks() {
        return s_.starts_with("Ks");
    }
    bool Ke() {
        return s_.starts_with("Ke");
    }
    bool Ns() {
        return s_.starts_with("Ns");
    }
    bool Ni() {
        return s_.starts_with("Ni");
    }
    bool Al() {
        return s_.starts_with("al");
    }
    std::string GetNewmtl() {
        s_ = s_.substr(6);
        Trim();
        return s_;
    }
    Vector GetKaKdKsKe() {
        s_ = s_.substr(2);
        Trim();
        return GetVector();
    }
    double GetNsNi() {
        s_ = s_.substr(2);
        Trim();
        return GetDouble();
    }
    std::array<double, 3> GetAl() {
        s_ = s_.substr(2);
        Trim();
        Vector v = GetVector();
        return {v[0], v[1], v[2]};
    }
};

class ReaderObj : Reader {

public:
    ReaderObj(std::string s) : Reader(s) {
        Trim();
    }
    bool V() {
        return s_.starts_with("v") && !Vn();
    }
    bool Vn() {
        return s_.starts_with("vn");
    }
    bool F() {
        return s_.starts_with("f");
    }

    bool Mtllib() {
        return s_.starts_with("mtllib");
    }
    bool Usemtl() {
        return s_.starts_with("usemtl");
    }
    bool S() {
        return s_.starts_with("S");
    }
    bool P() {
        return s_.starts_with("P");
    }
    Vector GetVnV() {
        s_ = s_.substr(2);
        Trim();
        return GetVector();
    }
    std::string GetMtllibUsemtl() {
        s_ = s_.substr(6);
        Trim();
        return s_;
    }
    std::pair<Vector, double> GetS() {
        s_ = s_.substr(2);
        Trim();
        std::vector<std::string> v = Split(s_);
        assert(v.size() == 4);
        double x = ConvertToDouble(v[0]), y = ConvertToDouble(v[1]), z = ConvertToDouble(v[2]),
               r = ConvertToDouble(v[3]);
        return {{x, y, z}, r};
    };
    std::pair<Vector, Vector> GetP() {
        s_ = s_.substr(2);
        Trim();
        std::vector<std::string> v = Split(s_);
        assert(v.size() == 6);
        double x = ConvertToDouble(v[0]), y = ConvertToDouble(v[1]), z = ConvertToDouble(v[2]),
               r = ConvertToDouble(v[3]), g = ConvertToDouble(v[4]), b = ConvertToDouble((v[5]));
        return {{x, y, z}, {r, g, b}};
    };
    std::vector<std::pair<int, std::optional<int>>> GetF() {
        std::vector<std::pair<int, std::optional<int>>> result;
        s_ = s_.substr(2);
        Trim();
        std::vector<std::string> v = Split(s_);
        for (std::string vertex : v) {
            Reader local_reader(vertex);
            std::vector<std::string> local_vector = Split(vertex, '/', false);
            assert(local_vector.size() <= 3);
            if (local_vector.size() == 3) {
                result.push_back(
                    {ConvertToDouble(local_vector[0]), ConvertToDouble(local_vector[2])});
            } else {
                result.push_back({ConvertToDouble(local_vector[0]), {}});
            }
        }
        return result;
    }
};
// just empty comment