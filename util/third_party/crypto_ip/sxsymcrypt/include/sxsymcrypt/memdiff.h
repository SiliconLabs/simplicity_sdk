/** Buffer comparison
 *
 * @file
 * @copyright Copyright 2023 Secure-IC S.A.S.
 * This file relies on Secure-IC S.A.S. software and patent portfolio.
 * This file cannot be used nor duplicated without prior approval from Secure-IC S.A.S.
 */

#ifndef MEMDIFF_HEADER_FILE
#define MEMDIFF_HEADER_FILE

#ifdef __cplusplus
extern "C" {
#endif


static inline int sx_memdiff(const char *a, const char *b, size_t sz)
{
    int r = 0;
    size_t i;

    for (i = 0; i < sz; i++, a++, b++)
        r |= *a ^ *b;

    return r;
}


#ifdef __cplusplus
}
#endif

#endif
