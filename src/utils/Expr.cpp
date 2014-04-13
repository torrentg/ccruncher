
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2014 Gerard Torrent
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//===========================================================================

#include <stack>
#include <iostream>
#include <cmath>
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <cassert>
#include "Expr.hpp"

using namespace std;
using namespace ccruncher;

#define CHAR_PARENTHESIS_OPEN '('
#define CHAR_PARENTHESIS_CLOSE ')'
#define CHAR_ARGUMENT_SEPARATOR ','

//=============================================================
// supported functions
//=============================================================
namespace ccruncher {

  //! Minus function (negate)
  double fminus(double x) { return -x; }
  //! Plus function (identity)
  double fplus(double x) { return +x; }
  //! Sqrt function
  double fsqrt(double x) { return sqrt(x); }
  //! Sin function
  double fsin(double x) { return sin(x); }
  //! Cos function
  double fcos(double x) { return cos(x); }
  //! Tan function
  double ftan(double x) { return tan(x); }
  //! Exponential function
  double fexp(double x) { return exp(x); }
  //! Logarithm function
  double flog(double x) { return log(x); }
  //! Absolute value function
  double ffabs(double x) { return fabs(x); }
  //! Addition function
  double fsum(double x, double y) { return x+y; }
  //! Subtraction function
  double frest(double x, double y) { return x-y; }
  //! Multiplication function
  double fprod(double x, double y) { return x*y; }
  //! Division function
  double fdiv(double x, double y) { return x/y; }
  //! Pow function
  double fpow(double x, double y) { return pow(x,y); }
  //! Minimum function
  double fmin(double x, double y) { return min(x,y); }
  //! Maximum function
  double fmax(double x, double y) { return max(x,y); }
  //! If function
  double fif(double x, double y, double z) { return (x!=0.0?y:z); }

} // namespace

/**
 * @todo Consider logical operators (eg. &&, ||).
 */
const Expr::operador ccruncher::Expr::operators[] =
{
  { "+", 0, true, &fsum },
  { "-", 0, true, &frest },
  { "*", 1, true, &fprod },
  { "/", 1, true, &fdiv },
  { "^", 2, true, &fpow }  // left associativity (like octave): 2^3^4=(2^3)^4
};

#define NUMOPERATORS (sizeof(operators)/sizeof(operador))

const Expr::function ccruncher::Expr::functions[] =
{
  { "+", 0, (ffunc0) &fplus },
  { "-", 0, (ffunc0) &fminus },
  { "sqrt", 1, (ffunc0) &fsqrt },
  { "sin", 1, (ffunc0) &fsin },
  { "cos", 1, (ffunc0) &fcos },
  { "tan", 1, (ffunc0) &ftan },
  { "abs", 1, (ffunc0) &ffabs },
  { "exp", 1, (ffunc0) &fexp },
  { "log", 1, (ffunc0) &flog },
  { "min", 2, (ffunc0) &fmin },
  { "max", 2, (ffunc0) &fmax },
  { "pow", 2, (ffunc0) &fpow },
  { "if", 3, (ffunc0) &fif }
};

#define NUMFUNCTIONS (sizeof(functions)/sizeof(function))

const Expr::constant ccruncher::Expr::constants[] =
{
  { "Pi", M_PI },
  { "E", 2.71828182845904523536 }
};

#define NUMCONSTANTS (sizeof(constants)/sizeof(constant))

/**************************************************************************//**
 * @details Check if input string starts with a function name.
 * @param[in] ptr Input string.
 * @param[out] tok Token to fill if function is identified.
 * @param[out] endptr Next character after identified function.
 * @return true = function found, false = function not found
 */
bool ccruncher::Expr::isFunction(const char *ptr, token *tok, char **endptr)
{
  for(int i=0; i<int(NUMFUNCTIONS); i++)
  {
    size_t len = strlen(functions[i].id);
    if(strncmp(ptr, functions[i].id, len) == 0 && !isalnum(*(ptr+len)))
    {
      tok->type = FUNCTION;
      tok->dat.n = i;
      *endptr = const_cast<char*>(ptr+len);
      return true;
    }
  }
  return false;
}

/**************************************************************************//**
 * @details Check if input string starts with a constant identifier.
 * @param[in] ptr Input string.
 * @param[out] tok Token to fill if constant is found.
 * @param[out] endptr Next character after constant.
 * @return true = constant found, false = constant not found
 */
bool ccruncher::Expr::isConstant(const char *ptr, token *tok, char **endptr)
{
  for(int i=0; i<int(NUMCONSTANTS); i++)
  {
    size_t len = strlen(constants[i].id);
    if(strncmp(ptr, constants[i].id, len) == 0 && !isalnum(*(ptr+len)))
    {
      tok->type = CONSTANT;
      tok->dat.n = i;
      *endptr = const_cast<char*>(ptr+len);
      return true;
    }
  }
  return false;
}

/**************************************************************************//**
 * @details Check if input string starts with an operator.
 * @param[in] ptr Input string.
 * @param[out] tok Token to fill if operator is found.
 * @param[out] endptr Next character after operator.
 * @return true = operator found, false = operator not found
 */
bool ccruncher::Expr::isOperator(const char *ptr, token *tok, char **endptr)
{
  for(int i=0; i<int(NUMOPERATORS); i++)
  {
    size_t len = strlen(operators[i].id);
    if(strncmp(ptr, operators[i].id, len) == 0)
    {
      tok->type = OPERATOR;
      tok->dat.n = i;
      *endptr = const_cast<char*>(ptr+len);
      return true;
    }
  }
  return false;
}

/**************************************************************************//**
 * @details Check if input string starts with a number.
 * @param[in] ptr Input string.
 * @param[out] tok Token to fill if number is found.
 * @param[out] endptr Next character after number.
 * @return true = number found, false = number not found
 */
bool ccruncher::Expr::isNumber(const char *ptr, token *tok, char **endptr)
{
  tok->dat.x = strtod(ptr, endptr);

  if (ptr == *endptr || *endptr == nullptr)
  {
    return false;
  }
  else
  {
    tok->type = NUMBER;
    return true;
  }
}

/**************************************************************************//**
 * @details Check if input string starts with a variable. A variable follows
 *          the pattern <code>[_a-zA-Z][_a-zA-Z0-9]*</code>.
 *          If a variable is identified and don't exist in the variables
 *          list then adds the variable in the list.
 * @param[in] ptr Input string.
 * @param[out] tok Token to fill if variable is found.
 * @param[out] endptr Next character after variable.
 * @param[in,out] variables List of variables.
 * @return true = variable found, false = variable not found
 */
bool ccruncher::Expr::isVariable(const char *ptr, token *tok, char **endptr, std::vector<variable> &variables)
{
  for(size_t i=0; i<variables.size(); i++)
  {
    size_t len = variables[i].id.size();
    if(strncmp(ptr, variables[i].id.c_str(), len) == 0 && !isalnum(*(ptr+len)) && *(ptr+len) != '_')
    {
      tok->type = VARIABLE;
      tok->dat.n = i;
      *endptr = const_cast<char*>(ptr+len);
      return true;
    }
  }

  variable var("",nullptr);
  while(isalnum(*ptr) || *ptr == '_')
  {
    var.id.push_back(*ptr);
    ptr++;
  }

  //TODO: check that isn't only composed by _'s
  if (var.id.length()>0 && !isdigit(var.id[0]))
  {
    var.ptr = nullptr;
    variables.push_back(var);
    tok->type = VARIABLE;
    tok->dat.n = variables.size()-1;
    *endptr = const_cast<char*>(ptr);
    return true;
  }

  return false;
}

/**************************************************************************//**
 * @param[in] ptr Input string.
 * @param[out] tok Token to fill.
 * @param[out] endptr Next character after the identified substring.
 * @param[in,out] variables List of variables.
 * @throw Exception Unrecognized text.
 */
void ccruncher::Expr::next(const char *ptr, token *tok, char **endptr, std::vector<variable> &variables) throw(Exception)
{
  while(isspace(*ptr)) ptr++;

  if (*ptr == '\0')
  {
    tok->type = END;
    *endptr = const_cast<char*>(ptr);
    return;
  }
  else if (*ptr == CHAR_PARENTHESIS_OPEN)
  {
    tok->type = PARENTHESIS_OPEN;
    tok->dat.n = 0;
    *endptr = const_cast<char*>(ptr+1);
    return;
  }
  else if (*ptr == CHAR_PARENTHESIS_CLOSE)
  {
    tok->type = PARENTHESIS_CLOSE;
    tok->dat.n = 0;
    *endptr = const_cast<char*>(ptr+1);
    return;
  }
  else if (*ptr == CHAR_ARGUMENT_SEPARATOR)
  {
    tok->type = ARGUMENT_SEPARATOR;
    tok->dat.n = 0;
    *endptr = const_cast<char*>(ptr+1);
    return;
  }
  else if (isOperator(ptr, tok, endptr)) return;
  else if (isNumber(ptr, tok, endptr)) return;
  else if (isFunction(ptr, tok, endptr)) return;
  else if (isConstant(ptr, tok, endptr)) return;
  else if (isVariable(ptr, tok, endptr, variables)) return;
  else throw Exception("unrecognized keyword");
}

/**************************************************************************//**
 * @param[in] type Type of token.
 * @return true if token is a number/variable/constant, false otherwise.
 */
inline bool ccruncher::Expr::isValue(TokenType type)
{
  if (type == NUMBER || type == VARIABLE || type == CONSTANT) return true;
  else return false;
}

/**************************************************************************//**
 * @details Checking is based on previous token analysis. Checks that
 *         requires additional info (eg. parentesis matching, number of
 *         arguments) are made directly in the Expr::compile method.
 * @param[in] prevtok Previous token.
 * @param[in] curtok Current token.
 * @throw Exception Syntax error.
 */
void ccruncher::Expr::check(token *prevtok, token *curtok) throw(Exception)
{
  if (prevtok->type == FUNCTION && functions[prevtok->dat.n].args == 0)
  {
    if (isValue(curtok->type) || (curtok->type==FUNCTION && functions[curtok->dat.n].args != 0) || curtok->type == PARENTHESIS_OPEN) return;
    else throw Exception("syntax error");
  }
  else if (isValue(curtok->type) || curtok->type == FUNCTION)
  {
    if (isValue(prevtok->type) || prevtok->type == FUNCTION || prevtok->type == PARENTHESIS_CLOSE) throw Exception("syntax error");
    else return;
  }
  else if (curtok->type == OPERATOR)
  {
    if (prevtok->type == OPERATOR || prevtok->type == FUNCTION || prevtok->type == PARENTHESIS_OPEN || prevtok->type == ARGUMENT_SEPARATOR || prevtok->type == END) throw Exception("syntax error");
    else return;
  }
  else if (curtok->type == PARENTHESIS_OPEN)
  {
    if (isValue(prevtok->type) || prevtok->type == PARENTHESIS_CLOSE) throw Exception("syntax error");
    else return;
  }
  else if (curtok->type == PARENTHESIS_CLOSE || curtok->type == ARGUMENT_SEPARATOR || curtok->type == END)
  {
    if (prevtok->type == OPERATOR || prevtok->type == FUNCTION || prevtok->type == PARENTHESIS_OPEN || prevtok->type == ARGUMENT_SEPARATOR || prevtok->type == END) throw Exception("syntax error");
    else return;
  }
  assert(false);
  throw Exception("unexpected token");
}

/**************************************************************************//**
 * @details Apply simplification rules in order to reduce the number of
 *         tokens in the RPN stack and the steps to perform in the
 *         expression evaluation.
 * @param[in] tok Token to push to the RPN stack.
 * @param[in,out] tokens RPN stack.
 */
void ccruncher::Expr::push(token &tok, std::vector<token> &tokens)
{
  tokens.push_back(tok);
  int pos = tokens.size()-1;

  while(true)
  {
    // RULE: NUMBER -> no simplification
    if (tokens[pos].type==NUMBER || tokens[pos].type==END)
    {
      break;
    }

    // RULE: replace CONSTANT -> NUMBER
    if (tokens[pos].type==CONSTANT)
    {
      tokens[pos].type = NUMBER;
      tokens[pos].dat.x = constants[tokens[pos].dat.n].x;
      break;
    }

    // RULE: remove plus sign
    if (tokens[pos].type==FUNCTION && tokens[pos].dat.n==0)
    {
      tokens.pop_back();
      pos--;
      break;
    }

    // RULE: remove double minus signs (eg. -(-x) -> x)
    if (pos >= 2 && tokens[pos].type==FUNCTION && tokens[pos].dat.n==1 && tokens[pos-1].type==FUNCTION && tokens[pos-1].dat.n==1)
    {
      tokens.pop_back();
      tokens.pop_back();
      pos -= 2;
      break;
    }

    // RULE: simplify minus sign (eg. -subexp*3 -> subexp*(-3), -subexp/3 -> subexp/(-3))
    if (pos >= 2 && tokens[pos].type==OPERATOR && (tokens[pos].dat.n==2 || tokens[pos].dat.n==3) && tokens[pos-1].type==NUMBER && tokens[pos-2].type==FUNCTION && tokens[pos-2].dat.n==1)
    {
      tokens[pos-2].type = NUMBER;
      tokens[pos-2].dat.x = -tokens[pos-1].dat.x;
      tokens[pos-1] = tokens[pos];
      tokens.pop_back();
      pos--;
      continue;
    }

    // RULE: numbers always at the end (eg. 1+x -> x+1, 2*x -> x*2)
    if (pos >= 2 && tokens[pos].type==OPERATOR && (tokens[pos].dat.n==0 || tokens[pos].dat.n==2) && tokens[pos-1].type==VARIABLE && tokens[pos-2].type==NUMBER)
    {
      token aux = tokens[pos-2];
      tokens[pos-2] = tokens[pos-1];
      tokens[pos-1] = aux;
      continue;
    }

    // RULE: two consecutive operators with equal precedence
    if (pos >= 3 && tokens[pos].type==OPERATOR && tokens[pos-1].type==NUMBER && tokens[pos-2].type==OPERATOR && operators[tokens[pos].dat.n].prec==operators[tokens[pos-2].dat.n].prec && tokens[pos-3].type==NUMBER)
    {
      // precedence '+' (eg. subexp+1+2 -> subexp+3, subexp+1-2 -> subexp+(-1), subexp-1+2 -> subexp+1, subexp-1-2 -> subexp+(-3))
      if (operators[tokens[pos].dat.n].prec == 0)
      {
        if (tokens[pos].dat.n == 1) tokens[pos-1].dat.x *= -1.0;
        if (tokens[pos-2].dat.n == 1) tokens[pos-3].dat.x *= -1.0;
        tokens[pos-3].dat.x += tokens[pos-1].dat.x;
        tokens[pos-2].dat.n = 0;
        tokens.pop_back();
        tokens.pop_back();
        pos -= 2;
        continue;
      }
      // precedence '*' (eg. subexp*1*2 -> subexp*2, subexp*1/2 -> subexp*(1/2), subexp/1*2 -> subexp*(2/1), subexp/1/2 -> subexp*(1/(1*2)))
      else if (operators[tokens[pos].dat.n].prec == 1)
      {
        if (tokens[pos].dat.n == 3) tokens[pos-1].dat.x = 1.0/tokens[pos-1].dat.x;
        if (tokens[pos-2].dat.n == 3) tokens[pos-3].dat.x = 1.0/tokens[pos-3].dat.x;
        tokens[pos-3].dat.x *= tokens[pos-1].dat.x;
        tokens[pos-2].dat.n = 2;
        tokens.pop_back();
        tokens.pop_back();
        pos -= 2;
        continue;
      }
      // precedence '^' (eg. subexp^1^2 -> subexp^(1*2))
      // only if left-associative, then x^y^z = (x^y)^z = x^(y*z)
      // see http://www.tcl.tk/cgi-bin/tct/tip/274.html
      else if (operators[tokens[pos].dat.n].prec == 2 && operators[tokens[pos].dat.n].left)
      {
        tokens[pos-3].dat.x *= tokens[pos-1].dat.x;
        tokens.pop_back();
        tokens.pop_back();
        pos -= 2;
        continue;
      }
    }

    // RULE: apply operator
    if (pos >= 2 && tokens[pos].type == OPERATOR && tokens[pos-1].type == NUMBER && tokens[pos-2].type == NUMBER)
    {
      tokens[pos-2].type = NUMBER;
      tokens[pos-2].dat.x = operators[tokens[pos].dat.n].func(tokens[pos-2].dat.x,tokens[pos-1].dat.x);
      tokens.pop_back();
      tokens.pop_back();
      pos -= 2;
      break;
    }

    // RULE: apply function
    if (tokens[pos].type == FUNCTION)
    {
      if (pos >= 1 && functions[tokens[pos].dat.n].args <= 1 && tokens[pos-1].type == NUMBER)
      {
        ffunc1 f = (ffunc1) functions[tokens[pos].dat.n].ptr;
        tokens[pos-1].type = NUMBER;
        tokens[pos-1].dat.x = f(tokens[pos-1].dat.x);
        tokens.pop_back();
        pos--;
        break;
      }

      if (pos >= 2 && functions[tokens[pos].dat.n].args == 2 && tokens[pos-1].type == NUMBER && tokens[pos-2].type == NUMBER)
      {
        ffunc2 f = (ffunc2) functions[tokens[pos].dat.n].ptr;
        tokens[pos-2].type = NUMBER;
        tokens[pos-2].dat.x = f(tokens[pos-2].dat.x,tokens[pos-1].dat.x);
        tokens.pop_back();
        tokens.pop_back();
        pos -= 2;
        break;
      }

      if (pos >= 3 && functions[tokens[pos].dat.n].args == 3 && tokens[pos-1].type == NUMBER && tokens[pos-2].type == NUMBER && tokens[pos-3].type == NUMBER)
      {
        ffunc3 f = (ffunc3) functions[tokens[pos].dat.n].ptr;
        tokens[pos-3].type = NUMBER;
        tokens[pos-3].dat.x = f(tokens[pos-3].dat.x,tokens[pos-2].dat.x,tokens[pos-1].dat.x);
        tokens.pop_back();
        tokens.pop_back();
        tokens.pop_back();
        pos -= 3;
        break;
      }
    }

    break;
  }
}

/**************************************************************************//**
 * @details Compile an expression using the Shunting-yard algorithm.
 * @see http://en.wikipedia.org/wiki/Shunting-yard_algorithm
 * @param[in] str Input string containing the expression.
 * @param[in,out] variables List of variables.
 * @param[out] tokens RPN stack. Previous tokens are not cleared.
 * @throw Exception Syntax error.
 */
void ccruncher::Expr::compile(const std::string &str, std::vector<variable> &variables, std::vector<token> &tokens) throw(Exception)
{
  return compile(str.c_str(), variables, tokens);
}

/**************************************************************************//**
 * @details Compile an expression using the Shunting-yard algorithm.
 * @see http://en.wikipedia.org/wiki/Shunting-yard_algorithm
 * @param[in] str Input string containing the expression.
 * @param[in,out] variables List of variables.
 * @param[out] tokens RPN stack. Previous tokens are not cleared.
 * @throw Exception Syntax error.
 */
void ccruncher::Expr::compile(const char *str, std::vector<variable> &variables, std::vector<token> &tokens) throw(Exception)
{
  assert(str != nullptr);
  token prevtok, curtok;
  stack<token, vector<token> > pile;
  stack<int, vector<int> > numargs;
  const char *ptr = str;
  char *endptr;

  prevtok.type = END;

  try
  {
    while(true)
    {
      next(ptr, &curtok, &endptr,variables);

      // special cases: '+' and '-' functions
      if (curtok.type == OPERATOR && (curtok.dat.n == 0 || curtok.dat.n == 1))
      {
        if (prevtok.type == END || prevtok.type == PARENTHESIS_OPEN || prevtok.type == ARGUMENT_SEPARATOR)
        {
          curtok.type = FUNCTION;
        }
      }

      check(&prevtok, &curtok);

      if (isValue(curtok.type))
      {
        push(curtok, tokens);
      }
      else if (curtok.type == FUNCTION)
      {
        pile.push(curtok);
      }
      else if (curtok.type == ARGUMENT_SEPARATOR)
      {
        while(!pile.empty() && pile.top().type != PARENTHESIS_OPEN)
        {
          push(pile.top(), tokens);
          pile.pop();
        }

        if (pile.empty() || pile.top().type != PARENTHESIS_OPEN) throw Exception("parenthesis matching");

        assert(!numargs.empty());
        numargs.top()++;
      }
      else if (curtok.type == OPERATOR)
      {
        if (!pile.empty() && pile.top().type == FUNCTION && functions[pile.top().dat.n].args == 0)
        {
          push(pile.top(), tokens);
          pile.pop();
        }

        if (operators[curtok.dat.n].left)
        {
          while(!pile.empty() && pile.top().type == OPERATOR && operators[curtok.dat.n].prec <= operators[pile.top().dat.n].prec)
          {
            push(pile.top(), tokens);
            pile.pop();
          }
        }
        else
        {
          while(!pile.empty() && pile.top().type == OPERATOR && operators[curtok.dat.n].prec < operators[pile.top().dat.n].prec)
          {
            push(pile.top(), tokens);
            pile.pop();
          }
        }
        pile.push(curtok);
      }
      else if (curtok.type == PARENTHESIS_OPEN)
      {
        pile.push(curtok);
        numargs.push(1);
      }
      else if (curtok.type == PARENTHESIS_CLOSE)
      {
        while(!pile.empty() && pile.top().type != PARENTHESIS_OPEN)
        {
          push(pile.top(), tokens);
          pile.pop();
        }

        if (!pile.empty() && pile.top().type == PARENTHESIS_OPEN) pile.pop();
        else throw Exception("parenthesis matching");

        if ((pile.empty() || pile.top().type != FUNCTION) && numargs.top() != 1) {
          throw Exception("unexpected arguments");
        }

        if (!pile.empty() && pile.top().type == FUNCTION && functions[pile.top().dat.n].args > 0)
        {
          int n = functions[pile.top().dat.n].args;
          if (n != numargs.top()) throw Exception("invalid number of arguments");
          push(pile.top(), tokens);
          pile.pop();
        }

        if (!pile.empty() && pile.top().type == FUNCTION && functions[pile.top().dat.n].args == 0)
        {
          push(pile.top(), tokens);
          pile.pop();
        }

        assert(!numargs.empty());
        numargs.pop();
      }
      else if (curtok.type == END)
      {
        while(!pile.empty())
        {
          if (pile.top().type != PARENTHESIS_OPEN)
          {
            push(pile.top(), tokens);
            pile.pop();
          }
          else
          {
            throw Exception("parenthesis matching");
          }
        }
        push(curtok, tokens);
        assert(numargs.empty());
        break;
      }

      ptr = const_cast<char*>(endptr);
      prevtok = curtok;
    }
  }
  catch(Exception &e)
  {
    char buf[128];
    sprintf(buf, "compile error at or near position %d", int(ptr-str)+1);
    throw Exception(e, buf);
  }
}

/**************************************************************************//**
 * @details Recode tokens in order to speedup evaluation:
 *          - FUNCTION -> FUNCTIONx
 *          - OPERATOR -> OPERATOR
 *          - VARIABLE -> VALREF
 *          Returns minimum stack size required by Expr::eval().
 * @param[in,out] tokens RPN stack.
 * @param[in,out] variables List of variables.
 * @return Minimum evaluation stack size.
 * @throw Exception Link error.
 */
int ccruncher::Expr::link(std::vector<token> &tokens, const std::vector<variable> &variables) throw(Exception)
{
  int size=0, max_size=0;

  for(int i=0; i<(int)tokens.size(); i++)
  {
    // replacing indexes by references
    switch(tokens[i].type)
    {
      case FUNCTION: {
        if (tokens[i].dat.n >= int(NUMFUNCTIONS)) throw Exception("function not found");
        if (functions[tokens[i].dat.n].args <= 1) tokens[i].type = FUNCTION1;
        else if (functions[tokens[i].dat.n].args == 2) tokens[i].type = FUNCTION2;
        else if (functions[tokens[i].dat.n].args == 3) tokens[i].type = FUNCTION3;
        tokens[i].dat.pfunc = functions[tokens[i].dat.n].ptr;
        break;
      }
      case OPERATOR: {
        tokens[i].type = FUNCTION2;
        if (tokens[i].dat.n >= int(NUMOPERATORS)) throw Exception("operator not found");
        else tokens[i].dat.pfunc = (ffunc0) operators[tokens[i].dat.n].func;
        break;
      }
      case VARIABLE: {
        if (tokens[i].dat.n >= (int)variables.size()) throw Exception("variable not found");
        if (variables[tokens[i].dat.n].ptr == nullptr) throw Exception("variable not linked");
        tokens[i].type = VALREF;
        tokens[i].dat.ptr = variables[tokens[i].dat.n].ptr;
        break;
      }
      default: break;
    }

    // computing stack size & validating tokens
    switch(tokens[i].type)
    {
      case VALREF:
      case NUMBER: {
        size++;
        if (size > max_size) max_size = size;
        break;
      }
      case FUNCTION2: {
        size--;
        break;
      }
      case FUNCTION3: {
        size -= 2;
        break;
      }
      case FUNCTION1:
      case END:
        break;
      default:
        throw Exception("corrupted stack");
    }
  }

  if (size != 1) throw Exception("corrupted stack");
  else return max_size;
}

/**************************************************************************//**
 * @details The evaluation of a RPN stack requires a stack that Recode tokens in order to speedup evaluation:
 *          - FUNCTION -> FUNCTIONx
 *          - OPERATOR -> OPERATOR
 *          - VARIABLE -> VALREF
 *          Returns minimum stack size required by Expr::eval().
 * @param[in] tokens RPN stack.
 * @param[in] maxsize Evaluation stack size.
 * @throw Exception Corrupted stack or stack size to low.
 */
double ccruncher::Expr::eval(const std::vector<token> &tokens, size_t maxsize) throw(Exception)
{
  return eval(&tokens[0], maxsize);
}

/**************************************************************************//**
 * @todo Use vector::reserve()
 * @todo Use push_back(), push_pop()
 * @todo Set maxsize default value to 0.
 * @todo Allow VARIABLE token?
 * @param[in] tokens RPN stack.
 * @param[in] maxsize Evaluation stack size.
 * @throw Exception Corrupted stack or stack size to low.
 */
double ccruncher::Expr::eval(const token *tokens, size_t maxsize) throw(Exception)
{
  vector<double> values(maxsize);
  size_t i=0, cont=0;

  while(true)
  {
    switch(tokens[i].type)
    {
      case VALREF: {
        if (cont >= maxsize) throw Exception("stack size too low");
        values[cont] = *((double*)tokens[i].dat.ptr);
        cont++;
        break;
      }
      case NUMBER: {
        if (cont >= maxsize) throw Exception("stack size too low");
        values[cont] = tokens[i].dat.x;
        cont++;
        break;
      }
      case FUNCTION1: {
        ffunc1 f = (ffunc1) tokens[i].dat.pfunc;
        values[cont-1] = f(values[cont-1]);
        break;
      }
      case FUNCTION2: {
        ffunc2 f = (ffunc2) tokens[i].dat.pfunc;
        values[cont-2] = f(values[cont-2],values[cont-1]);
        cont--;
        break;
      }
      case FUNCTION3: {
        ffunc3 f = (ffunc3) tokens[i].dat.pfunc;
        values[cont-3] = f(values[cont-3],values[cont-2],values[cont-1]);
        cont -= 2;
        break;
      }
      case END:
        if (cont != 1) throw Exception("corrupted stack");
        return values[0];
      default:
        throw Exception("corrupted stack");
    }
    i++;
  }
}

/**************************************************************************//**
 * @details Search the name of the function or operator pointed by this
 *          pointer to function.
 * @param[in] ptr Pointer to function (see Token#dat.ptr).
 * @return Function name/identifier. '???' if not found.
 */
const char *ccruncher::Expr::getFunctionName(const ffunc0 ptr)
{
  for(int i=0; i<int(NUMFUNCTIONS); i++) {
    if (functions[i].ptr == ptr) {
      return functions[i].id;
    }
  }

  for(int i=0; i<int(NUMOPERATORS); i++) {
    if (operators[i].func == (ffunc2)ptr) {
      return operators[i].id;
    }
  }

  return "???";
}

/**************************************************************************//**
 * @param[in] tokens RPN stack.
 * @param[in] variables List of variables.
 */
void ccruncher::Expr::debug(const std::vector<token> &tokens, const std::vector<variable> &variables)
{
  cout << "number of variables: " << variables.size() << endl;
  for(size_t i=0; i<variables.size(); i++)
  {
    cout << "\tvar " << (i+1) << " : id=" << variables[i].id << ", value=";
    if (variables[i].ptr==nullptr) cout << "NULL" << endl;
    else cout << *(variables[i].ptr) << endl;
  }

  cout << "reverse polish notation stack:" << endl;
  for(size_t i=0; i<tokens.size(); i++)
  {
    cout << "\t";

    switch(tokens[i].type)
    {
      case NUMBER:
        cout << "NUM\t" << tokens[i].dat.x << endl;
        break;
      case VARIABLE:
        cout << "VAR\t" << variables[tokens[i].dat.n].id << endl;
        break;
      case CONSTANT:
        cout << "CONST\t" << constants[tokens[i].dat.n].id << endl;
        break;
      case OPERATOR:
        cout << "OPER\t" << operators[tokens[i].dat.n].id << endl;
        break;
      case FUNCTION:
        cout << "FUNC\t" << functions[tokens[i].dat.n].id << endl;
        break;
      case END:
        cout << "END" << endl;
        break;
      case FUNCTION1:
        cout << "FUNC1\t" << getFunctionName(tokens[i].dat.pfunc) << endl;
        break;
      case FUNCTION2:
        cout << "FUNC2\t" << getFunctionName(tokens[i].dat.pfunc) << endl;
        break;
      case FUNCTION3:
        cout << "FUNC3\t" << getFunctionName(tokens[i].dat.pfunc) << endl;
        break;
      case VALREF:
        cout << "VALREF\t";
        for(size_t j=0; j<variables.size(); j++) {
          if (variables[j].ptr == tokens[j].dat.ptr) {
            cout << variables[j].id;
            break;
          }
        }
        cout << endl;
        break;
      default:
        cout << "UNKNOW" << endl;
        assert(false);
    }
  }
}

