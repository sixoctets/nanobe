#ifndef _MEMIO_H_
#define _MEMIO_H_

static inline void mmio_clr_u32(uint32_t base, uint32_t offset, uint32_t val)
{
	(*(volatile uint32_t *) ((base) + (offset))) &= ~val;
}

static inline void mmio_set_u32(uint32_t base, uint32_t offset, uint32_t val)
{
	(*(volatile uint32_t *) ((base) + (offset))) |= val;
}

#endif
