
#include <FizzBuzzWhizz/aligned_malloc.h>
#include <crtdbg.h>
#include <malloc.h>
#include <memory.h>

static unsigned char _cAlignSignFill    = 0xE9;     /* fill no-man's sign for aligned routines */
static unsigned char _cClearSignFill    = 0x00;     /* fill no-man's sign for free routines */

/*******************************************************************************
*static int _CheckBytes() - verify byte range set to proper value
*
*Purpose:
*       verify byte range set to proper value
*
*Entry:
*       unsigned char *pb       - pointer to start of byte range
*       unsigned char bCheck    - value byte range should be set to
*       size_t nSize            - size of byte range to be checked
*
*Return:
*       TRUE - if all bytes in range equal bcheck
*       FALSE otherwise
*
*******************************************************************************/
bool __cdecl _iso_check_bytes(
        unsigned char *pb,
        unsigned char bCheck,
        size_t nSize
       )
{
        bool bOkay = true;
        while (nSize--) {
            if (*pb++ != bCheck) {
/* Internal error report is just noise; calling functions all report results - JWM */
/*                _RPT3(_CRT_WARN, "memory check error at 0x%p = 0x%02X, should be 0x%02X.\n", */
/*                    (BYTE *)(pb-1),*(pb-1), bCheck); */
                bOkay = false;
            }
        }
        return bOkay;
}

//
// MS1B: BitScanForward
//
// 参考:
// http://stackoverflow.com/questions/466204/rounding-off-to-nearest-power-of-2
// http://stackoverflow.com/questions/364985/algorithm-for-finding-the-smallest-power-of-two-thats-greater-or-equal-to-a-giv
//

unsigned __cdecl iso_next_power_of_2(unsigned x)
{
#if 1
    // ms1b
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return ++x;
#else
    unsigned ms1b = 1;
    while (ms1b < x)
        ms1b <<= 1;

    return ms1b;
#endif
}

size_t __cdecl iso_adjust_alignment(size_t alignment)
{
    alignment = iso_next_power_of_2(alignment);
    _ASSERT(IS_POWER_OF_2(alignment));

    alignment = (alignment > sizeof(uintptr_t)) ? alignment : sizeof(uintptr_t);
    _ASSERT(alignment > 0);

    return alignment;
}

size_t __cdecl iso_get_alignment(size_t alignment)
{
    return iso_adjust_alignment(alignment);
}

void * __cdecl iso_aligned_malloc(size_t size, size_t alignment)
{
    size_t alignment_mask;
    size_t alloc_size;
    uintptr_t pvAlloc, pvData;
    ALIGN_BLOCK_HEADER *pBlockHdr;
#ifdef _DEBUG
    uintptr_t nFrontPaddedSize;
    uintptr_t nLastPaddedSize;
#endif

    /* 如果不是2的幂次方, 则调整到最接近的2的幂次方 */
    _ASSERT(IS_POWER_OF_2(alignment));
    if (!IS_POWER_OF_2(alignment)) {
        alignment = iso_adjust_alignment(alignment);
        _ASSERT(!IS_POWER_OF_2(alignment));
    }
    else {
        alignment = (alignment > sizeof(uintptr_t)) ? alignment : sizeof(uintptr_t);
        _ASSERT(alignment > 0);
    }

    alignment_mask = alignment - 1;

    // alloc_size align to alignment bytes (isn't must need)
    alloc_size = size + alignment_mask + sizeof(ALIGN_BLOCK_HEADER);

    pvAlloc = (uintptr_t)::malloc(alloc_size);
    if (pvAlloc != NULL) {
        // data pointer align to alignment bytes
        pvData = (uintptr_t)(((uintptr_t)pvAlloc + alignment_mask + sizeof(ALIGN_BLOCK_HEADER))
            & (~alignment_mask));

        pBlockHdr = (ALIGN_BLOCK_HEADER *)(pvData) - 1;
        _ASSERT((uintptr_t)pBlockHdr >= pvAlloc);

        pBlockHdr->pvAlloc = (void *)pvAlloc;
        ::memset((void *)pBlockHdr->Sign, _cAlignSignFill, ALIGN_SIGN_SIZE);

#ifdef _DEBUG
        // for debug
        nFrontPaddedSize = (uintptr_t)pvData - (uintptr_t)pvAlloc;
        nLastPaddedSize  = (uintptr_t)alloc_size - (uintptr_t)size - nFrontPaddedSize;

        _ASSERT(nFrontPaddedSize >= sizeof(ALIGN_BLOCK_HEADER));
        _ASSERT(nLastPaddedSize >= 0);
#endif
        return (void *)pvData;
    }

    return (void *)NULL;
}

void * __cdecl iso_aligned_realloc(void *ptr, size_t new_size, size_t alignment)
{
    //
    return NULL;
}

void * __cdecl iso_aligned_recalloc(void *ptr, size_t new_size, size_t alignment)
{
    //
    return NULL;
}

void * __cdecl iso_aligned_calloc(size_t size, size_t alignment)
{
    //
    return NULL;
}

void * __cdecl iso_aligned_offset_malloc(size_t size, size_t alignment, size_t offset)
{
    size_t alignment_mask;
    size_t alloc_size;
    uintptr_t pvAlloc, pvData, uintptr_offset;
    ALIGN_BLOCK_HEADER *pBlockHdr;
#ifdef _DEBUG
    uintptr_t nFrontPaddedSize;
    uintptr_t nLastPaddedSize;
#endif

    _ASSERT(offset == 0 || offset < size);
    if (offset >= size)
        return NULL;

    /* 如果不是2的幂次方, 则调整到最接近的2的幂次方 */
    _ASSERT(IS_POWER_OF_2(alignment));
    if (!IS_POWER_OF_2(alignment)) {
        alignment = iso_adjust_alignment(alignment);
        _ASSERT(!IS_POWER_OF_2(alignment));
    }
    else {
        alignment = (alignment > sizeof(uintptr_t)) ? alignment : sizeof(uintptr_t);
        _ASSERT(alignment > 0);
    }

    alignment_mask = alignment - 1;
    uintptr_offset = (0 - offset) & (sizeof(uintptr_t) - 1);

    // alloc_size align to alignment bytes (isn't must need)
    alloc_size = size + alignment_mask + sizeof(ALIGN_BLOCK_HEADER) + uintptr_offset;

    pvAlloc = (uintptr_t)::malloc(alloc_size);
    if (pvAlloc != NULL) {
        // data pointer align to alignment bytes
        pvData = (uintptr_t)((((uintptr_t)pvAlloc + alignment_mask + sizeof(ALIGN_BLOCK_HEADER)
            + uintptr_offset + offset) & (~alignment_mask)) - offset);

        pBlockHdr = (ALIGN_BLOCK_HEADER *)(pvData - uintptr_offset) - 1;
        _ASSERT((uintptr_t)pBlockHdr >= pvAlloc);

        pBlockHdr->pvAlloc = (void *)pvAlloc;
        ::memset((void *)pBlockHdr->Sign, _cAlignSignFill, ALIGN_SIGN_SIZE);

#ifdef _DEBUG
        // for debug
        nFrontPaddedSize = (uintptr_t)pvData - (uintptr_t)pvAlloc;
        nLastPaddedSize  = (uintptr_t)alloc_size - (uintptr_t)size - nFrontPaddedSize;

        _ASSERT(nFrontPaddedSize >= sizeof(ALIGN_BLOCK_HEADER));
        _ASSERT(nLastPaddedSize >= 0);
#endif
        return (void *)pvData;
    }

    return (void *)NULL;
}

void * __cdecl iso_aligned_offset_realloc(void *ptr, size_t new_size, size_t alignment, size_t offset)
{
    //
    return NULL;
}

void * __cdecl iso_aligned_offset_recalloc(void *ptr, size_t new_size, size_t alignment, size_t offset)
{
    //
    return NULL;
}

void * __cdecl iso_aligned_offset_calloc(size_t size, size_t alignment, size_t offset)
{
    //
    return NULL;
}

void * __cdecl _iso_free_block_header(ALIGN_BLOCK_HEADER *pBlockHdr)
{
    void *pvAlloc = NULL;

    _ASSERT(pBlockHdr != NULL);
    if (pBlockHdr != NULL) {
        pvAlloc = pBlockHdr->pvAlloc;
        _ASSERT((void *)pBlockHdr >= pvAlloc);

        if ((void *)pBlockHdr < pvAlloc) {
            // We don't know where pvData was allocated
            _RPT1(_CRT_ERROR, "Damage before 0x%p which was allocated by aligned routine\n", pvAlloc);
            pvAlloc = (void *)-2;
        }
        else {
            if (_iso_check_bytes(pBlockHdr->Sign, _cAlignSignFill, ALIGN_SIGN_SIZE)) {
                // Set and fill clear sign
                ::memset(pBlockHdr->Sign, _cClearSignFill, ALIGN_SIGN_SIZE);

                // Set pvAlloc's value to NULL
                pBlockHdr->pvAlloc = NULL;

                // Free memory block if need
                if (pvAlloc != NULL) {
                    ::free(pvAlloc);
                }
            }
            else {
                // We don't know where pvData was allocated
                _RPT1(_CRT_ERROR, "Damage before 0x%p which was allocated by aligned routine\n", pvAlloc);
                pvAlloc = (void *)-1;
            }
        }
    }
    return pvAlloc;
}

void * __cdecl iso_aligned_free(const void *pvData)
{
    ALIGN_BLOCK_HEADER *pBlockHdr;
    void *pvAlloc = NULL;

    _ASSERT(pvData != NULL);
    if (pvData != NULL) {
        pBlockHdr = (ALIGN_BLOCK_HEADER *)((uintptr_t)pvData & ~(sizeof(uintptr_t) - 1)) - 1;
        _ASSERT(pBlockHdr < pvData);

        pvAlloc = _iso_free_block_header(pBlockHdr);
        _ASSERT(pvAlloc != NULL);
        _ASSERT(pvAlloc != ((void *)-1));
        _ASSERT(pvAlloc != ((void *)-2));
    }
    return pvAlloc;
}
