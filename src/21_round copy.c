#include "./s21_decimal.h"

int s21_round(s21_decimal value, s21_decimal *result) {
  s21_decimal null = {0}, result2 = {0}, one = {{0b1, 0}};
  int ostatok = 0, bag = 0, trueOstatok = 0;
  *result = null;
  big_decimal number = to_big_decimal(value);
  big_decimal copyNumber = number, ten = {{0b1010, 0}};
  if (!convert_pow(number)) {
    *result = value;
  } else {
    if (!equal_null(number)) {
      for (int exp = convert_pow(number); exp > 0; exp--) {
        repeat_exponent(28, &copyNumber);
        ostatok = segmentation(copyNumber, ten, &copyNumber);
      }

      from_big_decimal_to_decimal(copyNumber, &value);
      printStruct(value);

      s21_truncate(value, &result2);

      printf("\njkhakjsfhkjsafhajksfhjaksfhaskjf\n");
      printStruct(result2);
      copyNumber = to_big_decimal(result2);
      value = result2;
      result2 = null;
      repeat_exponent(28, &copyNumber);
      ostatok = segmentation(copyNumber, ten, &copyNumber);
      printf("ostatok = %d\n", ostatok);
      value.bits[3] = value.bits[3] | 1 << 16;
      s21_truncate(value, &result2);
      if (ostatok == 1 || ostatok == 0) {
        if (!sign(value.bits[3])) {
          printf("!!!!!!!!!!!!!!!!\n");
          printStruct(result2);
          bag = s21_add(result2, one, &result2);
        } else {
          bag = s21_sub(result2, one, &result2);
        }
      }
      *result = result2;
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