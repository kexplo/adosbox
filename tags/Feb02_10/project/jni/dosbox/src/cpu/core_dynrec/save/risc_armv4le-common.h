/*
 *  Copyright (C) 2002-2008  The DOSBox Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* $Id: risc_armv4le-common.h,v 1.2 2008/09/19 16:48:02 c2woody Exp $ */


/* ARMv4 (little endian) backend by M-HT (common data/functions) */


// some configuring defines that specify the capabilities of this architecture
// or aspects of the recompiling

// protect FC_ADDR over function calls if necessaray
// #define DRC_PROTECT_ADDR_REG

// try to use non-flags generating functions if possible
#define DRC_FLAGS_INVALIDATION
// try to replace _simple functions by code
#define DRC_FLAGS_INVALIDATION_DCODE

// type with the same size as a pointer
#define DRC_PTR_SIZE_IM Bit32u

// calling convention modifier
#define DRC_CALL_CONV	/* nothing */
#define DRC_FC			/* nothing */

// use FC_REGS_ADDR to hold the address of "cpu_regs" and to access it using FC_REGS_ADDR
#define DRC_USE_REGS_ADDR
// use FC_SEGS_ADDR to hold the address of "Segs" and to access it using FC_SEGS_ADDR
#define DRC_USE_SEGS_ADDR

// register mapping
typedef Bit8u HostReg;

// "lo" registers
#define HOST_r0		 0
#define HOST_r1		 1
#define HOST_r2		 2
#define HOST_r3		 3
#define HOST_r4		 4
#define HOST_r5		 5
#define HOST_r6		 6
#define HOST_r7		 7
// "hi" registers
#define HOST_r8		 8
#define HOST_r9		 9
#define HOST_r10	10
#define HOST_r11	11
#define HOST_r12	12
#define HOST_r13	13
#define HOST_r14	14
#define HOST_r15	15

// register aliases
// "lo" registers
#define HOST_a1 HOST_r0
#define HOST_a2 HOST_r1
#define HOST_a3 HOST_r2
#define HOST_a4 HOST_r3
#define HOST_v1 HOST_r4
#define HOST_v2 HOST_r5
#define HOST_v3 HOST_r6
#define HOST_v4 HOST_r7
// "hi" registers
#define HOST_v5 HOST_r8
#define HOST_v6 HOST_r9
#define HOST_v7 HOST_r10
#define HOST_v8 HOST_r11
#define HOST_ip HOST_r12
#define HOST_sp HOST_r13
#define HOST_lr HOST_r14
#define HOST_pc HOST_r15



#ifdef UNDER_CE
/* Flags for CacheSync/CacheRangeFlush */
#define CACHE_SYNC_DISCARD      0x001   /* write back & discard all cached data */
#define CACHE_SYNC_INSTRUCTIONS 0x002   /* discard all cached instructions */
#define CACHE_SYNC_WRITEBACK    0x004   /* write back but don't discard data cache*/
#define CACHE_SYNC_FLUSH_I_TLB  0x008   /* flush I-TLB */
#define CACHE_SYNC_FLUSH_D_TLB  0x010   /* flush D-TLB */
#define CACHE_SYNC_FLUSH_TLB    (CACHE_SYNC_FLUSH_I_TLB|CACHE_SYNC_FLUSH_D_TLB)    /* flush all TLB */
#define CACHE_SYNC_L2_WRITEBACK 0x020   /* write-back L2 Cache */
#define CACHE_SYNC_L2_DISCARD   0x040   /* discard L2 Cache */
#define CACHE_SYNC_ALL          0x07F   /* sync and discard everything in Cache/TLB */

#ifdef __cplusplus
extern "C"
#endif

void CacheRangeFlush (LPVOID pAddr, DWORD dwLength, DWORD dwFlags);

/* mamaich */

static void cache_block_closing(Bit8u* block_start, Bitu block_size)
{
    CacheRangeFlush((void*)block_start,block_size,CACHE_SYNC_L2_WRITEBACK|CACHE_SYNC_WRITEBACK|CACHE_SYNC_INSTRUCTIONS|CACHE_SYNC_FLUSH_I_TLB);
LOG_MSG("Cache flush: %x",block_start,block_size);
//	CacheRangeFlush((void*)block_start,block_size,CACHE_SYNC_ALL);
}

#else

static void cache_block_closing(Bit8u* block_start,Bitu block_size) {
// GP2X BEGIN
	//flush cache
	register unsigned long _beg __asm ("a1") = (unsigned long)(block_start);				// block start
	register unsigned long _end __asm ("a2") = (unsigned long)(block_start+block_size);		// block end
	register unsigned long _flg __asm ("a3") = 0;
	__asm __volatile ("swi 0x9f0002		@ sys_cacheflush"
		: // no outputs
		: "r" (_beg), "r" (_end), "r" (_flg)
		);
// GP2X END
}

#endif








