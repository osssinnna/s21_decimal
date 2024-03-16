#include "s21_decimal.h"

int s21_is_equal(s21_decimal value1, s21_decimal value2) {
  int flag = 1;
  int zn1 = get_zn(value1);
  int zn2 = get_zn(value2);
  if ((!zn1 && zn2) || (zn1 && !zn2)) {
    if (check_zero(value1) == 1 && check_zero(value2) == 1) {
      flag = 1;
    } else {
      flag = 0;
    }
  } else {
    big_decimal big_value1, big_value2;
    big_zero_mantissa(&big_value1);
    big_zero_mantissa(&big_value2);

    big_dec_transf(value1, &big_value1);
    big_dec_transf(value2, &big_value2);
    com_denom(value1, value2, &big_value1, &big_value2);
    for (int i = 6; i >= 0; i--) {
      if (big_value1.bits[i] != big_value2.bits[i]) {
        flag = 0;
        break;
      }
    }
  }
  return flag;
}
