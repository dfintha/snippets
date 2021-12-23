// todo_before.cpp
// A macro that causes a compile-time error past a given date with a message.
// Author: Dénes Fintha
// Year: 2019
// -------------------------------------------------------------------------- //

// Interface

#include <string_view>

enum {
    JANUARY = 1,
    FEBRUARY = 2,
    MARCH = 3,
    APRIL = 4,
    MAY = 5,
    JUNE = 6,
    JULY = 7,
    AUGUST = 8,
    SEPTEMBER = 9,
    OCTOBER = 10,
    NOVEMBER = 11,
    DECEMBER = 12
};

#define TODO_BEFORE(y, m, d, message)                                           \
    do {                                                                        \
        constexpr std::string_view current_date_str{ __DATE__ };                \
                                                                                \
        constexpr std::string_view current_year_str =                           \
            current_date_str.substr(current_date_str.find_last_of(' ') + 1);    \
        constexpr std::string_view current_month_str =                          \
            current_date_str.substr(0, current_date_str.find_first_of(' '));    \
        constexpr std::string_view current_day_str =                            \
            current_date_str.substr(current_date_str.find_first_of(' ') + 1);   \
                                                                                \
        constexpr unsigned current_year =                                       \
            unsigned((current_year_str[0] - '0') * 1000 +                       \
                     (current_year_str[1] - '0') * 100 +                        \
                     (current_year_str[2] - '0') * 10 +                         \
                     (current_year_str[3] - '0'));                              \
        constexpr unsigned current_month = [] (const std::string_view& s) {     \
            if (s[0] == 'J' && s[1] == 'a' && s[2] == 'n') return 1;            \
            if (s[0] == 'F' && s[1] == 'e' && s[2] == 'b') return 2;            \
            if (s[0] == 'M' && s[1] == 'a' && s[2] == 'r') return 3;            \
            if (s[0] == 'A' && s[1] == 'p' && s[2] == 'r') return 4;            \
            if (s[0] == 'M' && s[1] == 'a' && s[2] == 'y') return 5;            \
            if (s[0] == 'J' && s[1] == 'u' && s[2] == 'n') return 6;            \
            if (s[0] == 'J' && s[1] == 'u' && s[2] == 'l') return 7;            \
            if (s[0] == 'A' && s[1] == 'u' && s[2] == 'g') return 8;            \
            if (s[0] == 'S' && s[1] == 'e' && s[2] == 'p') return 9;            \
            if (s[0] == 'O' && s[1] == 'c' && s[2] == 't') return 10;           \
            if (s[0] == 'N' && s[1] == 'o' && s[2] == 'v') return 11;           \
            if (s[0] == 'D' && s[1] == 'e' && s[2] == 'c') return 12;           \
            return 0;                                                           \
        }(current_month_str);                                                   \
        constexpr unsigned current_day =                                        \
            unsigned((current_day_str[0] - '0') * 10 +                          \
                     (current_day_str[1] - '0'));                               \
                                                                                \
        constexpr bool past_year = current_year < y;                            \
        constexpr bool past_month = current_year == y && current_month < m;     \
        constexpr bool past_day = current_year == y &&                          \
                                  current_month == m &&                         \
                                  current_day < d;                              \
                                                                                \
        static_assert(past_year || past_month || past_day, "TODO: " message);   \
    } while (false)

// Demonstration

int main() {
    TODO_BEFORE(2019, SEPTEMBER, 23, "Test this macro.");
    return 0;
}
