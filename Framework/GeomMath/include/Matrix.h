//
// /home/lawliet/workspace/cpp/game_engine/Framework/GeomMath/include/Matrix.h
// (Header automatically generated by the ispc compiler.)
// DO NOT EDIT THIS FILE.
//

#ifndef ISPC__HOME_LAWLIET_WORKSPACE_CPP_GAME_ENGINE_FRAMEWORK_GEOMMATH_INCLUDE_MATRIX_H
#define ISPC__HOME_LAWLIET_WORKSPACE_CPP_GAME_ENGINE_FRAMEWORK_GEOMMATH_INCLUDE_MATRIX_H

#include <stdint.h>



#ifdef __cplusplus
namespace ispc { /* namespace */
#endif // __cplusplus

#ifndef __ISPC_ALIGN__
#if defined(__clang__) || !defined(_MSC_VER)
// Clang, GCC, ICC
#define __ISPC_ALIGN__(s) __attribute__((aligned(s)))
#define __ISPC_ALIGNED_STRUCT__(s) struct __ISPC_ALIGN__(s)
#else
// Visual Studio
#define __ISPC_ALIGN__(s) __declspec(align(s))
#define __ISPC_ALIGNED_STRUCT__(s) __ISPC_ALIGN__(s) struct
#endif
#endif


///////////////////////////////////////////////////////////////////////////
// Functions exported from ispc code
///////////////////////////////////////////////////////////////////////////
#if defined(__cplusplus) && (! defined(__ISPC_NO_EXTERN_C) || !__ISPC_NO_EXTERN_C )
extern "C" {
#endif // __cplusplus
    extern void Identity(float * mat, float v, uint32_t order);
    extern void Inverse(const float * mat, float * ret, uint32_t order);
#if defined(__cplusplus) && (! defined(__ISPC_NO_EXTERN_C) || !__ISPC_NO_EXTERN_C )
} /* end extern C */
#endif // __cplusplus


#ifdef __cplusplus
} /* namespace */
#endif // __cplusplus

#endif // ISPC__HOME_LAWLIET_WORKSPACE_CPP_GAME_ENGINE_FRAMEWORK_GEOMMATH_INCLUDE_MATRIX_H
