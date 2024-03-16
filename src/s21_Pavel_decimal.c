#include "./s21_decimal.h"

int from_big_decimal_to_decimal(big_decimal number, s21_decimal *result) {
  s21_decimal null = {0};
  *result = null;
  int bag = 0, profit = 0, ostatok = 0, minus = sign(number.bits[7]),
      prevOstatok = 0;
  unsigned int exp = convert_pow(number);
  big_decimal ten = {{0b1010, 0}}, copyNumber = number;
  if (number.bits[3] == 0 && number.bits[4] == 0 && number.bits[5] == 0 &&
      number.bits[6] == 0 && exp < 29)
    profit = 1;
  if (!profit && !exp) bag = 1 + minus;
  if (!bag) {
    if (!profit) repeat_exponent(28, &number);
    while (!profit && !bag) {
      exp--;
      ostatok = segmentation(number, ten, &number);
      prevOstatok++;
      if (number.bits[3] == 0 && number.bits[4] == 0 && number.bits[5] == 0 &&
          number.bits[6] == 0 && exp < 29) {
        copyNumber = number;
        if (prevOstatok > 1 && ostatok == 0) ostatok++;
        round_segmentation(&copyNumber, ostatok);
        if (!number.bits[3]) {
          profit = 1;
          number = copyNumber;
        }
      } else {
        if (exp == 0) bag = 1 + minus;
      }
    }
    if (exp == 0 && (copyNumber.bits[3] || copyNumber.bits[4] ||
                     copyNumber.bits[5] || copyNumber.bits[6]))
      bag = 1 + minus;
    if (!bag) {
      repeat_exponent(exp, &number);
      if (minus) number.bits[7] = number.bits[7] | 1 << 31;
      result->bits[0] = number.bits[0];
      result->bits[1] = number.bits[1];
      result->bits[2] = number.bits[2];
      result->bits[3] = number.bits[7];
    }
  }
  return bag;
}

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  int result = OK;
  if (dst) memset(dst, 0, sizeof(s21_decimal));
  if (isnan(src) || src == INFINITY || src == -INFINITY) {
    result = CONVERTATION_FAULT;
    if (src == -INFINITY) dst->bits[3] = -2147483648;
  }
  if ((fabs(src) > 0 && fabs(src) < 1e-28) || fabs(src) >= pow(2, 96))
    result = CONVERTATION_FAULT;
  if (dst && (result != CONVERTATION_FAULT)) {
    char *str = calloc(129, sizeof(char));
    int scale = 0;
    if (src < 0) dst->bits[3] = dst->bits[3] | 1 << 31;
    src = fabs(src);
    float_to_decimal_helper(src, str, &scale);
    int minus = sign(dst->bits[3]);
    dst->bits[3] = 0;
    dst->bits[3] = scale << 16;
    if (minus) dst->bits[3] = dst->bits[3] | 1 << 31;
    transfer_to_2(&str);
    for (size_t i = 0; i < strlen(str); i++)
      if (str[strlen(str) - 1 - i] == '1') dst->bits[i / 32] |= 1 << (i % 32);
    free(str);
  } else {
    result = CONVERTATION_FAULT;
  }
  return result;
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_decimal null = {0};
  *result = null;
  big_decimal bigValue_1 = to_big_decimal(value_1);
  big_decimal bigValue_2 = to_big_decimal(value_2);
  big_decimal bigResult = to_big_decimal(*result);
  int bag = div_prev(bigValue_1, bigValue_2, &bigResult);
  if (!bag) bag = from_big_decimal_to_decimal(bigResult, result);
  return bag;
}

int div_prev(big_decimal value_1, big_decimal value_2, big_decimal *result) {
  int bag = 0, minus = 0;
  big_decimal null = {0};
  if (sign(value_1.bits[7]) != sign(value_2.bits[7])) {
    value_1.bits[7] = value_1.bits[7] & ~(1 << 31);
    value_2.bits[7] = value_2.bits[7] & ~(1 << 31);
    minus = 1;
  }
  if (equal_null(value_2)) {
    bag = 4;
    *result = null;  // 123 / 0   = 0  bag=3
  } else {
    if (equal_null(value_1)) {
      bag = 1;
      *result = null;
    }  // 0 / 123   = 0
    if (!bag) {
      big_decimal copy_1 = value_1, copy_2 = value_2;
      if (convert_pow(copy_1) != convert_pow(copy_2))
        equating_pow(&copy_1, &copy_2);
      if (less_or_equal(value_1, value_2) && !is_less_big(value_1, value_2)) {
        bag = 1;
        result->bits[0] = 1;
      }  // 123 / 123 = 1
    }
    if (!bag) {
      for (int i = convert_pow(value_2); i != 0; i--) {
        if (convert_pow(value_1)) {
          repeat_exponent(convert_pow(value_1) - 1, &value_1);
        } else {
          big_decimal ten = {{0b1010, 0}};
          big_decimal result_copy = {0};
          multiply(value_1, ten, &result_copy);
          value_1 = result_copy;
        }
      }
      value_2.bits[7] = 0;
      round_segmentation(result, segmentation(value_1, value_2, result));
      if (minus) result->bits[7] = result->bits[7] | 1 << 31;
      bag = 1;
    }
  }
  return bag - 1;
}

int segmentation(big_decimal value_1, big_decimal value_2,
                 big_decimal *result) {
  big_decimal result_copy = {0}, ostatok = {0}, null = {0};
  result_copy.bits[7] = result->bits[7];
  *result = result_copy;
  int stop = 0, exponentUP = 0;
  int targetSimvol = find_symbol(value_1) - 1;
  int pow_result = convert_pow(value_1);
  while (!stop) {
    if (targetSimvol + 1)
      if (value_1.bits[targetSimvol / 32] & 1 << targetSimvol % 32)
        ostatok.bits[0] = ostatok.bits[0] | 1;
    if (less_or_equal(value_2, ostatok)) {
      result_copy.bits[0] = result_copy.bits[0] | 1;
      degradation(ostatok, value_2, &ostatok);
    }
    move_left(&result_copy);
    move_left(&ostatok);
    targetSimvol--;
    if ((equal_null(ostatok) && targetSimvol + 1 <= 0) || pow_result > 28)
      stop = 1;
    if (targetSimvol + 1 <= 0 && !stop) {
      exponentUP = 1;
      move_right(&result_copy);
      big_decimal ten = {{0b1010, 0}};
      sum_int(*result, result_copy, result);
      if (pow_result == 28) stop++;
      if (pow_result < 28) {
        if (!equal_null(*result)) multiply(*result, ten, result);
        move_right(&ostatok);
        multiply(ostatok, ten, &value_1);
        ostatok = null;
        targetSimvol = find_symbol(value_1) - 1;
        pow_result++;
        result_copy = null;
      }
    }
  }
  move_right(&result_copy);
  move_right(&ostatok);
  if (!exponentUP) *result = result_copy;
  repeat_exponent(pow_result, result);
  move_right(&value_2);
  int round = 0;                            //   1 ==   >
  round = less_or_equal(value_2, ostatok);  //  -1 ==   <
  if (!round) {
    round = -1;
  } else {
    round = is_less_big(value_2, ostatok);
  }                                    //   0 ==   ==
  if (equal_null(ostatok)) round = 2;  //   2 ==   деление нацело
  return round;
}

void round_segmentation(big_decimal *number, int round) {
  big_decimal one = {{1, 0}};
  int exp = convert_pow(*number);
  repeat_exponent(exp, &one);
  if (round == 0 || round == 1) sum_int(*number, one, number);
  if (round == 0) number->bits[0] = number->bits[0] & ~1;
  repeat_exponent(exp, number);
}

big_decimal to_big_decimal(s21_decimal value) {
  big_decimal value_big = {0};
  value_big.bits[7] = value.bits[3];
  value_big.bits[0] = value.bits[0];
  value_big.bits[1] = value.bits[1];
  value_big.bits[2] = value.bits[2];
  return value_big;
}

//    value_1 <  value_2

int is_less_big(big_decimal value_1, big_decimal value_2) {
  int result = 0;
  if (sign(value_1.bits[7]) && !sign(value_2.bits[7])) {
    result = 2;  //если - и +
  } else {
    if (!sign(value_1.bits[7]) && sign(value_2.bits[7])) {
      result = 1;  //если + и -
    }
  }
  if (!result) {
    if (convert_pow(value_1) != convert_pow(value_2))
      equating_pow(&value_1, &value_2);
    int no = 0;
    for (int i = 0; i < 8; i++)
      if (value_1.bits[i] == value_2.bits[i]) no++;
    if (no == 8) result = 1;
  }  //если = и =
  if (!result) {
    for (int i = 6; i >= 0; i--) {
      for (int j = 31; j >= 0; j--) {
        if ((value_1.bits[i] & 1 << j) != (value_2.bits[i] & 1 << j)) {
          if ((value_1.bits[i] & 1 << j) < (value_2.bits[i] & 1 << j)) {
            result = 2;
          } else {
            result = 1;
          }
        }
        if (result) break;
      }
      if (result) break;
    }
  }
  return result - 1;
}

int less_or_equal(big_decimal value_1, big_decimal value_2) {
  int result = 0;
  if (sign(value_1.bits[7]) && !sign(value_2.bits[7])) {
    result = 2;  //если - и +
  } else {
    if (!sign(value_1.bits[7]) && sign(value_2.bits[7])) {
      result = 1;  //если + и -
    }
  }
  if (!result) {
    if (convert_pow(value_1) != convert_pow(value_2))
      equating_pow(&value_1, &value_2);
    int no = 0;
    for (int i = 0; i < 8; i++)
      if (value_1.bits[i] == value_2.bits[i]) no++;
    if (no == 8) result = 2;
  }  //если = и =
  if (!result) {
    for (int i = 6; i >= 0; i--) {
      for (int j = 31; j >= 0; j--) {
        if ((value_1.bits[i] & 1 << j) != (value_2.bits[i] & 1 << j)) {
          if ((value_1.bits[i] & 1 << j) < (value_2.bits[i] & 1 << j)) {
            result = 2;
          } else {
            result = 1;
          }
        }
        if (result) break;
      }
      if (result) break;
    }
  }
  return result - 1;
}

void reverse_str(char **str) {
  char *new = calloc(strlen(*str) + 1, sizeof(char));
  for (size_t i = 0; i < strlen(*str); i++)
    new[i] = (*str)[strlen(*str) - i - 1];
  free(*str);
  *str = new;
}

// value_1  -  value_2 = *result

int degradation(big_decimal value_1, big_decimal value_2, big_decimal *result) {
  big_decimal nul = {0};
  *result = nul;
  int no = 0;
  for (int i = 0; i < 7; i++)
    if (value_1.bits[i] == value_2.bits[i]) no++;
  result->bits[7] = value_1.bits[7];
  if (no != 7) {
    value_1.bits[7] = value_2.bits[7];
    if (is_less_big(value_1, value_2)) {
      big_decimal time = value_1;
      value_1 = value_2;
      value_2 = time;
      result->bits[7] = result->bits[7] | 1 << 31;
    }
    int limits = find_symbol(value_1);
    for (int j = 0; j < limits; j++) {
      value_2.bits[j / 32] = value_2.bits[j / 32] ^ 1 << j % 32;
    }
    big_decimal one = {{1, 0}}, result_time = {0};
    sum_int(value_2, one, &result_time);
    value_2 = result_time;
    result_time = nul;
    sum_int(value_1, value_2, &result_time);
    int cutting_symbol = find_symbol(result_time) - 1;
    result_time.bits[cutting_symbol / 32] =
        result_time.bits[cutting_symbol / 32] ^ 1 << cutting_symbol % 32;
    for (int i = 0; i < 7; i++) result->bits[i] = result_time.bits[i];
  }
  return 0;
}

int sign(unsigned int signs) { return 1 & signs >> 31; }

void equating_pow(big_decimal *value_1, big_decimal *value_2) {
  big_decimal ten = {{0b1010, 0}};
  big_decimal result = {0};
  big_decimal number_pow_min = *value_1;
  int oneGreater = 0, value_1_znak = sign(value_1->bits[7]),
      value_2_znak = sign(value_2->bits[7]);
  int pow_one = convert_pow(*value_1);
  int pow_two = convert_pow(*value_2);
  int pow_round = pow_two - pow_one;
  if (pow_one > pow_two) {
    number_pow_min = *value_2;
    pow_round = pow_one - pow_two;
    oneGreater = 1;
  }
  for (; pow_round > 0; pow_round--) {
    multiply(number_pow_min, ten, &result);
    number_pow_min = result;
  }
  if (oneGreater) {
    *value_2 = result;
    repeat_exponent(pow_one, value_2);
    if (value_2_znak) value_2->bits[7] = value_2->bits[7] | 1 << 31;
  } else {
    *value_1 = result;
    repeat_exponent(pow_two, value_1);
    if (value_1_znak) value_1->bits[7] = value_1->bits[7] | 1 << 31;
  }
}

void repeat_exponent(unsigned int origin, big_decimal *convert) {
  int minus = sign(convert->bits[7]);
  convert->bits[7] = 0;
  if (origin > 0) convert->bits[7] = origin << 16;
  if (minus) convert->bits[7] = convert->bits[7] | 1 << 31;
}

//////-----------------------value_1 * value_2 = result

void multiply(big_decimal value_1, big_decimal value_2, big_decimal *result) {
  int index_byte = 0, index_mul = 0;
  big_decimal null = {0};
  *result = null;
  if (equal_null(value_1) || equal_null(value_2)) {
    *result = null;
  } else {
    for (int i = 0; i < 7; i++) result->bits[i] = 0;
    for (int k = 0; k < find_symbol(value_2); k++) {
      big_decimal result_time = {0};
      for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 32; j++) {
          if (value_1.bits[i] & 1 << j &&
              value_2.bits[index_byte] & 1 << index_mul) {
            result_time.bits[i] = result_time.bits[i] | 1 << j;
          }
        }
      }
      for (int g = 0; g < k; g++) move_left(&result_time);
      sum_int(*result, result_time, result);
      index_mul++;
      if (index_mul > 31) {
        index_mul = 0;
        index_byte++;
      }
    }
  }
}

int find_symbol(big_decimal number) {
  int point = 224, stop = 0;
  for (int i = 6; i >= 0; i--) {
    for (int j = 31; j >= 0; j--) {
      if (number.bits[i] & 1 << j) {
        stop = 1;
        break;
      }
      point--;
    }
    if (stop) break;
  }
  return point;
}

void move_left(big_decimal *number) {
  for (int i = 6; i >= 0; i--) {
    if (number->bits[i] & 1 << 31)
      number->bits[i + 1] = number->bits[i + 1] | 1;
    number->bits[i] = number->bits[i] << 1;
  }
  number->bits[7] = number->bits[7] & ~1;
}

void move_right(big_decimal *number) {
  for (int i = 0; i <= 6; i++) {
    number->bits[i] = number->bits[i] >> 1;
    if (number->bits[i + 1] & 1) number->bits[i] = number->bits[i] | 1 << 31;
  }
}

int del_str_by_2(const char *str, char **new_str) {
  free(*new_str);
  *new_str = calloc(128, sizeof(char));
  char *current = *new_str;
  int ost = (*(str + strlen(str) - 1) - '0') % 2;
  int d = 0, opers = 0;
  while (*str) {
    opers++;
    d = d * 10 + (*str - '0');
    str++;
    while (d < 2 && *str) {
      if (opers != 1) {
        *current = '0';
        current++;
      }
      d *= 10;
      d += *str - '0';
      str++;
      opers++;
    }
    *current = (d / 2) + '0';
    current++;
    d -= (d / 2) * 2;
  }
  return ost;
}

int sum_int(big_decimal value_1, big_decimal value_2, big_decimal *result) {
  int buff = 0, bag = 0;
  big_decimal nul = {0};
  *result = nul;
  int no = 0;
  for (int i = 0; i < 7; i++)
    if (!value_1.bits[0] && !value_2.bits[0]) no++;
  if (no != 7) {
    for (int i = 0; i < 7; i++) {
      for (int j = 0; j < 32; j++) {
        if (value_1.bits[i] & 1 << j || value_2.bits[i] & 1 << j) {
          if (value_1.bits[i] & 1 << j && value_2.bits[i] & 1 << j) {
            if (buff) {
              result->bits[i] = result->bits[i] | 1 << j;
              buff--;
            }
            buff++;
          } else {
            if (!buff) result->bits[i] = result->bits[i] | 1 << j;
          }
        } else {
          if (buff) {
            result->bits[i] = result->bits[i] | 1 << j;
            buff--;
          }
        }
      }
    }
    if (buff) bag = 1;
  }
  return bag;
}

int equal_null(big_decimal value_1) {
  int no = 0;
  for (int i = 0; i < 7; i++)
    if (!value_1.bits[i]) no++;
  return no / 7;
}

int convert_pow(big_decimal number) {
  int point = 0;
  if (sign(number.bits[7])) {
    number.bits[7] = number.bits[7] << 1;
    if (number.bits[7]) point = number.bits[7] >> 17;
  } else {
    if (number.bits[7]) point = number.bits[7] >> 16;
  }
  return point;
}

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  int bag = 0;
  long double number = 0;
  bag = from_decimal_to_long_double(src, &number);
  *dst = number;
  return bag;
}

int from_decimal_to_long_double(s21_decimal src, long double *dst) {
  double pow = 1;
  long double number = 0;
  int bag = 0, point = 0;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 32; j++) {
      number = number + (src.bits[i] & 1) * pow;
      pow *= 2;
      src.bits[i] = src.bits[i] >> 1;
    }
  }
  pow = 1;
  src.bits[3] = src.bits[3] >> 16;
  for (int j = 0; j < 8; j++) {
    point = point + (src.bits[3] & 1) * pow;
    pow *= 2;
    src.bits[3] = src.bits[3] >> 1;
  }
  if (point > 28) {
    bag = 1;
    number = 0;
  } else {
    if (point != 0)
      for (int i = 1; i <= point; i++) number /= 10;
  }
  src.bits[3] = src.bits[3] >> 7;
  if (src.bits[3] & 1) number *= -1;
  *dst = number;
  *dst = number;
  if (number == INF || number == -INF ||
      number > 79228162514264337593543950335.0)
    bag = 1;
  return bag;
}

void float_to_decimal_helper(float src, char *str, int *scale) {
  sprintf(str, "%e", src);
  char *e = strchr(str, 'e');
  *scale += (*(e + 2) - '0') * 10;
  *scale += *(e + 3) - '0';
  if (*(e + 1) == '-') *scale *= -1;
  *e = '\0';
  int temp = 6;
  char *end = str + strlen(str) - 1;
  while (*end != '.' && *end == '0') {
    temp -= 1;
    end--;
  }
  *scale -= temp;
  if (*end == '.') {
    *end = '\0';
  } else {
    *(end + 1) = '\0';
    memmove(str + 1, str + 2, strlen(str + 2));
    *end = '\0';
  }
  while (*scale > 0) {
    end = str + strlen(str);
    *end = '0';
    *(end + 1) = '\0';
    (*scale)--;
  }
  if (*scale < 0) *scale *= -1;
}

void transfer_to_2(char **str) {
  char *new = NULL;
  int res = del_str_by_2(*str, &new);
  char *digit = calloc(129, sizeof(char)), *c = digit;
  while ((*new != '1' && *new != '0') || strlen(*str) > 1) {
    *(c++) = res + '0';
    *str = strcpy(*str, new);
    res = del_str_by_2(*str, &new);
  }
  *(c++) = res + '0';
  *c = *new;
  reverse_str(&digit);
  free(*str);
  *str = digit;
  free(new);
}

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  s21_decimal null = {0};
  *dst = null;
  int znak = 0;
  if (src >> 31 & 1) {
    znak++;
    if (src != -2147483648) src *= -1;
  }
  dst->bits[0] = src;
  dst->bits[3] = dst->bits[3] | znak << 31;
  return 0;
}
