#include "utilityfn.h"

#include <string.h>
#include <malloc.h>
#include <ctype.h> // for isspace isdigit
#include <stddef.h>

#include <math.h> // for log10 pow isnan isinf floor

//#include "dbg.h"

// http://www.netlib.org/fdlibm/
// https://stackoverflow.com/questions/16647278/minimal-implementation-of-sprintf-or-printf
// https://stackoverflow.com/questions/19083152/convert-floating-point-to-scientific-notation-in-c-nonstandard-format

#ifdef __UNITTEST__

#include <stdio.h>

#endif

int Utility_hexToInt(const char* s, int count) {
	int ret = 0;
	char c;
	while ((c = *s++) != '\0' && count--) {
		int n = 0;
		if('0' <= c && c <= '9') { n = c-'0'; }
		else if('a' <= c && c <= 'f') { n = 10 + c - 'a'; }
		else if('A' <= c && c <= 'F') { n = 10 + c - 'A'; }
		ret = n + (ret << 4);
	}
	return ret;
}

static const char hexchars[] = "0123456789ABCDEF";

void Utility_intToHex(char* dst, const unsigned char* ptr, int count) {
	int i;
	int j = (count << 1);
	//const char* hexchars = "0123456789ABCDEF";
	dst[j] = 0;
	for (i = 0; i < count; i++) {
		dst[--j] = hexchars[(ptr[i]) & 0xf];
		dst[--j] = hexchars[(ptr[i] >> 4) & 0xf];
	}
}

void Utility_reverse(char * str, int sz) {
	char * j;
	int c;
	j = str + sz - 1;
	while(str < j) {
		c = *str;
		*str++ = *j;
		*j-- = c;
	}
}

int Utility_intToA(char* ptr, int value, int radix) {
	int orig = value;
	int digit;
	char* dst = ptr;
	int sz = 0;
	if (value < 0) {
		value = -value;
	}
	if (value) {
		while (value) {
			digit = value % radix;
			*dst = hexchars[digit];
			dst++;
			value = value / radix;
			sz++;
		}
	} else {
		*dst = '0';
		dst++;
		sz++;
	}
	if (orig < 0) {
		*dst = '-';
		dst++;
		sz++;
	}
	*dst = '\x00';
	Utility_reverse(ptr, sz);
	return sz;
}

int Utility_intToAPadded(char* ptr, int value, int radix, int padding) {
	int orig = value;
	int digit;
	char* dst = ptr;
	int sz = 0;
	if (value < 0) {
		value = -value;
	}
	while (value) {
		digit = value % radix;
		*dst = hexchars[digit];
		dst++;
		value = value / radix;
		sz++;
	}
	padding = padding - sz;
	while (padding-- > 0) {
		*dst = '0';
		dst++;
		sz++;
	}
	if (orig < 0) {
		*dst = '-';
		dst++;
		sz++;
	}
	*dst = '\x00';
	Utility_reverse(ptr, sz);
	return sz;
}

int Utility_aToInt(const char * ptr) {
	int idx;
	idx = 0;
	int valueisnegative = 0;
	int result_value = 0;
	while (ptr[idx] == '\t' || ptr[idx] == ' ') {
		idx++;
	}
	if (ptr[idx] == '-') {
		valueisnegative = 1;
		idx++;
	} else if (ptr[idx] == '+') {
		idx++;
	}
	while (ptr[idx] >= '0' && ptr[idx] <= '9') {
		result_value = result_value * 10;
		result_value += (ptr[idx] - '0');
		idx++;
	}
	if (valueisnegative) {
		result_value = -(result_value);
	}
	return result_value;
}
/**
 * ASCII to Double
 * 
 */
double Utility_aToDouble(const char * ptr) {
	double val, power, rtn;
	int i, sign = 1;
	i = 0;
	while (ptr[i] == ' ' || ptr[i] == '\t') i++;
	if (ptr[i] == '-') {
		sign = -1;
		i++;
	} else if (ptr[i] == '+') {
		sign = 1;
		i++;
	}
	val = 0.0;
	while (ptr[i] >= '0' && ptr[i] <= '9') {
		val = 10.0 * val + (ptr[i] - '0');
		i++;
	}
	if (ptr[i] == '.') {
		i++;
	}
	power = 1.0;
	while (ptr[i] >= '0' && ptr[i] <= '9') {
		val = 10.0 * val + (ptr[i] - '0');
		power *= 10.0;
		i++;
	}
	rtn = sign * val / power;

	// Next work on exponent
	if (ptr[i] == 'e' || ptr[i] == 'E') {
		int esign;
		int expv = 0;
		i++;
		while (ptr[i] == ' ' || ptr[i] == '\t') { i++; }
		esign = (ptr[i] == '-') ? -1 : 1;
		if (ptr[i] == '+' || ptr[i] == '-') { i++; }
		while (ptr[i] >= '0' && ptr[i] <= '9') {
			expv = 10 * expv + (ptr[i] - '0');
			i++;
		}
		int l;
		// pow()?
		for (l = 0; l < expv; l++) {
			if (esign >= 0) {
				rtn *= 10;
			} else {
				rtn /= 10;
			}
		}
	}
	return rtn;
}

static const double PRECISION = 0.00000000000001;
#pragma warning(suppress: 4068)
#pragma GCC diagnostic push
#pragma warning(suppress: 4068)
#pragma GCC diagnostic ignored "-Wunused-const-variable="
static const int MAX_NUMBER_STRING_SIZE = 32;
#pragma warning(suppress: 4068)
#pragma GCC diagnostic pop

/**
 * Double to ASCII
 */
int Utility_doubleToAEx(char *s, double n, double precision) {
	// handle special cases
	if (isnan(n)) {
		strcpy(s, "nan");
		return 3;
	} else if (isinf(n)) {
		strcpy(s, "inf");
		return 3;
	} else if (n == 0.0) {
		strcpy(s, "0");
		return 1;
	} else {
		int digit, m, m1;
		int neg = (n < 0);
		char *c = s;
		if (neg) {
			n = -n;
		}
		// calculate magnitude
		m = (int)log10(n);
		int useExp = (m >= 14 || (neg && m >= 9) || m <= -9);
		if (neg) {
			*(c++) = '-';
		}
		// set up for scientific notation
		if (useExp) {
			if (m < 0) {
				m -= 1;
			}
			n = n / pow(10.0, m);
			m1 = m;
			m = 0;
		}
		if (m < 1.0) {
			m = 0;
		}
		// convert the number
		while (n > precision || m >= 0) {
			double weight = pow(10.0, m);
			if (weight > 0 && !isinf(weight)) {
				digit = (int)floor(n / weight);
				n -= (digit * weight);
				*(c++) = '0' + digit;
			}
			if (m == 0 && n > 0) {
				*(c++) = '.';
			}
			m--;
		}
		if (useExp) {
			// convert the exponent
			int i, j;
			*(c++) = 'e';
			if (m1 > 0) {
				*(c++) = '+';
			} else {
				*(c++) = '-';
				m1 = -m1;
			}
			m = 0;
			while (m1 > 0) {
				*(c++) = '0' + m1 % 10;
				m1 /= 10;
				m++;
			}
			c -= m;
			for (i = 0, j = m - 1; i<j; i++, j--) {
				int tmp;
				tmp = c[i];
				c[i] = c[j];
				c[j] = tmp;

				// swap without temporary
				// Silly trick, compiler will solve the problem for us.
				//c[i] ^= c[j];
				//c[j] ^= c[i];
				//c[i] ^= c[j];
			}
			c += m;
		}
		*(c) = '\0';
		return (int)(c - s);
	}
}

/**
 * Double to ASCII
 * 
 */
int Utility_doubleToA(char *s, double n) {
	// handle special cases
	if (isnan(n)) {
		strcpy(s, "nan");
		return 3;
	} else if (isinf(n)) {
		strcpy(s, "inf");
		return 3;
	} else if (n == 0.0) {
		strcpy(s, "0");
		return 1;
	} else {
		int digit, m, m1;
		int neg = (n < 0);
		char *c = s;
		if (neg) {
			n = -n;
		}
		// calculate magnitude
		m = (int)log10(n);
		int useExp = (m >= 14 || (neg && m >= 9) || m <= -9);
		if (neg) {
			*(c++) = '-';
		}
		// set up for scientific notation
		if (useExp) {
			if (m < 0) {
				m -= 1;
			}
			n = n / pow(10.0, m);
			m1 = m;
			m = 0;
		}
		if (m < 1.0) {
			m = 0;
		}
		// convert the number
		while (n > PRECISION || m >= 0) {
			double weight = pow(10.0, m);
			if (weight > 0 && !isinf(weight)) {
				digit = (int)floor(n / weight);
				n -= (digit * weight);
				*(c++) = '0' + digit;
			}
			if (m == 0 && n > 0) {
				*(c++) = '.';
			}
			m--;
		}
		if (useExp) {
			// convert the exponent
			int i, j;
			*(c++) = 'e';
			if (m1 > 0) {
				*(c++) = '+';
			} else {
				*(c++) = '-';
				m1 = -m1;
			}
			m = 0;
			while (m1 > 0) {
				*(c++) = '0' + m1 % 10;
				m1 /= 10;
				m++;
			}
			c -= m;
			for (i = 0, j = m-1; i<j; i++, j--) {
				int tmp;
				tmp = c[i];
				c[i] = c[j];
				c[j] = tmp;
			}
			c += m;
		}
		*(c) = '\0';
		return (int)(c - s);
	}
}

static int normalize(double *val) {
	int exponent = 0;
	double value = *val;

	while (value >= 1.0) {
		value /= 10.0;
		++exponent;
	}

	while (value < 0.1) {
		value *= 10.0;
		--exponent;
	}
	*val = value;
	return exponent;
}

static void ftoa_sci(char *buffer, double value) {
	int exponent = 0;
	int i;
	int digit;
	static const int width = 5;

	if (value == 0.0) {
		buffer[0] = '0';
		buffer[1] = '\0';
		return;
	}

	if (value < 0.0) {
		*buffer++ = '-';
		value = -value;
	}

	exponent = normalize(&value);

	digit = (int)(value * 10.0);
	*buffer++ = digit + '0';
	value = value * 10.0 - digit;
	--exponent;

	*buffer++ = '.';

	for (i = 0; i < width; i++) {
		int digit = (int)(value * 10.0);
		*buffer++ = digit + '0';
		value = value * 10.0 - digit;
	}

	*buffer++ = 'e';
	Utility_intToA(buffer, exponent, 10);
}

size_t Utility_doubleToASci(char * s, double n) {
	ftoa_sci(s, n);
	return strlen(s);
}

static int matches(char input, const char * matches) {
	while (*matches != '\0') {
		if (*matches == input) {
			return 1;
		}
		matches++;
	}
	return 0;
}

const char * const Utility_EMPTYSTRING = "";

char * Utility_reverseTokenise(char * string, char * tokens, int skip) {
	int len;
	int pos;
	int size;
	char * buf;
	len = (int)strlen(string);
	pos = (int)(len - 1);
	skip++;
	do {
		if (matches(string[pos], tokens)) {
			do {
				pos--;
			} while (matches(string[pos], tokens) && pos);
			len = pos + 1;
		}
		while (pos >= 0 && !matches(string[pos], tokens)) {
			//fprintf(stdout, "%c %d\n", string[pos], (int)pos);
			pos--;
		}
		skip--;
	} while (skip && pos >= 0);
	pos ++;
	size = len - pos;
	//fprintf(stdout, "pos %d len %d size %d\n", (int)pos, (int)len, (int)size);
	if (size == 0 || skip) {
		return (char *)Utility_EMPTYSTRING;
	}
	buf = malloc(size + 1);
	if (buf == NULL) {
		return NULL;
	}
	memcpy(buf, string + pos, size);
	buf[size] = '\0';
	return buf;
}

int Utility_removeCharacters(char * string, const char * tomatch) {
	int c;
	while ((c = *string)) {
		if (matches(c, tomatch)) {
			char * startat;
			startat = string;
			do {
				*startat = *(startat + 1);
				if (*startat == '\0') {
					break;
				}
				startat ++;
			} while (1);
		} else {
			string ++;
		}
	}
	return 0;
}

int Utility_endsWith(const char * string, const char * tomatch) {
	size_t len;
	size_t matchlen;
	size_t startat;
	matchlen = strlen(tomatch);
	len = strlen(string);
	startat = len - matchlen;
	if (strcmp(string + startat, tomatch) == 0) {
		return 1;
	}
	return 0;
}

// Check if the string is a valid signed integer
int Utility_isInteger(const char * str) {
	if (*str == '\0') {
		// empty string cannot be integer
		return 0;
	}
	if (*str == '-' || *str == '+') {
		str++;
	}
	if (*str == '\0') {
		return 0;
	}
	while (*str != '\0' && (*str >= '0' && *str <= '9')) {
		str++;
	}
	return *str == '\0';
}

// Check is the string is a valid decimal number (a natural number is ok too)
int Utility_isNumber(const char * str) {
	if (*str == '\0') {
		return 0;
	}
	if (*str == '-' || *str == '+') {
		str++;
	}
	if (*str == '\0' || *str == '.') {
		// . is not a number
		// +, -, -. and/or +. are not numbers
		// .1, +.1, -.1 are also not numbers 
		return 0;
	}
	while (*str != '\0' && (*str >= '0' && *str <= '9')) {
		str++;
	}
	// only one decimal place allowed in number
	if (*str == '.') {
		str++;
		// +32. is not a number
		if (*str == '\0') {
			return 0;
		}
		while (*str != '\0' && (*str >= '0' && *str <= '9')) {
			str++;
		}
	}
	return *str == '\0';
}

// Makes a nice string (ASCII printable characters) of a byte buffer.
// When I say nice I mean nicely formatted so it is easy to see the contents
// in hex and printable ASCII characters. LINE_LENGTH is the number of bytes
// per line.

#define LINE_LENGTH 16
static void dump_bytes(char * dest, char * prfx, char * bytes, int len) {
	int i, j;
	int count = 0;
	int offs = 0;
	size_t prfxlen;
	prfxlen = strlen(prfx);

	for (i = 0; i < len; i++) {
		//if (!count) {
		//	strcpy(dest + offs, prfx);
		//	offs += prfxlen;
		//}
		//sprintf(dest + offs, "%02x ", bytes[i] & 0xff);
		Utility_intToHex(dest + offs, &bytes[i], 1);
		offs += 2;
		*(char *)(dest + offs) = ' ';
		offs++;
		count ++;
		if (count == LINE_LENGTH || (i == len - 1)) {
			if (count < LINE_LENGTH) {
				int k;
				// Put some spaces in to line up the printable ASCII output
				// when the number of bytes is less than LINE_LENGTH.
				k = (LINE_LENGTH - count) * 3;
				for (j = 0; j < k; j++) {
					*(char *)(dest + offs) = ' ';
					offs ++;
				}
			}
			//*(char *)(dest + offs) = '|';
			//offs ++;
			for (j = (i - (count - 1)); j <= i; j++) {
				/* Filter unprintable characters */
				if ((bytes[j] & 0xff) >= 32 && (bytes[j] & 0xff) <= 126) {
					*(char *)(dest + offs) = bytes[j] & 0xff;
				} else {
					*(char *)(dest + offs) = '.';
				}
				offs ++;
			}
			*(char *)(dest + offs) = '\r';
			offs ++;
			*(char *)(dest + offs) = '\n';
			offs ++;
			count = 0;
		}
	}
	*(char *)(dest + offs) = '\0';
}
#undef LINE_LENGTH

int Utility_dumphex(char * destination, const char * source, int len) {
	dump_bytes(destination, "", (char *)source, len);
	return 0;
}

char * Utility_tokeniseString(char * string, char * delimiters, char ** context) {
	size_t string_length;
	size_t index;
	int found = 0;
	
	if (delimiters == NULL || (string == NULL && *context == NULL)) {
		return NULL;
	}
	
	if (string == NULL) {
		string = *context;
	}
	
	string_length = strlen(string);
	for (index = 0; index < string_length; index++) {
		found = matches(string[index], delimiters);
		if (found) {
			break;
		}
	}
	
	if (!found) {
		*context = NULL;
		return string;
	}
	
	/*
	 * we found at least one delimiter and replaced it with a \0
	 * now look for repeated delimiters and skip over them.
	 */
	string[index] = '\0';
	if (matches(string[index + 1], delimiters)) {
		do {
			index ++;
		} while (matches(string[index], delimiters) && string[index] != '\0');
		index --;
	}

	/* save the rest of the string */
	if (*(string + index + 1) != '\0') {
		*context = (string + index + 1);
	} else {
		*context = NULL;
	}

	return string;
}

static int isHexDigit(int c, int * val) {
	if (c >= '0' && c <= '9') {
		*val = c - '0';
		goto end_success;
	}
	if (c >= 'a' && c <= 'f') {
		*val = c - ('a' - 10);
		goto end_success;
	}
	if (c >= 'A' && c <= 'F') {
		*val = c - ('A' - 10);
		goto end_success;
	}
	return 0;
end_success:
	return 1;
}

// Read a string of hex digits.
// WARNING: this will not return until a non-hex digit is encountered.
void Utility_hexToBytes(char* dst, char* src) {
	int cur;
	int c;
	int val;
	do {
		c = *src;
		src++;
		if (isHexDigit(c, &val)) {
			cur = val;
		} else {
			break;
		}
		c = *src;
		src++;
		if (isHexDigit(c, &val)) {
			cur = (cur << 4) | val;
			*dst = cur;
		} else {
			*dst = cur;
			break;
		}
		dst++;
	} while (1);
	return;
}

#ifdef __UNITTEST__

#include <stdio.h>
#include <string.h>

int main() {
	char output[128];
	char compare[128];
	int someint = 0xabcdef01;
	int parsedint;

	memset(output, '-', 128);
	Utility_intToHex(output, (unsigned char*)&someint, 2);
	fprintf(stdout, "%s\n", output);

	strcpy(output, "ABCDEF01");
	parsedint = Utility_hexToInt(output, 4);
	fprintf(stdout, "%x\n", parsedint);

//	parsedint = 64000;
//	do (parsedint--); while (parsedint != 0);
//	fprintf(stdout, "%d\n", parsedint);

	someint = -54;
	sprintf(compare, "%d", someint);
	Utility_intToA(output, someint, 10);
	if (strcmp(compare, output) == 0) {
		fprintf(stdout, "Utility_intToA() Pass\n");
	} else {
		fprintf(stdout, "Utility_intToA() %s != %s\n", compare, output);
	}

	Utility_intToA(output, -54, 10);
	fprintf(stdout, "%s == %d\n", output, -54);

	return 0;
}

#endif

