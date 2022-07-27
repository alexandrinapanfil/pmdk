#include "spp_wrappers.h"
#include "libpmemobj/base.h"
#include "libpmemobj/tx_base.h"
#include "obj.h"
#include <fcntl.h>           /* For O_* constants */
#include <unistd.h>
#include <assert.h>
#include <set.h>
#include "os.h"
#include "tx.h"

#if defined(DEBUG) && !defined(SPP_OFF)
#  define size_check(x) assert(x < MAX_OBJ_SIZE)
#else
#  define size_check(x)
#endif

#if !defined(SPP_OFF)
#  define snapshot_check(oid, hoff, size) assert(oid.size >= (hoff + size) && "spp add_range overflow")
#else
#  define snapshot_check(oid, hoff, size)
#endif

/* does not need to change as we trust pmdk code */
PMEMobjpool *pmemobj_open(const char *path, const char *given_layout) {
    return pmemobj_open_unsafe(path, given_layout);
}

/* does not need to change as we trust pmdk code */
PMEMobjpool *pmemobj_create(const char *path, const char *real_layout, size_t poolsize, mode_t mode) {
    return pmemobj_create_unsafe(path, real_layout, poolsize, mode);
}

/* does not need to change as we trust pmdk code */
void pmemobj_close(PMEMobjpool *pop) {
    pmemobj_close_unsafe(pop);
}

/* pmemobj_root_construct unsafe version*/
PMEMoid pmemobj_root(PMEMobjpool *pop, size_t size) {
    size_check(size);
    return pmemobj_root_unsafe(pop, size);
}

/* tx_alloc_common unsafe version */
PMEMoid pmemobj_tx_alloc(size_t size, uint64_t type_num) {
    size_check(size);
    return pmemobj_tx_alloc_unsafe(size, type_num);
}

/* does not need to change as we trust pmdk code and only frees the respective block */
int pmemobj_tx_free(PMEMoid oid) {
    return pmemobj_tx_free_unsafe(oid);
}

/* does not need to change as we trust pmdk code and only frees the respective block */
int pmemobj_tx_xfree(PMEMoid oid, uint64_t flags) {
    return pmemobj_tx_xfree_unsafe(oid, flags);
}

/* tx_alloc_common unsafe version */
PMEMoid pmemobj_tx_zalloc(size_t size, uint64_t type_num) {
    size_check(size);
    return pmemobj_tx_zalloc_unsafe(size, type_num);
}

/* does not need to change as usable_size is independent by the object size */
size_t pmemobj_alloc_usable_size(PMEMoid oid) {
    size_check(oid.size);
    return pmemobj_alloc_usable_size_unsafe(oid);
}  

/* does not need to change as type_num is independent by the size */
uint64_t pmemobj_type_num(PMEMoid oid) {
    size_check(oid.size);
    return pmemobj_type_num_unsafe(oid);
}

/* obj_alloc_construct unsafe version */
int pmemobj_alloc(PMEMobjpool *pop, PMEMoid *oidp, size_t size,
	uint64_t type_num, pmemobj_constr constructor, void *arg) {
    size_check(size);
    return pmemobj_alloc_unsafe(pop, oidp, size, type_num, constructor, arg);
}

/* obj_alloc_construct unsafe version */
int pmemobj_zalloc(PMEMobjpool *pop, PMEMoid *oidp, size_t size,
    uint64_t type_num) {
    size_check(size);
    return pmemobj_zalloc_unsafe(pop, oidp, size, type_num);
}

/* obj_free unsafe version -- add operation entry for size */
void pmemobj_free(PMEMoid *oidp) {
    pmemobj_free_unsafe(oidp);
}

/* does not need to change as first object is independent from size */
PMEMoid pmemobj_first(PMEMobjpool *pop) {
    return pmemobj_first_unsafe(pop);
}

/* does not need to change as next object is independent from size */
PMEMoid pmemobj_next(PMEMoid oid) {
    return pmemobj_next_unsafe(oid);
}

/* tx_realloc_common unsafe version */
PMEMoid pmemobj_tx_realloc(PMEMoid oid, size_t size, uint64_t type_num) {
    size_check(size);
    return pmemobj_tx_realloc_unsafe(oid, size, type_num);
}

/* tx_realloc_common unsafe version */
PMEMoid pmemobj_tx_zrealloc(PMEMoid oid, size_t size, uint64_t type_num) {
    size_check(size);
    return pmemobj_tx_zrealloc_unsafe(oid, size, type_num);
}

/* obj_realloc_common unsafe version */
int pmemobj_realloc(PMEMobjpool *pop, PMEMoid *oidp, size_t size, uint64_t type_num) {
    size_check(size);
    return pmemobj_realloc_unsafe(pop, oidp, size, type_num);
}

/* obj_realloc_common unsafe version */
int pmemobj_zrealloc(PMEMobjpool *pop, PMEMoid *oidp, size_t size, uint64_t type_num) {
    size_check(size);
    return pmemobj_zrealloc_unsafe(pop, oidp, size, type_num);
}

/* tx_alloc_common unsafe version */
PMEMoid pmemobj_tx_xalloc(size_t size, uint64_t type_num, uint64_t flags) {
    size_check(size);
    return pmemobj_tx_xalloc_unsafe(size, type_num, flags);
}

/* obj_alloc_construct unsafe version */
int pmemobj_xalloc(PMEMobjpool *pop, PMEMoid *oidp, size_t size,
	uint64_t type_num, uint64_t flags, pmemobj_constr constructor, void *arg) {
    size_check(size);
    return pmemobj_xalloc_unsafe(pop, oidp, size, type_num, flags, constructor, arg);
}

/* check the snapshotting range in PMDK runtime if it's not already done */
int pmemobj_tx_xadd_range(PMEMoid oid, uint64_t hoff, size_t size, uint64_t flags) {
    snapshot_check(oid, hoff, size);
    return pmemobj_tx_xadd_range_unsafe(oid, hoff, size, flags);
}

/* check the snapshotting range in PMDK runtime if it's not already done */
int pmemobj_tx_add_range(PMEMoid oid, uint64_t hoff, size_t size) {
    snapshot_check(oid, hoff, size);
    return pmemobj_tx_add_range_unsafe(oid, hoff, size);
}

/* 
 * check the snapshotting range in PMDK runtime if it's not already done :
 * *ptr is derived from pmemobj_direct function -- compiler pass is responsible for the handling 
 */
int pmemobj_tx_add_range_direct(const void *ptr, size_t size) {
#ifndef SPP_OFF
    // uintptr_t tag = (((uintptr_t)ptr) >> ADDRESS_BITS) + size - 1;
    uintptr_t tag = ((((uintptr_t)ptr) & TAG_CLEAN) >> ADDRESS_BITS) + (size - 1);
    uintptr_t overflow_bit = (tag << ADDRESS_BITS) & OVERFLOW_KEEP; // keep the new overflow bit
    if (overflow_bit)
    {
        printf("%s : overflow in adding range\n", __func__);
        _exit(1);
    }

    uintptr_t ptrval = (uintptr_t)ptr & PTR_CLEAN; // clean previous tag
    // ptrval = ptrval | overflow_bit; // apply the new overflow bit

#ifdef DEBUG
    printf("tag:%lx overflow_bit:%lx ptrval:%lx\n", tag, overflow_bit, ptrval);
#endif

    return pmemobj_tx_add_range_direct_unsafe((void*)ptrval, size);
#else
    return pmemobj_tx_add_range_direct_unsafe(ptr, size);
#endif
}

/* 
 * check the snapshotting range in PMDK runtime if it's not already done :
 * *ptr is derived from pmemobj_direct function -- compiler pass is responsible for the handling 
 */
int pmemobj_tx_xadd_range_direct(const void *ptr, size_t size, uint64_t flags) {
#ifndef SPP_OFF
    // uintptr_t tag = (((uintptr_t)ptr) >> ADDRESS_BITS) + size - 1;
    uintptr_t tag = ((((uintptr_t)ptr) & TAG_CLEAN) >> ADDRESS_BITS) + (size - 1);
    uintptr_t overflow_bit = (tag << ADDRESS_BITS) & OVERFLOW_KEEP; // keep the new overflow bit
    if (overflow_bit)
    {
        printf("%s : overflow in adding range\n", __func__);
        _exit(1);
    }

    uintptr_t ptrval = (uintptr_t)ptr & PTR_CLEAN; // clean previous tag
    // ptrval = ptrval | overflow_bit; // apply the new overflow bit

#ifdef DEBUG
    printf("tag:%lx overflow_bit:%lx ptrval:%lx\n", tag, overflow_bit, ptrval);
#endif

    return pmemobj_tx_xadd_range_direct_unsafe((void*)ptrval, size, flags);
#else
    return pmemobj_tx_xadd_range_direct_unsafe(ptr, size, flags);
#endif
}

/* 
 * internal functions to be adapted :
 * pmemobj_root_construct -> obj_alloc_root : OK, root offset and root size is already in pop structure
 * pmemobj_root, pmemobj_direct need to return the encoded pointer
 * obj_alloc_construct : OK, added redo log entry for the oid.size
 * obj_realloc_common : OK, added redo log entry for the oid.size
 * obj_free : OK, added redo log entry for the oid.size
 * tx_alloc_common : OK, added oid.size set
 * tx_realloc_common : OK, no need to change as it is a combination of tx_free + tx_alloc
*/