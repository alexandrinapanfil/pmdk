#include "asan.h"

#include <assert.h>
#include "obj.h"

/*

shadow memory location: 0x7fff7000..0x10007FFF8000
 location in shadow memory: 0x8FFF6E00..0x2008FFF7000
 thus, the address range    0x8fff7000..0x2008fff7000 is marked unaccessible at the paging level
 this ensures that no instrumented code can try to read/write the shadow memory itself
 the range bw. 6E00 to 7000 is not protected, unfortunately, because the unaccessible region has to be 4kb-aligned.
 
 "The malloc call stack is stored in the left redzone (the larger the redzone, the larger the number of frames that can be stored) while the free call stack is stored in the beginning of the memory region itself."
    ->storing the call stack, which is composed of volatile/randomized pointers, is probably not helpful in the context of persistent memory
*/

uint8_t* pmemobj_asan_get_shadow_mem_location(void* _p) {
		  uint64_t p = (uint64_t)_p;
		  return (uint8_t*)((p>>3)+0x7fff8000);
}

// Even with no_sanitize, calls to memset get intercepted by ASan,
//  which is uncomfortable with us directly modifying the shadow memory
__attribute__((no_sanitize("address")))
void pmemobj_asan_memset(uint8_t* start, uint8_t byt, size_t len) {
	while (len) {
		*start = byt;
		start++;
		len--;
	}
}

__attribute__((no_sanitize("address")))
void pmemobj_asan_memcpy(void* dest_, const void* src_, size_t len) {
	uint8_t* dest = (uint8_t*)dest_;
	const uint8_t* src = (const uint8_t*)src_;
	while (len) {
		*(uint8_t*)dest = *(uint8_t*)src;
		dest++;
		src++;
		len--;
	}
}

// len in bytes
__attribute__((no_sanitize("address")))
void pmemobj_asan_mark_mem(void* start, size_t len, uint8_t tag) {
	assert((int8_t)tag <= 0);
	if ((uint64_t)start%8) {
		uint64_t misalignment = (uint64_t)start%8;
		/*uint8_t* shadow_pos = get_shadow_mem_location(start);
		*shadow_pos = tag;*/ // We can only enter this branch during the marking of the right red-zone for non-multiple-of-8 sized objects. In this case, we must not modify this bit of the shadow memory.
		start = (void*)((uint64_t)start+8-misalignment);
		len -= 8-misalignment;
	}
	pmemobj_asan_memset(pmemobj_asan_get_shadow_mem_location(start), tag, len/8);
	if (len%8) {
		int prot = len%8;
		uint8_t* shadow_pos = pmemobj_asan_get_shadow_mem_location((uint8_t*)start+len);
		if (tag)
			*shadow_pos = tag; // We don't really need to check the previous value of *shadow_start here, because pmemobj would not distribute the same 8-byte chunk to multiple objects.
		else
			*shadow_pos = (uint8_t)prot;
	}
}

void pmemobj_asan_alloc_sm_modify_persist(PMEMobjpool* pop, uint64_t lrz_off, size_t size_wo_redzone) {
	if (lrz_off == 0) {
		return ;
	}

	pmemobj_asan_mark_mem((uint8_t*)pop + lrz_off, pmemobj_asan_RED_ZONE_SIZE, pmemobj_asan_LEFT_REDZONE);
	pmemobj_asan_mark_mem((uint8_t*)pop + lrz_off + pmemobj_asan_RED_ZONE_SIZE, size_wo_redzone, pmemobj_asan_ADDRESSABLE);	
	pmemobj_asan_mark_mem((uint8_t*)pop + lrz_off + pmemobj_asan_RED_ZONE_SIZE + size_wo_redzone, pmemobj_asan_RED_ZONE_SIZE, pmemobj_asan_RIGHT_REDZONE);

	void* sm_start = (uint8_t*)pop + pop->shadow_mem_offset + lrz_off/8;
	size_t sm_len = (size_wo_redzone + 7 + 2*pmemobj_asan_RED_ZONE_SIZE)/8; // Round up, in case size_wo_redzone % 8 != 0

	pmemobj_persist(pop, sm_start, sm_len);
}