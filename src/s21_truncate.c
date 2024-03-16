#include "./s21_decimal.h"

int s21_truncate(s21_decimal value, s21_decimal *result) {
  int res = 0;
  s21_decimal buff = value;
  truncate(&value);
  *result = value;
  set_sign(buff, &result->bits[3]);
  return res;
}

void truncate(s21_decimal *value_3) {
  int pow = 0;
  pow = get_exponent(value_3->bits[3]);

  if (pow > 0) {
    big_decimal big_value_4 = {{10, 0}};
    big_decimal big_value_5 = {0};
    big_decimal big_value_6 = {0};
    equal_bits(*value_3, &big_value_5);
    big_value_5.bits[7] = 0;
    set_exponent(28, &big_value_5);
    for (int i = 0; i < pow; i++) {
      segmentation(big_value_5, big_value_4, &big_value_6);
      equal_big_bits(&big_value_6, &big_value_5);
      clear_bits(&big_value_6);
    }
    set_exponent(0, &big_value_5);
    from_big_decimal_to_decimal(big_value_5, value_3);
  }
}
