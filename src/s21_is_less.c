#include "s21_decimal.h"

int s21_is_less(s21_decimal value1, s21_decimal value2) {
  int flag = 1;
  int zn1 = get_zn(value1);
  int zn2 = get_zn(value2);
  if ((zn1 == 1) && (zn2 == 0)) {
    if (check_zero(value1) == 1 && check_zero(value2) == 1) {
      flag = 0;
    } else {
      flag = 1;
    }
  } else if ((zn1 == 0) && (zn2 == 1)) {
    flag = 0;
  } else {
    big_decimal big_value1, big_value2;
    big_zero_mantissa(&big_value1);
    big_zero_mantissa(&big_value2);
    big_dec_transf(value1, &big_value1);
    big_dec_transf(value2, &big_value2);
    if (s21_is_equal(value1, value2) == 1) {
      flag = 0;
    } else {
      com_denom(value1, value2, &big_value1, &big_value2);
      if (zn1 == 1) {
        for (int i = 6; i >= 0; i--) {
          if (big_value1.bits[i] < big_value2.bits[i]) {
            flag = 0;
            break;
          } else if (big_value1.bits[i] > big_value2.bits[i]) {
            flag = 1;
            break;
          }
        }
      } else {
        for (int i = 6; i >= 0; i--) {
          if (big_value1.bits[i] > big_value2.bits[i]) {
            flag = 0;
            break;
          } else if (big_value1.bits[i] < big_value2.bits[i]) {
            flag = 1;
            break;
          }
        }
      }
    }
  }
  return flag;
}
