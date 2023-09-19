
;; Function saxpy (saxpy, funcdef_no=22, decl_uid=3008, cgraph_uid=23, symbol_order=22)

void saxpy (int * res, int * x, int * y, int a, int size)
{
  int i;

  i = 0;
  goto <D.3014>;
  <D.3013>:
  _1 = (long unsigned int) i;
  _2 = _1 * 4;
  _3 = x + _2;
  _4 = *_3;
  _5 = a * _4;
  _6 = (long unsigned int) i;
  _7 = _6 * 4;
  _8 = y + _7;
  _9 = *_8;
  _10 = (long unsigned int) i;
  _11 = _10 * 4;
  _12 = res + _11;
  _13 = _5 + _9;
  *_12 = _13;
  i = i + 1;
  <D.3014>:
  if (i < size) goto <D.3013>; else goto <D.3011>;
  <D.3011>:
  return;
}



;; Function main (main, funcdef_no=23, decl_uid=3017, cgraph_uid=24, symbol_order=23)

__attribute__((access ("^1[ ]", )))
int main (int argc, char * * argv)
{
  int a;
  int * y;
  int * x;
  int * res;
  int size;
  int i;
  int D.3036;

  size = 100;
  a = 2;
  _1 = (long unsigned int) size;
  _2 = _1 * 4;
  res = malloc (_2);
  _3 = (long unsigned int) size;
  _4 = _3 * 4;
  x = malloc (_4);
  _5 = (long unsigned int) size;
  _6 = _5 * 4;
  y = malloc (_6);
  i = 0;
  goto <D.3028>;
  <D.3027>:
  _7 = (long unsigned int) i;
  _8 = _7 * 4;
  _9 = x + _8;
  _10 = i + 50;
  *_9 = _10;
  _11 = (long unsigned int) i;
  _12 = _11 * 4;
  _13 = y + _12;
  *_13 = i;
  i = i + 1;
  <D.3028>:
  if (i < size) goto <D.3027>; else goto <D.3025>;
  <D.3025>:
  saxpy (res, x, y, a, size);
  i = 0;
  goto <D.3032>;
  <D.3031>:
  _14 = (long unsigned int) i;
  _15 = _14 * 4;
  _16 = res + _15;
  _17 = *_16;
  printf ("res[%d] = %d ; ", i, _17);
  _18 = i + 1;
  _19 = _18 % 10;
  if (_19 == 0) goto <D.3034>; else goto <D.3035>;
  <D.3034>:
  __builtin_putchar (10);
  <D.3035>:
  i = i + 1;
  <D.3032>:
  if (i <= 99) goto <D.3031>; else goto <D.3029>;
  <D.3029>:
  D.3036 = 1;
  goto <D.3037>;
  D.3036 = 0;
  goto <D.3037>;
  <D.3037>:
  return D.3036;
}


