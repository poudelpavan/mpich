/*
 * Copyright (C) by Argonne National Laboratory
 *     See COPYRIGHT in top-level directory
 */

#include "mpiimpl.h"

/* -- Begin Profiling Symbol Block for routine MPI_Iprobe */
#if defined(HAVE_PRAGMA_WEAK)
#pragma weak MPI_Iprobe = PMPI_Iprobe
#elif defined(HAVE_PRAGMA_HP_SEC_DEF)
#pragma _HP_SECONDARY_DEF PMPI_Iprobe  MPI_Iprobe
#elif defined(HAVE_PRAGMA_CRI_DUP)
#pragma _CRI duplicate MPI_Iprobe as PMPI_Iprobe
#elif defined(HAVE_WEAK_ATTRIBUTE)
int MPI_Iprobe(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status) __attribute__ ((weak, alias("PMPI_Iprobe")));
#endif
/* -- End Profiling Symbol Block */

/* Define MPICH_MPI_FROM_PMPI if weak symbols are not supported to build
   the MPI routines */
#ifndef MPICH_MPI_FROM_PMPI
#undef MPI_Iprobe
#define MPI_Iprobe PMPI_Iprobe

#endif

/*@
   MPI_Iprobe - Nonblocking test for a message

Input Parameters:
+ source - rank of source or MPI_ANY_SOURCE (integer)
. tag - message tag or MPI_ANY_TAG (integer)
- comm - communicator (handle)

Output Parameters:
+ flag - flag (logical)
- status - status object (Status)

.N ThreadSafe

.N Fortran

.N Errors
.N MPI_SUCCESS

.N MPI_ERR_ARG
.N MPI_ERR_COMM
.N MPI_ERR_RANK
.N MPI_ERR_TAG
@*/

int MPI_Iprobe(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status)
{
    int mpi_errno = MPI_SUCCESS;
    MPIR_Comm *comm_ptr = NULL;
    MPIR_FUNC_TERSE_STATE_DECL(MPID_STATE_MPI_IPROBE);

    MPIR_ERRTEST_INITIALIZED_ORDIE();

    MPID_THREAD_CS_ENTER(GLOBAL, MPIR_THREAD_GLOBAL_ALLFUNC_MUTEX);
    MPIR_FUNC_TERSE_ENTER(MPID_STATE_MPI_IPROBE);

#ifdef HAVE_ERROR_CHECKING
    {
        MPID_BEGIN_ERROR_CHECKS;
        {
            MPIR_ERRTEST_COMM(comm, mpi_errno);
        }
        MPID_END_ERROR_CHECKS;
    }
#endif /* HAVE_ERROR_CHECKING */

    MPIR_Comm_get_ptr(comm, comm_ptr);

#ifdef HAVE_ERROR_CHECKING
    {
        MPID_BEGIN_ERROR_CHECKS;
        {
            MPIR_Comm_valid_ptr(comm_ptr, mpi_errno, FALSE);
            if (mpi_errno) {
                goto fn_fail;
            }
            MPIR_ERRTEST_RECV_RANK(comm_ptr, source, mpi_errno);
            MPIR_ERRTEST_RECV_TAG(tag, mpi_errno);
            MPIR_ERRTEST_ARGNULL(flag, "flag", mpi_errno);
            MPIR_ERRTEST_ARGNULL(status, "status", mpi_errno);
        }
        MPID_END_ERROR_CHECKS;
    }
#endif /* HAVE_ERROR_CHECKING */

    /* Return immediately for dummy process */
    if (unlikely(source == MPI_PROC_NULL)) {
        MPIR_Status_set_procnull(status);
        *flag = TRUE;
        goto fn_exit;
    }

    /* ... body of routine ... */
    /* FIXME: Is this correct for intercomms? */
    mpi_errno = MPID_Iprobe(source, tag, comm_ptr, MPIR_CONTEXT_INTRA_PT2PT, flag, status);
    if (mpi_errno != MPI_SUCCESS)
        goto fn_fail;
    /* ... end of body of routine ... */

  fn_exit:
    MPIR_FUNC_TERSE_EXIT(MPID_STATE_MPI_IPROBE);
    MPID_THREAD_CS_EXIT(GLOBAL, MPIR_THREAD_GLOBAL_ALLFUNC_MUTEX);
    return mpi_errno;

  fn_fail:
    /* --BEGIN ERROR HANDLINE-- */
#ifdef HAVE_ERROR_CHECKING
    mpi_errno = MPIR_Err_create_code(mpi_errno, MPIR_ERR_RECOVERABLE, __func__, __LINE__, MPI_ERR_OTHER, "**mpi_iprobe", "**mpi_iprobe %i %t %C %p %p", source, tag, comm, flag, status);
#endif
    mpi_errno = MPIR_Err_return_comm(comm_ptr, __func__, mpi_errno);
    /* --END ERROR HANDLING-- */
    goto fn_exit;
}
