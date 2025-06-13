#ifndef __khrplatform_h_
#define __khrplatform_h_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    #define KHRONOS_APIENTRY __stdcall
#else
    #define KHRONOS_APIENTRY
#endif

#if defined(KHRONOS_STATIC)
    #define KHRONOS_APICALL
#elif defined(_WIN32)
    #define KHRONOS_APICALL __declspec(dllimport)
#else
    #define KHRONOS_APICALL
#endif

typedef signed char khronos_int8_t;
typedef unsigned char khronos_uint8_t;
typedef signed short int khronos_int16_t;
typedef unsigned short int khronos_uint16_t;
typedef signed int khronos_int32_t;
typedef unsigned int khronos_uint32_t;
typedef signed long long int khronos_int64_t;
typedef unsigned long long int khronos_uint64_t;

typedef signed long int khronos_intptr_t;
typedef unsigned long int khronos_uintptr_t;
typedef signed long int khronos_ssize_t;
typedef unsigned long int khronos_usize_t;

#ifdef __cplusplus
}
#endif

#endif /* __khrplatform_h_ */
