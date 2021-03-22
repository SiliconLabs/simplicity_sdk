/***************************************************************************//**
 * @file
 * @brief Code Classification API (Internal)
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef _SLI_CODE_CLASSIFICATION_H_
#define _SLI_CODE_CLASSIFICATION_H_

// Standard Code Classes
#define SL_CODE_CLASS_TIME_CRITICAL timecritical

/******************************************************************************/
/* Helper Macros                                                              */
/******************************************************************************/

// Stringize tokens
#define _SL_CC_STRINGIZE(X) #X
#define _SL_CC_XSTRINGIZE(X) _SL_CC_STRINGIZE(X)
#define _SL_CC_CONCAT3(A, B, C) A B C
#define _SL_CC_CONCAT4(A, B, C, D) A B C D

/******************************************************************************/
/* Compiler Specific Macros                                                   */
/******************************************************************************/

// The directive that is built is dependent on the compiler. Section names are
// appended with an identifier generated from __COUNTER__ and __LINE__ so that
// functions are more likely to be separated into unique sections. Doing this
// allows the linker to discard unused functions with more granularity.
#if defined(__GNUC__) && !defined(__llvm__)

// With GCC, __attribute__ can be used to specify the input section of
// functions.
#define _SL_CC_SECTION(section_name, count, line) \
  __attribute__((section(_SL_CC_CONCAT3(_SL_CC_XSTRINGIZE(section_name), _SL_CC_XSTRINGIZE(count), _SL_CC_XSTRINGIZE(line)))))

#elif defined(__ICCARM__)

// With IAR, _Pragma can be used to specify the input section of
// functions.
#define _SL_CC_SECTION(section_name, count, line) \
  _Pragma(_SL_CC_XSTRINGIZE(_SL_CC_CONCAT4(location =, _SL_CC_XSTRINGIZE(section_name), _SL_CC_XSTRINGIZE(count), _SL_CC_XSTRINGIZE(line))))

#elif defined(__llvm__)

#define _SL_CC_SECTION(section_name)

#else
  #error "(sli_code_classification.h): Code classification does not support \
  the chosen compiler."
#endif // __GNUC__

/******************************************************************************/
/* Compiler Generic Macros                                                    */
/******************************************************************************/

// Build the linker section name based on the name of the component and the
// code classes.
#define _SL_CODE_CLASS_SECTION_CONCAT1(component, p1) \
  text_ ## component ## _ ## p1
#define _SL_CODE_CLASS_SECTION_CONCAT2(component, p1, p2) \
  text_ ## component ## _ ## p1 ## _ ## p2

// Build the compiler specific directives
#define _SL_CODE_CLASS1(component, c1) \
  _SL_CC_SECTION(_SL_CODE_CLASS_SECTION_CONCAT1(component, c1), __COUNTER__, __LINE__)
#define _SL_CODE_CLASS2(component, c1, c2) \
  _SL_CC_SECTION(_SL_CODE_CLASS_SECTION_CONCAT2(component, c1, c2), __COUNTER__, __LINE__)

// Utilities to dispatch a macro with the correct number of parameters.
// Update COUNT_N and COUNT macros if the upper limit of code class
// combinations increases.
#define _SL_CC_COUNT_N(_1, _2, N, ...) N
#define _SL_CC_COUNT(...) _SL_CC_COUNT_N(__VA_ARGS__, 2, 1)
#define _SL_CC_IDENTITY(N) N
#define _SL_CC_APPLY(macro, ...) _SL_CC_IDENTITY(macro(__VA_ARGS__))

// Dispatch _SL_CODE_CLASSX with the correct number of parameters.
#define _SL_CC_DISPATCH(N) _SL_CODE_CLASS ## N

/******************************************************************************/
/* Macro API (Internal)                                                       */
/******************************************************************************/

// Variadic macro to specify the code class membership of a function.
#if defined(SL_CODE_CLASSIFY)
#undef SL_CODE_CLASSIFY
#define SL_CODE_CLASSIFY(component, ...) \
  _SL_CC_IDENTITY(_SL_CC_APPLY(_SL_CC_DISPATCH, _SL_CC_COUNT(__VA_ARGS__)))(component, __VA_ARGS__)
#else
#define SL_CODE_CLASSIFY(component, ...)
#endif

#endif // _SLI_CODE_CLASSIFICATION_H_
