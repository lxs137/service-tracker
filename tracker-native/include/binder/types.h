//
// Created by liao xiangsen on 12/24/20.
//

#ifndef BINDER_TYPES_H
#define BINDER_TYPES_H

#include "base/utils.h"
#include "binder/driver_types.h"

#define TRACKER_BINDER_BEGIN namespace tracker { namespace binder {
#define TRACKER_BINDER_END }}
#define TRACKER_BINDER_USING using namespace tracker::binder;

TRACKER_BINDER_BEGIN

enum IoctlCmd {
  BINDER_WRITE_READ = 0xc0306201,
};

enum DriverProtocol {
  BC_TRANSACTION = 0x40406300,
  BC_REPLY = 0x40406301,
  BC_ACQUIRE_RESULT = 0x40046302,
  BC_FREE_BUFFER = 0x40086303,

  BC_INCREFS = 0x40046304,
  BC_ACQUIRE = 0x40046305,
  BC_RELEASE = 0x40046306,
  BC_DECREFS = 0x40046307,

  BC_INCREFS_DONE = 0x40106308,
  BC_ACQUIRE_DONE = 0x40106309,
  BC_ATTEMPT_ACQUIRE = 0x4008630a,
  BC_REGISTER_LOOPER = 0x630b,

  BC_ENTER_LOOPER = 0x630c,
  BC_EXIT_LOOPER = 0x630d,
  BC_REQUEST_DEATH_NOTIFICATION = 0x400c630e,
  BC_CLEAR_DEATH_NOTIFICATION = 0x400c630f,

  BC_DEAD_BINDER_DONE = 0x40086310,
};

binder_write_read parse_binder_write_read(uintptr_t cmd, void *dataPtr);

const size_t INTERFACE_MAX_LEN = 100;

struct BinderRPCContext {
  std::string interface;
  unsigned methodID;
  pid_t clientPID;

  BinderRPCContext(binder_write_read *dataPtr) {
      auto transDataPtr = reinterpret_cast<binder::binder_transaction_data*>(
          (char*)(dataPtr->write_buffer) + dataPtr->write_consumed + 4);
      auto bufferPtr = reinterpret_cast<uint8_t*>(transDataPtr->data.ptr.buffer);
      auto interfaceTokenLen = *(reinterpret_cast<int*>(bufferPtr + 4));
      char16_t interfaceTokenRaw[INTERFACE_MAX_LEN];
      memcpy(interfaceTokenRaw, bufferPtr + 8, interfaceTokenLen * 2);
      interfaceTokenRaw[interfaceTokenLen] = '\0';

      this->clientPID = transDataPtr->sender_pid;
      this->methodID = transDataPtr->code;
      this->interface = narrowChar16(interfaceTokenRaw);
  }
};

TRACKER_BINDER_END

#endif //BINDER_TYPES_H
