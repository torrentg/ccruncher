#include <MiniCppUnit.hxx>

int main(int argc, char *argv[])
{
  return TestFixtureFactory::theInstance().runTests(argc, argv) ? 0 : -1;
}

