#if !defined (__GNUC__)
# error Unsupported C++ compiler.
#else
# define COMPILER_GCC 1
#endif

#if !defined (__amd64__) && !defined (__amd64) && !defined (__x86_64__) && !defined (__x86_64)
# error Unsupported Architecture, only Amd64 is currently supported .
#else
# define ARCH_AMD64 1
#endif
