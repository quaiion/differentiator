#ifndef BIN_NODE_ACTIVE
#define BIN_NODE_ACTIVE

#include "common.hpp"

enum NODE_TYPE {BINARY_OPERATION, UNARY_OPERATION, CONSTANT, VARIABLE, DISTRUCTED};
enum OPERATION_TYPE {PLUS, MINUS, MULT, DIV, SIN, COS, TAN, COT, ASIN, ACOS, ATAN, ACOT, LOG, LN, POW, EXP};

const char * const OPERATION_SYMB [] = {"+" /* PLUS */, "-" /* MINUS */, "*" /* MULT */, "/" /* DIV */,                 // Индексы в массиве совмещены с константами в enum
                                        "sin" /* SIN */, "cos" /* COS */, "tan" /* TAN */, "cot" /* COT */,
                                        "arcsin" /* ASIN */, "arccos" /* ACOS */, "arctan" /* ATAN */,
                                        "arccot" /* ACOT */, "log" /* LOG */, "ln" /* LN */, "^" /* POW */,
                                        "exp" /* EXP */};

OPERATION_TYPE UNARY_OPERATIONS [] = {PLUS, MINUS, SIN, COS, TAN, COT, ASIN, ACOS, ATAN, ACOT, LN, EXP};
OPERATION_TYPE BINARY_OPERATIONS [] = {PLUS, MINUS, MULT, DIV, LOG, POW};
constexpr int NUM_OF_UNARY_OPERATIONS = 12;
constexpr int NUM_OF_BINARY_OPERATIONS = 6;

struct bin_node_t {

    long long data;
    bin_node_t *right;
    bin_node_t *left;
    NODE_TYPE type;
};

#endif
