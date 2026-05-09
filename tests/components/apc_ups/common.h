#pragma once
#include <cstring>
#include <string>
#include "esphome/components/apc_ups/apc_ups.h"

namespace esphome::apc_ups::testing {

class TestableApcUps : public ApcUps {
 public:
  void decode_and_publish(ENUMPollingCommand cmd, const std::string &response) {
    for (size_t i = 0; i < 41; i++) {
      if (used_polling_commands_[i].length > 0 && used_polling_commands_[i].identifier == cmd) {
        last_polling_command_ = i;
        break;
      }
    }
    strncpy(reinterpret_cast<char *>(read_buffer_), response.c_str(), APC_UPS_READ_BUFFER_LENGTH - 1);
    state_ = STATE_POLL_CHECKED;
    loop();  // decode: STATE_POLL_CHECKED -> STATE_POLL_DECODED
    loop();  // publish: STATE_POLL_DECODED -> STATE_IDLE
  }
};

}  // namespace esphome::apc_ups::testing
