#include "s21_decimal.h"

int s21_is_less_or_equal(s21_decimal value1, s21_decimal value2) {
  int flag = 1;
  int zn1 = get_zn(value1);
  int zn2 = get_zn(value2);
  if ((zn1 == 1) && (zn2 == 0)) {
    flag = 1;
  } else if ((zn1 == 0) && (zn2 == 1)) {
    if (check_zero(value1) == 1 && check_zero(value2) == 1) {
      flag = 1;
    } else {
      flag = 0;
    }
  } else {
    if ((s21_is_less(value1, value2) != 1) &&
        (s21_is_equal(value1, value2) != 1)) {
      flag = 0;
    }
  }
  return flag;
}
