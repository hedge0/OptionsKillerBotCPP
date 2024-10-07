#pragma once

#include <cstdint>
#include <atomic>

#if defined(__clang__) || (defined(__GNUC__) && defined(__llvm__))
#define OPTS_BOT_CLANG 1
#elif defined(_MSC_VER)
#define OPTS_BOT_MSVC 1
#elif defined(__GNUC__) && !defined(__clang__)
#define OPTS_BOT_GNUCXX 1
#endif

#if defined(OPTS_BOT_MSVC)
#define OPTS_BOT_VISUAL_STUDIO 1
#if defined(OPTS_BOT_CLANG)
#define OPTS_BOT_CLANG_VISUAL_STUDIO 1
#else
#define OPTS_BOT_REGULAR_VISUAL_STUDIO 1
#endif
#endif

#define OPTS_BOT_GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)

#if defined(macintosh) || defined(Macintosh) || (defined(__APPLE__) && defined(__MACH__))
#define OPTS_BOT_MAC 1
#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
#define OPTS_BOT_LINUX 1
#elif defined(WIN32) || defined(_WIN32) || defined(_WIN64)
#define OPTS_BOT_WIN 1
#endif

#if defined(__has_builtin)
#define OPTS_BOT_HAS_BUILTIN(x) __has_builtin(x)
#else
#define OPTS_BOT_HAS_BUILTIN(x) 0
#endif

#if !defined(OPTS_BOT_LIKELY)
#define OPTS_BOT_LIKELY(...) __VA_ARGS__ [[likely]]
#endif

#if !defined(OPTS_BOT_UNLIKELY)
#define OPTS_BOT_UNLIKELY(...) __VA_ARGS__ [[unlikely]]
#endif

#if defined(__cpp_inline_variables) && __cpp_inline_variables >= 201606L
#define OPTS_BOT_HAS_INLINE_VARIABLE 1
#elif __cplusplus >= 201703L
#define OPTS_BOT_HAS_INLINE_VARIABLE 1
#elif defined(OPTS_BOT_MSVC) && OPTS_BOT_MSVC >= 1912 && _MSVC_LANG >= 201703L
#define OPTS_BOT_HAS_INLINE_VARIABLE 1
#else
#define OPTS_BOT_HAS_INLINE_VARIABLE 0
#endif

#if OPTS_BOT_HAS_INLINE_VARIABLE
#define OPTS_BOT_ALWAYS_INLINE_VARIABLE inline constexpr
#else
#define OPTS_BOT_ALWAYS_INLINE_VARIABLE static constexpr
#endif

#if defined(NDEBUG)
#if defined(OPTS_BOT_MSVC)
#pragma warning(disable : C4820)
#define OPTS_BOT_NO_INLINE __declspec(noinline)
#define OPTS_BOT_FLATTEN inline [[msvc::flatten]]
#define OPTS_BOT_ALWAYS_INLINE [[msvc::forceinline]] inline
#define OPTS_BOT_MAYBE_ALWAYS_INLINE [[msvc::forceinline]] inline
#define OPTS_BOT_INLINE inline
#elif defined(OPTS_BOT_CLANG)
#define OPTS_BOT_NO_INLINE __attribute__((__noinline__))
#define OPTS_BOT_FLATTEN inline __attribute__((flatten))
#define OPTS_BOT_ALWAYS_INLINE inline __attribute__((always_inline))
#define OPTS_BOT_MAYBE_ALWAYS_INLINE inline __attribute__((always_inline))
#define OPTS_BOT_INLINE inline
#elif defined(OPTS_BOT_GNUCXX)
#define OPTS_BOT_NO_INLINE __attribute__((noinline))
#define OPTS_BOT_FLATTEN inline __attribute__((flatten))
#define OPTS_BOT_MAYBE_ALWAYS_INLINE inline
#define OPTS_BOT_ALWAYS_INLINE inline __attribute__((always_inline))
#define OPTS_BOT_INLINE inline
#endif
#else
#define OPTS_BOT_NO_INLINE
#define OPTS_BOT_FLATTEN
#define OPTS_BOT_ALWAYS_INLINE
#define OPTS_BOT_MAYBE_ALWAYS_INLINE
#define OPTS_BOT_INLINE
#endif

#if !defined OPTS_BOT_ALIGN
#define OPTS_BOT_ALIGN alignas(bytesPerStep)
#endif
