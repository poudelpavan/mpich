/*
 * Copyright (C) by Argonne National Laboratory
 *     See COPYRIGHT in top-level directory
 */

#ifndef CH4_WAIT_H_INCLUDED
#define CH4_WAIT_H_INCLUDED

#include "ch4_impl.h"

/* a local wrapper that accounts for persistent request */
MPL_STATIC_INLINE_PREFIX int get_vci_wrapper(MPIR_Request * req)
{
    int vci;
    if (req->kind == MPIR_REQUEST_KIND__PREQUEST_RECV ||
        req->kind == MPIR_REQUEST_KIND__PREQUEST_SEND) {
        if (req->u.persist.real_request) {
            vci = MPIDI_Request_get_vci(req->u.persist.real_request);
        } else {
            /* TODO: skip it in MPIDI_set_progress_vci_n */
            vci = 0;
        }
    } else {
        vci = MPIDI_Request_get_vci(req);
    }
    return vci;
}

MPL_STATIC_INLINE_PREFIX void MPIDI_set_progress_vci(MPIR_Request * req,
                                                     MPID_Progress_state * state)
{
    state->flag = MPIDI_PROGRESS_ALL;   /* TODO: check request is_local/anysource */
    state->progress_made = 0;

    int vci = get_vci_wrapper(req);
    MPID_THREAD_CS_ENTER(VCI, MPIDI_VCI(vci).lock);
    state->progress_counts[0] = MPIDI_global.progress_counts[vci];
    MPID_THREAD_CS_EXIT(VCI, MPIDI_VCI(vci).lock);

    state->vci_count = 1;
    state->vci[0] = vci;
    //fprintf(stdout, "%ld, exit MPIDI_set_progress_vci vci=%d\n", pthread_self(), vci);
}

MPL_STATIC_INLINE_PREFIX void MPIDI_set_progress_vci_n(int n, MPIR_Request ** reqs,
                                                       MPID_Progress_state * state)
{
    state->flag = MPIDI_PROGRESS_ALL;   /* TODO: check request is_local/anysource */
    state->progress_made = 0;

    int idx = 0;
    for (int i = 0; i < n; i++) {
        if (reqs[i] == NULL) {
            continue;
        }

        int vci = get_vci_wrapper(reqs[i]);
        int found = 0;
        for (int j = 0; j < idx; j++) {
            if (state->vci[j] == vci) {
                found = 1;
                break;
            }
        }
        if (!found) {
            state->vci[idx++] = vci;
            MPIR_Assert(vci < MPIDI_global.n_vcis);
        }
    }
    state->vci_count = idx;
    for (int i = 0; i < state->vci_count; i++) {
        int vci = state->vci[i];
        MPID_THREAD_CS_ENTER(VCI, MPIDI_VCI(vci).lock);
        state->progress_counts[i] = MPIDI_global.progress_counts[vci];
        MPID_THREAD_CS_EXIT(VCI, MPIDI_VCI(vci).lock);
    }
}

/* MPID_Test, MPID_Testall, MPID_Testany, MPID_Testsome */

MPL_STATIC_INLINE_PREFIX int MPID_Test(MPIR_Request * request_ptr, int *flag, MPI_Status * status)
{
    MPID_Progress_state progress_state;

    MPIDI_set_progress_vci(request_ptr, &progress_state);
    return MPIR_Test_state(request_ptr, flag, status, &progress_state);
}

MPL_STATIC_INLINE_PREFIX int MPID_Testall(int count, MPIR_Request * request_ptrs[],
                                          int *flag, MPI_Status array_of_statuses[],
                                          int requests_property)
{
    MPID_Progress_state progress_state;

    MPIDI_set_progress_vci_n(count, request_ptrs, &progress_state);
    return MPIR_Testall_state(count, request_ptrs, flag, array_of_statuses, requests_property,
                              &progress_state);
}

MPL_STATIC_INLINE_PREFIX int MPID_Testany(int count, MPIR_Request * request_ptrs[],
                                          int *indx, int *flag, MPI_Status * status)
{
    MPID_Progress_state progress_state;

    MPIDI_set_progress_vci_n(count, request_ptrs, &progress_state);
    return MPIR_Testany_state(count, request_ptrs, indx, flag, status, &progress_state);
}

MPL_STATIC_INLINE_PREFIX int MPID_Testsome(int incount, MPIR_Request * request_ptrs[],
                                           int *outcount, int array_of_indices[],
                                           MPI_Status array_of_statuses[])
{
    MPID_Progress_state progress_state;

    MPIDI_set_progress_vci_n(incount, request_ptrs, &progress_state);
    return MPIR_Testsome_state(incount, request_ptrs, outcount, array_of_indices,
                               array_of_statuses, &progress_state);
}

/* MPID_Wait, MPID_Waitall, MPID_Waitany, MPID_Waitsome */

MPL_STATIC_INLINE_PREFIX int MPID_Wait(MPIR_Request * request_ptr, MPI_Status * status)
{
    MPID_Progress_state progress_state;

    MPIDI_set_progress_vci(request_ptr, &progress_state);
    return MPIR_Wait_state(request_ptr, status, &progress_state);
}

MPL_STATIC_INLINE_PREFIX int MPID_Waitall(int count, MPIR_Request * request_ptrs[],
                                          MPI_Status array_of_statuses[], int request_properties)
{
    MPID_Progress_state progress_state;

    MPIDI_set_progress_vci_n(count, request_ptrs, &progress_state);
    return MPIR_Waitall_state(count, request_ptrs, array_of_statuses,
                              request_properties, &progress_state);
}

MPL_STATIC_INLINE_PREFIX int MPID_Waitany(int count, MPIR_Request * request_ptrs[],
                                          int *indx, MPI_Status * status)
{
    MPID_Progress_state progress_state;

    MPIDI_set_progress_vci_n(count, request_ptrs, &progress_state);
    return MPIR_Waitany_state(count, request_ptrs, indx, status, &progress_state);
}

MPL_STATIC_INLINE_PREFIX int MPID_Waitsome(int incount, MPIR_Request * request_ptrs[],
                                           int *outcount, int array_of_indices[],
                                           MPI_Status array_of_statuses[])
{
    MPID_Progress_state progress_state;

    MPIDI_set_progress_vci_n(incount, request_ptrs, &progress_state);
    return MPIR_Waitsome_state(incount, request_ptrs, outcount, array_of_indices,
                               array_of_statuses, &progress_state);
}

#endif /* CH4_WAIT_H_INCLUDED */
