/* libunwind - a platform-independent unwind library
   Copyright (C) 2002-2004 Hewlett-Packard Co
        Contributed by David Mosberger-Tang <davidm@hpl.hp.com>

   Modified for x86_64 by Max Asbock <masbock@us.ibm.com>

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

#include "unwind_i.h"
#include <signal.h>

#if 0 // TODO(mundaym): port to s390x?
/* Recognise PLT entries such as:
     3bdf0: ff 25 e2 49 13 00 jmpq   *0x1349e2(%rip)
     3bdf6: 68 ae 03 00 00    pushq  $0x3ae
     3bdfb: e9 00 c5 ff ff    jmpq   38300 <_init+0x18> */
static int
is_plt_entry (struct dwarf_cursor *c)
{
  unw_word_t w0, w1;
  unw_accessors_t *a;
  int ret;

  a = unw_get_accessors (c->as);
  if ((ret = (*a->access_mem) (c->as, c->ip, &w0, 0, c->as_arg)) < 0
      || (ret = (*a->access_mem) (c->as, c->ip + 8, &w1, 0, c->as_arg)) < 0)
    return 0;

  ret = (((w0 & 0xffff) == 0x25ff)
         && (((w0 >> 48) & 0xff) == 0x68)
         && (((w1 >> 24) & 0xff) == 0xe9));

  Debug (14, "ip=0x%lx => 0x%016lx 0x%016lx, ret = %d\n", c->ip, w0, w1, ret);
  return ret;
}
#endif

PROTECTED int
unw_handle_signal_frame (unw_cursor_t *cursor)
{
  struct cursor *c = (struct cursor *) cursor;
  int ret, i, handler_type;
  unw_word_t sc_addr, sp, sp_addr = c->dwarf.cfa;
  struct dwarf_loc sp_loc = DWARF_LOC (sp_addr, 0);
  ucontext_t *sc_ptr;

  if ((ret = dwarf_get (&c->dwarf, sp_loc, &sp)) < 0)
    return -UNW_EUNSPEC;

  handler_type = unw_is_signal_frame (cursor);
  Debug(1, "unw_is_signal_frame()=%d\n", handler_type);

  /* Save the SP and PC to be able to return execution at this point
     later in time (unw_resume).  */
  c->sigcontext_sp = c->dwarf.cfa - 160;
  c->sigcontext_pc = c->dwarf.ip;

  switch (handler_type)
    {
    case 1: /* sigreturn */
      sc_addr = sp_addr + 8;
      break;
    case 2: /* rt_sigreturn */
      sc_addr = sp_addr + sizeof(siginfo_t) + 8;
      break;
    default:
      return -UNW_EUNSPEC;
    }

  c->sigcontext_format = S390X_SCF_LINUX_RT_SIGFRAME;
  c->sigcontext_addr = sc_addr;
  c->frame_info.frame_type = UNW_X86_64_FRAME_SIGRETURN;
  c->frame_info.cfa_reg_offset = sc_addr - sp_addr;

  sc_ptr = (ucontext_t*)sc_addr;
  /* TODO(mundaym): sc_ptr->sregs might equal NULL? */

  /* Update the dwarf cursor.
     Set the location of the registers to the corresponding addresses of the
     uc_mcontext / sigcontext structure contents.  */
  for (i = UNW_S390X_R0; i <= UNW_S390X_R15; ++i)
    c->dwarf.loc[i] = DWARF_LOC (&sc_ptr->uc_mcontext.gregs[i - UNW_S390X_R0], 0);
  for (i = UNW_S390X_F0; i <= UNW_S390X_F15; ++i)
    c->dwarf.loc[i] = DWARF_LOC (&sc_ptr->uc_mcontext.fpregs.fprs[i - UNW_S390X_F0], 0);
  /* TODO(mundaym): access regs, vxrs */

  c->dwarf.loc[UNW_S390X_IP] = DWARF_LOC (&sc_ptr->uc_mcontext.psw.addr, 0);

  /* Set SP/CFA and PC/IP.  */
  dwarf_get (&c->dwarf, c->dwarf.loc[UNW_S390X_R15], &c->dwarf.cfa);
  dwarf_get (&c->dwarf, c->dwarf.loc[UNW_S390X_IP], &c->dwarf.ip);

  /* Bias DWARF CFA. */
  c->dwarf.cfa += 160;

  c->dwarf.pi_valid = 0;
  c->dwarf.use_prev_instr = 0;

  return 1;
}

PROTECTED int
unw_step (unw_cursor_t *cursor)
{
  struct cursor *c = (struct cursor *) cursor;
  unw_word_t prev_cfa = c->dwarf.cfa, prev_ip = c->dwarf.ip;
  int ret = 0, i;
  (void)i;

  Debug (1, "(cursor=%p, ip=0x%016lx, cfa=0x%016lx)\n",
         c, c->dwarf.ip, c->dwarf.cfa);

  /* Check if this is a signal frame. */
  if (unw_is_signal_frame (cursor) > 0)
    return unw_handle_signal_frame (cursor);

  /* Try DWARF-based unwinding... */
  c->sigcontext_format = S390X_SCF_NONE;
  ret = dwarf_step (&c->dwarf);

  /* TODO(mundaym): figure out what code is inserting the odd value */
  if (ret > 0 && c->dwarf.ip&1 == 1)
    {
      c->dwarf.ip = 0; /* odd values indicate end of chain */
      return 0;
    }

  /* We could try and use the backchain if the DWARF is missing.
     Since it isn't enabled by default however this is very risky. */
  if (ret == -UNW_ENOINFO)
    {
      c->dwarf.ip = 0;
      return 0;
    }

  if (ret > 0 && c->dwarf.ip == 0)
    return 0;

  return ret;
}
