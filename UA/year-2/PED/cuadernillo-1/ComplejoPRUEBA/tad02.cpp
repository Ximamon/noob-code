#include <iostream>

using namespace std;

#include "../include/tcomplejo.h"

int
main(void)
{
  TComplejo a;
  TComplejo b(a);
  TComplejo c;
  c = a;
  
  if(a != b)
    cout << "SI" << endl;
  else
    cout << "NO" << endl;

  c.Re(1);

  if(a != c)
    cout << "SI" << endl;
  else
    cout << "NO" << endl;

  return 0;
}
