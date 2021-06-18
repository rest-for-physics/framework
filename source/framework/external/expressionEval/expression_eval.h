#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <algorithm>
#include <memory>
#include <cassert>
#include <exception>
#include <map>

// custom (basic) Either implementation

template<class T, class U>
class Either {
    bool _isLeft;
    union {
	T left;
	U right;
    };
    template <class T_, class U_> friend Either<T_, U_> Left(T_ x);
    template <class T_, class U_> friend Either<T_, U_> Right(U_ x);
public:
    bool isLeft(){
	return _isLeft;
    }
    bool isRight(){
	return !_isLeft;
    }
    T unsafeGetLeft(){
	// unsafe if not in `isLeft` check!
	return left;
    }
    U unsafeGetRight(){
	// unsafe if not in `isLeft` check!
	return right;
    }
    T getLeft(){
	if(_isLeft){
	    return left;
	}
	else{
	    throw std::runtime_error(std::string("Cannot return `left` as Either is right!"));
	}
    }
    U getRight(){
	if(!_isLeft){
	    return right;
	}
	else{
	    throw std::runtime_error(std::string("Cannot return `right` as Either is left!"));
	}
    }
};

template <class T_, class U_> Either<T_, U_> Left(T_ x){
    Either<T_, U_> result;
    result._isLeft = true;
    result.left = x;
    return result;
}

template <class T_, class U_> Either<T_, U_> Right(U_ x){
    Either<T_, U_> result;
    result._isLeft = false;
    result.right = x;
    return result;
}

using namespace std;

enum TokenKind {
    tkInvalid,
    tkIdent, tkFloat,
    tkParensOpen, tkParensClose,
    tkBracketOpen, tkBracketClose,
    tkMul, tkDiv, tkPlus, tkMinus,
    tkLess, tkGreater, tkLessEq, tkGreaterEq,
    tkEqual, tkUnequal,
    tkAnd, tkOr, tkNot,
    tkSqrt
};

static inline string toString(TokenKind tkKind){
    switch(tkKind) {
        case tkParensOpen : return "(";
        case tkParensClose : return ")";
        case tkBracketOpen : return "[";
        case tkBracketClose : return "]";
        case tkMul : return "*";
        case tkDiv : return "/";
        case tkPlus : return "+";
        case tkMinus : return "-";
        case tkLess : return "<";
        case tkGreater : return ">";
        case tkLessEq : return "<=";
        case tkGreaterEq : return ">=";
        case tkAnd : return "&&";
        case tkOr : return "||";
        case tkUnequal : return "!=";
        case tkEqual : return "==";
        case tkNot : return "!";
        case tkSqrt : return "sqrt";
        case tkIdent: return "Ident";
        case tkFloat: return "Float";
        default: return "Invalid " + tkKind;
    }
}

typedef struct Token {
    Token(TokenKind tkKind): kind(tkKind), name(toString(tkKind)) {};
    Token(TokenKind tkKind, string name): kind(tkKind), name(name) {};
    Token operator=(const Token& tok) {return tok;};
    const TokenKind kind;
    const string name;
} Token;

static inline string toString(Token tok){
    string res = toString(tok.kind);
    switch(tok.kind){
        case tkIdent:
            res += ": " + tok.name;
            break;
        case tkFloat:
            res += ": " + tok.name;
            break;
	default: break;
    }
    return res;
}

static inline Token toIdentAndClear(string &s){
    if(s.length() > 0){
        Token tok = Token(tkIdent, s);
        s.clear();
        return tok;
    }
    else{
        return Token(tkInvalid);
    }
}

enum NodeKind {
    nkUnary, nkBinary, nkFloat, nkIdent,
    nkBracketExpr, // for map access
    nkExpression // for root as well as parens
    // , nkCall (for sqrt, pow etc), ?
};

enum BinaryOpKind {
    boMul, boDiv, boPlus, boMinus,
    boLess, boGreater, boLessEq, boGreaterEq,
    boEqual, boUnequal,
    boAnd, boOr
};

enum UnaryOpKind {
    uoPlus, uoMinus, uoNot
};

static inline string toString(NodeKind kind){
    string result = "";
    switch(kind){
        case nkUnary:
	    result = "nkUnary";
            break;
	case nkBinary:
	    result = "nkBinary";
            break;
	case nkFloat:
	    result = "nkFloat";
            break;
        case nkIdent:
	    result = "nkIdent";
            break;
	case nkExpression:
	    result = "nkExpression";
	    break;
	case nkBracketExpr:
	    result = "nkBracketExpr";
	    break;

	default: break;
    }
    return result;
}

class Node {
public:
    NodeKind kind;
    virtual UnaryOpKind GetUnaryOp() {
	throw domain_error("Invalid call to `GetUnaryOp` for node of kind " + toString(kind));
	return uoPlus;}; // wtf why have to return something
    virtual shared_ptr<Node> GetUnaryNode() {
	throw domain_error("Invalid call to `GetUnaryNode` for node of kind " + toString(kind));
	return nullptr;};
    virtual void SetUnaryNode(shared_ptr<Node> node) {
	throw domain_error("Invalid call to `SetUnaryNode` for node of kind " + toString(kind));
    };
    virtual BinaryOpKind GetBinaryOp() {
	throw domain_error("Invalid call to `GetBinaryNode` for node of kind " + toString(kind));
	return boPlus;
    };
    virtual shared_ptr<Node> GetLeft() {
	throw domain_error("Invalid call to `GetLeft` for node of kind " + toString(kind));
	return nullptr;
    };
    virtual shared_ptr<Node> GetRight() {
	throw domain_error("Invalid call to `GetRight` for node of kind " + toString(kind));
	return nullptr;
    };
    virtual void SetLeft(shared_ptr<Node> node) {
	throw domain_error("Invalid call to `SetLeft` for node of kind " + toString(kind));
    };
    virtual void SetRight(shared_ptr<Node> node) {
	throw domain_error("Invalid call to `SetRight` for node of kind " + toString(kind));
    };
    virtual double GetVal() {
	throw domain_error("Invalid call to `GetVal` for node of kind " + toString(kind));
	return 0.0;
    };
    virtual string GetIdent() {
	throw domain_error("Invalid call to `GetIdent` for node of kind " + toString(kind));
	return "";
    };
    virtual shared_ptr<Node> GetExprNode() {
	throw domain_error("Invalid call to `GetExprNode` for node of kind " + toString(kind));
	return nullptr;
    };
    virtual shared_ptr<Node> GetNode() {
	throw domain_error("Invalid call to `GetNode` for node of kind " + toString(kind));
	return nullptr;
    };
    virtual shared_ptr<Node> GetArg() {
	throw domain_error("Invalid call to `GetArg` for node of kind " + toString(kind));
	return nullptr;
    };
    virtual void SetNode(shared_ptr<Node> node) {
	throw domain_error("Invalid call to `SetNode` for node of kind " + toString(kind));
    };
    virtual void SetArg(shared_ptr<Node> node) {
	throw domain_error("Invalid call to `SetArg` for node of kind " + toString(kind));
    };
    virtual void SetExprNode(shared_ptr<Node>) {
	throw domain_error("Invalid call to `SetExprNode` for node of kind " + toString(kind));
    };
};

class UnaryNode: public Node {
public:
    UnaryNode (UnaryOpKind op, shared_ptr<Node> n): op(op), n(n) {
        kind = nkUnary;
    };
    UnaryOpKind GetUnaryOp() override {return op;};
    shared_ptr<Node> GetUnaryNode() override {return n;};
    void SetUnaryNode(shared_ptr<Node> node) override {n = node;};
    UnaryOpKind op;
    shared_ptr<Node> n;
};

class BinaryNode: public Node {
public:
    BinaryNode(BinaryOpKind op, shared_ptr<Node> left, shared_ptr<Node>(right)):
        op(op), left(left), right(right) {
        kind = nkBinary;
    };
    BinaryOpKind GetBinaryOp() override {return op;};
    shared_ptr<Node> GetLeft() override {return left;};
    shared_ptr<Node> GetRight() override {return right;};
    void SetLeft(shared_ptr<Node> node) override {left = node;};
    void SetRight(shared_ptr<Node> node) override {right = node;};
    BinaryOpKind op;
    shared_ptr<Node> left;
    shared_ptr<Node> right;
};

class FloatNode: public Node {
public:
    FloatNode(double val): val(val) {
        kind = nkFloat;
    };
    double GetVal() override {return val;};
    double val;
};

class IdentNode: public Node {
public:
    IdentNode(string ident): ident(ident) {
        kind = nkIdent;
    };
    string GetIdent() override {return ident;};
    string ident;
};

class ExpressionNode: public Node {
public:
    ExpressionNode(shared_ptr<Node> n): node(n) {
	kind = nkExpression;
    };
    shared_ptr<Node> GetExprNode() override {return node;};
    void SetExprNode(shared_ptr<Node> n) override {node = n;};
    shared_ptr<Node> node;
};

class BracketExprNode: public Node {
public:
    // this node is bracket expressions, i.e. map access via
    // `foo[bar]`
    BracketExprNode(shared_ptr<Node> node, shared_ptr<Node> arg): node(node), arg(arg) {
	kind = nkBracketExpr;
    };
    shared_ptr<Node> GetNode() override { return node; };
    void SetNode(shared_ptr<Node> n) override { node = n; };
    shared_ptr<Node> GetArg() override { return arg; };
    void SetArg(shared_ptr<Node> n) override { arg = n; };
    shared_ptr<Node> node;
    shared_ptr<Node> arg;
};


using Expression = shared_ptr<Node>;

static inline BinaryOpKind toBinaryOpKind(TokenKind kind){
    switch(kind){
        case tkMul: return boMul;
        case tkDiv: return boDiv;
        case tkPlus: return boPlus;
        case tkMinus: return boMinus;
        case tkLess: return boLess;
        case tkGreater: return boGreater;
        case tkLessEq: return boLessEq;
        case tkGreaterEq: return boGreaterEq;
        case tkEqual: return boEqual;
        case tkUnequal: return boUnequal;
        case tkAnd: return boAnd;
        case tkOr: return boOr;
        default:
            throw runtime_error("Invalid binary op kind for token " + toString(kind));
    }
}

static inline string toStr(BinaryOpKind op){
    switch(op){
        case boMul: return "*";
        case boDiv: return "/";
        case boPlus: return "+";
        case boMinus: return "-";
        case boGreater: return ">";
        case boGreaterEq: return ">=";
        case boLess: return "<";
        case boLessEq: return "<=";
        case boEqual: return "==";
        case boUnequal: return "!=";
        case boAnd: return "&&";
        case boOr: return "||";
        default: return "";
    }
}

static inline UnaryOpKind toUnaryOpKind(TokenKind kind){
    switch(kind){
        case tkPlus: return uoPlus;
        case tkMinus: return uoMinus;
        case tkNot: return uoNot;
        default:
            throw runtime_error("Invalid unary op kind for token " + toString(kind));
    }
}

static inline string toStr(UnaryOpKind op){
    switch(op){
        case uoPlus: return "+";
        case uoMinus: return "-";
        case uoNot: return "!";
        default: return "";
    }
}

inline string astToStr(shared_ptr<Node> n){
    // recursively call this proc until we reach ident or float nodes.
    // NOTE: This proc assumes that the children of unary / binary are never NULL!
    if(n == NULL) return "";
    string res;
    switch(n->kind){
        case nkUnary:
            res += "(" + toStr(n->GetUnaryOp()) + " " + astToStr(n->GetUnaryNode()) + ")";
            break;
        case nkBinary:
            res += "(" + toStr(n->GetBinaryOp()) + " " + astToStr(n->GetLeft()) + " " + astToStr(n->GetRight()) + ")";
            break;
        case nkFloat:
            res += to_string(n->GetVal());
            break;
        case nkIdent:
            res += n->GetIdent();
            break;
        case nkExpression:
            res += "(" + astToStr(n->GetExprNode()) + ")";
            break;
        case nkBracketExpr:
            res += "([] " + astToStr(n->GetNode()) + " " + astToStr(n->GetArg()) + ")";
            break;
    }
    return res;
}

template <class T>
static vector<T> sliceCopy(vector<T> v, int start, int end){
    // returns inclusive (copied) sliceCopy from start to end
    vector<T> result;
    if(start < 0) return result;
    int endIdx = min(end, (int)v.size() - 1);
    for(int i = start; i <= endIdx; i++){
        result.push_back(v[i]);
    }
    return result;
}

static inline shared_ptr<Node> binaryNode(BinaryOpKind op, shared_ptr<Node> left, shared_ptr<Node> right){
    return shared_ptr<Node>(new BinaryNode(op, left, right));
}

static inline shared_ptr<Node> binaryNode(BinaryOpKind op){
    return shared_ptr<Node>(new BinaryNode(op, nullptr, nullptr));
}

static inline shared_ptr<Node> unaryNode(UnaryOpKind op, shared_ptr<Node> n){
    return shared_ptr<Node>(new UnaryNode(op, n));
}

static inline shared_ptr<Node> unaryNode(UnaryOpKind op){
    return shared_ptr<Node>(new UnaryNode(op, nullptr));
}

static inline shared_ptr<Node> expressionNode(){
    return shared_ptr<Node>(new ExpressionNode(nullptr));
}

static inline shared_ptr<Node> bracketExprNode(){
    return shared_ptr<Node>(new BracketExprNode(nullptr, nullptr));
}

static inline shared_ptr<Node> identOrFloatNode(Token tok){
    try{
        double val = stod(tok.name);
        return shared_ptr<Node>(new FloatNode(val));
    }
    catch (...) {
        // not a valid float, just an identifier
        return shared_ptr<Node>(new IdentNode(tok.name));
    }
}

static inline bool binaryOrUnary(vector<Token> tokens, int idx){
    if((idx > 0 &&
	(tokens[idx-1].kind == tkFloat ||
	 tokens[idx-1].kind == tkIdent ||
	 tokens[idx-1].kind == tkParensClose ||
	 tokens[idx-1].kind == tkBracketClose)) &&
       (((size_t)idx <= tokens.size() - 1 &&
	 (tokens[idx+1].kind == tkFloat ||
	  tokens[idx+1].kind == tkIdent ||
	  tokens[idx+1].kind == tkParensOpen ||
	  tokens[idx+1].kind == tkBracketOpen)))){
        return true;
    }
    else{
        return false;
    }
}

inline shared_ptr<Node> parseNode(Token tok, vector<Token>& tokens, int idx){
    shared_ptr<Node> result;
    switch(tok.kind){
        case tkParensOpen:
	    result = expressionNode();
            break;
        case tkParensClose:
	    throw domain_error(string("tkParensClose is an invalid token to parse. It is handled implicitly ") +
			       string("by a recursive call to `tokensToAst`!"));
	    break;
        case tkBracketOpen:
	    result = bracketExprNode();
            break;
        case tkBracketClose:
	    throw domain_error(string("tkBracketClose is an invalid token to parse. It is handled implicitly ") +
			       string("by a recursive call to `tokensToAst`!"));
	    break;
        case tkMul:
            result = binaryNode(toBinaryOpKind(tok.kind));
            break;
        case tkDiv:
            result = binaryNode(toBinaryOpKind(tok.kind));
            break;
        case tkLess:
            result = binaryNode(toBinaryOpKind(tok.kind));
            break;
        case tkGreater:
            result = binaryNode(toBinaryOpKind(tok.kind));
            break;
        case tkLessEq:
            result = binaryNode(toBinaryOpKind(tok.kind));
            break;
        case tkGreaterEq:
            result = binaryNode(toBinaryOpKind(tok.kind));
            break;
        case tkEqual:
            result = binaryNode(toBinaryOpKind(tok.kind));
            break;
        case tkUnequal:
            result = binaryNode(toBinaryOpKind(tok.kind));
            break;
        case tkAnd:
            result = binaryNode(toBinaryOpKind(tok.kind));
            break;
        case tkOr:
            result = binaryNode(toBinaryOpKind(tok.kind));
            break;
        case tkNot:
            result = unaryNode(toUnaryOpKind(tok.kind));
            break;
        case tkPlus: {// possibly binary or unary
            bool isBinary = binaryOrUnary(tokens, idx);
            if(isBinary){
                result = binaryNode(toBinaryOpKind(tok.kind));
            }
            else{
                result = unaryNode(toUnaryOpKind(tok.kind));
            }
            break;
        }

        case tkMinus: {
            bool isBinary = binaryOrUnary(tokens, idx);
            if(isBinary){
                result = binaryNode(toBinaryOpKind(tok.kind));
            }
            else{
                result = unaryNode(toUnaryOpKind(tok.kind));
            }
            break;
        }
        case tkSqrt: // do nothing
            break;
        case tkIdent:
            result = identOrFloatNode(tokens[idx]);
            break;
        case tkFloat:
            result = identOrFloatNode(tokens[idx]);
            break;
        default:
            cout << "INVALID TOKEN: " << toString(tok) << endl;
            abort();
    }
    return result;
}

static inline int getPrecedence(TokenKind tkKind){
    switch(tkKind) {
        case tkBracketOpen : return 11;
        case tkBracketClose : return 11;
        case tkParensOpen : return 10;
        case tkParensClose : return 10;
        case tkMul : return 9;
        case tkDiv : return 9;
        case tkPlus : return 8;
        case tkMinus : return 8;
        case tkLess : return 7;
        case tkGreater : return 7;
        case tkLessEq : return 7;
        case tkGreaterEq : return 7;
        case tkEqual : return 7;
        case tkUnequal : return 7;
        case tkAnd : return 6;
        case tkOr : return 6;
        case tkNot : return 6;
        case tkSqrt : return 5;
        case tkIdent: return 0;
        case tkFloat: return 0;
        default: return -1;
    }
}

static inline int getPrecedence(BinaryOpKind opKind){
    switch(opKind) {
        case boMul : return 9;
        case boDiv : return 9;
        case boPlus : return 8;
        case boMinus : return 8;
        case boLess : return 7;
        case boGreater : return 7;
        case boLessEq : return 7;
        case boGreaterEq : return 7;
        case boEqual : return 7;
        case boUnequal : return 7;
        case boAnd : return 6;
        case boOr : return 6;
        default: return -1;
    }
}

static inline int getPrecedence(UnaryOpKind opKind){
    switch(opKind) {
        case uoPlus : return 8;
        case uoMinus : return 8;
        case uoNot : return 6;
        default: return -1;
    }
}

static inline Token nextOpTok(vector<Token> tokens, int& idx){
    while((size_t)idx < tokens.size()){
        if(tokens[idx].kind > tkParensClose && tokens[idx].kind < tkSqrt){
            return tokens[idx];
        }
        else{
            idx++;
        }
    }
    return Token(tkInvalid);
}

static inline bool setLastRightNode(shared_ptr<Node> node, shared_ptr<Node> right){
    if(node->GetRight() != nullptr){
        auto bval = setLastRightNode(node->GetRight(), right);
        if(bval){
            return true;
        }
    }
    node->SetRight(right);
    return true;
}

static inline bool setLastUnaryNode(shared_ptr<Node> node, shared_ptr<Node> right){
    if(node->kind != nkUnary){
        auto bval = setLastUnaryNode(node->GetRight(), right);
        if(bval){
            return true;
        }
    }
    node->SetUnaryNode(right);
    return true;
}

static inline void setNodeWithPrecedence(shared_ptr<Node> node, int precedence){
    while(node->kind == nkBinary &&
	  ((node->GetRight()->kind == nkBinary &&
	    getPrecedence(node->GetRight()->GetBinaryOp()) < precedence) ||
	   node->GetRight() == nullptr)){
	node = node->GetRight();
    }
}

static inline void setNode(Expression& ast, shared_ptr<Node> toSet,
             int lastPrecedence, Token nextOp){
    if(toSet->kind == nkBinary){
        // have to find correct place to insert new node
	auto node = ast;
	setNodeWithPrecedence(node, getPrecedence(toSet->GetBinaryOp()));
	if(node->kind == nkBinary){
            assert(node->kind == nkBinary);
	    if(getPrecedence(node->GetBinaryOp()) < getPrecedence(toSet->GetBinaryOp())){
                auto tmp = node->GetRight();
                toSet->SetLeft(tmp);
                node->SetRight(toSet);
	    }
	    else{
		toSet->SetLeft(node);
		ast = toSet;
	    }
	}
	else{
	    toSet->SetLeft(ast);
	    ast = toSet;
	}
    }
    else{
        // traverse the result node until we find a binary node with an operator of
        // *higher* precedence than `toSet` or a nullptr node
	auto node = ast;
	setNodeWithPrecedence(node, getPrecedence(toSet->GetBinaryOp()));
        if(node->GetRight() == nullptr){
            node->SetRight(nullptr);
            toSet->SetLeft(node);
            node->SetRight(toSet);
        }
        else{
            auto tmp = node->GetRight();
            toSet->SetLeft(tmp);
            node->SetRight(toSet);
        }
    }
}

static inline int findClosingParens(vector<Token> tokens, int idx){
    int parensCounter = 0;
    for(size_t i = (size_t)idx; i < tokens.size(); i++){
	switch(tokens[i].kind){
	    case tkParensOpen:
		parensCounter++;
		break;
	    case tkParensClose:
		if(parensCounter == 0){
		    return i;
		}
		else{
		    parensCounter--;
		}
		break;
	    default: break;
	}
    }
    throw domain_error(string("Encounterd end of `findClosingParens`. Points to invalid vector of tokens") +
		       string(" because no closing parens were found. Should have thrown earlier exception") +
		       string(" in `verifyTokens`!"));
}

static inline int findClosingBracket(vector<Token> tokens, int idx){
    // TODO: could be combined with closing parens, if we hand an additional argument what
    // we are looking for
    for(size_t i = (size_t)idx; i < tokens.size(); i++){
	switch(tokens[i].kind){
	    case tkBracketOpen:
		throw domain_error(string("Nested bracket expressions are not supported."));
		break;
	    case tkBracketClose:
		return i;
	    default: break;
	}
    }
    throw domain_error(string("Encounterd end of `findClosingBracket`. Points to invalid vector of tokens") +
		       string(" because no closing parens were found. Should have thrown earlier exception") +
		       string(" in `verifyTokens`!"));
}


static inline Expression tokensToAst(vector<Token> tokens){
    // parses the given vector of tokens into an AST. Done by respecting operator precedence
    Expression result = nullptr;
    shared_ptr<Node> n;
    int idx = 0;
    int lastPrecedence = 0;
    bool lastWasUnary = false;
    while((size_t)idx < tokens.size()){
        // essentially have to skip until we find an operator based token and *not*
        // float / ident. Thene decide where to attach the resulting thing based
        // on precedence table
        int opIdx = idx;
	// current token
        auto tok = tokens[idx];
	// next operator we will encounter (may be this node)
        auto nextOp = nextOpTok(tokens, ref(opIdx));
        int precedence = getPrecedence(tok.kind);

        // order switch statement based on precedence.
        n = parseNode(tok, ref(tokens), idx);

	// for some nodes we do eager parsing, jump ahead and parse everything part that node
	// TODO: can't we do the same for regular parens and then have simpler stuff without
	// the whole set last node stuff? No, that won't help too much, because that's due to
	// parsing from left to right
	switch(nextOp.kind){
	    case tkBracketOpen: {
		// parse bracket expression directly
		// current node needs to be an ident, make that exception
		if(n->kind != nkIdent){
		    throw domain_error("Bracket expression may only be used on identifiers");
		}
		// find closing bracket of this (idx+2 because we skip ahead)
		int closingIdx = findClosingBracket(tokens, idx+2);
		// now get the argument
		auto argNode = tokensToAst(sliceCopy(tokens, idx+2, closingIdx-1));
		auto brExp = bracketExprNode();
		// assign the node and argument
		brExp->SetNode(n);
		brExp->SetArg(argNode);
		n = brExp;
		idx = closingIdx;
		break;
	    }
	    default: break;
	}
	// possiblyy fully parse the expression node
	if(n->kind == nkExpression){
	    // recurse and set the result to this expression
	    assert(n->GetExprNode() == nullptr);
	    // find closing parens of this
	    int closingIdx = findClosingParens(tokens, idx+1);
	    auto exprNode = tokensToAst(sliceCopy(tokens, idx+1, closingIdx-1));
	    n->SetExprNode(exprNode);
	    idx = closingIdx;
	}
        if(result == nullptr){
            // start by result being first node (e.g. pure string or float)
            result = n;
        }
        // check if we are looking at a unary/binary op, if so build tree.
        if(nextOp.kind == tok.kind){
            switch(n->kind){
                case nkBinary:
                    // add last element as left child
                    setNode(ref(result), n, lastPrecedence, nextOp);
                    break;
                case nkUnary:
                    // single child comes after this token
                    lastWasUnary = true;
                    // TODO: fix for case result isn't binary!!
                    setLastRightNode(result, n);
                    break;
                // else nothing to do
		default: break;
            }
        }
        else{
            switch(result->kind){
                case nkBinary:
                    // add n to result right
                    // traverse and set **last** right node
                    setLastRightNode(result, n);
                    break;
                case nkUnary:
                    // add to result node
                    result = n;
		    break;
		case nkExpression:
		    result = n;
		    break;
		case nkBracketExpr:
		    result = n;
		    break;
		default: break;
            }
            // append to last
            if(lastWasUnary){
                setLastUnaryNode(result, n);
            }
            lastWasUnary = false;
        }

        if(nextOp.kind == tok.kind){
            lastPrecedence = precedence;
        }
        idx++;
    }
#ifdef DEBUG_EXPRESSIONS
    cout << "Resulting expression " << astToStr(result) << endl;
#endif

    return result;
}

static inline void addTokenIfValid(vector<Token>& v, Token tok){
    // adds the given token to `v` if `tok` is not `tkInvalid`
    if(tok.kind != tkInvalid){
        v.push_back(tok);
    }
}

inline Either<double, bool> negative(Either<double, bool> x){
    assert(x.isLeft());
    return Left<double, bool>(-x.unsafeGetLeft());
}

inline Either<double, bool> negateCmp(Either<double, bool> x){
    // NOTE: we do *not* support arbitrary casting of floats to bools!
    assert(x.isRight());
    return Left<double, bool>(!x.unsafeGetRight());
}

inline Either<double, bool> multiply(Either<double, bool> x, Either<double, bool> y){
    assert(x.isLeft());
    assert(y.isLeft());
    return Left<double, bool>(x.unsafeGetLeft() * y.unsafeGetLeft());
}

inline Either<double, bool> divide(Either<double, bool> x, Either<double, bool> y){
    assert(x.isLeft());
    assert(y.isLeft());
    return Left<double, bool>(x.unsafeGetLeft() / y.unsafeGetLeft());
}

inline Either<double, bool> plusCmp(Either<double, bool> x, Either<double, bool> y){
    assert(x.isLeft());
    assert(y.isLeft());
    return Left<double, bool>(x.unsafeGetLeft() + y.unsafeGetLeft());
}

inline Either<double, bool> minusCmp(Either<double, bool> x, Either<double, bool> y){
    assert(x.isLeft());
    assert(y.isLeft());
    return Left<double, bool>(x.unsafeGetLeft() - y.unsafeGetLeft());
}

inline Either<double, bool> lessCmp(Either<double, bool> x, Either<double, bool> y){
    assert(x.isLeft());
    assert(y.isLeft());
    return Right<double, bool>(x.unsafeGetLeft() < y.unsafeGetLeft());
}

inline Either<double, bool> greaterCmp(Either<double, bool> x, Either<double, bool> y){
    assert(x.isLeft());
    assert(y.isLeft());
    return Right<double, bool>(x.unsafeGetLeft() > y.unsafeGetLeft());
}

inline Either<double, bool> lessEq(Either<double, bool> x, Either<double, bool> y){
    assert(x.isLeft());
    assert(y.isLeft());
    return Right<double, bool>(x.unsafeGetLeft() <= y.unsafeGetLeft());
}

inline Either<double, bool> greaterEq(Either<double, bool> x, Either<double, bool> y){
    assert(x.isLeft());
    assert(y.isLeft());
    return Right<double, bool>(x.unsafeGetLeft() >= y.unsafeGetLeft());
}

inline Either<double, bool> equal(Either<double, bool> x, Either<double, bool> y){
    if(x.isLeft() && y.isLeft()){
	// TODO: make comparison float precision safe
	return Right<double, bool>(x.unsafeGetLeft() == y.unsafeGetLeft());
    }
    else if(x.isRight() && y.isRight()){
	return Right<double, bool>(x.unsafeGetRight() == y.unsafeGetRight());
    }
    else{
	throw domain_error("Cannot compare a float and a bool for equality!");
    }
}

inline Either<double, bool> unequal(Either<double, bool> x, Either<double, bool> y){
    if(x.isLeft() && y.isLeft()){
	return Right<double, bool>(x.unsafeGetLeft() != y.unsafeGetLeft());
    }
    else if(x.isRight() && y.isRight()){
	return Right<double, bool>(x.unsafeGetRight() != y.unsafeGetRight());
    }
    else{
	throw domain_error("Cannot compare a float and a bool for inequality!");
    }
}

inline Either<double, bool> andCmp(Either<double, bool> x, Either<double, bool> y){
    if(x.isRight() && y.isRight()){
	return Right<double, bool>(x.unsafeGetRight() && y.unsafeGetRight());
    }
    else{
	throw domain_error("Cannot compute `and` of float and bool or two floats!");
    }
}

inline Either<double, bool> orCmp(Either<double, bool> x, Either<double, bool> y){
    if(x.isRight() && y.isRight()){
	return Right<double, bool>(x.unsafeGetRight() || y.unsafeGetRight());
    }
    else{
	throw domain_error("Cannot compute `or` of float and bool or two floats!");
    }
}

inline Either<double, bool> evaluate(map<string, float> m, map<string, map<int, float>> maps, shared_ptr<Node> n){
    switch(n->kind){
	case nkBinary:
	    // recurse on both childern
	    switch(n->GetBinaryOp()){
		case boMul: return multiply(evaluate(m, maps, n->GetLeft()), evaluate(m, maps, n->GetRight()));
		case boDiv: return divide(evaluate(m, maps, n->GetLeft()), evaluate(m, maps, n->GetRight()));
		case boPlus: return plusCmp(evaluate(m, maps, n->GetLeft()), evaluate(m, maps, n->GetRight()));
		case boMinus: return minusCmp(evaluate(m, maps, n->GetLeft()), evaluate(m, maps, n->GetRight()));
		case boLess: return lessCmp(evaluate(m, maps, n->GetLeft()), evaluate(m, maps, n->GetRight()));
		case boGreater: return greaterCmp(evaluate(m, maps, n->GetLeft()), evaluate(m, maps, n->GetRight()));
		case boLessEq: return lessEq(evaluate(m, maps, n->GetLeft()), evaluate(m, maps, n->GetRight()));
		case boGreaterEq: return greaterEq(evaluate(m, maps, n->GetLeft()), evaluate(m, maps, n->GetRight()));
		case boEqual: return equal(evaluate(m, maps, n->GetLeft()), evaluate(m, maps, n->GetRight()));
		case boUnequal: return unequal(evaluate(m, maps, n->GetLeft()), evaluate(m, maps, n->GetRight()));
		case boAnd: return andCmp(evaluate(m, maps, n->GetLeft()), evaluate(m, maps, n->GetRight()));
		case boOr: return orCmp(evaluate(m, maps, n->GetLeft()), evaluate(m, maps, n->GetRight()));
		default:
		    throw runtime_error("Invalid binary op kind for token " + astToStr(n));
	    }
	case nkUnary:
	    // apply unary op
	    switch(n->GetUnaryOp()){
		case uoPlus: return evaluate(m, maps, n->GetUnaryNode());
		case uoMinus: return negative(evaluate(m, maps, n->GetUnaryNode()));
		case uoNot: return negateCmp(evaluate(m, maps, n->GetUnaryNode()));
	    }
	case nkIdent:
	    // return value stored for ident
	    // TODO: check if element exists in map (/in REST)
	    return Left<double, bool>(m[n->GetIdent()]);
	case nkFloat:
	    return Left<double, bool>(n->GetVal());
	case nkExpression:
	    return evaluate(m, maps, n->GetExprNode());
	case nkBracketExpr:
	    // get identifier from maps
	    auto obs = n->GetNode();
	    auto mapObs = maps[obs->GetIdent()];
	    auto arg = n->GetArg();
	    return Left<double, bool>(mapObs[(int)arg->GetVal()]);
    }
    throw logic_error("Invalid code branch in `evaluate`. Should never end up here!");
}

static inline void raiseIfLastOp(bool lastWasBinaryOp, bool lastWasUnaryOp, bool lastWasIdentOrFloat = false){
    if(!lastWasIdentOrFloat && lastWasBinaryOp){
	throw runtime_error("Parsing of expression failed. Binary operation followed by binary operation!");
    }
    else if(!lastWasIdentOrFloat && lastWasUnaryOp){
	throw runtime_error("Parsing of expression failed. Unary operation followed by binary operation!");
    }
}

static inline void verifyTokens(vector<Token> tokens){
    bool lastWasBinaryOp = false;
    bool lastWasUnaryOp = false;
    bool lastWasIdentOrFloat = false;
    int parensCounter = 0;
    bool inBracket = false;
    //int idx = 0;
    for(auto tok : tokens){
	switch(tok.kind){
	    case tkParensOpen :
		parensCounter++;
		break;
	    case tkParensClose :
		parensCounter--;
		if(parensCounter < 0){
		    throw runtime_error("Parsing of expression faild. Closing parenthesis before opening!");
		}
		break;
	    case tkBracketOpen :
		if(inBracket){
		    throw runtime_error("Nested bracket expressions not allowed.");
		}
		inBracket = true;
		break;
	    case tkBracketClose :
		if(!inBracket){
		    throw runtime_error("Closing bracket without opening bracket not allowed.");
		}
		inBracket = false;
		break;
	    case tkMul :
		raiseIfLastOp(lastWasBinaryOp, lastWasUnaryOp, lastWasIdentOrFloat);
		lastWasBinaryOp = true;
		lastWasUnaryOp = false;
		break;
	    case tkDiv :
		raiseIfLastOp(lastWasBinaryOp, lastWasUnaryOp, lastWasIdentOrFloat);
		lastWasBinaryOp = true;
		lastWasUnaryOp = false;
		break;
	    case tkPlus :
		// TODO: check for unary!
		raiseIfLastOp(lastWasBinaryOp, lastWasUnaryOp, lastWasIdentOrFloat);
		lastWasBinaryOp = true;
		lastWasUnaryOp = false;
		break;
	    case tkMinus :
		raiseIfLastOp(lastWasBinaryOp, lastWasUnaryOp, lastWasIdentOrFloat);
		lastWasBinaryOp = true;
		lastWasUnaryOp = false;
		break;
	    case tkLess :
		raiseIfLastOp(lastWasBinaryOp, lastWasUnaryOp, lastWasIdentOrFloat);
		lastWasBinaryOp = true;
		lastWasUnaryOp = false;
		break;
	    case tkGreater :
		raiseIfLastOp(lastWasBinaryOp, lastWasUnaryOp, lastWasIdentOrFloat);
		lastWasBinaryOp = true;
		lastWasUnaryOp = false;
		break;
	    case tkLessEq :
		raiseIfLastOp(lastWasBinaryOp, lastWasUnaryOp, lastWasIdentOrFloat);
		lastWasBinaryOp = true;
		lastWasUnaryOp = false;
		break;
	    case tkGreaterEq :
		raiseIfLastOp(lastWasBinaryOp, lastWasUnaryOp, lastWasIdentOrFloat);
		lastWasBinaryOp = true;
		lastWasUnaryOp = false;
		break;
	    case tkAnd :
		raiseIfLastOp(lastWasBinaryOp, lastWasUnaryOp, lastWasIdentOrFloat);
		lastWasBinaryOp = true;
		lastWasUnaryOp = false;
		break;
	    case tkOr :
		raiseIfLastOp(lastWasBinaryOp, lastWasUnaryOp, lastWasIdentOrFloat);
		lastWasBinaryOp = true;
		lastWasUnaryOp = false;
		break;
	    case tkUnequal :
		raiseIfLastOp(lastWasBinaryOp, lastWasUnaryOp, lastWasIdentOrFloat);
		lastWasBinaryOp = true;
		lastWasUnaryOp = false;
		break;
	    case tkEqual :
		raiseIfLastOp(lastWasBinaryOp, lastWasUnaryOp, lastWasIdentOrFloat);
		lastWasBinaryOp = true;
		lastWasUnaryOp = false;
		break;
	    case tkNot :
		raiseIfLastOp(lastWasBinaryOp, lastWasUnaryOp, lastWasIdentOrFloat);
		lastWasUnaryOp = true;
		lastWasBinaryOp = false;
		break;
	    case tkIdent:
		lastWasIdentOrFloat = true;
		lastWasBinaryOp = false;
		lastWasUnaryOp = false;
		break;
	    case tkFloat:
		lastWasIdentOrFloat = true;
		lastWasBinaryOp = false;
		lastWasUnaryOp = false;
		break;
	    default:
		throw runtime_error("Token of kind " + toString(tok.kind) + " is not supported yet!");
	}
    }
    if(parensCounter != 0){
	throw runtime_error("Parsing of expression faild. Closing parentheses missing!");
    }
    if(lastWasBinaryOp){
	throw runtime_error("Parsing of expression faild. Last token was a binary token!");
    }
    if(lastWasUnaryOp){
	throw runtime_error("Parsing of expression faild. Last token was a unary token!");
    }
    assert(lastWasIdentOrFloat);
}

inline Expression parseExpression(string s){
    int idx = 0;
    vector<Token> tokens;
    string curBuf;
    while((size_t)idx < s.length()){
        switch(s[idx]){
            // single char tokens
            case '(':
                addTokenIfValid(ref(tokens), toIdentAndClear(ref(curBuf)));
                addTokenIfValid(ref(tokens), Token(tkParensOpen));
                break;
            case ')':
                addTokenIfValid(ref(tokens), toIdentAndClear(ref(curBuf)));
                addTokenIfValid(ref(tokens), Token(tkParensClose));
                break;
            case '[':
                addTokenIfValid(ref(tokens), toIdentAndClear(ref(curBuf)));
                addTokenIfValid(ref(tokens), Token(tkBracketOpen));
                break;
            case ']':
                addTokenIfValid(ref(tokens), toIdentAndClear(ref(curBuf)));
                addTokenIfValid(ref(tokens), Token(tkBracketClose));
                break;
            case '*':
                addTokenIfValid(ref(tokens), toIdentAndClear(ref(curBuf)));
                addTokenIfValid(ref(tokens), Token(tkMul));
                break;
            case '/':
                addTokenIfValid(ref(tokens), toIdentAndClear(ref(curBuf)));
                addTokenIfValid(ref(tokens), Token(tkDiv));
                break;
            case '+':
                addTokenIfValid(ref(tokens), toIdentAndClear(ref(curBuf)));
                addTokenIfValid(ref(tokens), Token(tkPlus));
                break;
            case '-':
                addTokenIfValid(ref(tokens), toIdentAndClear(ref(curBuf)));
                addTokenIfValid(ref(tokens), Token(tkMinus));
                break;
            // possible multi char tokens, i.e. accumulate until next token?
            case '<':
                // peek next
                addTokenIfValid(ref(tokens), toIdentAndClear(ref(curBuf)));
                if((size_t)(idx + 1) < s.length() && s[idx + 1] == '='){
                    addTokenIfValid(ref(tokens), Token(tkLessEq));
                    idx++;
                }
                else{
                    addTokenIfValid(ref(tokens), Token(tkLess));
                }
                break;
            case '>':
                addTokenIfValid(ref(tokens), toIdentAndClear(ref(curBuf)));
                if((size_t)(idx + 1) < s.length() && s[idx + 1] == '='){
                    addTokenIfValid(ref(tokens), Token(tkGreaterEq));
                    idx++;
                }
                else{
                    addTokenIfValid(ref(tokens), Token(tkGreater));
                }
                break;
            case '&':
		// TODO: replace by `and` and `or`
                addTokenIfValid(ref(tokens), toIdentAndClear(ref(curBuf)));
                // assert after is another &
                if(!((size_t)(idx + 1) < s.length()) && s[idx+1] != '&'){
                    // raise a parsing error
                    throw runtime_error("Bad formula! " + to_string(idx) + "  " + to_string(s.length()));
                }
                addTokenIfValid(ref(tokens), Token(tkAnd));
                // skip the next element
                idx++;
                break;
            case '|':
		// TODO: replace by `and` and `or`
                addTokenIfValid(ref(tokens), toIdentAndClear(ref(curBuf)));
                // assert after is another &
                if(!((size_t)(idx + 1) < s.length()) && s[idx+1] != '|'){
                    // raise a parsing error
                    throw runtime_error("Bad formula! " + to_string(idx) + "  " + to_string(s.length()));
                }
                addTokenIfValid(ref(tokens), Token(tkOr));
                // skip the next element
                idx++;
                break;
            case '!':
                addTokenIfValid(ref(tokens), toIdentAndClear(ref(curBuf)));
                // assert after is another =
                if(((size_t)(idx + 1) < s.length()) && s[idx+1] == '='){
		    // is unequal
		    addTokenIfValid(ref(tokens), Token(tkUnequal));
		    // skip the next element
		    idx++;
                }
		else if(((size_t)(idx + 1) < s.length()) && s[idx+1] != ' '){
		    // should be unary `!`
		    // TODO we should ideally have a `s[idx+1]` in letters / numbers check
		    addTokenIfValid(ref(tokens), Token(tkNot));
		}
		else{
                    // raise a parsing error
                    throw runtime_error("Bad formula! " + to_string(idx) + "  " + to_string(s.length()) + ". `!` followed by space invalid!");
		}
                break;
            case '=':
                addTokenIfValid(ref(tokens), toIdentAndClear(ref(curBuf)));
                // assert after is another =
                if(!((size_t)(idx + 1) < s.length()) && s[idx+1] != '='){
                    // raise a parsing error
                    throw runtime_error("Bad formula! " + to_string(idx) + "  " + to_string(s.length()));
                }
                addTokenIfValid(ref(tokens), Token(tkEqual));
                // skip the next element
                idx++;
                break;
            case ' ':
		// check if previous was an `and` / `or`. If so is operator!
		if(curBuf == "and"){
		    addTokenIfValid(ref(tokens), Token(tkAnd));
		    curBuf.clear();
		}
		else if(curBuf == "or"){
		    addTokenIfValid(ref(tokens), Token(tkOr));
		    curBuf.clear();
		}
		else if(curBuf == "not"){
		    addTokenIfValid(ref(tokens), Token(tkNot));
		    curBuf.clear();
		}
		else{
		    // else might be an identifier or float
		    addTokenIfValid(ref(tokens), toIdentAndClear(ref(curBuf)));
		}
                break;
            default:
                curBuf += s[idx];
        }
        idx++;
    }
    addTokenIfValid(ref(tokens), toIdentAndClear(ref(curBuf)));

    // possibly raise if order of tokens is bad / invalid input
    verifyTokens(tokens);

#ifdef DEBUG_EXPRESSIONS
    int i = 0;
    for(auto tok : tokens){
        cout << "i " << i << "  " << toString(tok) << endl;
        i++;
    }
#endif
    return tokensToAst(tokens);
}
