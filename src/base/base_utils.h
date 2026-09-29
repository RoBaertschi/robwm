#define cast(T) (T)

#if !defined (ENABLE_ASSERT)
# define ENABLE_ASSERT 1
#endif

#define Stmt(...) do { __VA_ARGS__ } while(0)

#define AssertBreak() (__builtin_trap())

#if ENABLE_ASSERT
# include <stdio.h>
# define Assert(c) Stmt(if (!(c)) { printf("assertion failed: " #c "\n"); AssertBreak(); } )
#endif

#define Stringify_(S) #S
#define Stringify(S) Stringify_(S)
#define Glue_(A,B) A##B
#define Glue(A,B) Glue_(A,B)

#define ArrayCount(a) (sizeof(a)/sizeof(*(a)))

#define UintFromPtr(p) cast(Uint)(cast(char*)p - cast(char*)0)
#define PtrFromUint(n) cast(void*)(cast(char*)0 + (n))

#define Member(T,m) ((cast(T*)0)->m)
#define OffsetOfMember(T,m) UintFromPtr(&Member(T,m))

#define IsPowerOfTwo(x) ((x) != 0 && ((x) & ((x) - 1)) == 0)

#define Kilobyte (1_u64 << 10)
#define Megabyte (1_u64 << 20)
#define Gigabyte (1_u64 << 30)
#define Terabyte (1_u64 << 40)

#define global static
#define function static
#define function_global
#define function_static static

#define c_linkage_begin extern "C" {
#define c_linkage_end }
#define c_linkage extern "C"

template <typename T>
function T min(T a, T b) {
    return a < b ? a : b;
}

template <typename T>
function T max(T a, T b) {
    return a > b ? a : b;
}

template <typename T>
function T clamp(T min, T value, T max) {
    return max(min, min(value, max));
}

template <typename T>
function T clamp_top(T value, T max) {
    return min(value, max);
}

template <typename T>
function T clamp_bot(T min, T value) {
    return max(min, value);
}

template <typename T>
function T align_up(T pointer, T alignment) {
    Assert(IsPowerOfTwo(alignment));

    T align_up_mask = alignment - 1;
    return (pointer + align_up_mask) & (~align_up_mask);
}

function U8 operator  ""_u8 (unsigned long long value) { return cast(U8)value; }
function U16 operator ""_u16(unsigned long long value) { return cast(U16)value; }
function U32 operator ""_u32(unsigned long long value) { return cast(U32)value; }
function U64 operator ""_u64(unsigned long long value) { return cast(U64)value; }

function I8  operator ""_i8 (unsigned long long value) { return cast(I8)value; }
function I16 operator ""_i16(unsigned long long value) { return cast(I16)value; }
function I32 operator ""_i32(unsigned long long value) { return cast(I32)value; }
function I64 operator ""_i64(unsigned long long value) { return cast(I64)value; }

function Int  operator ""_int (unsigned long long value) { return cast(Int)value; }
function Uint operator ""_uint(unsigned long long value) { return cast(Uint)value; }

#define FMT_U8  "%" PRIu8
#define FMT_U16 "%" PRIu16
#define FMT_U32 "%" PRIu32
#define FMT_U64 "%" PRIu64

#define FMT_I8  "%" PRIi8
#define FMT_I16 "%" PRIi16
#define FMT_I32 "%" PRIi32
#define FMT_I64 "%" PRIi64

#define FMT_INT  FMT_I64
#define FMT_UINT FMT_U64

#include <string.h>
#define MemoryZero(p, z)      memset((p), 0, (z))
#define MemoryZeroStruct(p)   MemoryZero((p), sizeof(*(p)))
#define MemoryZeroArray(p)    MemoryZero((p), sizeof(p))
#define MemoryZeroTyped(p, c) MemoryZero((p), sizeof(*(p))*(c))

#define MemoryMatch(a, b, z) (memcmp((a),(b),(z)) == 0)

#define MemoryCopy(d, s, z)      memmove((d), (s), (z))
#define MemoryCopyStruct(d, s)   MemoryCopy((d), (s), Min(sizeof(*(d)), sizeof(*(s))))
#define MemoryCopyArray(d, s)    MemoryCopy((d), (s), Min(sizeof(d), sizeof(s)))
#define MemoryCopyTyped(d, s, c) MemoryCopy((d), (s), Min(sizeof(*(d)), sizeof(*(s)))*(c))

// Linked lists

#define DLLPushBack_NP(f,l,n,next,prev) ((f)==0?\
((f)=(l)=(n),(n)->next=(n)->prev=0):\
((n)->prev=(l),(l)->next=(n),(l)=(n),(n)->next=0))
#define DLLPushBack(f,l,n) DLLPushBack_NP(f,l,n,next,prev)

#define DLLPushFront(f,l,n) DLLPushBack_NP(l,f,n,prev,next)

#define DLLRemove_NP(f,l,n,next,prev) ((f)==(n)?\
((f)==(l)?\
((f)=(l)=(0)):\
((f)=(f)->next,(f)->prev=0)):\
(l)==(n)?\
((l)=(l)->prev,(l)->next=0):\
((n)->next->prev=(n)->prev,\
(n)->prev->next=(n)->next))
#define DLLRemove(f,l,n) DLLRemove_NP(f,l,n,next,prev)

#define SLLQueuePush_N(f,l,n,next) (((f)==0?\
(f)=(l)=(n):\
((l)->next=(n),(l)=(n))),\
(n)->next=0)
#define SLLQueuePush(f,l,n) SLLQueuePush_N(f,l,n,next)

#define SLLQueuePushFront_N(f,l,n,next) ((f)==0?\
((f)=(l)=(n),(n)->next=0):\
((n)->next=(f),(f)=(n)))
#define SLLQueuePushFront(f,l,n) SLLQueuePushFront_N(f,l,n,next)

#define SLLQueuePop_N(f,l,next) ((f)==(l)?\
(f)=(l)=0:\
((f)=(f)->next))
#define SLLQueuePop(f,l) SLLQueuePop_N(f,l,next)

#define SLLStackPush_N(f,n,next) ((n)->next=(f),(f)=(n))
#define SLLStackPush(f,n) SLLStackPush_N(f,n,next)

#define SLLStackPop_N(f,next) ((f)==0?0:\
((f)=(f)->next))
#define SLLStackPop(f) SLLStackPop_N(f,next)

// Defer

// https://www.gingerbill.org/article/2015/08/19/defer-in-cpp/
template <typename F>
struct privDefer {
	F f;
	privDefer(F f) : f(f) {}
	~privDefer() { f(); }
};

template <typename F>
privDefer<F> defer_func(F f) {
	return privDefer<F>(f);
}

#define DEFER(x)    Glue(x, __COUNTER__)
#define defer(code) auto DEFER(_defer_) = defer_func([&](){code;})
