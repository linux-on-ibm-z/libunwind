/* Copyright (C) 2004 Hewlett-Packard Co.
     Contributed by David Mosberger-Tang <davidm@hpl.hp.com>.

This file is part of libunwind.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.  */

#ifndef ucontext_i_h
#define ucontext_i_h

#include <ucontext.h>

/* These are dummy structures used only for obtaining the offsets of the
   various structure members. */
static ucontext_t dmy_ctxt __attribute__((unused));

#define UC_MCONTEXT_GREGS_R0 ((void *)&dmy_ctxt.uc_mcontext.gpregs[0] - (void *)&dmy_ctxt)
#define UC_MCONTEXT_GREGS_R1 ((void *)&dmy_ctxt.uc_mcontext.gpregs[1] - (void *)&dmy_ctxt)
#define UC_MCONTEXT_GREGS_R2 ((void *)&dmy_ctxt.uc_mcontext.gpregs[2] - (void *)&dmy_ctxt)
#define UC_MCONTEXT_GREGS_R3 ((void *)&dmy_ctxt.uc_mcontext.gpregs[3] - (void *)&dmy_ctxt)
#define UC_MCONTEXT_GREGS_R4 ((void *)&dmy_ctxt.uc_mcontext.gpregs[4] - (void *)&dmy_ctxt)
#define UC_MCONTEXT_GREGS_R5 ((void *)&dmy_ctxt.uc_mcontext.gpregs[5] - (void *)&dmy_ctxt)
#define UC_MCONTEXT_GREGS_R6 ((void *)&dmy_ctxt.uc_mcontext.gpregs[6] - (void *)&dmy_ctxt)
#define UC_MCONTEXT_GREGS_R7 ((void *)&dmy_ctxt.uc_mcontext.gpregs[7] - (void *)&dmy_ctxt)
#define UC_MCONTEXT_GREGS_R8 ((void *)&dmy_ctxt.uc_mcontext.gpregs[8] - (void *)&dmy_ctxt)
#define UC_MCONTEXT_GREGS_R9 ((void *)&dmy_ctxt.uc_mcontext.gpregs[9] - (void *)&dmy_ctxt)
#define UC_MCONTEXT_GREGS_R10 ((void *)&dmy_ctxt.uc_mcontext.gpregs[10] - (void *)&dmy_ctxt)
#define UC_MCONTEXT_GREGS_R11 ((void *)&dmy_ctxt.uc_mcontext.gpregs[11] - (void *)&dmy_ctxt)
#define UC_MCONTEXT_GREGS_R12 ((void *)&dmy_ctxt.uc_mcontext.gpregs[12] - (void *)&dmy_ctxt)
#define UC_MCONTEXT_GREGS_R13 ((void *)&dmy_ctxt.uc_mcontext.gpregs[13] - (void *)&dmy_ctxt)
#define UC_MCONTEXT_GREGS_R14 ((void *)&dmy_ctxt.uc_mcontext.gpregs[14] - (void *)&dmy_ctxt)
#define UC_MCONTEXT_GREGS_R15 ((void *)&dmy_ctxt.uc_mcontext.gpregs[15] - (void *)&dmy_ctxt)

#endif
