
#include <iostream>
#include <string>
#include <cstdlib>

#define NUM_RATINGS 5
#define NUM_SECTORS 2
#define NUM_CLIENTS 50
#define NUM_ASSETS 3

using namespace std;

string rating[NUM_RATINGS] = {"A", "B", "C", "D", "E"};
string sectors[NUM_SECTORS] = {"S1", "S2"};

int main(int argc, char *argv[])
{
  for (int i=0;i<NUM_CLIENTS;i++)
  {
    cout << "<client ";
    cout << "rating='" << rating[rand()%(NUM_RATINGS-1)] << "' ";
    cout << "sector='" << sectors[rand()%NUM_SECTORS] << "' ";
    cout << "name='client" << i << "' ";
    cout << "id='" << i << "'>\n";

    for (int j=0;j<NUM_ASSETS;j++)
    {
      cout << "  <asset class='bond' ";
      cout << "id='" << i << "-" << j << "'>\n";

      cout << "    ";
      cout << "<property name='issuedate' value='01/01/2000'/>\n";
      cout << "    ";
      cout << "<property name='term' value='120'/>\n";
      cout << "    ";
      cout << "<property name='nominal' value='1500'/>\n";
      cout << "    ";
      cout << "<property name='rate' value='0.07'/>\n";
      cout << "    ";
      cout << "<property name='ncoupons' value='120'/>\n";
      cout << "    ";
      cout << "<property name='adquisitiondate' value='1/1/2003'/>\n";
      cout << "    ";
      cout << "<property name='adquisitionprice' value='1500'/>\n";

      cout << "  </asset>\n";
    }

    cout << "</client>\n";
  }
}
