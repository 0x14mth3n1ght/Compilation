
;; Function saxpy (saxpy, funcdef_no=22, decl_uid=3008, cgraph_uid=23, symbol_order=22)

void saxpy (int * res, int * x, int * y, int a, int size)
{
  int i;
  long unsigned int _1;
  long unsigned int _2;
  int * _3;
  int _4;
  int _5;
  int * _6;
  int _7;
  int * _8;
  int _9;

  <bb 2> [local count: 118111600]:
  if (size_13(D) > 0)
    goto <bb 3>; [89.00%]
  else
    goto <bb 4>; [11.00%]

  <bb 3> [local count: 955630225]:
  # i_22 = PHI <i_19(3), 0(2)>
  _1 = (long unsigned int) i_22;
  _2 = _1 * 4;
  _3 = x_14(D) + _2;
  _4 = *_3;
  _5 = _4 * a_15(D);
  _6 = y_16(D) + _2;
  _7 = *_6;
  _8 = res_17(D) + _2;
  _9 = _5 + _7;
  *_8 = _9;
  i_19 = i_22 + 1;
  if (size_13(D) > i_19)
    goto <bb 3>; [89.00%]
  else
    goto <bb 4>; [11.00%]

  <bb 4> [local count: 118111600]:
  return;

}



;; Function main (main, funcdef_no=23, decl_uid=3017, cgraph_uid=24, symbol_order=23) (executed once)

__attribute__((access ("^1[ ]", )))
int main (int argc, char * * argv)
{
  int * y;
  int * x;
  int * res;
  int i;
  long unsigned int _1;
  long unsigned int _2;
  int * _3;
  int _4;
  int * _6;
  long unsigned int _7;
  long unsigned int _8;
  int * _9;
  int _10;
  int _11;
  int _12;

  <bb 2> [local count: 10737416]:
  res_20 = malloc (400);
  x_22 = malloc (400);
  y_24 = malloc (400);

  <bb 3> [local count: 1063004409]:
  # i_35 = PHI <i_30(3), 0(2)>
  _1 = (long unsigned int) i_35;
  _2 = _1 * 4;
  _3 = x_22 + _2;
  _4 = i_35 + 50;
  *_3 = _4;
  _6 = y_24 + _2;
  *_6 = i_35;
  i_30 = i_35 + 1;
  if (i_30 <= 99)
    goto <bb 3>; [99.00%]
  else
    goto <bb 4>; [1.00%]

  <bb 4> [local count: 10737416]:
  saxpy (res_20, x_22, y_24, 2, 100);

  <bb 5> [local count: 1063004409]:
  # i_36 = PHI <i_16(7), 0(4)>
  _7 = (long unsigned int) i_36;
  _8 = _7 * 4;
  _9 = res_20 + _8;
  _10 = *_9;
  printf ("res[%d] = %d ; ", i_36, _10);
  _11 = i_36 + 1;
  _12 = _11 % 10;
  if (_12 == 0)
    goto <bb 6>; [33.00%]
  else
    goto <bb 7>; [67.00%]

  <bb 6> [local count: 350791453]:
  __builtin_putchar (10);

  <bb 7> [local count: 1063004409]:
  # i_16 = PHI <_11(5), _11(6)>
  if (i_16 <= 99)
    goto <bb 5>; [99.00%]
  else
    goto <bb 8>; [1.00%]

  <bb 8> [local count: 10737416]:
  return 1;

}


