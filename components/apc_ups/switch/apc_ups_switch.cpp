#include "apc_ups_switch.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace apc_ups {

static const char *const TAG = "apc_ups.switch";

void ApcUpsSwitch::dump_config() { LOG_SWITCH("", "ApcUps Switch", this); }
void ApcUpsSwitch::write_state(bool state) {
  if (state) {
    if (this->on_command_.length() > 0) {
      this->parent_->switch_command(this->on_command_);
    }
  } else {
    if (this->off_command_.length() > 0) {
      this->parent_->switch_command(this->off_command_);
    }
  }
}

}  // namespace apc_ups
}  // namespace esphome
