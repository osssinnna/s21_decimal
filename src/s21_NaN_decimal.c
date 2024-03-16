#include "s21_decimal.h"

// Функции возвращают код ошибки:

// 0 - OK
// 1 - число слишком велико или равно бесконечности
// 2 - число слишком мало или равно отрицательной бесконечности
// 3 - деление на 0

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int res = 0;
  int profit = 0;

  big_decimal big_value_1 = {0};
  big_decimal big_value_2 = {0};
  big_decimal big_value_3 = {0};

  if (check_sign(value_1, value_2) == 1) {
    equal_bits(value_1, &big_value_1);
    equal_bits(value_2, &big_value_2);

    if (equal_null(big_value_1)) {
      *result = value_2;
      profit++;
    }
    if (equal_null(big_value_2)) {
      *result = value_1;
      profit++;
    }
    if (equal_null(big_value_2) && equal_null(big_value_1)) {
      result->bits[3] = 0;
      profit++;
    }

    if (profit == 0) {
      compare_pow(&big_value_1, &big_value_2, &big_value_3);

      bit_add(big_value_1, big_value_2, &big_value_3);
      set_sign(value_1, &big_value_3.bits[7]);
      res = from_big_decimal_to_decimal(big_value_3, result);
    }
  } else {
    if (get_sign_int(value_2.bits[3]) == 1) {
      load_bit(&value_2.bits[3], 31, 0);
      res = s21_sub(value_1, value_2, result);
    }
    if (get_sign_int(value_1.bits[3]) == 1) {
      load_bit(&value_1.bits[3], 31, 0);
      res = s21_sub(value_2, value_1, result);
    }
  }
  return res;
}

int bit_add(big_decimal big_value_1, big_decimal big_value_2,
            big_decimal *big_value_3) {
  int buff[32] = {0};
  int buffer = 0;
  for (int j = 0; j < 7; j++) {
    for (int i = 0; i < 32; i++) {
      int bit_value_1 = get_bit(big_value_1.bits[j], i);
      int bit_value_2 = get_bit(big_value_2.bits[j], i);
      int bit_value_3 = bit_value_1 + bit_value_2 + buffer;
      switch (bit_value_3) {
        case 0:
          buff[i] = 0;
          buffer = 0;
          break;
        case 1:
          buff[i] = 1;
          buffer = 0;
          break;
        case 2:
          buff[i] = 0;
          buffer = 1;
          break;
        default:
          buff[i] = 1;
          buffer = 1;
          break;
      }
    }
    set_bit(buff, &big_value_3->bits[j]);
  }
  return buffer;
}

int get_bit(unsigned int n, int i) {
  int bit = (n >> i) & 1u;
  return bit;
}

void set_bit(int buff[32], unsigned int *bits) {
  for (int i = 0; i < 32; i++) {
    *bits += buff[i] * pow(2, i);
  }
}

int get_exponent(unsigned int n) {
  int res = 0;
  for (int i = 16, j = 0; i < 24; i++, j++) {
    int bit = get_bit(n, i);
    res += bit * pow(2, j);
  }
  return res;
}

void compare_pow(big_decimal *big_value_1, big_decimal *big_value_2,
                 big_decimal *big_value_3) {
  int pow_1 = 0;
  int pow_2 = 0;
  pow_1 = get_exponent(big_value_1->bits[7]);  // 1   12300   12,3
  pow_2 = get_exponent(big_value_2->bits[7]);  // 3   123   0,123
  int pow = pow_1 - pow_2;
  if (pow <= 0) {  //
    pow_10(big_value_1, abs(pow));
    set_exponent(pow_2, big_value_3);
  } else if (pow > 0) {
    pow_10(big_value_2, abs(pow));
    set_exponent(pow_1, big_value_3);
  }
}

void set_exponent(int pow, big_decimal *big_value_3) {
  for (int i = 16, j = 0; i < 24; i++, j++) {
    load_bit(&big_value_3->bits[7], i, get_bit(pow, j));
  }
}

void equal_bits(s21_decimal value_1, big_decimal *big_value_1) {
  for (int i = 0; i < 3; i++) {
    big_value_1->bits[i] = value_1.bits[i];
  }
  big_value_1->bits[7] = value_1.bits[3];
}

void equal_big_bits(big_decimal *big_value_1, big_decimal *big_value_2) {
  for (int i = 0; i < 8; i++) {
    big_value_2->bits[i] = big_value_1->bits[i];
  }
}

void pow_10(big_decimal *big_value_2, int n) {
  for (int i = 0; i < n; i++) {
    big_decimal big_value_4 = {0};
    big_decimal big_value_5 = {0};

    equal_big_bits(big_value_2, &big_value_4);
    equal_big_bits(big_value_2, &big_value_5);

    left(&big_value_4, 3);
    left(&big_value_5, 1);

    clear_bits(big_value_2);
    bit_add(big_value_4, big_value_5, big_value_2);
  }
}

void clear_bits(big_decimal *big_value_2) {
  for (int i = 0; i < 8; i++) {
    big_value_2->bits[i] = 0;
  }
}

void load_bit(unsigned int *x, int n, int m) {
  if (m == 1) {
    *x |= (1 << n);
  } else {
    *x &= ~(1 << n);
  }
}

void left(big_decimal *big_value_1, int n) {
  for (int i = 0; i < n; i++) {
    for (int j = 6; j >= 0; j--) {
      if (get_bit(big_value_1->bits[j], 31) == 1 && j != 7) {
        load_bit(&big_value_1->bits[j + 1], 0, 1);
        big_value_1->bits[j] = big_value_1->bits[j] << 1;
      } else {
        big_value_1->bits[j] = big_value_1->bits[j] << 1;
      }
    }
  }
}

int get_sign_int(unsigned int n) {
  int res = 0;
  if (get_bit(n, 31) == 1) {
    res = 1;
  } else {
    res = 0;
  }
  return res;
}

int check_sign(s21_decimal value_1, s21_decimal value_2) {  // ++ , +-, -+, --
  int res = 0;
  if (get_sign_int(value_1.bits[3]) == get_sign_int(value_2.bits[3])) {
    res = 1;
  }
  return res;
}

void set_sign(s21_decimal value_1, unsigned int *x) {
  if (get_sign_int(value_1.bits[3]) == 1) {
    load_bit(x, 31, 1);
  } else {
    load_bit(x, 31, 0);
  }
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int res = 0;
  s21_truncate(src, &src);
  if (src.bits[1] == 0 && src.bits[2] == 0 &&
      ((get_bit(src.bits[0], 31) != 1) ||
       (src.bits[0] == 2147483648 && get_sign_int(src.bits[3]) == 1))) {
    *dst = src.bits[0];
    set_sign_to_int(src, dst);
  } else {
    res = 1;
  }
  return res;
}

void set_sign_to_int(s21_decimal value_1, int *x) {
  if (get_sign_int(value_1.bits[3]) == 1) {
    *x = -*x;
  }
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int res = 0;

  big_decimal big_value_1 = {0};
  big_decimal big_value_2 = {0};
  big_decimal big_value_3 = {0};
  equal_bits(value_1, &big_value_1);
  equal_bits(value_2, &big_value_2);

  bit_mul(big_value_1, big_value_2, &big_value_3);

  mul_pow(big_value_1, big_value_2, &big_value_3);

  if (check_sign(value_1, value_2) != 1) {
    { load_bit(&big_value_3.bits[7], 31, 1); }
  }

  res = from_big_decimal_to_decimal(big_value_3, result);

  return res;
}

int bit_mul(big_decimal big_value_1, big_decimal big_value_2,
            big_decimal *big_value_3) {
  big_decimal big_value_4 = {0};
  big_decimal big_value_5 = {0};
  big_decimal big_value_6 = {0};

  int count = 0;

  for (int t = 0; t < 7; t++) {
    for (int j = 0; j < 32; j++) {
      int bit2 = get_bit(big_value_2.bits[t], j);
      if (bit2 == 1) {
        clear_bits(&big_value_4);
        equal_big_bits(&big_value_1, &big_value_4);
        big_value_4.bits[7] = 0;
        big_value_5.bits[7] = 0;
        left(&big_value_4, count);

        bit_add(big_value_5, big_value_4, &big_value_6);

        equal_big_bits(&big_value_6, &big_value_5);
        clear_bits(&big_value_6);
      }

      equal_big_bits(&big_value_5, big_value_3);
      count++;
    }
  }
  return 0;
}

void mul_pow(big_decimal big_value_1, big_decimal big_value_2,
             big_decimal *big_value_3) {
  int pow_1 = 0;
  int pow_2 = 0;
  pow_1 = get_exponent(big_value_1.bits[7]);  // 1   12300   12,3
  pow_2 = get_exponent(big_value_2.bits[7]);  // 3   123   0,123
  int pow = pow_1 + pow_2;
  set_exponent(pow, big_value_3);
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int res = 0;
  int profit = 0;

  big_decimal big_value_1 = {0};
  big_decimal big_value_2 = {0};
  big_decimal big_value_3 = {0};

  s21_decimal value_5 = value_1;
  s21_decimal value_6 = value_2;

  load_bit(&value_5.bits[3], 31, 0);
  load_bit(&value_6.bits[3], 31, 0);

  if (check_sign(value_1, value_2) == 1) {
    int val = s21_is_less(value_5, value_6);
    if (val == 0) {
      equal_bits(value_1, &big_value_1);
      equal_bits(value_2, &big_value_2);
      set_sign(value_1, &big_value_3.bits[7]);
    } else {
      equal_bits(value_1, &big_value_2);
      equal_bits(value_2, &big_value_1);
      inverse_sign(value_1, &big_value_3.bits[7]);
    }

    if (equal_zero(value_1)) {
      *result = value_2;
      result->bits[3] = big_value_3.bits[7];
      profit++;
    }
    if (equal_zero(value_2)) {
      *result = value_1;
      result->bits[3] = big_value_3.bits[7];
      profit++;
    }
    if (equal_zero(value_2) && equal_zero(value_1)) {
      result->bits[3] = 0;
      profit++;
    }

    if (profit == 0) {
      compare_pow(&big_value_1, &big_value_2, &big_value_3);
      bit_sub(big_value_1, big_value_2, &big_value_3);
      res = from_big_decimal_to_decimal(big_value_3, result);
    }

  } else {
    if (get_sign_int(value_1.bits[3]) == 1) {
      load_bit(&value_2.bits[3], 31, 1);
    }
    if (get_sign_int(value_1.bits[3]) == 0) {
      load_bit(&value_2.bits[3], 31, 0);
    }
    res = s21_add(value_1, value_2, result);
  }
  return res;
}

int bit_sub(big_decimal big_value_1, big_decimal big_value_2,
            big_decimal *big_value_3) {
  int buff[32] = {0};
  int buffer = 0;
  for (int j = 0; j < 7; j++) {
    for (int i = 0; i < 32; i++) {
      int bit_value_1 = get_bit(big_value_1.bits[j], i);
      int bit_value_2 = get_bit(big_value_2.bits[j], i);
      int bit_value_3 = bit_value_1 - bit_value_2 - buffer;
      switch (bit_value_3) {
        case -1:
          buff[i] = 1;
          buffer = 1;
          break;
        case 0:
          buff[i] = 0;
          buffer = 0;
          break;
        case 1:
          buff[i] = 1;
          buffer = 0;
          break;
        default:
          buff[i] = 0;
          buffer = 1;
          break;
      }
    }
    set_bit(buff, &big_value_3->bits[j]);
  }
  return buffer;
}

void inverse_sign(s21_decimal value_1, unsigned int *x) {
  if (get_sign_int(value_1.bits[3]) == 1) {
    load_bit(x, 31, 0);
  } else {
    load_bit(x, 31, 1);
  }
}

int equal_zero(s21_decimal value_1) {
  int no = 0;
  for (int i = 0; i < 3; i++)
    if (!value_1.bits[i]) no++;
  return no / 7;
}
