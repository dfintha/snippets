#define PP_ISEMPTY(...)                 ((#__VA_ARGS__)[0] == '\0')

#define PP_CAT2(x1, x2)                 x1 ## x2
#define PP_XCAT2(x1, x2)                PP_CAT2(x1, x2)
#define PP_XCAT3(x1, x2, x3)            PP_XCAT2(x1, PP_XCAT2(x2, x3))
#define PP_XCAT4(x1, x2, x3, x4)        PP_XCAT2(x1, PP_XCAT3(x2, x3, x4))
#define PP_XCAT5(x1, x2, x3, x4, x5)    PP_XCAT2(x1, PP_XCAT4(x2, x3, x4, x5))

#define PP_STR(x)                       #x
#define PP_XSTR(x)                      PP_STR(x)

#define PP_WSCOPE_BEGIN                 do {
#define PP_WSCOPE_END                   } while (0)

#define PP_LSCOPE_BEGIN                 [&] {
#define PP_LSCOPE_END                   }

#if defined(__GNUC__)
#define PP_UNUSED(...) \
    PP_XCAT3(__VA_ARGS__, _unused_line, __LINE__) __attribute__((unused))
#else
#define PP_UNUSED(...) \
    PP_XCAT3(__VA_ARGS__, _unused_line, __LINE__)
#endif

#include <stdio.h>

int main(PP_UNUSED(int argc), PP_UNUSED(char **argv)) {
    printf("%s\n", PP_ISEMPTY() ? "Yes" : "No");
    printf("%s\n", PP_ISEMPTY(X) ? "No" : "Yes");

    int PP_XCAT3(my_, int_, variable);
    my_int_variable = 42;

#define X 42
    printf("%s\n", PP_STR(X));
    printf("%s\n", PP_XSTR(X));
#undef X

    return 0;
}
