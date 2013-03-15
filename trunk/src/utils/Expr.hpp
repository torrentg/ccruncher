
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2013 Gerard Torrent
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

#ifndef _Expr_
#define _Expr_

//---------------------------------------------------------------------------

#include <string>
#include <vector>
#include "Exception.hpp"

namespace ccruncher {

//=============================================================
// Expr
//=============================================================

class Expr
{

  private:

    // internal typedef's
    typedef double (*ffunc1)(double);
    typedef double (*ffunc2)(double,double);
    typedef double (*ffunc3)(double,double,double);

  private:

    // internal struct
    struct constant
    {
      const char id[3];
      double x;
    };

    // internal struct
    struct operador
    {
      const char id[2]; // operator identifier
      char prec; // operator precedence, see http://en.wikipedia.org/wiki/Order_of_operations
      bool left; // operator associativity, see http://en.wikipedia.org/wiki/Operator_associativity
      ffunc2 func; // pointer to function
    };

    // internal struct
    struct function
    {
      const char id[5]; // function identifier
      int args; // number of arguments, 0 is used for minus and plus signs functions
      void *ptr; // pointer to function
      //TODO: add flag force_eval (eg. rand())
    };

  public:

    enum TokenType
    {
      OPERATOR = 0, // usual operators (eg. +,-,*,/,^)
      NUMBER = 1, // number, only double type is supported (eg. 3.25)
      VARIABLE = 2, // variable referenced by index (eg. x)
      CONSTANT = 3, // predefined constants (eg, Pi, E)
      FUNCTION = 4, // function by index, dat.n = function index (eg. sin(x))
      PARENTHESIS_OPEN = 5, // only used at parsing stage
      PARENTHESIS_CLOSE =6, // only used at parsing stage
      ARGUMENT_SEPARATOR = 7, // only used at parsing stage
      END = 8,
      FUNCTION1 = 9, // 1-arg function by ptr (eg. sin(x))
      FUNCTION2 = 10, // 2-args function by ptr (eg. min(x,y))
      FUNCTION3 = 11, // 3-args function by ptr (eg. if(x,y,z))
      VALREF = 12 // variable referenced by ptr (eg. x)
    };

    struct token
    {
      TokenType type;
      union
      {
        int n;
        double x;
        void *ptr;
      } dat;
    };

    struct variable
    {
      std::string id;
      double *ptr;
      variable() : id("?"), ptr(NULL) {}
      variable(const std::string &s, double *p) : id(s), ptr(p) {}
    };

  private:

    // list of operators
    static const operador operators[];
    // list of functions
    static const function functions[];
    // list of constants
    static const constant constants[];

  private:

    // check if the current token is a function
    static bool isFunction(const char *, token *, char **);
    // check if the current token is a constant
    static bool isConstant(const char *, token *, char **);
    // check if the current token is a operator
    static bool isOperator(const char *, token *, char **);
    // check if the current token is a number
    static bool isNumber(const char *, token *, char **);
    // check if the current token is a variable
    static bool isVariable(const char *, token *, char **, std::vector<variable> &);
    // parse the next token
    static void next(const char *, token *, char **, std::vector<variable> &) throw(Exception);
    // rewrite the current token (if needed)
    static bool rewrite(token *, token *);
    // syntax checking
    static void check(token *, token *) throw(Exception);
    // push token to RPN instructions pile
    static void push(token &, std::vector<token> &tokens);
    // check if is a value (number or constant or variable)
    static bool isValue(TokenType type);
    // returns function name by ptr value
    static const char *getFunctionName(const void *ptr);
    // public constructor not allowed
    Expr() { }

  public:

    // compile an expression
    static void compile(const std::string &, std::vector<variable> &variables, std::vector<token> &tokens) throw(Exception);
    // link an expression
    static int link(std::vector<token> &tokens, const std::vector<variable> &variables) throw(Exception);
    // evalue an expression
    static double eval(const std::vector<token> &tokens, size_t) throw(Exception);
    // evalue an expression
    static double eval(const token *tok, size_t) throw(Exception);
    // serialize expression
    static void debug(const std::vector<token> &tokens, const std::vector<variable> &variables);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
