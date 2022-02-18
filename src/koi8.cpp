// koi8.cpp
// Utilities to translate between the KOI-8 encoding families and UTF-8 (by
// using a look-up-table), or lossy ASCII (by zeroing the top bits).
// Author: Dénes Fintha
// Year: 2022
// -------------------------------------------------------------------------- //

// Interface

#include <cstddef>
#include <cstdint>

enum class koi8_variant : char {
    koi8r,
    koi8b,
    koi8u
};

const char * koi8_char_to_utf8_codepoint(uint8_t koi8c, koi8_variant mode);
char * koi8_string_to_utf8_string(const uint8_t *koi8str, koi8_variant mode);
uint8_t utf8_codepoint_to_koi8_char(const char *utf8, koi8_variant mode, size_t *step = nullptr);
uint8_t * utf8_string_to_koi8_string(const char *utf8, koi8_variant mode);
char * cut_top_bit_of_koi8_string(uint8_t *koi8str);
char * switch_case_of_ascii_string(char *utf8str);

// Implementation

#include <cctype>
#include <cstring>
#include <stdexcept>
#include <vector>

struct koi8_entry {
    uint8_t koi8;
    const char *utf8;
};

static const std::vector<koi8_entry> koi8r_dictionary = {
                     { 0x01, "\x01"}, { 0x02, "\x02"}, { 0x03, "\x03"},
    { 0x04, "\x04"}, { 0x05, "\x05"}, { 0x06, "\x06"}, { 0x07, "\x07"},
    { 0x08, "\x08"}, { 0x09, "\x09"}, { 0x0A, "\x0A"}, { 0x0B, "\x0B"},
    { 0x0C, "\x0C"}, { 0x0D, "\x0D"}, { 0x0E, "\x0E"}, { 0x0F, "\x0F"},
    { 0x10, "\x10"}, { 0x11, "\x11"}, { 0x12, "\x12"}, { 0x13, "\x13"},
    { 0x14, "\x14"}, { 0x15, "\x15"}, { 0x16, "\x16"}, { 0x17, "\x17"},
    { 0x18, "\x18"}, { 0x19, "\x19"}, { 0x1A, "\x1A"}, { 0x1B, "\x1B"},
    { 0x1C, "\x1C"}, { 0x1D, "\x1D"}, { 0x1E, "\x1E"}, { 0x1F, "\x1F"},
    { 0x20, " " }, { 0x21, "!" }, { 0x22, "\"" }, { 0x23, "#" },
    { 0x24, "$" }, { 0x25, "%" }, { 0x26, "&" }, { 0x27, "'" },
    { 0x28, "(" }, { 0x29, ")" }, { 0x2A, "*" }, { 0x2B, "+" },
    { 0x2C, "," }, { 0x2D, "-" }, { 0x2E, "." }, { 0x2F, "/" },
    { 0x30, "0" }, { 0x31, "1" }, { 0x32, "2" }, { 0x33, "3" },
    { 0x34, "4" }, { 0x35, "5" }, { 0x36, "6" }, { 0x37, "7" },
    { 0x38, "8" }, { 0x39, "9" }, { 0x3A, ":" }, { 0x3B, ";" },
    { 0x3C, "<" }, { 0x3D, "=" }, { 0x3E, ">" }, { 0x3F, "?" },
    { 0x40, "@" }, { 0x41, "A" }, { 0x42, "B" }, { 0x43, "C" },
    { 0x44, "D" }, { 0x45, "E" }, { 0x46, "F" }, { 0x47, "G" },
    { 0x48, "H" }, { 0x49, "I" }, { 0x4A, "J" }, { 0x4B, "K" },
    { 0x4C, "L" }, { 0x4D, "M" }, { 0x4E, "N" }, { 0x4F, "O" },
    { 0x50, "P" }, { 0x51, "Q" }, { 0x52, "R" }, { 0x53, "S" },
    { 0x54, "T" }, { 0x55, "U" }, { 0x56, "V" }, { 0x57, "W" },
    { 0x58, "X" }, { 0x59, "Y" }, { 0x5A, "Z" }, { 0x5B, "[" },
    { 0x5C, "\\" }, { 0x5D, "]" }, { 0x5E, "^" }, { 0x5F, "_" },
    { 0x60, "`" }, { 0x61, "a" }, { 0x62, "b" }, { 0x63, "c" },
    { 0x64, "d" }, { 0x65, "e" }, { 0x66, "f" }, { 0x67, "g" },
    { 0x68, "h" }, { 0x69, "i" }, { 0x6A, "j" }, { 0x6B, "k" },
    { 0x6C, "l" }, { 0x6D, "m" }, { 0x6E, "n" }, { 0x6F, "o" },
    { 0x70, "p" }, { 0x71, "q" }, { 0x72, "r" }, { 0x73, "s" },
    { 0x74, "t" }, { 0x75, "u" }, { 0x76, "v" }, { 0x77, "w" },
    { 0x78, "x" }, { 0x79, "y" }, { 0x7A, "z" }, { 0x7B, "{" },
    { 0x7C, "|" }, { 0x7D, "}" }, { 0x7E, "~" }, { 0x7F, "\x7F"},
    { 0x80, "─" }, { 0x81, "│" }, { 0x82, "┌" }, { 0x83, "┐" },
    { 0x84, "└" }, { 0x85, "┘" }, { 0x86, "├" }, { 0x87, "┤" },
    { 0x88, "┬" }, { 0x89, "┴" }, { 0x8A, "┼" }, { 0x8B, "▀" },
    { 0x8C, "▄" }, { 0x8D, "█" }, { 0x8E, "▌" }, { 0x8F, "▐" },
    { 0x90, "░" }, { 0x91, "▒" }, { 0x92, "▓" }, { 0x93, "⌠" },
    { 0x94, "■" }, { 0x95, "∙" }, { 0x96, "√" }, { 0x97, "≈" },
    { 0x98, "≤" }, { 0x99, "≥" }, { 0x9A, " " }, { 0x9B, "⌡" },
    { 0x9C, "°" }, { 0x9D, "²" }, { 0x9E, "·" }, { 0x9F, "÷" },
    { 0xA0, "═" }, { 0xA1, "║" }, { 0xA2, "╒" }, { 0xA3, "ё" },
    { 0xA4, "╓" }, { 0xA5, "╔" }, { 0xA6, "╕" }, { 0xA7, "╖" },
    { 0xA8, "╗" }, { 0xA9, "╘" }, { 0xAA, "╙" }, { 0xAB, "╚" },
    { 0xAC, "╛" }, { 0xAD, "╜" }, { 0xAE, "╝" }, { 0xAF, "╞" },
    { 0xB0, "╟" }, { 0xB1, "╠" }, { 0xB2, "╡" }, { 0xB3, "Ё" },
    { 0xB4, "╢" }, { 0xB5, "╣" }, { 0xB6, "╤" }, { 0xB7, "╥" },
    { 0xB8, "╦" }, { 0xB9, "╧" }, { 0xBA, "╨" }, { 0xBB, "╩" },
    { 0xBC, "╪" }, { 0xBD, "╫" }, { 0xBE, "╬" }, { 0xBF, "©" },
    { 0xC0, "ю" }, { 0xC1, "а" }, { 0xC2, "б" }, { 0xC3, "ц" },
    { 0xC4, "д" }, { 0xC5, "е" }, { 0xC6, "ф" }, { 0xC7, "г" },
    { 0xC8, "х" }, { 0xC9, "и" }, { 0xCA, "й" }, { 0xCB, "к" },
    { 0xCC, "л" }, { 0xCD, "м" }, { 0xCE, "н" }, { 0xCF, "о" },
    { 0xD0, "п" }, { 0xD1, "я" }, { 0xD2, "р" }, { 0xD3, "с" },
    { 0xD4, "т" }, { 0xD5, "у" }, { 0xD6, "ж" }, { 0xD7, "в" },
    { 0xD8, "ь" }, { 0xD9, "ы" }, { 0xDA, "з" }, { 0xDB, "ш" },
    { 0xDC, "э" }, { 0xDD, "щ" }, { 0xDE, "ч" }, { 0xDF, "ъ" },
    { 0xE0, "Ю" }, { 0xE1, "А" }, { 0xE2, "Б" }, { 0xE3, "Ц" },
    { 0xE4, "Д" }, { 0xE5, "Е" }, { 0xE6, "Ф" }, { 0xE7, "Г" },
    { 0xE8, "Х" }, { 0xE9, "И" }, { 0xEA, "Й" }, { 0xEB, "К" },
    { 0xEC, "Л" }, { 0xED, "М" }, { 0xEE, "Н" }, { 0xEF, "О" },
    { 0xF0, "П" }, { 0xF1, "Я" }, { 0xF2, "Р" }, { 0xF3, "С" },
    { 0xF4, "Т" }, { 0xF5, "У" }, { 0xF6, "Ж" }, { 0xF7, "В" },
    { 0xF8, "Ь" }, { 0xF9, "Ы" }, { 0xFA, "З" }, { 0xFB, "Ш" },
    { 0xFC, "Э" }, { 0xFD, "Щ" }, { 0xFE, "Ч" }, { 0xFF, "Ъ" }
};

static const std::vector<koi8_entry> koi8b_dictionary = {
                     { 0x01, "\x01"}, { 0x02, "\x02"}, { 0x03, "\x03"},
    { 0x04, "\x04"}, { 0x05, "\x05"}, { 0x06, "\x06"}, { 0x07, "\x07"},
    { 0x08, "\x08"}, { 0x09, "\x09"}, { 0x0A, "\x0A"}, { 0x0B, "\x0B"},
    { 0x0C, "\x0C"}, { 0x0D, "\x0D"}, { 0x0E, "\x0E"}, { 0x0F, "\x0F"},
    { 0x10, "\x10"}, { 0x11, "\x11"}, { 0x12, "\x12"}, { 0x13, "\x13"},
    { 0x14, "\x14"}, { 0x15, "\x15"}, { 0x16, "\x16"}, { 0x17, "\x17"},
    { 0x18, "\x18"}, { 0x19, "\x19"}, { 0x1A, "\x1A"}, { 0x1B, "\x1B"},
    { 0x1C, "\x1C"}, { 0x1D, "\x1D"}, { 0x1E, "\x1E"}, { 0x1F, "\x1F"},
    { 0x20, " " }, { 0x21, "!" }, { 0x22, "\"" }, { 0x23, "#" },
    { 0x24, "$" }, { 0x25, "%" }, { 0x26, "&" }, { 0x27, "'" },
    { 0x28, "(" }, { 0x29, ")" }, { 0x2A, "*" }, { 0x2B, "+" },
    { 0x2C, "," }, { 0x2D, "-" }, { 0x2E, "." }, { 0x2F, "/" },
    { 0x30, "0" }, { 0x31, "1" }, { 0x32, "2" }, { 0x33, "3" },
    { 0x34, "4" }, { 0x35, "5" }, { 0x36, "6" }, { 0x37, "7" },
    { 0x38, "8" }, { 0x39, "9" }, { 0x3A, ":" }, { 0x3B, ";" },
    { 0x3C, "<" }, { 0x3D, "=" }, { 0x3E, ">" }, { 0x3F, "?" },
    { 0x40, "@" }, { 0x41, "A" }, { 0x42, "B" }, { 0x43, "C" },
    { 0x44, "D" }, { 0x45, "E" }, { 0x46, "F" }, { 0x47, "G" },
    { 0x48, "H" }, { 0x49, "I" }, { 0x4A, "J" }, { 0x4B, "K" },
    { 0x4C, "L" }, { 0x4D, "M" }, { 0x4E, "N" }, { 0x4F, "O" },
    { 0x50, "P" }, { 0x51, "Q" }, { 0x52, "R" }, { 0x53, "S" },
    { 0x54, "T" }, { 0x55, "U" }, { 0x56, "V" }, { 0x57, "W" },
    { 0x58, "X" }, { 0x59, "Y" }, { 0x5A, "Z" }, { 0x5B, "[" },
    { 0x5C, "\\" }, { 0x5D, "]" }, { 0x5E, "^" }, { 0x5F, "_" },
    { 0x60, "`" }, { 0x61, "a" }, { 0x62, "b" }, { 0x63, "c" },
    { 0x64, "d" }, { 0x65, "e" }, { 0x66, "f" }, { 0x67, "g" },
    { 0x68, "h" }, { 0x69, "i" }, { 0x6A, "j" }, { 0x6B, "k" },
    { 0x6C, "l" }, { 0x6D, "m" }, { 0x6E, "n" }, { 0x6F, "o" },
    { 0x70, "p" }, { 0x71, "q" }, { 0x72, "r" }, { 0x73, "s" },
    { 0x74, "t" }, { 0x75, "u" }, { 0x76, "v" }, { 0x77, "w" },
    { 0x78, "x" }, { 0x79, "y" }, { 0x7A, "z" }, { 0x7B, "{" },
    { 0x7C, "|" }, { 0x7D, "}" }, { 0x7E, "~" }, { 0x7F, "\x7F"},
    { 0xA3, "ё" }, { 0xB3, "Ё" },
    { 0xC0, "ю" }, { 0xC1, "а" }, { 0xC2, "б" }, { 0xC3, "ц" },
    { 0xC4, "д" }, { 0xC5, "е" }, { 0xC6, "ф" }, { 0xC7, "г" },
    { 0xC8, "х" }, { 0xC9, "и" }, { 0xCA, "й" }, { 0xCB, "к" },
    { 0xCC, "л" }, { 0xCD, "м" }, { 0xCE, "н" }, { 0xCF, "о" },
    { 0xD0, "п" }, { 0xD1, "я" }, { 0xD2, "р" }, { 0xD3, "с" },
    { 0xD4, "т" }, { 0xD5, "у" }, { 0xD6, "ж" }, { 0xD7, "в" },
    { 0xD8, "ь" }, { 0xD9, "ы" }, { 0xDA, "з" }, { 0xDB, "ш" },
    { 0xDC, "э" }, { 0xDD, "щ" }, { 0xDE, "ч" }, { 0xDF, "ъ" },
    { 0xE0, "Ю" }, { 0xE1, "А" }, { 0xE2, "Б" }, { 0xE3, "Ц" },
    { 0xE4, "Д" }, { 0xE5, "Е" }, { 0xE6, "Ф" }, { 0xE7, "Г" },
    { 0xE8, "Х" }, { 0xE9, "И" }, { 0xEA, "Й" }, { 0xEB, "К" },
    { 0xEC, "Л" }, { 0xED, "М" }, { 0xEE, "Н" }, { 0xEF, "О" },
    { 0xF0, "П" }, { 0xF1, "Я" }, { 0xF2, "Р" }, { 0xF3, "С" },
    { 0xF4, "Т" }, { 0xF5, "У" }, { 0xF6, "Ж" }, { 0xF7, "В" },
    { 0xF8, "Ь" }, { 0xF9, "Ы" }, { 0xFA, "З" }, { 0xFB, "Ш" },
    { 0xFC, "Э" }, { 0xFD, "Щ" }, { 0xFE, "Ч" }, { 0xFF, "Ъ" }
};

static const std::vector<koi8_entry> koi8u_dictionary = {
                     { 0x01, "\x01"}, { 0x02, "\x02"}, { 0x03, "\x03"},
    { 0x04, "\x04"}, { 0x05, "\x05"}, { 0x06, "\x06"}, { 0x07, "\x07"},
    { 0x08, "\x08"}, { 0x09, "\x09"}, { 0x0A, "\x0A"}, { 0x0B, "\x0B"},
    { 0x0C, "\x0C"}, { 0x0D, "\x0D"}, { 0x0E, "\x0E"}, { 0x0F, "\x0F"},
    { 0x10, "\x10"}, { 0x11, "\x11"}, { 0x12, "\x12"}, { 0x13, "\x13"},
    { 0x14, "\x14"}, { 0x15, "\x15"}, { 0x16, "\x16"}, { 0x17, "\x17"},
    { 0x18, "\x18"}, { 0x19, "\x19"}, { 0x1A, "\x1A"}, { 0x1B, "\x1B"},
    { 0x1C, "\x1C"}, { 0x1D, "\x1D"}, { 0x1E, "\x1E"}, { 0x1F, "\x1F"},
    { 0x20, " " }, { 0x21, "!" }, { 0x22, "\"" }, { 0x23, "#" },
    { 0x24, "$" }, { 0x25, "%" }, { 0x26, "&" }, { 0x27, "'" },
    { 0x28, "(" }, { 0x29, ")" }, { 0x2A, "*" }, { 0x2B, "+" },
    { 0x2C, "," }, { 0x2D, "-" }, { 0x2E, "." }, { 0x2F, "/" },
    { 0x30, "0" }, { 0x31, "1" }, { 0x32, "2" }, { 0x33, "3" },
    { 0x34, "4" }, { 0x35, "5" }, { 0x36, "6" }, { 0x37, "7" },
    { 0x38, "8" }, { 0x39, "9" }, { 0x3A, ":" }, { 0x3B, ";" },
    { 0x3C, "<" }, { 0x3D, "=" }, { 0x3E, ">" }, { 0x3F, "?" },
    { 0x40, "@" }, { 0x41, "A" }, { 0x42, "B" }, { 0x43, "C" },
    { 0x44, "D" }, { 0x45, "E" }, { 0x46, "F" }, { 0x47, "G" },
    { 0x48, "H" }, { 0x49, "I" }, { 0x4A, "J" }, { 0x4B, "K" },
    { 0x4C, "L" }, { 0x4D, "M" }, { 0x4E, "N" }, { 0x4F, "O" },
    { 0x50, "P" }, { 0x51, "Q" }, { 0x52, "R" }, { 0x53, "S" },
    { 0x54, "T" }, { 0x55, "U" }, { 0x56, "V" }, { 0x57, "W" },
    { 0x58, "X" }, { 0x59, "Y" }, { 0x5A, "Z" }, { 0x5B, "[" },
    { 0x5C, "\\" }, { 0x5D, "]" }, { 0x5E, "^" }, { 0x5F, "_" },
    { 0x60, "`" }, { 0x61, "a" }, { 0x62, "b" }, { 0x63, "c" },
    { 0x64, "d" }, { 0x65, "e" }, { 0x66, "f" }, { 0x67, "g" },
    { 0x68, "h" }, { 0x69, "i" }, { 0x6A, "j" }, { 0x6B, "k" },
    { 0x6C, "l" }, { 0x6D, "m" }, { 0x6E, "n" }, { 0x6F, "o" },
    { 0x70, "p" }, { 0x71, "q" }, { 0x72, "r" }, { 0x73, "s" },
    { 0x74, "t" }, { 0x75, "u" }, { 0x76, "v" }, { 0x77, "w" },
    { 0x78, "x" }, { 0x79, "y" }, { 0x7A, "z" }, { 0x7B, "{" },
    { 0x7C, "|" }, { 0x7D, "}" }, { 0x7E, "~" }, { 0x7F, "\x7F"},
    { 0x80, "─" }, { 0x81, "│" }, { 0x82, "┌" }, { 0x83, "┐" },
    { 0x84, "└" }, { 0x85, "┘" }, { 0x86, "├" }, { 0x87, "┤" },
    { 0x88, "┬" }, { 0x89, "┴" }, { 0x8A, "┼" }, { 0x8B, "▀" },
    { 0x8C, "▄" }, { 0x8D, "█" }, { 0x8E, "▌" }, { 0x8F, "▐" },
    { 0x90, "░" }, { 0x91, "▒" }, { 0x92, "▓" }, { 0x93, "⌠" },
    { 0x94, "■" }, { 0x95, "∙" }, { 0x96, "√" }, { 0x97, "≈" },
    { 0x98, "≤" }, { 0x99, "≥" }, { 0x9A, " " }, { 0x9B, "⌡" },
    { 0x9C, "°" }, { 0x9D, "²" }, { 0x9E, "·" }, { 0x9F, "÷" },
    { 0xA0, "═" }, { 0xA1, "║" }, { 0xA2, "╒" }, { 0xA3, "ё" },
    { 0xA4, "є" }, { 0xA5, "╔" }, { 0xA6, "і" }, { 0xA7, "ї" },
    { 0xA8, "╗" }, { 0xA9, "╘" }, { 0xAA, "╙" }, { 0xAB, "╚" },
    { 0xAC, "╛" }, { 0xAD, "ґ" }, { 0xAE, "╝" }, { 0xAF, "╞" },
    { 0xB0, "╟" }, { 0xB1, "╠" }, { 0xB2, "╡" }, { 0xB3, "Ё" },
    { 0xB4, "Є" }, { 0xB5, "╣" }, { 0xB6, "І" }, { 0xB7, "Ї" },
    { 0xB8, "╦" }, { 0xB9, "╧" }, { 0xBA, "╨" }, { 0xBB, "╩" },
    { 0xBC, "╪" }, { 0xBD, "Ґ" }, { 0xBE, "╬" }, { 0xBF, "©" },
    { 0xC0, "ю" }, { 0xC1, "а" }, { 0xC2, "б" }, { 0xC3, "ц" },
    { 0xC4, "д" }, { 0xC5, "е" }, { 0xC6, "ф" }, { 0xC7, "г" },
    { 0xC8, "х" }, { 0xC9, "и" }, { 0xCA, "й" }, { 0xCB, "к" },
    { 0xCC, "л" }, { 0xCD, "м" }, { 0xCE, "н" }, { 0xCF, "о" },
    { 0xD0, "п" }, { 0xD1, "я" }, { 0xD2, "р" }, { 0xD3, "с" },
    { 0xD4, "т" }, { 0xD5, "у" }, { 0xD6, "ж" }, { 0xD7, "в" },
    { 0xD8, "ь" }, { 0xD9, "ы" }, { 0xDA, "з" }, { 0xDB, "ш" },
    { 0xDC, "э" }, { 0xDD, "щ" }, { 0xDE, "ч" }, { 0xDF, "ъ" },
    { 0xE0, "Ю" }, { 0xE1, "А" }, { 0xE2, "Б" }, { 0xE3, "Ц" },
    { 0xE4, "Д" }, { 0xE5, "Е" }, { 0xE6, "Ф" }, { 0xE7, "Г" },
    { 0xE8, "Х" }, { 0xE9, "И" }, { 0xEA, "Й" }, { 0xEB, "К" },
    { 0xEC, "Л" }, { 0xED, "М" }, { 0xEE, "Н" }, { 0xEF, "О" },
    { 0xF0, "П" }, { 0xF1, "Я" }, { 0xF2, "Р" }, { 0xF3, "С" },
    { 0xF4, "Т" }, { 0xF5, "У" }, { 0xF6, "Ж" }, { 0xF7, "В" },
    { 0xF8, "Ь" }, { 0xF9, "Ы" }, { 0xFA, "З" }, { 0xFB, "Ш" },
    { 0xFC, "Э" }, { 0xFD, "Щ" }, { 0xFE, "Ч" }, { 0xFF, "Ъ" }
};

static const std::vector<koi8_entry>& get_dictionary(koi8_variant mode) {
    switch (mode) {
        case koi8_variant::koi8r:
            return koi8r_dictionary;
        case koi8_variant::koi8b:
            return koi8b_dictionary;
        case koi8_variant::koi8u:
            return koi8u_dictionary;
    }

    throw std::logic_error("invalid koi-8 variant selected");
}

const char * koi8_char_to_utf8_codepoint(uint8_t koi8c, koi8_variant mode) {
    const auto& dictionary = get_dictionary(mode);
    for (const auto& entry : dictionary) {
        if (entry.koi8 == koi8c)
            return entry.utf8;
    }
    return "";
}

char * koi8_string_to_utf8_string(const uint8_t *koi8str, koi8_variant mode) {
    const auto length = strlen((const char *) koi8str);
    char *result = new char[length * 3 + 1];
    char *where = result;

    for (size_t i = 0; i < length; ++i) {
        const char *addition = koi8_char_to_utf8_codepoint(koi8str[i], mode);
        strcpy(where, addition);
        where += strlen(addition);
    }

    return result;
}

uint8_t utf8_codepoint_to_koi8_char(const char *utf8, koi8_variant mode, size_t *step) {
    const auto& dictionary = get_dictionary(mode);
    for (const auto& entry : dictionary) {
        const size_t entry_length = strlen(entry.utf8);
        if (strncmp(entry.utf8, utf8, entry_length) == 0) {
            if (step != nullptr)
                *step = entry_length;
            return entry.koi8;
        }
    }

    if (step != nullptr)
        *step = 0;
    return 0x20;
}

uint8_t * utf8_string_to_koi8_string(const char *utf8, koi8_variant mode) {
    uint8_t *result = new uint8_t[strlen(utf8)];
    uint8_t *where = result;

    while (*utf8 != '\0') {
        size_t step;
        *where++ = utf8_codepoint_to_koi8_char(utf8, mode, &step);
        utf8 += step;
    }

    return result;
}

char * cut_top_bit_of_koi8_string(uint8_t *koi8str) {
    const auto length = strlen(reinterpret_cast<char *>(koi8str));
    for (size_t i = 0; i < length; ++i) {
        koi8str[i] = koi8str[i] & 0b01111111;
    }
    return reinterpret_cast<char *>(koi8str);
}

char * switch_case_of_ascii_string(char *utf8str) {
    const auto length = strlen(utf8str);
    for (size_t i = 0; i < length; ++i) {
        if (isupper(utf8str[i])) {
            utf8str[i] = tolower(utf8str[i]);
        } else if (islower(utf8str[i])) {
            utf8str[i] = toupper(utf8str[i]);
        }
    }
    return utf8str;
}

// Demonstration

#include <cstdio>

void print_u8_array(const uint8_t *array, size_t length) {
    fputs("{ ", stdout);
    for (size_t i = 1; i <= length; ++i) {
        fprintf(stdout, "0x%02X ", array[i - 1]);
        if (i % 8 == 0)
            fputs("\n  ", stdout);
    }
    fputs("}\n", stdout);
}

int main() {
    const uint8_t original[] = {
        0xF7, 0xCF, 0xD3, 0xD4, 0xCF, 0xCB, 0x20,
        0xEB, 0xCF, 0xCD, 0xC1, 0xCE, 0xC4, 0xC9, 0xD2, 0xD3, 0xCB, 0xC9, 0xC5, 0x2C, 0x20,
        0xFA, 0xC1, 0xCB, 0xC1, 0xDA, 0x20,
        0xED, 0xEF, 0x20,
        0xF3, 0xF3, 0xF3, 0xF2,
        0x00
    };

    const auto *utf8 = koi8_string_to_utf8_string(original, koi8_variant::koi8r);
    printf("%s\n", utf8);
    auto *koi8 = utf8_string_to_koi8_string(utf8, koi8_variant::koi8r);
    print_u8_array(koi8, strlen(reinterpret_cast<const char *> (koi8)));
    printf("%s\n", switch_case_of_ascii_string(cut_top_bit_of_koi8_string(koi8)));

    delete[] utf8;
    delete[] koi8;
    return 0;
}
