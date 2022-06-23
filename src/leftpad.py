# leftpad.py
# The infamous leftpad function, rewritten in Python 3.
# Author: Dénes Fintha
# Year: 2022
# ---------------------------------------------------------------------------- #

# Implementation

import typing

def leftpad(string: str, length: int, fill: typing.Union[str, int]) -> str:
    old_length = len(string)
    new_length = old_length if length <= old_length else length
    pad_length = new_length - old_length

    if isinstance(fill, str):
        return fill[0] * pad_length + string

    if isinstance(fill, int):
        return str(fill)[0] * pad_length + string

    raise TypeError("fill can be of either type str or int")

# Demonstration

import typing

def test_leftpad(
    description: str,
    string: str,
    length: int,
    fill: typing.Union[str, int],
    expected: str
) -> None:
    print(f"Testing if {description}")
    result = leftpad(string, length, fill)
    print(f"  Expected: '{expected}'")
    print(f"       Got: '{result}'")
    print(f"    Result: {'Passed' if result == expected else 'Failed'}\n")

def main() -> None:
    test_leftpad(
        "padding an empty string to a length to 0 results in an empty string",
        string="",
        length=0,
        fill=' ',
        expected=""
    )

    test_leftpad(
        "padding to a shorter length results in the same string",
        string="foo",
        length=2,
        fill=' ',
        expected="foo"
    )

    test_leftpad(
        "padding to a negative length results in the same string",
        string="foo",
        length=-2,
        fill=' ',
        expected="foo"
    )

    test_leftpad(
        "padding a non-empty string to its length results in the same string",
        string="foo",
        length=3,
        fill=' ',
        expected="foo"
    )

    test_leftpad(
        "padding to a longer string with a single character fills to the left",
        string="foo",
        length=4,
        fill='_',
        expected="_foo"
    )

    test_leftpad(
        "padding to a longer string with a number fills with its first digit",
        string="foo",
        length=4,
        fill=12,
        expected="1foo"
    )

    test_leftpad(
        "padding to a longer string with a negative number fills with -",
        string="foo",
        length=4,
        fill=-12,
        expected="-foo"
    )

    test_leftpad(
        "padding to a longer string with a string fills with its first char",
        string="foo",
        length=4,
        fill="abc",
        expected="afoo"
    )

if __name__ == "__main__":
    main()
