/*
Exp  ->  AddExp

		Exp.v

Number  ->  IntConst  |  floatConst

PrimaryExp  ->  '('  Exp  ')'  |  Number
		PrimaryExp.v

UnaryExp  ->  PrimaryExp  |  UnaryOp  UnaryExp
		UnaryExp.v

UnaryOp  ->  '+'  |  '-'

MulExp  ->  UnaryExp  {  ('*'  |  '/')  UnaryExp  }
		MulExp.v

AddExp  ->  MulExp  {  ('+'  |  '-')  MulExp  }
		AddExp.v
*/
#include<map>
#include<cassert>
#include<string>
#include<iostream>
#include<vector>
#include<set>
#include<queue>

#define TODO assert(0 && "TODO")
//#define DEBUG_DFA
//#define DEBUG_PARSER

int char2digit(char c);
int compute_value(std::string str);

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


struct  DFA {
	DFA();
	~DFA();

	// Do not allow copy and assignment
	DFA(const DFA&) = delete;
	DFA& operator=(const DFA&) = delete;

	/**
	  *  Name: next
	  *  Func: Take a char as input, change state to next state, and output a Token if necessary.
	  *  @param[in] input: The input character.
	  *  @param[out] buf: The output Token buffer
	  *  @return: True if a Token is produced, i.e. the buf is valid.
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
	case State::Empty:
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

	case State::IntLiteral:
		if ((input >= '0' && input <= '9') || (input >= 'a' && input <= 'z') || (input >= 'A' && input <= 'Z')) {
			cur_state = State::IntLiteral;
			return false;
		}
		else if (input == '+' || input == '-' || input == '*' || input == '/' || input == '(' || input == ')' || input == '\n') {
			cur_state = State::op;
			buf.type = TokenType::INTLTR;
			cur_str.pop_back();
			buf.value = cur_str;
			cur_str = input;
			return true;
		}
		else {
			cur_state = State::Empty;
			cur_str = "";
			return false;
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

// hw2
enum class NodeType {
	TERMINAL,  // 终结符
	EXP,
	NUMBER,
	PRIMARYEXP,
	UNARYEXP,
	UNARYOP,
	MULEXP,
	ADDEXP,
	NONE
};

std::string toString(NodeType nt) {
	switch (nt) {
	case  NodeType::TERMINAL:  return  "Terminal";
	case  NodeType::EXP:  return  "Exp";
	case  NodeType::NUMBER:  return  "Number";
	case  NodeType::PRIMARYEXP:  return  "PrimaryExp";
	case  NodeType::UNARYEXP:  return  "UnaryExp";
	case  NodeType::UNARYOP:  return  "UnaryOp";
	case  NodeType::MULEXP:  return  "MulExp";
	case  NodeType::ADDEXP:  return  "AddExp";
	case  NodeType::NONE:  return  "NONE";
	default:
		assert(0 && "invalid  node  type");
		break;
	}
	return  "";
}

// 语法树结点类
struct AstNode {
	int value;
	NodeType type;  // node type
	AstNode* parent;
	std::vector<AstNode*> children;

	AstNode(NodeType t = NodeType::NONE, AstNode* p = nullptr) : type(t), parent(p), value(0) {}

	virtual  ~AstNode() 
	{
		for (auto child : children) {
			delete child;
		}
	}

	AstNode(const AstNode&) = delete;
	AstNode& operator=(const AstNode&) = delete;
};

// 语法分析器 Parser 类
// take a token stream as input, then parsing it, output a AST
struct Parser {
	uint32_t index;  // current token index
	const std::vector<Token>& token_stream;

	Parser(const std::vector<Token>& tokens) : index(0), token_stream(tokens) {}
	~Parser() {}

	/**
	  *  @brief  creat  the  abstract  syntax  tree
	  *  @return  the  root  of  abstract  syntax  tree
	  */
	AstNode* get_abstract_syntax_tree() {
		if (token_stream.size() == 0) {
			return nullptr;
		}

		AstNode* root = new AstNode(NodeType::EXP, nullptr);
		if (parse_Exp(root)) {
			return root;
		}
		else {
			return nullptr;
		}
	}

	// Exp  ->  AddExp
	bool parse_Exp(AstNode* root);

	// AddExp  ->  MulExp  {  ('+'  |  '-')  MulExp  }
	bool parse_AddExp(AstNode* root);

	// MulExp  ->  UnaryExp  {  ('*'  |  '/')  UnaryExp  }
	bool parse_MulExp(AstNode* root);

	// UnaryExp  ->  PrimaryExp  |  UnaryOp  UnaryExp
	bool parse_UnaryExp(AstNode* root);

	// PrimaryExp  ->  '('  Exp  ')'  |  Number
	bool parse_PrimaryExp(AstNode* root);

	// UnaryOp  ->  '+'  |  '-'
	bool parse_UnaryOp(AstNode* root);

	// Number  ->  IntConst  |  floatConst
	bool parse_Number(AstNode* root);

// for debug, u r not required to use this
// how to use this: in ur local enviroment, defines the macro DEBUG_PARSER and add this function in every parse fuction
	void log(AstNode* node) {
#ifdef DEBUG_PARSER
		std::cout << "in parse" << toString(node->type) << ", cur_token_type::" << toString(token_stream[index].type) << ", token_val::" << token_stream[index].value << 'n';
#endif
	}

};


bool Parser::parse_Exp(AstNode* root)
{
	AstNode* child = new AstNode(NodeType::ADDEXP, root);
	if (parse_AddExp(child)) {
		root->children.push_back(child);
		root->value = child->value;
		return true;
	}
	else {
		root->children.clear();
		return false;
	}
}


bool Parser::parse_AddExp(AstNode* root)
{
	AstNode* child_1 = new AstNode(NodeType::MULEXP, root);
	if (parse_MulExp(child_1)) {
		root->children.push_back(child_1);
		root->value = child_1->value;
	}
	else {
		root->children.clear();
		return false;
	}

	// 不进入 { } 内
	if (index >= token_stream.size()) {
		return true;
	}

	int count = 0;
	while (token_stream[index].type == TokenType::PLUS || token_stream[index].type == TokenType::MINU) {
		int record = index;  // 记录运算符位置

		AstNode* child_2 = new AstNode(NodeType::TERMINAL, root);
		root->children.push_back(child_2);
		index++;
		count++;

		AstNode* child_3 = new AstNode(NodeType::MULEXP, root);
		if (parse_MulExp(child_3)) {
			root->children.push_back(child_3);
			if (token_stream[record].type == TokenType::PLUS) {
				root->value += child_3->value;
			}
			else {
				root->value -= child_3->value;
			}
		}
		else {
			root->children.clear();
			while (count--) {
				index--;
			}
			return false;
		}

		if (index >= token_stream.size()) {
			break;
		}
	}

	return true;
}


bool Parser::parse_MulExp(AstNode* root)
{
	AstNode* child_1 = new AstNode(NodeType::UNARYEXP, root);
	if (parse_UnaryExp(child_1)) {
		root->children.push_back(child_1);
		root->value = child_1->value;
	}
	else {
		root->children.clear();
		return false;
	}

	// 不进入 { } 内
	if (index >= token_stream.size()) {
		return true;
	}

	int count = 0;
	while (token_stream[index].type == TokenType::MULT || token_stream[index].type == TokenType::DIV) {
		int record = index;  // 记录运算符位置

		AstNode* child_2 = new AstNode(NodeType::TERMINAL, root);
		root->children.push_back(child_2);
		index++;
		count++;

		AstNode* child_3 = new AstNode(NodeType::UNARYEXP, root);
		if (parse_UnaryExp(child_3)) {
			root->children.push_back(child_3);
			if (token_stream[record].type == TokenType::MULT) {
				root->value *= child_3->value;
			}
			else {
				root->value /= child_3->value;
			}
		}
		else {
			root->children.clear();
			while (count--) {
				index--;
			}
			return false;
		}

		if (index >= token_stream.size()) {
			break;
		}
	}

	return true;
}


bool Parser::parse_UnaryExp(AstNode* root)
{
	AstNode* child_1 = new AstNode(NodeType::PRIMARYEXP, root);
	if (parse_PrimaryExp(child_1)) {
		root->children.push_back(child_1);
		root->value = child_1->value;
		return true;
	}

	int record = index;  // 记录 UnaryOp 的符号位置
	AstNode* child_2 = new AstNode(NodeType::UNARYOP, root);
	if (parse_UnaryOp(child_2)) {
		root->children.push_back(child_2);
	}
	else {
		root->children.clear();
		return false;
	}

	AstNode* child_3 = new AstNode(NodeType::UNARYEXP, root);
	if (parse_UnaryExp(child_3)) {
		root->children.push_back(child_3);
		if (token_stream[record].type == TokenType::PLUS) {
			root->value = child_3->value;
		}
		else {
			root->value = -child_3->value;
		}
	}
	else {
		root->children.clear();
		return false;
	}

	return true;
}


bool Parser::parse_PrimaryExp(AstNode* root)
{
	AstNode* child = new AstNode(NodeType::NUMBER, root);
	if (parse_Number(child)) {
		root->children.push_back(child);
		root->value = child->value;
		return true;
	}

	if (token_stream[index].type == TokenType::LPARENT) {
		AstNode* child_1 = new AstNode(NodeType::TERMINAL, root);
		root->children.push_back(child_1);
		index++;
	}
	else {
		root->children.clear();
		return false;
	}

	AstNode* child_2 = new AstNode(NodeType::EXP, root);
	if (parse_Exp(child_2)) {
		root->children.push_back(child_2);
		root->value = child_2->value;
	}
	else {
		root->children.clear();
		index--;
		return false;
	}

	if (token_stream[index].type == TokenType::RPARENT) {
		AstNode* child_3 = new AstNode(NodeType::TERMINAL, root);
		root->children.push_back(child_3);
		index++;
	}
	else {
		root->children.clear();
		root->value = 0;
		index -= 2;
		return false;
	}

	return true;
}


bool Parser::parse_UnaryOp(AstNode* root)
{
	if (token_stream[index].type == TokenType::PLUS || token_stream[index].type == TokenType::MINU) {
		AstNode* child = new AstNode(NodeType::TERMINAL, root);
		root->children.push_back(child);
		index++;
		return true;
	}
	else {
		root->children.clear();
		return false;
	}
}


bool Parser::parse_Number(AstNode* root)
{
	if (token_stream[index].type == TokenType::INTLTR) {
		AstNode* child = new AstNode(NodeType::TERMINAL, root);
		child->value = compute_value(token_stream[index].value);
		root->children.push_back(child);
		root->value = child->value;
		index++;
		return true;
	}
	else {
		root->children.clear();
		return false;
	}
}


int char2digit(char c)
{
	return c - '0';
}

int compute_value(std::string str)
{
	if (str.empty()) {
		return 0;
	}

	int result = 0;
	size_t i = 0;

	// 1. 十六进制、八进制、二进制
	if (i < str.size() && str[i] == '0') {
		i++;

		// 十六进制
		if (i < str.size() && str[i] == 'x') {
			i++;
			while (i < str.size()) {
				char c = std::tolower(str[i]);
				if (std::isdigit(c)) {
					result = result * 16 + char2digit(c);
				}
				else if (c >= 'a' && c <= 'f') {
					result = result * 16 + (c - 'a' + 10);
				}
				else {
					break;
				}
				i++;
			}
		}

		// 二进制
		else if (i < str.size() && str[i] == 'b') {
			i++;
			while (i < str.size() && std::isdigit(str[i]) && str[i] == '0' || str[i] == '1') {
				result = result * 2 + char2digit(str[i]);
				i++;
			}
		}

		// 八进制
		else {
			while (i < str.size() && std::isdigit(str[i]) && str[i] >= '0' && str[i] <= '7') {
				result = result * 8 + char2digit(str[i]);
				i++;
			}
		}
	}

	// 2. 十进制
	else {
		while (i < str.size() && std::isdigit(str[i])) {
			result = result * 10 + char2digit(str[i]);
			i++;
		}
	}

	return result;
}


int main() 
{
	std::string stdin_str;
	std::getline(std::cin, stdin_str);
	stdin_str += "\n";

	DFA dfa;
	Token tk;
	std::vector<Token> tokens;

	for (size_t i = 0; i < stdin_str.size(); i++) {
		if (dfa.next(stdin_str[i], tk)) {
			std::cout << toString(tk.type) << "  " << tk.value << '\n';
			tokens.push_back(tk);
		}
	}

	//  hw2
	Parser parser(tokens);
	auto root = parser.get_abstract_syntax_tree();
	std::cout << root->value;

	return  0;
}