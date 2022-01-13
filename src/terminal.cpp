// terminal.cpp
// ANSI terminal styling functions.
// Author: Dénes Fintha
// Year: 2022
// -------------------------------------------------------------------------- //

// Interface

#include <iostream>

enum class terminal_color : short {
    black = 0,
    red = 1,
    green = 2,
    yellow = 3,
    blue = 4,
    magenta = 5,
    cyan = 6,
    gray = 7,
    dark_gray = black + 60,
    light_red = red + 60,
    light_green = green + 60,
    light_yellow = yellow + 60,
    light_blue = blue + 60,
    light_magenta = magenta + 60,
    light_cyan = cyan + 60,
    white = gray + 60
};

enum class terminal_effect : short {
    bold = 1,
    italic = 3,
    underline = 4,
    blink = 5,
    reverse = 7,
    strikethrough = 9
};

class terminal_modifier {
public:
    virtual ~terminal_modifier() noexcept;
    virtual void execute(std::ostream& stream) const = 0;
};

class terminal_foreground final : public terminal_modifier {
public:
    terminal_foreground(terminal_color color);
    void execute(std::ostream& stream) const override;
private:
    terminal_color color;
};

class terminal_background final : public terminal_modifier {
public:
    terminal_background(terminal_color color);
    void execute(std::ostream& stream) const override;
private:
    terminal_color color;
};

class terminal_text_effect final : public terminal_modifier {
public:
    terminal_text_effect(terminal_effect effect);
    void execute(std::ostream& stream) const override;
private:
    terminal_effect effect;
};

class terminal_reset final : public terminal_modifier {
public:
    void execute(std::ostream& stream) const override;
};

std::ostream& operator<<(std::ostream& stream, const terminal_modifier& f);

// Implementation

terminal_modifier::~terminal_modifier() noexcept {
}

terminal_foreground::terminal_foreground(terminal_color color) : color(color) {
}

void terminal_foreground::execute(std::ostream& stream) const {
    stream << "\x1B" << '[' << (static_cast<int>(color) + 30) << 'm';
}

terminal_background::terminal_background(terminal_color color) : color(color) {
}

void terminal_background::execute(std::ostream& stream) const {
    stream << "\x1B" << '[' << (static_cast<int>(color) + 40) << 'm';
}

terminal_text_effect::terminal_text_effect(terminal_effect effect) : effect(effect) {
}

void terminal_text_effect::execute(std::ostream& stream) const {
    stream << "\x1B" << '[' << static_cast<int>(effect) << 'm';
}

void terminal_reset::execute(std::ostream& stream) const {
    stream << "\x1B" << '[' << 0 << 'm';
}

std::ostream& operator<<(std::ostream& stream, const terminal_modifier& f) {
    f.execute(stream);
    return stream;
}

// Demonstration

int main() {
    std::cout
        << terminal_foreground(terminal_color::light_blue)
        << terminal_text_effect(terminal_effect::bold)
        << terminal_text_effect(terminal_effect::italic)
        << "Hello, "
        << terminal_reset()
        << "world"
        << terminal_foreground(terminal_color::light_red)
        << terminal_background(terminal_color::white)
        << "!"
        << std::endl;
    return 0;
}
