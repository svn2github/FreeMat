#include <ffi.h>
#include <ffi_common.h>

void __declspec(naked) ffi_call_SYSV(void (*func)(char *, extended_cif *), 
				     /*@out@*/ extended_cif *p1, 
				     unsigned s1, unsigned s2, 
				     /*@out@*/ unsigned *sp2, 
				     void (*fn)()) { 
  __asm {
    push ebp;
    mov         ebp,esp;
    mov         ecx,dword ptr [ebp+10h];
    sub         esp,ecx;
    mov         eax,esp;
    push        dword ptr [ebp+0Ch];
    push        eax;
    call        dword ptr [ebp+8];
    add         esp,8;
    call        dword ptr [ebp+1Ch];
    mov         ecx,dword ptr [ebp+10h];
    add         esp,ecx;
    mov         ecx,dword ptr [ebp+14h];
    cmp         dword ptr [ebp+18h],0;
    jne         retint;
    cmp         ecx,2;
    jne         noretval;
    fstp        st(0);
    jmp         epilogue;
  retint:
    cmp         ecx,1;
    jne         retfloat;
    mov         ecx,dword ptr [ebp+18h];
    mov         dword ptr [ecx],eax;
    jmp         epilogue;
  retfloat:
    cmp         ecx,2;
    jne         retdouble;
    mov         ecx,dword ptr [ebp+18h];
    fstp        dword ptr [ecx];
    jmp         epilogue;
  retdouble:
    cmp         ecx,3;
    jne         retlongdouble;
    mov         ecx,dword ptr [ebp+18h];
    fstp        qword ptr [ecx];
    jmp         epilogue;
  retlongdouble:
    cmp         ecx,4;
    jne         retint64;
    mov         ecx,dword ptr [ebp+18h];
    fstp        tbyte ptr [ecx];
    jmp         epilogue;
  retint64:
    cmp         ecx,0Ch;
    jne         retstruct;
    mov         ecx,dword ptr [ebp+18h];
    mov         dword ptr [ecx],eax;
    mov         dword ptr [ecx+4],edx;
  noretval:
  epilogue:
  retstruct:
    mov         esp,ebp;
    pop         ebp;
    ret;
  }
}

void ffi_call_STDCALL(void (*func)(char *, extended_cif *),
		      /*@out@*/ extended_cif *p1,
		      unsigned s1, unsigned s2,
		      /*@out@*/ unsigned *sp2,
		      void (*fn)()) {
}

