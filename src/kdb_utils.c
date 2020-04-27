/* --- General use utils for handling kdb objects --- 
 * The purpose of this script is to house utility functions which are used here for
 * the hdf5 interface but are also generally applicable for other interfaces
*/

#include "kdb_utils.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

/* get k string or symbol name */
char * getkstring(K x){
  char *s=NULL;
  int len;
  switch (xt){
    case -KC :
      s = calloc(2,1); s[0] = xg; break;
    case KC :
      s = calloc(1+xn,1); memmove(s, xG, xn); break;
    case -KS : 
      len = 1+strlen(xs);
      s = calloc(len,1); memmove(s, xs, len); break;
    default : krr("invalid name");
  }
  return s;
}

/* type checking functionality */
int checkType(const C* tc, ...){
  va_list args;
  K x;
  static C lt[256]= " tvunzdmpscfejihg xb*BX GHIJEFCSPMDZNUVT";
  static C b[256];
  const C* tc0= tc;
  I match=0;
  lt[20 + 98]= '+';
  lt[20 + 99]= '!';
  va_start(args, tc);
  for(; *tc;){
    match= 0;
    x= va_arg(args, K);
    if(!x){
      strcpy(b, "incomplete type string ");
      break;
    };
    if('[' == *tc){
      while(*tc && ']' != *tc){
        match= match || lt[20 + xt] == *tc;
        ++tc;
      }
    }
    else
      match= lt[20 + xt] == *tc;
    if(!match){
      strcat(strcpy(b, "type:expected "), tc0);
      break;
    };
    ++tc;
  }
  va_end(args);
  if(!match)
    krr(b);
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
