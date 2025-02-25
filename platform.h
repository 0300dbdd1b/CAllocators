#ifndef PLATFORM_H
#define PLATFORM_H

//	-----------------------------------------------------------------------------------------------------------
//										Platform Detection
//	-----------------------------------------------------------------------------------------------------------

#if defined(_WIN32) || defined(_WIN64)
	#define PLATFORM_WINDOWS
	#define PLATFORM_NAME "Windows"
#elif defined(__linux__)
	#define PLATFORM_LINUX
	#define PLATFORM_NAME "Linux"
#elif defined(__APPLE__) && defined(__MACH__)
	#define PLATFORM_MACOS
	#define PLATFORM_NAME "macOS"
#elif defined(__FreeBSD__)
	#define PLATFORM_FREEBSD
	#define PLATFORM_NAME "FreeBSD"
#else
	#error "Unsupported platform"
#endif

//	-----------------------------------------------------------------------------------------------------------
//										Compiler Detection
//	-----------------------------------------------------------------------------------------------------------

#if defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER)
	#define COMPILER_GCC
	#define COMPILER_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#define COMPILER_NAME "GCC"
#elif defined(__clang__)
	#define COMPILER_CLANG
	#define COMPILER_VERSION (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
	#define COMPILER_NAME "Clang"
#elif defined(_MSC_VER)
	#define COMPILER_MSVC
	#define COMPILER_VERSION _MSC_VER
	#define COMPILER_NAME "MSVC"
#elif defined(__INTEL_COMPILER)
	#define COMPILER_INTEL
	#define COMPILER_VERSION __INTEL_COMPILER
	#define COMPILER_NAME "Intel"
#else
	#error "Unsupported compiler"
#endif

//	-----------------------------------------------------------------------------------------------------------
//								Include Standard Headers
//	-----------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#if defined(PLATFORM_WINDOWS)
    #include <windows.h>
    #include <direct.h>
    #include <io.h>
    #include <fcntl.h>
    #include <process.h>
    #define PATH_MAX_SIZE MAX_PATH
#else
    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <pthread.h>
    #include <errno.h>
    #include <dirent.h>
    #include <limits.h>
    #define PATH_MAX_SIZE PATH_MAX
#endif


//	-----------------------------------------------------------------------------------------------------------
//										Warning Management
//	-----------------------------------------------------------------------------------------------------------

#if defined(__GNUC__) || defined(__clang__)
	#define DISABLE_WARNING(warning) _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored warning")
	#define ENABLE_WARNING _Pragma("GCC diagnostic pop")
#elif defined(_MSC_VER)
	#define DISABLE_WARNING(warning) __pragma(warning(push)) __pragma(warning(disable : warning))
	#define ENABLE_WARNING __pragma(warning(pop))
#else
	#define DISABLE_WARNING(warning)
	#define ENABLE_WARNING
#endif


//	-----------------------------------------------------------------------------------------------------------
//								Compiler-Specific Macros for Portability
//	-----------------------------------------------------------------------------------------------------------

#if defined(__GNUC__) || defined(__clang__)

	#define MAYBE_UNUSED __attribute__((unused))
	#define UNUSED __attribute__((unused))
	#define UNUSED_FUNCTION __attribute__((unused))
	#define DEPRECATED(msg) __attribute__((deprecated(msg)))
	#define FORCE_INLINE __attribute__((always_inline)) inline
	#define NO_INLINE __attribute__((noinline))
	#define PURE_FUNCTION __attribute__((pure))
	#define CONST_FUNCTION __attribute__((const))
	#define NO_RETURN __attribute__((noreturn))
	#define HOT_FUNCTION __attribute__((hot))
	#define COLD_FUNCTION __attribute__((cold))
	#define LIKELY(x) __builtin_expect(!!(x), 1)
	#define UNLIKELY(x) __builtin_expect(!!(x), 0)
	#define EXPORT_SYMBOL __attribute__((visibility("default")))
	#define HIDDEN_SYMBOL __attribute__((visibility("hidden")))
	#define THREAD_LOCAL __thread
	#define ALIGNAS(x) __attribute__((aligned(x)))
	#define ALIGNOF(x) __alignof__(x)
	#define CLEANUP(func) __attribute__((cleanup(func)))
	#define MEMORY_BARRIER() __asm__ __volatile__("" ::: "memory")
    #define SLEEP(milliseconds) usleep((milliseconds) * 1000)

#elif defined(_MSC_VER)

	#define MAYBE_UNUSED __pragma(warning(suppress : 4100 4101))
	#define UNUSED __pragma(warning(suppress : 4100 4101))
	#define UNUSED_FUNCTION __pragma(warning(suppress : 4100 4101))
	#define DEPRECATED(msg) __declspec(deprecated(msg))
	#define FORCE_INLINE __forceinline
	#define NO_INLINE __declspec(noinline)
	#define PURE_FUNCTION // Not supported in MSVC
	#define CONST_FUNCTION // Not supported in MSVC
	#define NO_RETURN __declspec(noreturn)
	#define HOT_FUNCTION // No equivalent in MSVC
	#define COLD_FUNCTION // No equivalent in MSVC
	#define LIKELY(x) (x)
	#define UNLIKELY(x) (x)
	#define EXPORT_SYMBOL __declspec(dllexport)
	#define HIDDEN_SYMBOL // No equivalent in MSVC
	#define THREAD_LOCAL __declspec(thread)
	#define ALIGNAS(x) __declspec(align(x))
	#define ALIGNOF(x) __alignof(x)
	#define CLEANUP(func) // Cleanup unsupported in MSVC
	#include <intrin.h>
	#define MEMORY_BARRIER() _ReadWriteBarrier()
    #define SLEEP(milliseconds) Sleep(milliseconds)
#else

// Defaults for unsupported compilers
	#define MAYBE_UNUSED
	#define UNUSED
	#define UNUSED_FUNCTION
	#define DEPRECATED(msg)
	#define FORCE_INLINE inline
	#define NO_INLINE
	#define PURE_FUNCTION
	#define CONST_FUNCTION
	#define NO_RETURN
	#define HOT_FUNCTION
	#define COLD_FUNCTION
	#define LIKELY(x) (x)
	#define UNLIKELY(x) (x)
	#define EXPORT_SYMBOL
	#define HIDDEN_SYMBOL
	#define THREAD_LOCAL
	#define ALIGNAS(x)
	#define ALIGNOF(x)
	#define CLEANUP(func)
	#define MEMORY_BARRIER()
	#define SLEEP(milliseconds)
	#endif

//	-----------------------------------------------------------------------------------------------------------
//								Utility Macros for Assertions and Debugging
//	-----------------------------------------------------------------------------------------------------------

#include <assert.h>

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
    #define STATIC_ASSERT(expr, msg) _Static_assert(expr, msg)
#elif defined(__cplusplus) && (__cplusplus >= 201103L)
    #define STATIC_ASSERT(expr, msg) static_assert(expr, msg)
#else
    #define PLATFORM_CONCAT_IMPL(a, b) a##b
    #define PLATFORM_CONCAT(a, b) PLATFORM_CONCAT_IMPL(a, b)
    #define PLATFORM_UNIQUE_NAME(base) PLATFORM_CONCAT(base, __LINE__)
    #define STATIC_ASSERT(expr, msg) \
        typedef char PLATFORM_UNIQUE_NAME(static_assertion_)[(expr) ? 1 : -1]
#endif

//	-----------------------------------------------------------------------------------------------------------
//								Type Definitions
//	-----------------------------------------------------------------------------------------------------------

#ifndef PLATFORM_TYPES
#define PLATFORM_TYPES

#include <stdint.h>
#include <stddef.h>

typedef uint8_t  U8;  // Unsigned 8-bit integer
typedef uint16_t U16; // Unsigned 16-bit integer
typedef uint32_t U32; // Unsigned 32-bit integer
typedef uint64_t U64; // Unsigned 64-bit integer

typedef int8_t   I8;  // Signed 8-bit integer
typedef int16_t  I16; // Signed 16-bit integer
typedef int32_t  I32; // Signed 32-bit integer
typedef int64_t  I64; // Signed 64-bit integer

typedef float    F32; // 32-bit floating point
typedef double   F64; // 64-bit floating point

typedef uintptr_t UPTR; // Unsigned pointer-sized integer
typedef intptr_t  IPTR; // Signed pointer-sized integer

STATIC_ASSERT(sizeof(U8) == 1, "U8_must_be_1_byte");
STATIC_ASSERT(sizeof(U16) == 2, "U16_must_be_2_bytes");
STATIC_ASSERT(sizeof(U32) == 4, "U32_must_be_4_bytes");
STATIC_ASSERT(sizeof(U64) == 8, "U64_must_be_8_bytes");

STATIC_ASSERT(sizeof(I8) == 1, "I8_must_be_1_byte");
STATIC_ASSERT(sizeof(I16) == 2, "I16_must_be_2_bytes");
STATIC_ASSERT(sizeof(I32) == 4, "I32_must_be_4_bytes");
STATIC_ASSERT(sizeof(I64) == 8, "I64_must_be_8_bytes");

STATIC_ASSERT(sizeof(F32) == 4, "F32_must_be_4_bytes");
STATIC_ASSERT(sizeof(F64) == 8, "F64_must_be_8_bytes");

STATIC_ASSERT(sizeof(UPTR) == sizeof(void *), "UPTR_must_match_pointer_size");
STATIC_ASSERT(sizeof(IPTR) == sizeof(void *), "IPTR_must_match_pointer_size");

#endif

//	-----------------------------------------------------------------------------------------------------------
//								Portable File I/O
//	-----------------------------------------------------------------------------------------------------------

#if defined(PLATFORM_WINDOWS)
    typedef HANDLE FILE_HANDLE;
    typedef DWORD FILE_SIZE;

    #define FILE_ACCESS_READ GENERIC_READ
    #define FILE_ACCESS_WRITE GENERIC_WRITE
    #define FILE_ACCESS_READWRITE (GENERIC_READ | GENERIC_WRITE)

    #define FILE_MODE_CREATE_NEW CREATE_NEW
    #define FILE_MODE_OPEN_EXISTING OPEN_EXISTING
    #define FILE_MODE_CREATE_ALWAYS CREATE_ALWAYS
    #define FILE_MODE_OPEN_ALWAYS OPEN_ALWAYS

    #define PORTABLE_OPEN(path, access, mode) CreateFileA(path, access, 0, NULL, mode, FILE_ATTRIBUTE_NORMAL, NULL)
    #define PORTABLE_CLOSE(handle) CloseHandle(handle)
    #define PORTABLE_READ(handle, buffer, size, read_bytes) ReadFile(handle, buffer, size, read_bytes, NULL)
    #define PORTABLE_WRITE(handle, buffer, size, written_bytes) WriteFile(handle, buffer, size, written_bytes, NULL)

#else
    typedef int FILE_HANDLE;
    typedef off_t FILE_SIZE;

    #define FILE_ACCESS_READ O_RDONLY
    #define FILE_ACCESS_WRITE O_WRONLY
    #define FILE_ACCESS_READWRITE O_RDWR

    #define FILE_MODE_CREATE_NEW (O_CREAT | O_EXCL)
    #define FILE_MODE_OPEN_EXISTING 0
    #define FILE_MODE_CREATE_ALWAYS (O_CREAT | O_TRUNC)
    #define FILE_MODE_OPEN_ALWAYS O_CREAT

    #define PORTABLE_OPEN(path, access, mode) open(path, access, mode)
    #define PORTABLE_CLOSE(handle) close(handle)
    #define PORTABLE_READ(handle, buffer, size, read_bytes) (*read_bytes = read(handle, buffer, size))
    #define PORTABLE_WRITE(handle, buffer, size, written_bytes) (*written_bytes = write(handle, buffer, size))
#endif

//	-----------------------------------------------------------------------------------------------------------
//								Portable Networking
//	-----------------------------------------------------------------------------------------------------------

#if defined(PLATFORM_WINDOWS)
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef SOCKET SOCKET_TYPE;
    #define CLOSE_SOCKET closesocket
    #define SOCKET_INIT() \
        do { \
            WSADATA wsaData; \
            WSAStartup(MAKEWORD(2, 2), &wsaData); \
        } while (0)
    #define SOCKET_CLEANUP() WSACleanup()
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    typedef int SOCKET_TYPE;
    #define CLOSE_SOCKET close
    #define SOCKET_INIT() ((void)0)
    #define SOCKET_CLEANUP() ((void)0)
#endif

//	-----------------------------------------------------------------------------------------------------------
//								Portable Threading
//	-----------------------------------------------------------------------------------------------------------

#if defined(PLATFORM_WINDOWS)
    typedef HANDLE THREAD_HANDLE;
    #define THREAD_RETURN DWORD WINAPI
    #define THREAD_CREATE(handle, func, arg) \
        *(handle) = CreateThread(NULL, 0, func, arg, 0, NULL)
    #define THREAD_JOIN(handle) WaitForSingleObject(handle, INFINITE)
    #define THREAD_EXIT() ExitThread(0)
#else
    typedef pthread_t THREAD_HANDLE;
    #define THREAD_RETURN void *
    #define THREAD_CREATE(handle, func, arg) pthread_create(handle, NULL, func, arg)
    #define THREAD_JOIN(handle) pthread_join(handle, NULL)
    #define THREAD_EXIT() pthread_exit(NULL)
#endif

//	-----------------------------------------------------------------------------------------------------------
//								Portable Directories
//	-----------------------------------------------------------------------------------------------------------

#if defined(PLATFORM_WINDOWS)
    #define MKDIR(path) _mkdir(path)
    #define GET_CWD(buffer, size) _getcwd(buffer, size)
    #define DIR_SEPARATOR '\\'
#else
    #define MKDIR(path) mkdir(path, 0755)
    #define GET_CWD(buffer, size) getcwd(buffer, size)
    #define DIR_SEPARATOR '/'
#endif

//	-----------------------------------------------------------------------------------------------------------
//								Helper Macros
//	-----------------------------------------------------------------------------------------------------------

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define ZERO_MEMORY(ptr, size) memset((ptr), 0, (size))
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, min_val, max_val) (MIN(MAX((x), (min_val)), (max_val)))
#define IN_RANGE(x, min_val, max_val) ((x) >= (min_val) && (x) <= (max_val))

#endif // PLATFORM_H
