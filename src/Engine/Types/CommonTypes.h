#pragma once

// Of course, Afterlife Engine is 64-bit only and we act accordingly no bitness checks

#pragma warning(push)
#pragma warning(disable: 4146) // we aint popping this, its an annoying and useless warning either way.

#define _GLOBAL_ static inline
#define _GLOBAL_CONST_ const _GLOBAL_
#define _GLOBAL_CEX_ constexpr _GLOBAL_

#ifdef _MSC_VER
#define COMPILER_AGNOSTIC_int8  __int8
#define COMPILER_AGNOSTIC_int16 __int16
#define COMPILER_AGNOSTIC_int32 __int32
#define COMPILER_AGNOSTIC_int64 __int64
#endif

#ifdef __GNUC__
#define COMPILER_AGNOSTIC_int8  char
#define COMPILER_AGNOSTIC_int16 short
#define COMPILER_AGNOSTIC_int32 int
#define COMPILER_AGNOSTIC_int64 long long
#endif

#ifdef __clang__
#define COMPILER_AGNOSTIC_int8  char
#define COMPILER_AGNOSTIC_int16 short
#define COMPILER_AGNOSTIC_int32 int
#define COMPILER_AGNOSTIC_int64 long long
#endif

typedef unsigned COMPILER_AGNOSTIC_int8  uint8;
typedef unsigned COMPILER_AGNOSTIC_int16 uint16;
typedef unsigned COMPILER_AGNOSTIC_int32 uint32;
typedef unsigned COMPILER_AGNOSTIC_int64 uint64;

typedef signed COMPILER_AGNOSTIC_int8  int8;
typedef signed COMPILER_AGNOSTIC_int16 int16;
typedef signed COMPILER_AGNOSTIC_int32 int32;
typedef signed COMPILER_AGNOSTIC_int64 int64;

typedef float float32;
typedef double float64;

_GLOBAL_CEX_ uint8 min_uint8 = 0;
_GLOBAL_CEX_ uint16 min_uint16 = 0;
_GLOBAL_CEX_ uint32 min_uint32 = 0;
_GLOBAL_CEX_ uint64 min_uint64 = 0;
_GLOBAL_CEX_ int8 min_int8 = -128;
_GLOBAL_CEX_ int16 min_int16 = -32768;
_GLOBAL_CEX_ int32 min_int32 = -2147483648;

#ifdef _MSC_VER
_GLOBAL_CEX_ int64 min_int64 = -9223372036854775807;
#endif
#ifdef __GNUC__
_GLOBAL_CEX_ int64 min_int64 = -__INT64_C(9223372036854775807)-1;
#endif

_GLOBAL_CEX_ uint8 max_uint8 = 255;
_GLOBAL_CEX_ uint16 max_uint16 = 65535;
_GLOBAL_CEX_ uint32 max_uint32 = 4294967295;
#ifdef _MSC_VER
_GLOBAL_CEX_ uint64 max_uint64 = 18446744073709551615;
#endif
#ifdef __GNUC__
_GLOBAL_CEX_ uint64 max_uint64 = __UINT64_C(18446744073709551615);
#endif
_GLOBAL_CEX_ int8 max_int8 = 127;
_GLOBAL_CEX_ int16 max_int16 = 32767;
_GLOBAL_CEX_ int32 max_int32 = 2147483647;
_GLOBAL_CEX_ int64 max_int64 = 9223372036854775807;

_GLOBAL_CEX_ float32 min_float32 = 1.175494351e-38f;
_GLOBAL_CEX_ float32 max_float32 = 3.402823466e+38f;
_GLOBAL_CEX_ float32 inf_float32 = __builtin_huge_valf();
_GLOBAL_CEX_ float32 nan_float32 = (float32)__builtin_nan("0"); // quiet NaN

_GLOBAL_CEX_ float64 min_float64 = 2.2250738585072014e-308;
_GLOBAL_CEX_ float64 max_float64 = 1.7976931348623158e+308;
_GLOBAL_CEX_ float64 inf_float64 = __builtin_huge_val();
_GLOBAL_CEX_ float64 nan_float64 = __builtin_nan("0"); // quiet NaN

typedef uint8 byte;
typedef uint16 word;
typedef uint32 dword;
typedef uint64 qword;
_GLOBAL_CEX_ uint8 min_byte = 0;
_GLOBAL_CEX_ uint8 max_byte = max_uint8;
_GLOBAL_CEX_ uint16 min_word = 0;
_GLOBAL_CEX_ uint16 max_word = max_uint16;
_GLOBAL_CEX_ uint32 min_dword = 0;
_GLOBAL_CEX_ uint32 max_dword = max_uint32;
_GLOBAL_CEX_ uint64 min_qword = 0;
_GLOBAL_CEX_ uint64 max_qword = max_uint64;

typedef uint64 sizeT;
_GLOBAL_CEX_ uint64 min_sizeT = 0;
_GLOBAL_CEX_ uint64 max_sizeT = max_uint64;

_GLOBAL_CEX_ sizeT KB = 1024;
_GLOBAL_CEX_ sizeT MB = 1024 * KB;
_GLOBAL_CEX_ sizeT GB = 1024 * MB;
_GLOBAL_CEX_ sizeT TB = 1024 * GB;
_GLOBAL_CEX_ sizeT PB = 1024 * TB;

#undef COMPILER_AGNOSTIC_int8
#undef COMPILER_AGNOSTIC_int16
#undef COMPILER_AGNOSTIC_int32
#undef COMPILER_AGNOSTIC_int64