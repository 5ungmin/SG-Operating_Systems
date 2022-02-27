#define FRACTION (1 << 14)

int int_to_float (int);
int float_to_int (int);
int float_add_float (int, int);
int float_sub_float (int, int);
int float_mul_float (int, int);
int float_div_float (int, int);
int float_add_int (int, int);
int int_sub_float (int, int);
int float_mul_int (int, int);
int float_div_int (int, int);

/* int -> float */
int
int_to_float (int i)
{
    return i * FRACTION;
}

/* float -> int */
int
float_to_int (int f)
{
    if (f >= 0)
    {
        return (f + FRACTION / 2) / FRACTION;
    }
    else
    {
        return (f - FRACTION / 2) / FRACTION;
    }
}

/* float + float */
int
float_add_float (int f1, int f2)
{
  return f1 + f2;
}

/* float - float */
int
float_sub_float (int f1, int f2)
{
  return f1 - f2;
}

/* float * float */
int
float_mul_float (int f1, int f2)
{
  int64_t temp = f1;
  temp = temp * f2 / FRACTION;
  return (int) temp;
}

/* float / float */
int
float_div_float (int f1, int f2)
{
  int64_t temp = f1;
  temp = temp * FRACTION / f2;
  return (int) temp;
}

/* float + int */
int
float_add_int (int f, int i)
{
  return f + i * FRACTION;
}

/* int - float */
int 
int_sub_float (int i, int f)
{
  return i * FRACTION - f;
}

/* float * int */
int
float_mul_int (int f, int i)
{
    return f * i;
}

/* float / int */
int
float_div_int (int f, int i)
{
    return f / i;
}