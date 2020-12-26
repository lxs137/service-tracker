//
// Created by liao xiangsen on 12/25/20.
//

#ifndef BINDER_DRIVER_TYPES_H
#define BINDER_DRIVER_TYPES_H

namespace tracker { namespace binder {

/**
 * FROM kernel/include/uapi/linux/android/binder.h
 */


//typedef __u32 binder_size_t;
//typedef __u32 binder_uintptr_t;

typedef __u64 binder_size_t;
typedef __u64 binder_uintptr_t;

/*
 * This is the flattened representation of a Binder object for transfer
 * between processes.  The 'offsets' supplied as part of a binder transaction
 * contains offsets into the data where these structures occur.  The Binder
 * driver takes care of re-writing the structure type and data as it moves
 * between processes.
 */
struct flat_binder_object {
  /* 8 bytes for large_flat_header. */
  __u32		type;
  __u32		flags;

  /* 8 bytes of data. */
  union {
    binder_uintptr_t	binder;	/* local object */
    __u32			handle;	/* remote object */
  };

  /* extra data associated with local object */
  binder_uintptr_t	cookie;
};

/*
 * On 64-bit platforms where user code may run in 32-bits the driver must
 * translate the buffer (and local binder) addresses appropriately.
 */

struct binder_write_read {
  binder_size_t		write_size;	/* bytes to write */
  binder_size_t		write_consumed;	/* bytes consumed by driver */
  binder_uintptr_t	write_buffer;
  binder_size_t		read_size;	/* bytes to read */
  binder_size_t		read_consumed;	/* bytes consumed by driver */
  binder_uintptr_t	read_buffer;
};

struct binder_transaction_data {
  /* The first two are only used for bcTRANSACTION and brTRANSACTION,
   * identifying the target and contents of the transaction.
   */
  union {
    /* target descriptor of command transaction */
    __u32	handle;
    /* target descriptor of return transaction */
    binder_uintptr_t ptr;
  } target;
  binder_uintptr_t	cookie;	/* target object cookie */

  // method id of the remote service to execute
  __u32		code;		/* transaction command */

  /* General information about the transaction. */
  __u32	        flags;
  pid_t		sender_pid;
  uid_t		sender_euid;
  binder_size_t	data_size;	/* number of bytes of data */
  binder_size_t	offsets_size;	/* number of bytes of offsets */

  /* If this transaction is inline, the data immediately
   * follows here; otherwise, it ends with a pointer to
   * the data buffer.
   */
  union {
    struct {
      /* transaction data */
      binder_uintptr_t	buffer;
      /* offsets from buffer to flat_binder_object structs */
      binder_uintptr_t	offsets;
    } ptr;
    __u8	buf[8];
  } data;
};

}}

#endif //BINDER_DRIVER_TYPES_H
