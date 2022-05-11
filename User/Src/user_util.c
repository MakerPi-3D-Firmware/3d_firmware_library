#include "user_common.h"

// Are the two floating point numbers equal
bool user_is_float_data_equ(const float p_a, const float p_b)
{
  const float EPSINON = 0.000001F;
  bool result = false;
  const float x = p_a - p_b; // float

  if ((x >= -EPSINON) && (x <= EPSINON))
  {
    result = true;
  }

  return (result);
}

// Get encrypted characters
char user_decryption_code(char source, int key, int pos)
{
  if ((source >= 'a') && (source <= 'z'))
  {
    source = source - ((key + pos) % 26);

    if (source < 'a')
    {
      source += 26;
    }
  }
  else if ((source >= 'A') && (source <= 'Z'))
  {
    source = source - ((key + pos) % 26);

    if (source < 'A')
    {
      source += 26;
    }
  }
  else if ((source >= '0') && (source <= '9'))
  {
    source = source - ((key + pos) % 10);

    if (source < '0')
    {
      source += 10;
    }
  }

  return source;
}




