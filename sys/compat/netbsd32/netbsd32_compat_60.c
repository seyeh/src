/*	$NetBSD: netbsd32_compat_60.c,v 1.6 2020/01/29 15:47:52 ad Exp $	*/

/*-
 * Copyright (c) 2008, 2020 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Christos Zoulas.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: netbsd32_compat_60.c,v 1.6 2020/01/29 15:47:52 ad Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/module.h>
#include <sys/time.h>
#include <sys/dirent.h>
#include <sys/lwp.h>
#include <sys/syscallargs.h>
#include <sys/syscallvar.h>

#include <compat/netbsd32/netbsd32.h>
#include <compat/netbsd32/netbsd32_syscall.h>
#include <compat/netbsd32/netbsd32_syscallargs.h>
#include <compat/netbsd32/netbsd32_conv.h>

int
compat_60_netbsd32__lwp_park(struct lwp *l,
    const struct compat_60_netbsd32__lwp_park_args *uap, register_t *retval)
{
	/* {
		syscallarg(const netbsd32_timespecp) ts;
		syscallarg(lwpid_t) unpark;
		syscallarg(netbsd32_voidp) hint;
		syscallarg(netbsd32_voidp) unparkhint;
	} */
	struct timespec ts, *tsp;
	struct netbsd32_timespec ts32;
	int error;

	if (SCARG_P32(uap, ts) == NULL)
		tsp = NULL;
	else {
		error = copyin(SCARG_P32(uap, ts), &ts32, sizeof ts32);
		if (error != 0)
			return error;
		netbsd32_to_timespec(&ts32, &ts);
		tsp = &ts;
	}

	if (SCARG(uap, unpark) != 0) {
		error = lwp_unpark(&SCARG(uap, unpark), 1);
		if (error != 0)
			return error;
	}

	return lwp_park(CLOCK_REALTIME, TIMER_ABSTIME, tsp);
}

static struct syscall_package compat_netbsd32_60_syscalls[] = {
	{ NETBSD32_SYS_compat_60_netbsd32__lwp_park, 0,
	    (sy_call_t *)compat_60_netbsd32__lwp_park },
	{ 0, 0, NULL }
}; 


MODULE(MODULE_CLASS_EXEC, compat_netbsd32_60, "compat_netbsd32_80,compat_60");

static int
compat_netbsd32_60_modcmd(modcmd_t cmd, void *arg)
{

	switch (cmd) {
	case MODULE_CMD_INIT:
		return syscall_establish(&emul_netbsd32,
		    compat_netbsd32_60_syscalls);

	case MODULE_CMD_FINI:
		return syscall_disestablish(&emul_netbsd32,
		    compat_netbsd32_60_syscalls);

	default:
		return ENOTTY;
	}
}
