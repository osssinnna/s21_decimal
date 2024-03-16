#include "s21_decimal.h"
int s21_is_not_equal(s21_decimal value1, s21_decimal value2) {
  int flag = 1;
  if (s21_is_equal(value1, value2) == 1) flag = 0;
  return flag;
}
