/* -----------------------------------------------------------------*-C-*-
   ffitarget.h - Copyright (c) 1996-2003  Red Hat, Inc.
   Target configuration macros for IA-64.

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   ``Software''), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED ``AS IS'', WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL CYGNUS SOLUTIONS BE LIABLE FOR ANY CLAIM, DAMAGES OR
   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
   OTHER DEALINGS IN THE SOFTWARE.

   ----------------------------------------------------------------------- */

#ifndef LIBFFI_TARGET_H
#define LIBFFI_TARGET_H

#ifndef LIBFFI_ASM
typedef unsigned long          ffi_arg;
typedef signed long            ffi_sarg;

typedef enum ffi_abi {
  FFI_FIRST_ABI = 0,
  FFI_UNIX,   	/* Linux and all Unix variants use the same conventions	*/
  FFI_DEFAULT_ABI = FFI_UNIX,
  FFI_LAST_ABI = FFI_DEFAULT_ABI + 1
} ffi_abi;
#endif

/* ---- Definitions for closures ----------------------------------------- */

#define FFI_CLOSURES 1
#define FFI_TRAMPOLINE_SIZE 24  /* Really the following struct, which 	*/
				/* can be interpreted as a C function	*/
				/* descriptor:				*/

#ifndef LIBFFI_ASM
struct ffi_ia64_trampoline_struct {
    void * code_pointer;	/* Pointer to ffi_closure_UNIX	*/
    void * fake_gp;		/* Pointer to closure, installed as gp	*/
    void * real_gp;		/* Real gp value, reinstalled by 	*/
				/* ffi_closure_UNIX.			*/
};
#endif

#endif

