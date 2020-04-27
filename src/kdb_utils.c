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

// check types of args
int kdbCheckType(const char *typePattern, ...){
  int match = 0;
  static char errstr[256];
  static char ktypes[256] = " tvunzdmpscfejihg xb*BX GHIJEFCSPMDZNUVT";
  ktypes[20 + 98] = '+';
  ktypes[20 + 99] = '!';
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
