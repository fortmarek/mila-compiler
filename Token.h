//
// Created by Marek Fořt on 2020-05-10.
//

#ifndef SIMPLEFRONTEND_TOKEN_H
#define SIMPLEFRONTEND_TOKEN_H

#include <string>

enum Kind {
    tok_eof =           -1,
    tok_unspecified = 0,

    // numbers and identifiers
            tok_identifier =    -2,
    tok_number =        -3,

    // keywords
            tok_begin =         -4,
    tok_end =           -5,
    tok_const =         -6,
    tok_procedure =     -7,
    tok_forward =       -8,
    tok_function =      -9,
    tok_if =            -10,
    tok_then =          -11,
    tok_else =          -12,
    tok_program =       -13,
    tok_while =         -14,
    tok_exit =          -15,
    tok_var =           -16,
    tok_integer =       -17,
    tok_for =           -18,
    tok_do =            -19,
    tok_break = -42,

    // 2-character operators
    tok_notequal =      -20,
    tok_lessequal =     -21,
    tok_greaterequal =  -22,
    tok_assign =        -23,
    tok_or =            -24,

    // 3-character operators (keywords)
            tok_mod =           -25,
    tok_div =           -26,
    tok_not =           -27,
    tok_and =           -28,
    tok_xor =           -29,

    // keywords in for loop
            tok_to =            -30,
    tok_downto =        -31,

    // 1-character operators
    tok_init = -32,
    tok_type = -33,
    tok_dot = -34,
    tok_left_paren = -35,
    tok_right_paren = -36,
    tok_multiplier = -37,
    tok_plus = -38,
    tok_minus = -39,
    tok_division = -40,
    tok_divider = -41,
    tok_comma = -42,
    tok_greater = -43,
    tok_less = -44,
};


class Token {
public:
    Token(Kind kind, std::string value);
    Token() = default;
    Kind getKind();
    std::string getValue();
private:
    Kind kind;
    std::string value;
};

#endif //SIMPLEFRONTEND_TOKEN_H
