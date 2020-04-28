#include "kdb_utils.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

// get c string from kdb char, string or symbol
char * kdbGetString(K x){
  char *str = NULL;
  int len;
  switch (xt){
    case -KC:
      str = malloc(2);
      str[0] = xg;
      str[1] = 0;
      break;
    case  KC:
      str = malloc(1+xn);
      memmove(str, xG, xn);
      str[xn] = 0;
      break;
    case -KS: 
      len = 1 + strlen(xs);
      str = malloc(len);
      memmove(str, xs, len);
      str[len] = 0;
      break;
    default:
      krr((S)"type");
  }
  return str;
}

// check types of args vs typePattern
int kdbCheckType(const char *typePattern, ...){
  int match = 0;
  static char errstr[256];
  static char ktypes[256] = " tvunzdmpscfejihg xb*BX GHIJEFCSPMDZNUVT"; // atom/list types
  ktypes[20 + 98] = '+'; // table
  ktypes[20 + 99] = '!'; // dictionary
  K arg;
  short argtype;
  va_list args;
  va_start(args, typePattern);
  const C* tc = typePattern;
  while(*tc){
    match = 0;
    arg = va_arg(args, K);
    if(!arg){
      strcpy(errstr, "incomplete type string");
      break;
    };
    argtype = 20 + arg->t;
    if('[' == *tc){
      while( *tc && (']' != *tc) ){
        match = match || (ktypes[argtype] == *tc);
        ++tc;
      }
    }
    else
      match = ktypes[argtype] == *tc;
    if(!match){
      strcat(strcpy(errstr, "type: expected "), typePattern);
      break;
    }
    ++tc;
  }
  va_end(args);
  if(!match)
    krr(errstr);
  return match;
}

// create dictionary (key, val, key, val, ...)
K kdbCreateDict0(void *dummy, ...){
  va_list args;
  K keys = ktn(KS, 0);
  K vals = ktn(0 , 0);
  S key;
  K val;
  va_start(args, dummy);
  while( (key=va_arg(args, S)) && (val=va_arg(args, K)) ){
    js(&keys, ss(key));
    jk(&vals, val);
  }
  va_end(args);
  return xD(keys, vals);
}
