#ifndef ENUMSTRINGIFIER_HPP
#define ENUMSTRINGIFIER_HPP

#include <string>
#include <map>
#include <stdexcept>
#include <ostream>

/*
 * Usage with custom enum:
 *      - Create enum
 *      - Create a const map<enum, string> map_to_string
 *      - Create a const map<string, enum> map_from_string
 *      - typedef EnumStringifier from this newly created triple
 * Example:
 *  enum TestEnum {
 *      TEST1
 *  };
 *  const std::map<TestEnum, std::string> map_to_string = { {TestEnum::TEST1, "TEST1"} };
 *  const std::map<std::string, TestEnum> map_from_string = { {"TEST1", Test::TEST1 }};
 *
 *  typedef EnumStringifier<Test, map_to_string, map_from_string> Test;
 *
 * Usage:
 *  Test::to_string(Test::TEST1); // "TEST1"
 *  Test::from_string("TEST1");   // Test::TEST1
 */

template <typename Enum, const std::map<Enum, std::string>& MapToString, const std::map<std::string, Enum>& MapFromString>
class EnumStringifier {
public:
    static constexpr std::string to_string(Enum e) {
        if(!MapToString.contains(e)) {
            throw std::runtime_error("Not a valid type (enum type e => string s)");
        }
        return MapToString.at(e);
    }

    static constexpr Enum from_string(const std::string& src) {
        if(!MapFromString.contains(src)) {
            throw std::runtime_error("Not a valid type (string s => enum type e)");
        }
        return MapFromString.at(src);
    }
};

#endif
