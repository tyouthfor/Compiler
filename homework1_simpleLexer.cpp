#include<map>
#include<cassert>
#include<string>
#include<iostream>

#define TODO assert(0 && "TODO")
//#define DEBUG_DFA

enum class State
{
    Empty,       // space, \n, \r, ...
    IntLiteral,  // int literal, like '1', '01900', '0xAB', '0b11001', ...
    op           // operators and '(', ')'.
};

std::string toString(State s)
{
    switch (s) {
        case State::Empty: return "Empty";
        case State::IntLiteral: return "IntLiteral";
        case State::op: return "op";
        default: assert(0 && "invalid State");
    }
    return "";
}

enum class TokenType
{
    INTLTR,   // int literal
    PLUS,     // '+'
    MINU,     // '-'
    MULT,     // '*'
    DIV,      // '/'
    LPARENT,  // '('
    RPARENT   // ')'
};

std::string toString(TokenType type)
{
    switch (type)
    {
        case TokenType::INTLTR: return "INTLTR";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINU: return "MINU";
        case TokenType::MULT: return "MULT";
        case TokenType::DIV: return "DIV";
        case TokenType::LPARENT: return "LPARENT";
        case TokenType::RPARENT: return "RPARENT";
        default: assert(0 && "invalid token type");
    }
    return "";
}

struct Token
{
    TokenType type;
    std::string value;
};

struct DFA
{
    DFA();
    ~DFA();

    // Do not allow copy and assignment
    DFA(const DFA&) = delete;
    DFA& operator=(const DFA&) = delete;

    /**
      *  Func: Take a char as input, change state to next state, and output a Token if necessary.
      *  @param[in] input: The input character.
      *  @param[out] buf: The  output Token buffer
      *  @return: True if a Token is produced, the buf is valid.
      */
    bool next(char input, Token& buf);

    /**
      *  Func: Reset the DFA state to begin.
      */
    void reset();

private:
    State cur_state;  // Record current state of the DFA
    std::string cur_str;  // Record input characters
};


DFA::DFA() : cur_state(State::Empty), cur_str() {}


DFA::~DFA() {}


//bool is_operator(char c)
//{
//    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')' || c == '\n');
//}


TokenType get_op_type(std::string s)
{
    if (s.size() > 1) {
        return TokenType::INTLTR;
    }
    char c = s[0];
    switch (c) {
        case '+': return TokenType::PLUS;
        case '-': return TokenType::MINU;
        case '*': return TokenType::MULT;
        case '/': return TokenType::DIV;
        case '(': return TokenType::LPARENT;
        case ')': return TokenType::RPARENT;
        default: break;
    }
}


bool DFA::next(char input, Token& buf)
{
    if (input == ' ') {
        return false;
    }

    cur_str += input;

    switch (cur_state) {
        case State::Empty: {
            if ((input >= '0' && input <= '9') || (input >= 'a' && input <= 'z') || (input >= 'A' && input <= 'Z')) {
                cur_state = State::IntLiteral;
                return false;
            }
            else if (input == '(' || input == '-') {
                cur_state = State::op;
                buf.type = get_op_type(cur_str);
                buf.value = cur_str;
                cur_str = "";
                return true;
            }
            else {
                cur_state = State::Empty;
                cur_str = "";
                return false;
            }
        }
        
        case State::IntLiteral: {

            // 同一个数
            if ((input >= '0' && input <= '9') || (input >= 'a' && input <= 'z') || (input >= 'A' && input <= 'Z')) {
                cur_state = State::IntLiteral;
                return false;
            }

            // cur_str = "当前的数 + input character"
            else if (input == '+' || input == '-' || input == '*' || input == '/' || input == '(' || input == ')' || input == '\n') {
                cur_state = State::op;
                buf.type = TokenType::INTLTR;
                cur_str.pop_back();
                buf.value = cur_str;  // 当前的数
                cur_str = input;  // input character
                return true;
            }
            else {
                cur_state = State::Empty;
                cur_str = "";
                return false;
            }
        }
        

    case State::op:
        if (cur_str.size() == 2) {
            if ((input >= '0' && input <= '9') || (input >= 'a' && input <= 'z') || (input >= 'A' && input <= 'Z')) {
                cur_state = State::IntLiteral;
            }
            else if (input == '+' || input == '-' || input == '*' || input == '/' || input == '(' || input == ')' || input == '\n') {
                cur_state = State::op;
            }
            else {
                cur_state = State::Empty;
            }
            cur_str.pop_back();
            buf.type = get_op_type(cur_str);
            buf.value = cur_str;
            cur_str = input;
            return true;
        }
        else {
            if ((input >= '0' && input <= '9') || (input >= 'a' && input <= 'z') || (input >= 'A' && input <= 'Z')) {
                cur_state = State::IntLiteral;
                buf.type = TokenType::INTLTR;
                buf.value = input;
                return false;
            }
            else if (input == '+' || input == '-' || input == '*' || input == '/' || input == '(' || input == ')' || input == '\n') {
                cur_state = State::op;
                buf.type = get_op_type(cur_str);
                buf.value = cur_str;
                cur_str = "";
                return true;
            }
            else {
                cur_state = State::Empty;
                cur_str = "";
                return false;
            }
        }

    default:
        return false;
    }
}

void DFA::reset()
{
    cur_state = State::Empty;
    cur_str = "";
}

int main()
{
    std::string stdin_str;
    std::getline(std::cin, stdin_str);
    stdin_str += "\n";

    DFA dfa;
    Token tk;

    int n = stdin_str.size();
    for (size_t i = 0; i < n; ++i) {
        if (dfa.next(stdin_str[i], tk)) {
            std::cout << toString(tk.type) << "  " << tk.value << '\n';
        }
    }
    return 0;
}