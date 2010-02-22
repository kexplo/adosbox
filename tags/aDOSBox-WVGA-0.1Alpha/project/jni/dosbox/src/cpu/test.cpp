#include "dosbox.h"
#include "mem.h"

__volatile__ void testmem()
{
Bit8u * testptr;
	testptr = (Bit8u*)1000;
	LOG_MSG("%d",host_readw(testptr));
	LOG_MSG("%d",host_readd(testptr));
	host_writew(testptr,0x1234);
	host_writed(testptr+10,0xAABBCCDD);
}
