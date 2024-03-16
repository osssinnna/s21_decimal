#include "s21_decimal.h"

void set_buff_to_bit(int buff[256], big_decimal *big_value_3) {
  int n = 0;
  for (int j = 0; j < 7; j++) {
    unsigned int vrem = 0;
    for (int i = 0; i < 32; i++) {
      vrem += buff[n] * pow(2, i);  //  поменять на n
      n++;
    }
    big_value_3->bits[j] = vrem;
  }
}

int s21_div_my(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int res = 0;

  big_decimal big_value_1 = {0};
  big_decimal big_value_2 = {0};
  big_decimal big_value_3 = {0};

  equal_bits(value_1, &big_value_1);
  equal_bits(value_2, &big_value_2);

  div_bit(big_value_1, big_value_2, &big_value_3);

  res = from_big_decimal_to_decimal(big_value_3, result);

  return res;
}

void div_bit(big_decimal big_value_1, big_decimal big_value_2,
             big_decimal *big_value_3) {
  big_decimal big_value_4 = {0};
  big_decimal big_value_5 = {0};
  big_decimal big_value_6 = {0};

  equal_big_bits(&big_value_1, &big_value_4);
  equal_big_bits(&big_value_2, &big_value_5);

  int count = 0;
  int buff[256] = {0};
  int i = 0;

  equal_big_bits(&big_value_4, &big_value_6);

  while (big_value_6.bits[0] >= big_value_5.bits[0]) {
    clear_bits(&big_value_6);
    bit_sub(big_value_4, big_value_5, &big_value_6);
    clear_bits(&big_value_4);
    equal_big_bits(&big_value_6, &big_value_4);
    count++;
  }

  if (big_value_6.bits[0] != 0) {
    buff[i] = count;
    i++;
    pow_10(&big_value_4, 1);
    div_bit(big_value_4, big_value_5, &big_value_6);
  } else {
    buff[i] = count;
  }

  set_buff_to_bit(buff,
                  big_value_3);  // надо поправить если делимое меньше делителя
}

// void truncate(s21_decimal *value_3) {
//   int pow = 0;
//   pow = get_exponent(value_3->bits[3]);
//   if (pow > 0) {
//     big_decimal big_value_4 = {{10, 0}};
//     big_decimal big_value_5 = {0};
//     big_decimal big_value_6 = {0};

//     equal_bits(*value_3, &big_value_5);
//     big_value_5.bits[7] = 0;

//     for (int i = 0; i < pow; i++) {
//       div_prev(big_value_5, big_value_4, &big_value_6);

//       equal_big_bits(&big_value_6, &big_value_5);
//       clear_bits(&big_value_6);
//     }
//     set_exponent(0, &big_value_5);
//     from_big_decimal_to_decimal(big_value_5, value_3);
//   }
// }

void printBitsint(int number) {
  unsigned int timeByte = 0;
  for (int i = 0, j = 31; i < 32; i++, j--) {
    timeByte = timeByte | (number & 1) << j;
    number = number >> 1;
  }
  for (int i = 0; i < 32; i++) {
    printf("%d", (timeByte & 1));
    timeByte = timeByte >> 1;
  }
  printf("\n");
}

int s21_truncate_my(
    s21_decimal value,
    s21_decimal *result) {  // и когда здесь вообще res должен быть равен 1?
  int res = 0;
  truncate(&value);
  *result = value;
  return res;
}

int s21_negate_my(s21_decimal value, s21_decimal *result) {  // тоже самое
  int res = 0;
  inverse_sign(value, &value.bits[3]);
  *result = value;
  return res;
}

int s21_floor_my(s21_decimal value, s21_decimal *result) {  //и тут
  int res = 0;
  s21_decimal value_zero = {{0}};
  int val = s21_is_less(value, value_zero);
  if (val == 0) {
    s21_truncate(value, result);
  } else {
    s21_decimal value_int = {{0}};
    s21_decimal value_res = {{0}};
    s21_truncate(value, &value_int);
    load_bit(&value.bits[3], 31, 0);
    load_bit(&value_int.bits[3], 31, 0);
    s21_sub(value, value_int, &value_res);
    if (s21_is_less_or_equal(value_res, value_zero) == 0) {
      s21_decimal value_1 = {{1, 0}};
      s21_add(value_int, value_1, result);
    } else {
      s21_truncate(value, result);
    }
    load_bit(&result->bits[3], 31, 1);
  }
  return res;
}

int s21_round_my(s21_decimal value, s21_decimal *result) {  // и тут :/
  int res = 0;

  s21_decimal value_2 = value;
  s21_decimal value_int = {{0}};
  s21_decimal value_res = {{0}};
  s21_decimal value_05 = {{5, 0, 0, 65536}};
  s21_truncate(value, &value_int);
  load_bit(&value.bits[3], 31, 0);
  load_bit(&value_int.bits[3], 31, 0);
  s21_sub(value, value_int, &value_res);
  if ((s21_is_less(value_res, value_05) == 0 &&
       (s21_is_equal(value_res, value_05) == 0)) ||
      (s21_is_equal(value_res, value_05) == 1 &&
       get_bit(value_int.bits[0], 0) == 1)) {
    s21_decimal value_1 = {{1, 0}};
    s21_add(value_int, value_1, result);
  } else {
    s21_truncate(value, result);
  }
  set_sign(value_2, &result->bits[3]);

  return res;
}

void norm_pow(big_decimal *big_value_3) {
  int pow = 0;
  int n = 0;
  pow = get_exponent(big_value_3->bits[7]);
  if (pow > 28) {
    n = pow - 28;

    big_decimal big_value_4 = {{10, 0}};
    big_decimal big_value_5 = {0};
    big_decimal big_value_6 = {0};

    equal_big_bits(big_value_3, &big_value_5);
    big_value_5.bits[7] = 0;

    for (int i = 0; i < n; i++) {
      div_prev(big_value_5, big_value_4, &big_value_6);

      equal_big_bits(&big_value_6, &big_value_5);
      clear_bits(&big_value_6);
    }
    equal_big_bits(&big_value_5, big_value_3);
    set_exponent(28, big_value_3);
  }
}

int compare_value(s21_decimal value_1, s21_decimal value_2) {
  int res = 0;
  if (value_1.bits[0] < value_2.bits[0]) {
    res = 1;
  }
  return res;
}

int s21_sub_my(s21_decimal value_1, s21_decimal value_2,
               s21_decimal *result) {  // минус
  value_2.bits[3] = value_2.bits[3] ^ 1 << 31;
  big_decimal bigValue_1 = to_big_decimal(value_1);
  big_decimal bigValue_2 = to_big_decimal(value_2);
  big_decimal bigResult = to_big_decimal(*result);
  int bag = 0, profit = 0;
  if (sign(bigValue_1.bits[7]) && sign(bigValue_2.bits[7])) {
    bag = s21_add_my(value_1, value_2, result);
  } else {
    if (sign(bigValue_1.bits[7]) > sign(bigValue_2.bits[7])) {
      big_decimal time = bigValue_1;
      bigValue_1 = bigValue_2;
      bigValue_2 = time;
    }
    if (!profit) {
      if (equal_null(bigValue_1)) {
        *result = value_2;
        profit++;
      }
      if (equal_null(bigValue_2)) {
        *result = value_1;
        profit++;
      }
      if (!profit) {
        if (convert_pow(bigValue_1) != convert_pow(bigValue_2))
          equating_pow(&bigValue_1, &bigValue_2);
        if (sign(bigValue_1.bits[7]) == sign(bigValue_2.bits[7])) {
          bag = s21_add_my(value_1, value_2, result);
        } else {
          degradation(bigValue_1, bigValue_2, &bigResult);
          bag = from_big_decimal_to_decimal(bigResult, result);
        }
      }
    }
    if (profit && result->bits[0] == 0 && result->bits[1] == 0 &&
        result->bits[2] == 0)
      result->bits[3] &= ~(1 << 31);
  }
  return bag;
}

// int s21_is_greater(s21_decimal value_1, s21_decimal value_2) {
// return s21_is_less(value_2,value_1);
// }

// int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2) {
// return s21_is_less_or_equal(value_2,value_1);
// }

// int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
// return
// s21_is_greater_or_equal(value_1,value_2)&&s21_is_less_or_equal(value_1,value_2);
// }

// int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2){
// return (!s21_is_equal(value_1,value_2));
// }

// int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
// big_decimal bigValue_1 = to_big_decimal(value_1);
// big_decimal bigValue_2 = to_big_decimal(value_2);
// if(equal_null(bigValue_1)&&equal_null(bigValue_2)&&(znak(bigValue_1.bits[7])!=znak(bigValue_2.bits[7]))){
// bigValue_1.bits[7]=0;
// bigValue_2.bits[7]=0;
// }
// if(znak(bigValue_1.bits[7])&&znak(bigValue_2.bits[7])){
// big_decimal timeNumber=bigValue_1;
// bigValue_1=bigValue_2;
// bigValue_2=timeNumber;
// }
// return less(bigValue_1,bigValue_2);
// }

// int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2){
// big_decimal bigValue_1 = to_big_decimal(value_1);
// big_decimal bigValue_2 = to_big_decimal(value_2);
// if(equal_null(bigValue_1)&&equal_null(bigValue_2)&&(znak(bigValue_1.bits[7])!=znak(bigValue_2.bits[7]))){
// bigValue_1.bits[7]=0;
// bigValue_2.bits[7]=0;
// }
// if(znak(bigValue_1.bits[7])&&znak(bigValue_2.bits[7])){
// big_decimal timeNumber=bigValue_1;
// bigValue_1=bigValue_2;
// bigValue_2=timeNumber;
// }
// return less_or_equal(bigValue_1,bigValue_2);
// }

int s21_add_my(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int bag = 0, profit = 0;
  big_decimal bigValue_1 = to_big_decimal(value_1);
  big_decimal bigValue_2 = to_big_decimal(value_2);
  big_decimal bigResult = to_big_decimal(*result);
  if (sign(value_1.bits[3]) != sign(value_2.bits[3])) {
    if (sign(value_1.bits[3])) {
      value_1.bits[3] = value_1.bits[3] ^ 1 << 31;
      bag = s21_sub_my(value_2, value_1, result);
    } else {
      value_2.bits[3] = value_2.bits[3] ^ 1 << 31;
      bag = s21_sub_my(value_1, value_2, result);
    }
  } else {
    if (equal_null(bigValue_1)) {
      *result = value_2;
      profit++;
    }
    if (equal_null(bigValue_2)) {
      *result = value_1;
      profit++;
    }
    if (!profit) {
      if (convert_pow(bigValue_1) != convert_pow(bigValue_2))
        equating_pow(&bigValue_1, &bigValue_2);
      sum_int(bigValue_1, bigValue_2, &bigResult);
      bigResult.bits[7] = bigValue_1.bits[7];
      bag = from_big_decimal_to_decimal(bigResult, result);
      if (profit && result->bits[0] == 0 && result->bits[1] == 0 &&
          result->bits[2] == 0)
        result->bits[3] &= ~(1 << 31);
    }
  }
  return bag;
}

void printBits(unsigned int number) {
  unsigned int timeByte = 0;
  for (int i = 0, j = 31; i < 32; i++, j--) {
    timeByte = timeByte | (number & 1) << j;
    number = number >> 1;
  }
  for (int i = 0; i < 32; i++) {
    // if(!(i%4))printf(" ");
    printf("%d", (timeByte & 1));
    timeByte = timeByte >> 1;
  }
  printf("\n");
}

void printStruct(s21_decimal dst) {
  for (int i = 0; i < 4; i++) {
    printf("bytes №%d = ", i + 1);
    printBits(dst.bits[i]);
  }
}

void printBigStruct(big_decimal dst) {
  for (int i = 0; i < 8; i++) {
    printf("bytes №%d = ", i + 1);
    printBits(dst.bits[i]);
  }
}

int s21_mul_my(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  big_decimal bigValue_1 = to_big_decimal(value_1);
  big_decimal bigValue_2 = to_big_decimal(value_2);
  big_decimal bigResult = to_big_decimal(*result);
  s21_decimal null = {0};
  int bag = 0, profit = 0;
  if (equal_null(bigValue_1) || equal_null(bigValue_2)) {
    *result = null;
    profit++;
  }
  if (!profit) {
    if (s21_is_less(value_1, value_2)) {
      multiply(bigValue_2, bigValue_1, &bigResult);
    } else {
      multiply(bigValue_1, bigValue_2, &bigResult);
    }
    repeat_exponent(convert_pow(bigValue_1) + convert_pow(bigValue_2),
                    &bigResult);
    if (sign(value_1.bits[3]) != sign(value_2.bits[3]))
      bigResult.bits[7] = bigResult.bits[7] | 1 << 31;
    bag = from_big_decimal_to_decimal(bigResult, result);
  }
  if (profit && result->bits[0] == 0 && result->bits[1] == 0 &&
      result->bits[2] == 0 && (sign(value_1.bits[3]) || sign(value_2.bits[3])))
    result->bits[3] |= 1 << 31;
  return bag;
}
