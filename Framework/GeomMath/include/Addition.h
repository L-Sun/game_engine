//
// C:/Users/54405/workspace/game_engine/Framework/GeomMath/include/Addition.h
// (Header automatically generated by the ispc compiler.)
// DO NOT EDIT THIS FILE.
//

#ifndef ISPC_C__USERS_54405_WORKSPACE_GAME_ENGINE_FRAMEWORK_GEOMMATH_INCLUDE_ADDITION_H
#define ISPC_C__USERS_54405_WORKSPACE_GAME_ENGINE_FRAMEWORK_GEOMMATH_INCLUDE_ADDITION_H

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
    extern void AddByElement(const float * a, const float * b, float * result, uint32_t count);
    extern void AddByNum(const float * a, const float b, float * result, uint32_t count);
    extern void IncreaceByElement(float * traget, const float * src, uint32_t count);
    extern void IncreaceByNum(float * traget, const float num, uint32_t count);
#if defined(__cplusplus) && (! defined(__ISPC_NO_EXTERN_C) || !__ISPC_NO_EXTERN_C )
} /* end extern C */
#endif // __cplusplus


#ifdef __cplusplus
} /* namespace */
#endif // __cplusplus

#endif // ISPC_C__USERS_54405_WORKSPACE_GAME_ENGINE_FRAMEWORK_GEOMMATH_INCLUDE_ADDITION_H
