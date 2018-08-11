#include "macro.h"

u16 get2Byte(char* c){
  u16 res = c[1];
  res = (res << 8) | c[0];

  return res;
};

s32 get4Byte(char* c){
  s32* res = (s32*)c;
//  *res = c[3] << 24 | c[2] << 16 | c[1] <<8 | c[0];

  return *res;
};
