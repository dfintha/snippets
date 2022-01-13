// cpaint.js
// Simple source code colorization for C/C++ code snippets in HTML DOM
// text elements.
// Author: Dénes Fintha
// Year: 2022
// -------------------------------------------------------------------------- //

// Implementation

function cpaint(textNode) {
    let re_keyword = new RegExp(
        "\\b(alignas|alignof|and|and_eq|asm|atomic_cancel|atomic_commit|atomic_noexcept|auto|" +
        "bitand|bitor|bool|break|case|catch|char|char8_t|char16_t|char32_t|class|compl|concept|" +
        "const|consteval|constexpr|constinit|const_cast|continue|co_await|co_return|co_yield|" +
        "decltype|default|delete|do|double|dynamic_cast|else|enum|explicit|export|extern|false|" +
        "float|for|friend|goto|if|inline|int|long|mutable|namespace|new|noexcept|not|not_eq|" +
        "nullptr|operator|or|or_eq|private|protected|public|reflexpr|register|reinterpret_cast|" +
        "requires|return|short|signed|sizeof|static|static_assert|static_cast|struct|switch|" +
        "synchronized|template|this|thread_local|throw|true|try|typedef|typeid|typename|union|" +
        "unsigned|using|virtual|void|volatile|wchar_t|while|xor|xor_eq)\\b",
        "gm"
    );
    let re_clib_type = new RegExp(
        "\\b(ssize_t|tm|time_t|clock_t|timespec|thrd_t|mtx_t|cnd_t|tss_t|fpos_t|atomic_bool|" +
        "atomic_char|atomic_schar|atomic_uchar|atomic_short|atomic_ushort|atomic_int|atomic_uint|" +
        "atomic_long|atomic_ulong|atomic_llong|atomic_ullong|atomic_char16_t|atomic_char32_t|" +
        "atomic_wchar_t|atomic_int_least8_t|atomic_uint_least8_t|atomic_int_least16_t|" +
        "atomic_uint_least16_t|atomic_int_least32_t|atomic_uint_least32_t|atomic_int_least64_t|" +
        "atomic_uint_least64_t|atomic_int_fast8_t|atomic_uint_fast8_t|atomic_int_fast16_t|" +
        "atomic_uint_fast16_t|atomic_int_fast32_t|atomic_uint_fast32_t|atomic_int_fast64_t|" +
        "atomic_uint_fast64_t|atomic_intptr_t|atomic_uintptr_t|atomic_size_t|atomic_ptrdiff_t|" +
        "atomic_intmax_t|atomic_uintmax_t|memory_order|atomic_flag|va_list|ptrdiff_t|max_align_t|" +
        "div_t|ldiv_t|lldiv_t|imaxdiv_t|float_t|double_t|size_t|FILE|fenv_t|fexcept_t|int8_t|" +
        "int16_t|int32_t|int64_t|int_fast8_t|int_fast16_t|int_fast32_t|int_fast64_t|int_least8_t|" +
        "int_least16_t|int_least32_t|int_least64_t|intmax_t|intptr_t|uint8_t|uint16_t|uint32_t|" +
        "uint64_t|uint_fast8_t|uint_fast16_t|uint_fast32_t|uint_fast64_t|uint_least8_t|" +
        "uint_least16_t|uint_least32_t|uint_least64_t|uintmax_t|uintptr_t)\\b",
        "gm"
    );
    let re_preprocessor = new RegExp("^(\\s*#.*)$", "gm");
    let re_int_number = new RegExp("\\b((?:0x[0-9a-fA-F]+|0b[01]+|[0-9]+)[Ll]{0,2})\\b", "gm");
    let re_float_number = new RegExp("\\b((?:\.[0-9]+|[0-9]+\.|[0-9]+\.[0-9]+)[LlFf]?)\\b", "gm");
    let re_string = new RegExp("(\\\".*\\\")", "gm");
    let re_line_comment = new RegExp("(//.*$)", "gm");
    let re_interval_comment = new RegExp("(/\\*.*\\*/)", "gms");

    textNode.innerHTML = textNode.innerHTML
        .replace(re_keyword, "<span class='keyword'>$1</span>")
        .replace(re_clib_type, "<span class='typename'>$1</span>")
        .replace(re_int_number, "<span class='value'>$1</span>")
        .replace(re_float_number, "<span class='value'>$1</span>")
        .replace(re_string, "<span class='value'>$1</span>")
        .replace(re_line_comment, "<span class='comment'>$1</span>")
        .replace(re_interval_comment, "<span class='comment'>$1</span>")
        .replace(re_preprocessor, "<span class='preprocessor'>$1</span>");
}
