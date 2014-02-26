
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

#include <iostream>
#include "ExprTest.hpp"
#include "Expr.hpp"

using namespace std;
using namespace ccruncher;

#define EPSILON 1e-12

//===========================================================================
// test1. check error detection
//===========================================================================
void ccruncher_test::ExprTest::test1()
{
  vector<Expr::variable> variables;
  vector<Expr::token> tokens;

  // void expression
  ASSERT_THROW(Expr::compile("", variables, tokens));
  // void parentesis
  ASSERT_THROW(Expr::compile("1+()", variables, tokens));
  // parenthesis matching
  ASSERT_THROW(Expr::compile("(1+2", variables, tokens));
  ASSERT_THROW(Expr::compile("1+2)", variables, tokens));
  ASSERT_THROW(Expr::compile("((1+2)", variables, tokens));
  ASSERT_THROW(Expr::compile("(1+2))", variables, tokens));
  ASSERT_THROW(Expr::compile(")2(", variables, tokens));
  // double prefix unary
  ASSERT_THROW(Expr::compile("++1", variables, tokens));
  ASSERT_THROW(Expr::compile("--1", variables, tokens));
  // operator + operator
  ASSERT_THROW(Expr::compile("1++2", variables, tokens));
  ASSERT_THROW(Expr::compile("1--2", variables, tokens));
  ASSERT_THROW(Expr::compile("1**2", variables, tokens));
  ASSERT_THROW(Expr::compile("1//2", variables, tokens));
  ASSERT_THROW(Expr::compile("1^^2", variables, tokens));
  ASSERT_THROW(Expr::compile("1+*2", variables, tokens));
  ASSERT_THROW(Expr::compile("1*+2", variables, tokens));
  ASSERT_THROW(Expr::compile("1+/2", variables, tokens));
  ASSERT_THROW(Expr::compile("1/-2", variables, tokens));
  // value + value
  ASSERT_THROW(Expr::compile("1 1", variables, tokens));
  ASSERT_THROW(Expr::compile("1 Pi", variables, tokens));
  ASSERT_THROW(Expr::compile("E Pi", variables, tokens));
  ASSERT_THROW(Expr::compile("x y", variables, tokens));
  ASSERT_THROW(Expr::compile("1 y", variables, tokens));
  ASSERT_THROW(Expr::compile("x Pi", variables, tokens));
  // value + parenthesis
  ASSERT_THROW(Expr::compile("3 (1+1)", variables, tokens));
  ASSERT_THROW(Expr::compile("(1+1) 3", variables, tokens));
  // invalid argument separator
  ASSERT_THROW(Expr::compile("(1,1)", variables, tokens));
  // unknow function matching
  ASSERT_THROW(Expr::compile("coco(1)", variables, tokens));
  // invalid number of arguments
  ASSERT_THROW(Expr::compile("sin(1,2)", variables, tokens));
  ASSERT_THROW(Expr::compile("cos(1,2,3)", variables, tokens));
  ASSERT_THROW(Expr::compile("max(1)", variables, tokens));
  ASSERT_THROW(Expr::compile("if(1)", variables, tokens));
  // invalid variable identifier
  ASSERT_THROW(Expr::compile("2x", variables, tokens));
  ASSERT_THROW(Expr::compile("sin+1", variables, tokens));
  ASSERT_THROW(Expr::compile("cos+1", variables, tokens));
  // other stuff
  ASSERT_THROW(Expr::compile(",", variables, tokens));
  ASSERT_THROW(Expr::compile("-", variables, tokens));
  ASSERT_THROW(Expr::compile("+", variables, tokens));
  ASSERT_THROW(Expr::compile("*", variables, tokens));
  ASSERT_THROW(Expr::compile("/", variables, tokens));
  ASSERT_THROW(Expr::compile("exp", variables, tokens));
  ASSERT_THROW(Expr::compile("(,3)", variables, tokens));
  ASSERT_THROW(Expr::compile("(3,)", variables, tokens));
  ASSERT_THROW(Expr::compile("(3),", variables, tokens));
  ASSERT_THROW(Expr::compile("max(,3)", variables, tokens));
}

//===========================================================================
// test2. check variable identification
//===========================================================================
void ccruncher_test::ExprTest::test2()
{
  vector<Expr::variable> variables;
  vector<Expr::token> tokens;

  variables.clear();
  ASSERT_NO_THROW(Expr::compile("a", variables, tokens));
  ASSERT(variables.size()==1 && variables[0].id == "a");
  ASSERT_NO_THROW(Expr::compile("a2", variables, tokens));
  ASSERT(variables.size()==2 && variables[1].id == "a2");
  ASSERT_NO_THROW(Expr::compile("a+a2", variables, tokens));
  ASSERT(variables.size()==2);
  ASSERT_NO_THROW(Expr::compile("a+aVeryLongVariablesWithSomeCharsInUpperCase", variables, tokens));
  ASSERT(variables.size()==3 && variables[2].ptr == NULL);
  ASSERT_NO_THROW(Expr::compile("_a", variables, tokens));
  ASSERT(variables.size()==4);
  ASSERT_NO_THROW(Expr::compile("a_", variables, tokens));
  ASSERT(variables.size()==5);
}

//===========================================================================
// test3. check compilation+link+eval
//===========================================================================
void ccruncher_test::ExprTest::test3()
{
  double x=1.0, y=2.0, z=3.0;
  vector<Expr::variable> variables;
  vector<Expr::token> tokens;
  vector<row> formulas;

  variables.push_back(Expr::variable("x", &x));
  variables.push_back(Expr::variable("y", &y));
  variables.push_back(Expr::variable("z", &z));

  formulas.push_back(row(" 3 ", 3.0, 2, 1));
  formulas.push_back(row(" 1 + 2 ", 3.0, 2, 1));
  formulas.push_back(row("1+x", 2.0, 4, 2));
  formulas.push_back(row("(1+x)", 2.0, 4, 2));
  formulas.push_back(row("(1)+(x)", 2.0, 4, 2));
  formulas.push_back(row(" - ( - 1)", 1.0, 2, 1));
  formulas.push_back(row("-(-sin(Pi))", 0.0, 2, 1));
  formulas.push_back(row("cos(Pi)",-1.0, 2, 1));
  formulas.push_back(row("-tan(Pi)",0.0, 2, 1));
  formulas.push_back(row("1+2*3",7.0, 2, 1));
  formulas.push_back(row("1*2+3",5.0, 2, 1));
  formulas.push_back(row("-3*2+5",-1.0, 2, 1));
  formulas.push_back(row("sin(-3*(2)+5)^(2+9)", -0.149771989860108, 2, 1));
  formulas.push_back(row("sin(-1)^11", -0.149771989860108, 2, 1));
  formulas.push_back(row("3*min(1, 2) -2^4", -13, 2, 1));
  formulas.push_back(row(" 2^3^4", 4096.0, 2, 1));
  formulas.push_back(row("-(-(-(-(-1+1))))", 0.0, 2, 1));
  formulas.push_back(row("2*sin(x*3+2)", -1.91784854932628, 9, 3));
  formulas.push_back(row("-( cos(tan(Pi-2*Pi) - 1) / E^E )", -0.0356534879269318, 2, 1));
  formulas.push_back(row("(((3-x*5/5*2)-2))", -1.0, 8, 3));
  formulas.push_back(row("(cos(Pi)* 5)^(1+x)", 25.0, 6, 3));
  formulas.push_back(row("max(1+2,min(1,sin(Pi)))", 3.0, 2, 1));
  formulas.push_back(row("-tan(1+cos(2*Pi)/5) -x/(y+1)^(5+1)", -2.57352336423880, 10, 4));
  formulas.push_back(row("(1+2)^(2+3)", 243.0, 2, 1));
  formulas.push_back(row("1/2^2", 0.25, 2, 1));
  formulas.push_back(row("- 2 + x ^ y * 2", 0.0, 8, 3));
  formulas.push_back(row("2^3^(+4)", 4096.0, 2, 1));
  formulas.push_back(row("2*sqrt(x^2 + y^2 * cos(z)^2)", 4.43636814220855, 16, 5));
  formulas.push_back(row("x*cos(y)+sqrt(z^2-x^2*sin(y)^2)", 2.44273024887787, 19, 5));
  formulas.push_back(row("x^x+y^y+z^z", 32.0, 12, 3));
  formulas.push_back(row("if(-3^3,1,2)", 1.0, 2, 1));
  formulas.push_back(row("1+2+3", 6.0, 2, 1));
  formulas.push_back(row("1*2*3", 6.0, 2, 1));
  formulas.push_back(row("(1+2+3)*(3+4)", 42.0, 2, 1));
  formulas.push_back(row("-x*3", -3.0, 4, 2));
  formulas.push_back(row("-z/3", -1.0, 4, 2));
  formulas.push_back(row("-(x+1)*3", -6.0, 6, 2));
  formulas.push_back(row("-(y+1)/3", -1.0, 6, 2));
  formulas.push_back(row("(x+1)+1+2", 5.0, 4, 2));
  formulas.push_back(row("(x+1)+1-2", 1.0, 4, 2));
  formulas.push_back(row("(x+1)-1+2", 3.0, 4, 2));
  formulas.push_back(row("(x+1)-1-2", -1.0, 4, 2));
  formulas.push_back(row("(x*1)+1+2", 4.0, 6, 2));
  formulas.push_back(row("(x*1)+1-2", 0.0, 6, 2));
  formulas.push_back(row("(x*1)-1+2", 2.0, 6, 2));
  formulas.push_back(row("(x*1)-1-2", -2.0, 6, 2));
  formulas.push_back(row("(x*1)*1*2", 2.0, 4, 2));
  formulas.push_back(row("(x*1)*1/2", 0.5, 4, 2));
  formulas.push_back(row("(x*1)/1*2", 2.0, 4, 2));
  formulas.push_back(row("(x*1)/1/2", 0.5, 4, 2));
  formulas.push_back(row("(x+1)*1*2", 4.0, 6, 2));
  formulas.push_back(row("(x+1)*1/2", 1.0, 6, 2));
  formulas.push_back(row("(x+1)/1*2", 4.0, 6, 2));
  formulas.push_back(row("(x+1)/1/2", 1.0, 6, 2));
  formulas.push_back(row("(x^1)^2^3", 1.0, 4, 2));
  formulas.push_back(row("(x+1)^2^3", 64.0, 6, 2));

  for(int i=0; i<(int)formulas.size(); i++)
  {
    tokens.clear();
    //cout << formulas[i].formula << endl;

    Expr::compile(formulas[i].formula, variables, tokens);
    ASSERT_EQUALS(formulas[i].num_tokens, (int)tokens.size());

    int stack_size = Expr::link(tokens, variables);
    ASSERT_EQUALS(formulas[i].stack_size, stack_size);

    double result = Expr::eval(tokens, stack_size);
    ASSERT_EQUALS_EPSILON(formulas[i].result, result, EPSILON);
  }
}

//===========================================================================
// test4. check link+eval errors
//===========================================================================
void ccruncher_test::ExprTest::test4()
{
  vector<Expr::variable> variables;
  vector<Expr::token> tokens;

  Expr::compile("x+y", variables, tokens);

  // variable not linked
  ASSERT_THROW(Expr::link(tokens, variables));

  // variable not found
  variables.clear();
  ASSERT_THROW(Expr::link(tokens, variables));

  // stack size too low
  double x=0.0;
  variables.push_back(Expr::variable("x", &x));
  variables.push_back(Expr::variable("y", &x));
  ASSERT_NO_THROW(Expr::link(tokens, variables));
  ASSERT_THROW(Expr::eval(tokens, 1));
}

