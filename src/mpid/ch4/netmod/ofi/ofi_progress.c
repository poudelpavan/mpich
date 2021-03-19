/*
 * Copyright (C) by Argonne National Laboratory
 *     See COPYRIGHT in top-level directory
 */

#include "mpidimpl.h"
#include "ofi_impl.h"
#include "ofi_events.h"

static int handle_deferred_ops(int vni);

static int handle_deferred_ops(int vni)
{

    int mpi_errno = MPI_SUCCESS;
    MPIDI_OFI_deferred_am_isend_req_t *dreq = MPIDI_OFI_global.am_list[vni].deferred_am_isend_q;
    int vni_src = vni; //dreq->comm->seq % MPIDI_CH4_MAX_VCIS;
    int vni_dst = vni; //dreq->comm->seq % MPIDI_CH4_MAX_VCIS;

    MPIR_FUNC_VERBOSE_STATE_DECL(MPID_STATE_MPIDI_OFI_HANDLE_DEFERRED_OPS);
    MPIR_FUNC_VERBOSE_ENTER(MPID_STATE_MPIDI_OFI_HANDLE_DEFERRED_OPS);

    if (dreq) {
        switch (dreq->op) {
            case MPIDI_OFI_DEFERRED_AM_OP__ISEND_EAGER:
                mpi_errno = MPIDI_OFI_do_am_isend_eager(dreq->rank, dreq->comm, dreq->handler_id,
                                                        NULL, 0, dreq->buf, dreq->count,
                                                        dreq->datatype, vni_src, vni_dst, dreq->sreq, true);
                break;
            case MPIDI_OFI_DEFERRED_AM_OP__ISEND_PIPELINE:
                mpi_errno = MPIDI_OFI_do_am_isend_pipeline(dreq->rank, dreq->comm, dreq->handler_id,
                                                           NULL, 0, dreq->buf, dreq->count,
                                                           dreq->datatype, vni_src, vni_dst, dreq->sreq,
                                                           dreq->data_sz, true);
                break;
            case MPIDI_OFI_DEFERRED_AM_OP__ISEND_RDMA_READ:
                mpi_errno = MPIDI_OFI_do_am_isend_rdma_read(dreq->rank, dreq->comm,
                                                            dreq->handler_id, NULL, 0, dreq->buf,
                                                            dreq->count, dreq->datatype, vni_src, vni_dst, dreq->sreq,
                                                            true);
                break;
            default:
                MPIR_Assert(0);
        }
        MPIR_ERR_CHECK(mpi_errno);
    }

  fn_exit:
    MPIR_FUNC_VERBOSE_EXIT(MPID_STATE_MPIDI_OFI_HANDLE_DEFERRED_OPS);
    return mpi_errno;
  fn_fail:
    goto fn_exit;
}

int MPIDI_OFI_progress(int vci, int blocking)
{
    int mpi_errno;
    struct fi_cq_tagged_entry wc[MPIDI_OFI_NUM_CQ_ENTRIES];
    ssize_t ret;
    MPIR_FUNC_VERBOSE_STATE_DECL(MPID_STATE_MPIDI_OFI_PROGRESS);
    MPIR_FUNC_VERBOSE_ENTER(MPID_STATE_MPIDI_OFI_PROGRESS);

    int vni = MPIDI_OFI_vci_to_vni(vci);
    if (vni >= MPIDI_OFI_global.num_vnis) {
        /* The requests generated from ofi will have vni/vci within our range.
         * All requests that have vci beyond num_vnis are not from us, nothing
         * to do, so simply return.
         * NOTE: it is not an error since global progress will poll every vci.
         */
        return MPI_SUCCESS;
    }

    if (unlikely(MPIDI_OFI_get_buffered(wc, 1))){
        fprintf(stdout, "%ld, MPIDI_OFI_handle_cq_entries, get_buffered, vni=%d\n", pthread_self(), vni);
        mpi_errno = MPIDI_OFI_handle_cq_entries(wc, 1, vni);
    }
    else if (likely(1)) {
        ret = fi_cq_read(MPIDI_OFI_global.ctx[vni].cq, (void *) wc, MPIDI_OFI_NUM_CQ_ENTRIES);
        // fprintf(stdout, "%ld, MPIDI_OFI_progress, vni=%d, ret=%d\n", pthread_self(), vni, ret);
        if (likely(ret > 0)){
            //fprintf(stdout, "%ld, ctx[%d].cq=%ld\n", pthread_self(), vni, MPIDI_OFI_global.ctx[vni].cq->fid.fclass);
            mpi_errno = MPIDI_OFI_handle_cq_entries(wc, ret, vni);
        }
        else if (ret == -FI_EAGAIN)
            mpi_errno = MPI_SUCCESS;
        else{
            fprintf(stdout, "%ld, ret=%ld, ctx[%d].cq=%p, cq->fid.fclass=%ld\n", pthread_self(),ret, vni, &MPIDI_OFI_global.ctx[vni].cq, MPIDI_OFI_global.ctx[vni].cq->fid.fclass);
            mpi_errno = MPIDI_OFI_handle_cq_error(vni, ret);
        }
    }

    handle_deferred_ops(vni);

    MPIR_FUNC_VERBOSE_EXIT(MPID_STATE_MPIDI_OFI_PROGRESS);

    return mpi_errno;
}
