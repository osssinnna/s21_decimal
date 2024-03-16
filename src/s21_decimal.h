#ifndef SRC_S21_DECIMAL_H_
#define SRC_S21_DECIMAL_H_

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INF 1.0 / 0.0
#define MAXBITS 4294967295U
#define FLOAT_MIN 1e-28
enum { OK, CONVERTATION_FAULT };

typedef struct {
  unsigned int bits[4];
} s21_decimal;

typedef struct {
  unsigned int bits[8];
} big_decimal;

int s21_floor(s21_decimal value, s21_decimal *result);

int s21_round(s21_decimal value, s21_decimal *result);

int s21_truncate(s21_decimal value, s21_decimal *result);

int s21_negate(s21_decimal value, s21_decimal *result);

//------------------------------------------------------

// void printBits(unsigned int number);
// void printStruct(s21_decimal dst);
// void printBigStruct(big_decimal dst);

int s21_from_int_to_decimal(int src, s21_decimal *dst);
int s21_from_decimal_to_float(s21_decimal src, float *dst);
int convert_pow(big_decimal number);
int sign(unsigned int signs);
void equating_pow(big_decimal *value_1, big_decimal *value_2);
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int sum_int(big_decimal value_1, big_decimal value_2, big_decimal *result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
void multiply(big_decimal value_1, big_decimal value_2, big_decimal *result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int degradation(big_decimal value_1, big_decimal value_2, big_decimal *result);
void move_left(big_decimal *number);
void move_right(big_decimal *number);
int find_symbol(big_decimal number);
int s21_is_less(s21_decimal value_1, s21_decimal value_2);
int is_less_big(big_decimal value_1, big_decimal value_2);
void repeat_exponent(unsigned int origin, big_decimal *convert);
int equal_null(big_decimal value_1);
int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2);
int less_or_equal(big_decimal value_1, big_decimal value_2);
int s21_is_greater(s21_decimal value_1, s21_decimal value_2);
int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int div_prev(big_decimal value_1, big_decimal value_2, big_decimal *result);
int segmentation(big_decimal value_1, big_decimal value_2, big_decimal *result);
void round_segmentation(big_decimal *number, int round);
big_decimal to_big_decimal(s21_decimal value);
int from_big_decimal_to_decimal(big_decimal number, s21_decimal *result);
int s21_from_float_to_decimal(float src, s21_decimal *dst);
void float_to_decimal_helper(float src, char *str, int *scale);
void transfer_to_2(char **str);
int del_str_by_2(const char *str, char **new_str);
void reverse_str(char **str);

//---------------------------------------------------------   NaN

int s21_add_my(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_sub_my(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mul_my(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_div_my(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

int bit_add(big_decimal big_value_1, big_decimal big_value_2,
            big_decimal *big_value_3);
int get_bit(unsigned int n, int i);
void set_bit(int buff[32], unsigned int *bits);
int get_exponent(unsigned int n);
void equal_bits(s21_decimal value_1, big_decimal *big_value_1);
void set_buff_to_bit(int buff[256], big_decimal *big_value_3);
void left(big_decimal *big_value_1, int n);
void load_bit(unsigned int *x, int n, int m);
void pow_10(big_decimal *big_value_2, int n);
void clear_bits(big_decimal *big_value_2);
void compare_pow(big_decimal *big_value_1, big_decimal *big_value_2,
                 big_decimal *big_value_3);
int get_sign_int(unsigned int n);
int check_sign(s21_decimal value_1, s21_decimal value_2);
void set_sign(s21_decimal value_1, unsigned int *x);
int bit_sub(big_decimal big_value_1, big_decimal big_value_2,
            big_decimal *big_value_3);
int compare_value(s21_decimal value_1, s21_decimal value_2);
int bit_mul(big_decimal big_value_1, big_decimal big_value_2,
            big_decimal *big_value_3);
void equal_big_bits(big_decimal *big_value_1, big_decimal *big_value_2);
void div_bit(big_decimal big_value_1, big_decimal big_value_2,
             big_decimal *big_value_3);
int equal_zero(s21_decimal value_1);
void norm_pow(big_decimal *big_value_3);
void set_exponent(int pow, big_decimal *big_value_3);
void inverse_sign(s21_decimal value_1, unsigned int *x);
void mul_pow(big_decimal big_value_1, big_decimal big_value_2,
             big_decimal *big_value_3);

// not_my

int s21_from_decimal_to_int(s21_decimal src, int *dst);
void truncate(s21_decimal *big_value_3);
void set_sign_to_int(s21_decimal value_1, int *x);

int s21_floor_my(s21_decimal value, s21_decimal *result);
int s21_round_my(s21_decimal value, s21_decimal *result);
int s21_truncate_my(s21_decimal value, s21_decimal *result);
int s21_negate_my(s21_decimal value, s21_decimal *result);

//---------------------------------------Dasha

//вспомогательные функции
int get_zn(s21_decimal);                    //получить знак
void set_big_bit(big_decimal *, int, int);  //заменить бит
int dv_des_pow(s21_decimal);  //получение степени десятки в десятичной сс
int dv_des_mantissa(s21_decimal);  //преобразование мантиссы в двоичную сс
void big_des_dv_mantissa(
    int, big_decimal *);  //преобразование мантисы в лесятичную сс
void big_zero_mantissa(big_decimal *);  //заполнение нулями
void com_denom(s21_decimal, s21_decimal, big_decimal *, big_decimal *);
void multiplicat(big_decimal, big_decimal, big_decimal *);

int check_zero(s21_decimal);
void big_dec_transf(s21_decimal, big_decimal *);
int find_symbol_d(big_decimal number);
int move_left_d(big_decimal *number);
int sum_int_d(big_decimal value_1, big_decimal value_2, big_decimal *result);

//----------------------------------------------------------------

int from_decimal_to_long_double(s21_decimal src, long double *dst);

s21_decimal set_to_null(s21_decimal value);
void set_bit_0(s21_decimal *value, int pos);
void set_bit_1(s21_decimal *value, int pos);
void set_exponent_normal(s21_decimal *value, int exp);

#endif  // SRC_S21_DECIMAL_H_
