#ifndef __UTILITYFN_H__
#define __UTILITYFN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

int Utility_hexToInt(const char* s, int count);
void Utility_intToHex(char* dst, const unsigned char* ptr, int count);
int Utility_intToA(char* ptr, int value, int radix);
int Utility_intToAPadded(char* ptr, int value, int radix, int padding);
int Utility_aToInt(const char* ptr);
double Utility_aToDouble(const char * ptr);
int Utility_doubleToA(char *s, double n);
int Utility_doubleToAEx(char *s, double n, double precision);
size_t Utility_doubleToASci(char * s, double n);
void Utility_hexToBytes(char* dst, char* src);
extern const char * const Utility_EMPTYSTRING;
char * Utility_reverseTokenise(char * string, char * tokens, int skip);
char * Utility_tokeniseString(char * string, char * delimiters, char ** context);
int Utility_removeCharacters(char * string, const char * tomatch);
int Utility_endsWith(const char * string, const char * tomatch);
int Utility_isInteger(const char * str);
int Utility_isNumber(const char * str);
int Utility_dumphex(char * destination, const char * source, int len);

#ifdef __cplusplus
};
#endif

#endif // __UTILITYFN_H__
