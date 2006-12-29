/************************************************************************ 
 * This file is part of PDCurses. PDCurses is public domain software;	*
 * you may use it for any purpose. This software is provided AS IS with	*
 * NO WARRANTY whatsoever.						*
 *									*
 * If you use PDCurses in an application, an acknowledgement would be	*
 * appreciated, but is not mandatory. If you make corrections or	*
 * enhancements to PDCurses, please forward them to the current		*
 * maintainer for the benefit of other users.				*
 *									*
 * See the file maintain.er for details of the current maintainer.	*
 ************************************************************************/

#include "pdcdos.h"

#ifdef __DJGPP__
# include <unistd.h>
#endif

#include <time.h>

RCSID("$Id: pdcutil.c,v 1.15 2006/12/29 18:19:37 wmcbrine Exp $");

void PDC_beep(void)
{
	PDCREGS regs;

	PDC_LOG(("PDC_beep() - called\n"));

	regs.W.ax = 0x0e07;       /* Write ^G in TTY fashion */
	regs.W.bx = 0;
	PDCINT(0x10, regs);
}

void PDC_napms(int ms)
{
	PDC_LOG(("PDC_napms() - called: ms=%d\n", ms));

#if defined(__DJGPP__)

	usleep(1000 * ms);

#elif defined(DOS)

# if defined(__TURBOC__) || defined(__WATCOMC__)

	delay(ms);

# elif defined(_MSC_VER) || defined(_QC)
	{
		clock_t goal = (ms / 50) + clock();
		while (goal > clock())
		;
	}
# elif defined(__PACIFIC__)
	{
		/* get number of ticks since startup from address 0040:006ch
		   1 sec. = 18.2065  */

		volatile far long *ticks = MK_FP(0x0040, 0x006c);
		long goal = (ms / 50) + *ticks;
		while (goal > *ticks)
		;
	}
# endif
#endif
}

const char *PDC_sysname(void)
{
	return "DOS";
}

#if defined(__WATCOMC__) && defined(__386__)

void PDC_dpmi_int(int vector, pdc_dpmi_regs *rmregs)
{
	union REGPACK regs = {0};

	regs.w.ax  = 0x300;
	regs.h.bl  = vector;
	regs.x.edi = FP_OFF(rmregs);
	regs.x.es  = FP_SEG(rmregs);

	intr(0x31, &regs);
}

#endif
