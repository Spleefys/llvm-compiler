#include <stdio.h>

int echo(const char *str) {
  printf("%s\n", str);
  return 0;
}

int print(const int number) {
  printf("%d\n", number);
  return 0;
}

int echoc(const char ch) {
  printf("%c\n", ch);
  return 0;
}
