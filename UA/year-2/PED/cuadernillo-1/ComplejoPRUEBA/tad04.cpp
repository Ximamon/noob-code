#include <iostream>

using namespace std;

#include "../include/tcomplejo.h"

int
main(void)
{
  TComplejo a, b, s, r, m;


  s = a + b;
  cout << s << endl;

  r = a - b;
  cout << r << endl;

  m = a * b;
  cout << m << endl;
    
  return 0;

}
