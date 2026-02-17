#include <iostream>

using namespace std;

#include "../include/tcomplejo.h"

int
main(void)
{
  TComplejo a;
  TComplejo b(a);

  b.~TComplejo();


  cout << a << " " << b << endl;

  return 0;
}

