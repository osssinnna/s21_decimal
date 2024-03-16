#include "s21_decimal.h"

int find_symbol_d(big_decimal number) {
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

int move_left_d(big_decimal *number) {
  for (int i = 6; i >= 0; i--) {
    if (number->bits[i] & 1 << 31)
      number->bits[i + 1] = number->bits[i + 1] | 1;
    number->bits[i] = number->bits[i] << 1;
  }
  return number->bits[7] & 1;
}
int sum_int_d(big_decimal value_1, big_decimal value_2, big_decimal *result) {
  int buff = 0, bag = 0;
  big_decimal nul = {0};
  *result = nul;
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
  return bag;
}
void multiplicat(big_decimal value_1, big_decimal value_2,
                 big_decimal *result) {
  int index_byte = 0, index_mul = 0;
  for (int i = 0; i < 7; i++) result->bits[i] = 0;
  for (int k = 0; k < find_symbol_d(value_2); k++) {
    big_decimal result_time = {0};
    for (int i = 0; i < 7; i++) {
      for (int j = 0; j < 32; j++) {
        if (value_1.bits[i] & 1 << j &&
            value_2.bits[index_byte] & 1 << index_mul) {
          result_time.bits[i] = result_time.bits[i] | 1 << j;
        }
      }
    }
    for (int g = 0; g < k; g++) move_left_d(&result_time);
    sum_int_d(*result, result_time, result);
    index_mul++;
    if (index_mul > 31) {
      index_mul = 0;
      index_byte++;
    }
  }
}

//--------------ЗАМЕНА
//БИТА------------------------------------------------------
void set_big_bit(big_decimal *value, int index, int set) {
  unsigned move = 1;
  unsigned mask = move << (index % 32);
  if (set == 0) {
    value->bits[index / 32] = value->bits[index / 32] & ~mask;
  }
}
//----------------ОПРЕДЕЛЕНИЕ
//ЗНАКА----------------------------------------------
int get_zn(s21_decimal value) { return value.bits[3] >> 31; }
//-------------------------------------------------------------------------------

//---------------СТЕПЕНЬ
//ДЕСЯТКИ-------------------------------------------------
//  из двоичной сс в десятичную
int dv_des_pow(s21_decimal value) {
  int two_pow = 1, sum = 0;
  value.bits[3] = value.bits[3] >> 16;
  for (int i = 0; i < 8; i++) {
    if (value.bits[3] & 1) {
      sum += two_pow;
    }
    two_pow *= 2;
    value.bits[3] = value.bits[3] >> 1;
  }
  return sum;
}
//-------------------------------------------------------------------------------

//---------------------ЗАПОЛНЕНЯЕМ НУЛЯМИ БОЛЬШОЕ
//ЧИСЛО---------------------------
void big_zero_mantissa(big_decimal *value) {
  for (int i = 0; i < 224; i++) {
    set_big_bit(value, i, 0);
  }
}

//----------------ПРЕОБРАЗОВАНИЕ В БОЛЬШОЕ
//ЧИСЛО---------------------------------
void big_dec_transf(s21_decimal value1, big_decimal *big_value1) {
  for (int i = 0; i < 3; i++) {
    big_value1->bits[i] = value1.bits[i];
  }
  big_value1->bits[7] = value1.bits[3];
}
//-------------------------------------------------------------------------------
//---------ПРОВЕРКА НА
//НОЛЬ------------------------------------------------------
int check_zero(s21_decimal value) {
  int flag = 0;
  if (value.bits[0] == 0 && value.bits[1] == 0 && value.bits[2] == 0) {
    flag = 1;
  }
  return flag;
}

//----------------ОБЩИЙ
//ЗНАМЕНАТЕЛЬ----------------------------------------------
void com_denom(s21_decimal value1, s21_decimal value2, big_decimal *big_value1,
               big_decimal *big_value2) {
  int razn, pow1 = dv_des_pow(value1), pow2 = dv_des_pow(value2);
  big_decimal ten = {{10, 0, 0, 0, 0, 0, 0}};
  if (pow1 < pow2) {
    razn = pow2 - pow1;
    for (int i = 0; i < razn; i++) {
      multiplicat(ten, *big_value1, big_value1);
    }
  } else if (pow1 > pow2) {
    razn = pow1 - pow2;
    for (int i = 0; i < razn; i++) {
      multiplicat(ten, *big_value2, big_value2);
    }
  }
}
