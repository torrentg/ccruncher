
/*=================================================================

compiled using:
  g++ bugcpp.cpp

result obtained (in a CA_ES system):
  g++: Error intern: Violació de segment (programa cc1plus)
  Per favor envieu un informe complet d'error.
  Consulta <URL:http://bugzilla.redhat.com/bugzilla> per a més instruccions.

system settings:
  Pentium IV 2400 (with hyperthreading)
  Fedora Core 2 (stable version, updated at 31/10/2004)
  g++ 3.3.3

bugid assigned (http://bugzilla.redhat.com/bugzilla):
  137.738

=================================================================*/

#include <math.h>
#include <algorithm>
#include <string>

using namespace std;

class MyClass
{
  public:
    MyClass(string _name) {name = _name;}

    string name;
};

int main( int argc, char* argv[] )
{
  MyClass obj = New MyClass("rest");
}

