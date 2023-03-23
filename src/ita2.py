# ita2.py
# Baudot-Murray code utility functions, supporting both the international ITA2
# standard and US TTY variants.
# Author: Dénes Fintha
# Year: 2023
# ---------------------------------------------------------------------------- #

# Implementation

CODEPAGES = {
    "ita2std": {
        "letters": {
            "000x00": "\0", "000x01":  "E", "000x10": "\n", "000x11":  "A",
            "001x00":  " ", "001x01":  "S", "001x10":  "I", "001x11":  "U",
            "010x00": "\r", "010x01":  "D", "010x10":  "R", "010x11":  "J",
            "011x00":  "N", "011x01":  "F", "011x10":  "C", "011x11":  "K",
            "100x00":  "T", "100x01":  "Z", "100x10":  "L", "100x11":  "W",
            "101x00":  "H", "101x01":  "Y", "101x10":  "P", "101x11":  "Q",
            "110x00":  "O", "110x01":  "B", "110x10":  "G", "110x11":  "f",
            "111x00":  "M", "111x01":  "X", "111x10":  "V", "111x11":  "l",
        },
        "figures": {
            "000x00": "\0", "000x01":  "3", "000x10": "\n", "000x11":  "-",
            "001x00":  " ", "001x01":  "'", "001x10":  "8", "001x11":  "7",
            "010x00": "\r", "010x01":  "e", "010x10":  "4", "010x11": "\a",
            "011x00":  ",", "011x01":  "!", "011x10":  ":", "011x11":  "(",
            "100x00":  "5", "100x01":  "+", "100x10":  ")", "100x11":  "2",
            "101x00":  "£", "101x01":  "6", "101x10":  "0", "101x11":  "1",
            "110x00":  "9", "110x01":  "?", "110x10":  "&", "110x11":  "f",
            "111x00":  ".", "111x01":  "/", "111x10":  "=", "111x11":  "l",
        }
    },
    "ita2us": {
        "letters": {
            "000x00": "\0", "000x01":  "E", "000x10": "\n", "000x11":  "A",
            "001x00":  " ", "001x01":  "S", "001x10":  "I", "001x11":  "U",
            "010x00": "\r", "010x01":  "D", "010x10":  "R", "010x11":  "J",
            "011x00":  "N", "011x01":  "F", "011x10":  "C", "011x11":  "K",
            "100x00":  "T", "100x01":  "Z", "100x10":  "L", "100x11":  "W",
            "101x00":  "H", "101x01":  "Y", "101x10":  "P", "101x11":  "Q",
            "110x00":  "O", "110x01":  "B", "110x10":  "G", "110x11":  "f",
            "111x00":  "M", "111x01":  "X", "111x10":  "V", "111x11":  "l",
        },
        "figures": {
            "000x00": "\0", "000x01":  "3", "000x10": "\n", "000x11":  "-",
            "001x00":  " ", "001x01": "\a", "001x10":  "8", "001x11":  "7",
            "010x00": "\r", "010x01":  "$", "010x10":  "4", "010x11":  "'",
            "011x00":  ",", "011x01":  "!", "011x10":  ":", "011x11":  "(",
            "100x00":  "5", "100x01": "\"", "100x10":  ")", "100x11":  "2",
            "101x00":  "#", "101x01":  "6", "101x10":  "0", "101x11":  "1",
            "110x00":  "9", "110x01":  "?", "110x10":  "&", "110x11":  "f",
            "111x00":  ".", "111x01":  "/", "111x10":  ";", "111x11":  "l",
        }
    }
}

def ita2_encode(
    message: str,
    codepage: str = "ita2std",
    letters_mode: bool = True
) -> list:
    """Encodes a message with ITA2 Baudot coding."""

    def kfind(dictionary: dict, value: str) -> str:
        """Finds the first key of a value in a dictionary."""
        for key, val in dictionary.items():
            if val == value:
                return key
        return ""

    message = message.upper()
    letters = CODEPAGES[codepage]["letters"]
    figures = CODEPAGES[codepage]["figures"]
    result = [kfind(letters, "l" if letters_mode else "f")]

    for character in message:
        if letters_mode:
            if character in letters.values():
                result.append(kfind(letters, character))
            else:
                letters_mode = False
                result.append(kfind(figures, "f"))
                result.append(kfind(figures, character))
        else:
            if character in figures.values():
                result.append(kfind(figures, character))
            else:
                letters_mode = True
                result.append(kfind(letters, "l"))
                result.append(kfind(letters, character))
    return result

def ita2_decode(
    message: list,
    codepage: str = "ita2std",
    letters_mode: bool = True
) -> str:
    """Decodes a message encoded with ITA2 Baudot coding."""
    letters = CODEPAGES[codepage]["letters"]
    figures = CODEPAGES[codepage]["figures"]
    result = ""
    for code in message:
        if letters[code] == "l":
            letters_mode = True
        elif figures[code] == "f":
            letters_mode = False
        else:
            result += letters[code] if letters_mode else figures[code]
    return result

def ita2_draw(code: list) -> None:
    """
    Displays a ITA2 Baudot-encoded message in a fashion resembling punched paper
    tapes used by teletypewriters.
    """
    print("━" * (len(code) * 2 + 2))
    for i in range(6):
        print(" ", end="")
        # This is intentional, the order should be well-defined.
        # pylint: disable-next=C0200
        for j in range(len(code)):
            if code[j][5 - i] == "1":
                print("⬤", end=" ")
            elif code[j][5 - i] == "0":
                print(" ", end=" ")
            elif code[j][5 - i] == "x":
                print("●", end=" ")
        print(" ")
    print("━" * (len(code) * 2 + 2))

# Demonstration

def ita2_test() -> None:
    """
    Tests the encoding/decoding capability, using messages from Harlan Ellison's
    1967 novel, 'I Have No Mouth, and I Must Scream'.
    """

    print("")
    print(" I THINK, THEREFORE I AM")
    english = "\n\r\n\r\n\r\n\rI THINK, THEREFORE I AM\r\n\r\n\r\n\r\n"
    english_encoded = ita2_encode(english, letters_mode=False)
    ita2_draw(english_encoded[1:])
    english_decoded = ita2_decode(english_encoded)
    if english != english_decoded:
        return False

    print("")
    print(" COGITO ERGO SUM")
    latin = "\n\r\n\r\n\r\n\r\n\r\n\r\nCOGITO ERGO SUM\r\n\r\n\r\n\r\n\r\n\r\n\r"
    latin_encoded = ita2_encode(latin, letters_mode=False)
    ita2_draw(latin_encoded[1:])
    latin_decoded = ita2_decode(latin_encoded)
    if latin != latin_decoded:
        return False

    return True

if __name__ == "__main__":
    ita2_test()
