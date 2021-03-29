/*
 * Copyright (C) by Argonne National Laboratory
 *     See COPYRIGHT in top-level directory
 */

#ifndef POSIX_IMPL_H_INCLUDED
#define POSIX_IMPL_H_INCLUDED

#include <mpidimpl.h>
#include "mpidch4r.h"

#include "posix_types.h"
#include "posix_eager.h"
#include "posix_eager_impl.h"

/* This function implements posix vci to vni(context) mapping.
 * Currently, it only supports one-to-one mapping.
 */
MPL_STATIC_INLINE_PREFIX int MPIDI_POSIX_vci_to_vni(int vci)
{
    return vci;
}

MPL_STATIC_INLINE_PREFIX int MPIDI_POSIX_vci_to_vni_assert(int vci)
{
    int vni = MPIDI_POSIX_vci_to_vni(vci);
    MPIR_Assert(vni < MPIDI_POSIX_global.num_vnis);
    return vni;
}

#endif /* POSIX_IMPL_H_INCLUDED */
