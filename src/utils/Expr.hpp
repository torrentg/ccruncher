
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2018 Gerard Torrent
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

#pragma once

#include <string>
#include <vector>
#include "Exception.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief Evaluates mathematical expressions.
 *
 * @details This class provides methods to evaluate mathematical expressions
 *          (eg. 3*x+sin(Pi)). Parse the mathematical expression and compile
 *          to Reverse Polish Notation (RPN). Supports the following features:
 *            - parentesis
 *            - constants
 *            - variables
 *            - expression simplification
 *            - compile / link / eval
 *            - compile result is an array of tokens
 */
class Expr
{

  private:

    //! Generic function pointer (equivalent to void*)
    typedef double (*ffunc0)();
    //! Function with 1 argument
    typedef double (*ffunc1)(double);
    //! Function with 2 arguments
    typedef double (*ffunc2)(double,double);
    //! Function with 3 arguments
    typedef double (*ffunc3)(double,double,double);

  private:

    //! Internal struct
    struct constant
    {
      //! Constant name
      const char id[3];
      //! Constant value
      double x;
    };

    //! Internal struct
    struct operador
    {
      //! Operator identifier
      const char id[2];
      /**
       * @brief Operator precedence.
       * @see http://en.wikipedia.org/wiki/Order_of_operations
       */
      char prec;
      /**
       * @brief Operator associativity.
       * @see http://en.wikipedia.org/wiki/Operator_associativity
       */
      bool left;
      //! Pointer to function
      ffunc2 func;
    };

    /**
     * @brief Internal struct
     * @todo Add flag force_eval (eg. rand()).
     */
    struct function
    {
      //! Function name
      const char id[5];
      //! Number of arguments. 0 is used for minus and plus signs functions.
      int args;
      //! Generic function pointer
      ffunc0 ptr;
    };

  public:

    //! Type of compiled instruction
    enum class TokenType
    {
      OPERATOR = 0,           //!< Usual operators (eg. +,-,*,/,^)
      NUMBER = 1,             //!< Number, only double type is supported (eg. 3.25)
      VARIABLE = 2,           //!< Variable referenced by index (eg. x)
      CONSTANT = 3,           //!< Predefined constants (eg, Pi, E)
      FUNCTION = 4,           //!< Function by index, dat.n = function index (eg. sin(x))
      PARENTHESIS_OPEN = 5,   //!< Only used at parsing stage
      PARENTHESIS_CLOSE =6,   //!< Only used at parsing stage
      ARGUMENT_SEPARATOR = 7, //!< Only used at parsing stage
      END = 8,                //!< Last token identifier
      FUNCTION1 = 9,          //!< 1-arg function by ptr (eg. sin(x))
      FUNCTION2 = 10,         //!< 2-args function by ptr (eg. min(x,y))
      FUNCTION3 = 11,         //!< 3-args function by ptr (eg. if(x,y,z))
      VALREF = 12             //!< Variable referenced by ptr (eg. x)
    };

    //! Compiled instruction
    struct token
    {
      //! Type of token
      TokenType type;
      //! Token data
      union
      {
        int n;
        double x;
        void *ptr;
        ffunc0 pfunc;
      } dat;
    };

    //! Numeric variable
    struct variable
    {
      //! Variable name
      std::string id;
      //! Pointer to variable value
      double *ptr;
      //! Default constructor
      variable() : id("?"), ptr(nullptr) {}
      //! Constructor
      variable(const std::string &s, double *p) : id(s), ptr(p) {}
    };

  private:

    //! List of operators
    static const operador operators[];
    //! List of functions
    static const function functions[];
    //! List of constants
    static const constant constants[];

  private:

    //! Check if the current token is a function
    static bool isFunction(const char *, token *, char **);
    //! Check if the current token is a constant
    static bool isConstant(const char *, token *, char **);
    //! Check if the current token is a operator
    static bool isOperator(const char *, token *, char **);
    //! Check if the current token is a number
    static bool isNumber(const char *, token *, char **);
    //! Check if the current token is a variable
    static bool isVariable(const char *, token *, char **, std::vector<variable> &);
    //! Parse the next token
    static void next(const char *, token *, char **, std::vector<variable> &);
    //! Syntax checking
    static void check(token *, token *);
    //! Push token to RPN instructions pile
    static void push(token &, std::vector<token> &tokens);
    //! Check if is a value (number or constant or variable)
    static bool isValue(TokenType type);
    //! Returns function name by ptr value
    static const char *getFunctionName(const ffunc0 ptr);

  public:

    //! Compile an expression
    static void compile(const char *, std::vector<variable> &variables, std::vector<token> &tokens);
    //! Compile an expression
    static void compile(const std::string &, std::vector<variable> &variables, std::vector<token> &tokens);
    //! Link an expression
    static int link(std::vector<token> &tokens, const std::vector<variable> &variables);
    //! Evalue an expression
    static double eval(const std::vector<token> &tokens, size_t maxsize);
    //! Evalue an expression
    static double eval(const token *tok, size_t maxsize);
    //! trace RPN stack to stdout
    static void debug(const std::vector<token> &tokens, const std::vector<variable> &variables);

};

} // namespace
