#include "apc_ups.h"
#include "esphome/core/log.h"

namespace esphome {
namespace apc_ups {

static const char *const TAG = "apc_ups";

void ApcUps::setup() {
  this->state_ = STATE_IDLE;
  this->command_start_millis_ = 0;
  this->add_polling_command_("Y", POLLING_Y);
}

void ApcUps::empty_uart_buffer_() {
  uint8_t byte;
  while (this->available()) {
    this->read_byte(&byte);
  }
}

void ApcUps::loop() {
  // Read message
  if (this->state_ == STATE_IDLE) {
    this->empty_uart_buffer_();
    switch (this->send_next_command_()) {
      case 0:
        // no command send (empty queue) time to poll
        if (millis() - this->last_poll_ > this->update_interval_) {
          this->send_next_poll_();
          this->last_poll_ = millis();
        }
        return;
        break;
      case 1:
        // command send
        return;
        break;
    }
  }
  if (this->state_ == STATE_COMMAND_COMPLETE) {
    if (this->read_pos_ > 0) {
      ESP_LOGE(TAG, "Command not successful");
    } else {
      ESP_LOGI(TAG, "Command successful");
    }
    this->command_queue_[this->command_queue_position_] = std::string("");
    this->command_queue_position_ = (command_queue_position_ + 1) % COMMAND_QUEUE_LENGTH;
    this->state_ = STATE_IDLE;
  }

  if (this->state_ == STATE_POLL_DECODED) {
    std::string mode;
    switch (this->used_polling_commands_[this->last_polling_command_].identifier) {
      case POLLING_Y:
        this->publish_state_(this->smart_mode_, value_smart_mode_);
        break;
      case POLLING_B:
        this->publish_state_(this->battery_voltage_, value_battery_voltage_);
        break;
      case POLLING_C:
        this->publish_state_(this->internal_temperature_, value_internal_temperature_);
        break;
      case POLLING_F:
        this->publish_state_(this->grid_frequency_, value_grid_frequency_);
        break;
      case POLLING_G:
        this->publish_state_(this->cause_of_last_transfer_, value_cause_of_last_transfer_);
        break;
      case POLLING_L:
        this->publish_state_(this->grid_voltage_, value_grid_voltage_);
        break;
      case POLLING_O:
        this->publish_state_(this->ac_output_voltage_, value_ac_output_voltage_);
        break;
      case POLLING_P:
        this->publish_state_(this->ac_output_load_, value_ac_output_load_);
        break;
      case POLLING_Q:
        this->publish_state_(this->status_bitmask_, value_status_bitmask_);
        this->publish_state_(this->runtime_calibration_, check_bit_(value_status_bitmask_, 1));
        this->publish_state_(this->smart_trim_, check_bit_(value_status_bitmask_, 2));
        this->publish_state_(this->smart_boost_, check_bit_(value_status_bitmask_, 4));
        this->publish_state_(this->on_line_, check_bit_(value_status_bitmask_, 8));
        this->publish_state_(this->on_battery_, check_bit_(value_status_bitmask_, 16));
        this->publish_state_(this->output_overloaded_, check_bit_(value_status_bitmask_, 32));
        this->publish_state_(this->battery_low_, check_bit_(value_status_bitmask_, 64));
        this->publish_state_(this->replace_battery_, check_bit_(value_status_bitmask_, 128));
        break;
      case POLLING_LOWER_A:
        this->publish_state_(this->protocol_info_, value_protocol_info_);
        break;
      case POLLING_LOWER_B:
        this->publish_state_(this->firmware_revision_, value_firmware_revision_);
        break;
      case POLLING_LOWER_F:
        this->publish_state_(this->state_of_charge_, value_state_of_charge_);
        break;
      case POLLING_LOWER_J:
        this->publish_state_(this->estimated_runtime_, value_estimated_runtime_);
        break;
      case POLLING_LOWER_T:
        this->publish_state_(this->ambient_temperature_, value_ambient_temperature_);
        break;
      default:
        ESP_LOGD(TAG, "Response not implemented");
        break;
    }
    this->state_ = STATE_IDLE;
  }

  if (this->state_ == STATE_POLL_CHECKED) {
    std::string fc;
    char tmp[APC_UPS_READ_BUFFER_LENGTH];
    sprintf(tmp, "%s", this->read_buffer_);
    switch (this->used_polling_commands_[this->last_polling_command_].identifier) {
      case POLLING_Y:
        ESP_LOGD(TAG, "Decode Y");
        this->value_smart_mode_ = (this->read_buffer_[0] == 'S' && this->read_buffer_[1] == 'M');
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_B:
        ESP_LOGD(TAG, "Decode B");
        // "13.61\r\n"
        sscanf(tmp, "%f", &value_battery_voltage_);  // NOLINT
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_C:
        ESP_LOGD(TAG, "Decode C");
        // "36\r\n"
        sscanf(tmp, "%f", &value_internal_temperature_);  // NOLINT
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_F:
        ESP_LOGD(TAG, "Decode F");
        // "50.00\r\n"
        sscanf(tmp, "%f", &value_grid_frequency_);  // NOLINT
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_G:
        ESP_LOGD(TAG, "Decode G");
        // "G\r\n"
        this->value_cause_of_last_transfer_ = this->read_buffer_[0];
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_L:
        ESP_LOGD(TAG, "Decode L");
        // "231.8\r\n"
        sscanf(tmp, "%f", &value_grid_voltage_);  // NOLINT
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_O:
        ESP_LOGD(TAG, "Decode O");
        // "231.8\r\n"
        sscanf(tmp, "%f", &value_ac_output_voltage_);  // NOLINT
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_P:
        ESP_LOGD(TAG, "Decode P");
        // "009.1\r\n"
        sscanf(tmp, "%f", &value_ac_output_load_);  // NOLINT
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_Q:
        ESP_LOGD(TAG, "Decode Q");
        // "08\r\n"
        sscanf(tmp, "%x", &value_status_bitmask_);  // NOLINT
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_LOWER_A:
        ESP_LOGD(TAG, "Decode a");
        // "3.!$%+?=#|.\x01\x0E\x1A\')+-89@ABDEFGKLMNOPQRSUVWXYZabcdefgjklmnopqrsuxyz~\r\n"
        this->value_protocol_info_ = tmp;
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_LOWER_B:
        ESP_LOGD(TAG, "Decode b");
        // "21.3.I\r\n"
        this->value_firmware_revision_ = tmp;
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_LOWER_F:
        ESP_LOGD(TAG, "Decode f");
        // "100.0\r\n"
        sscanf(tmp, "%f", &value_state_of_charge_);  // NOLINT
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_LOWER_J:
        ESP_LOGD(TAG, "Decode j");
        // "0042:\r\n"
        sscanf(tmp, "%f:", &value_estimated_runtime_);  // NOLINT
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_LOWER_T:
        ESP_LOGD(TAG, "Decode t");
        // "80.5\r\n"
        sscanf(tmp, "%f", &value_ambient_temperature_);  // NOLINT
        this->state_ = STATE_POLL_DECODED;
        break;
      default:
        this->state_ = STATE_IDLE;
        break;
    }
    return;
  }

  if (this->state_ == STATE_POLL_COMPLETE) {
    if (this->read_buffer_[0] == 'N' && this->read_buffer_[1] == 'A') {
      this->state_ = STATE_IDLE;
      return;
    }
    this->state_ = STATE_POLL_CHECKED;
    return;
  }

  if (this->state_ == STATE_COMMAND || this->state_ == STATE_POLL) {
    while (this->available()) {
      uint8_t byte;
      this->read_byte(&byte);

      if (this->read_pos_ == APC_UPS_READ_BUFFER_LENGTH) {
        this->read_pos_ = 0;
        this->empty_uart_buffer_();
      }
      this->read_buffer_[this->read_pos_] = byte;
      this->read_pos_++;

      // end of answer
      if (byte == 0x0D) {
        this->read_buffer_[this->read_pos_] = 0;
        this->empty_uart_buffer_();
        if (this->state_ == STATE_POLL) {
          this->state_ = STATE_POLL_COMPLETE;
        }
        if (this->state_ == STATE_COMMAND) {
          this->state_ = STATE_COMMAND_COMPLETE;
        }
      }
    }  // available
  }
  if (this->state_ == STATE_COMMAND) {
    // Wait for a negative response. On success the UPS doesn't respond
    if (millis() - this->command_start_millis_ > esphome::apc_ups::ApcUps::COMMAND_TIMEOUT) {
      this->command_start_millis_ = millis();
      this->state_ = STATE_COMMAND_COMPLETE;
      return;
    }
  }
  if (this->state_ == STATE_POLL) {
    if (millis() - this->command_start_millis_ > esphome::apc_ups::ApcUps::COMMAND_TIMEOUT) {
      // command timeout
      ESP_LOGD(TAG, "timeout command to poll: %s", this->used_polling_commands_[this->last_polling_command_].command);
      this->state_ = STATE_IDLE;
    }
  }
}

uint8_t ApcUps::check_incoming_length_(uint8_t length) {
  if (this->read_pos_ - 3 == length) {
    return 1;
  }
  return 0;
}

// send next command used
uint8_t ApcUps::send_next_command_() {
  if (this->command_queue_[this->command_queue_position_].length() != 0) {
    const char *command = this->command_queue_[this->command_queue_position_].c_str();
    uint8_t byte_command[16];
    uint8_t length = this->command_queue_[this->command_queue_position_].length();
    for (uint8_t i = 0; i < length; i++) {
      byte_command[i] = (uint8_t) this->command_queue_[this->command_queue_position_].at(i);
    }
    this->state_ = STATE_COMMAND;
    this->command_start_millis_ = millis();
    this->empty_uart_buffer_();
    this->read_pos_ = 0;
    this->write_str(command);
    // end Byte
    ESP_LOGD(TAG, "Sending command from queue: %s with length %d", command, length);
    return 1;
  }
  return 0;
}

void ApcUps::send_next_poll_() {
  this->last_polling_command_ = (this->last_polling_command_ + 1) % 15;
  if (this->used_polling_commands_[this->last_polling_command_].length == 0) {
    this->last_polling_command_ = 0;
  }
  if (this->used_polling_commands_[this->last_polling_command_].length == 0) {
    // no command specified
    return;
  }
  this->state_ = STATE_POLL;
  this->command_start_millis_ = millis();
  this->empty_uart_buffer_();
  this->read_pos_ = 0;
  this->write_array(this->used_polling_commands_[this->last_polling_command_].command,
                    this->used_polling_commands_[this->last_polling_command_].length);
  // end Byte
  ESP_LOGD(TAG, "Sending polling command : %s with length %d",
           this->used_polling_commands_[this->last_polling_command_].command,
           this->used_polling_commands_[this->last_polling_command_].length);
}

void ApcUps::queue_command_(const char *command, uint8_t length) {
  uint8_t next_position = command_queue_position_;
  for (uint8_t i = 0; i < COMMAND_QUEUE_LENGTH; i++) {
    uint8_t testposition = (next_position + i) % COMMAND_QUEUE_LENGTH;
    if (command_queue_[testposition].length() == 0) {
      command_queue_[testposition] = command;
      ESP_LOGD(TAG, "Command queued successfully: %s with length %u at position %d", command,
               command_queue_[testposition].length(), testposition);
      return;
    }
  }
  ESP_LOGD(TAG, "Command queue full dropping command: %s", command);
}

void ApcUps::switch_command(const std::string &command) {
  ESP_LOGD(TAG, "got command: %s", command.c_str());
  queue_command_(command.c_str(), command.length());
}
void ApcUps::dump_config() {
  ESP_LOGCONFIG(TAG, "ApcUps:");
  ESP_LOGCONFIG(TAG, "used commands:");
  for (auto &used_polling_command : this->used_polling_commands_) {
    if (used_polling_command.length != 0) {
      ESP_LOGCONFIG(TAG, "%s", used_polling_command.command);
    }
  }
}
void ApcUps::update() {}

void ApcUps::add_polling_command_(const char *command, ENUMPollingCommand polling_command) {
  for (auto &used_polling_command : this->used_polling_commands_) {
    if (used_polling_command.length == strlen(command)) {
      uint8_t len = strlen(command);
      if (memcmp(used_polling_command.command, command, len) == 0) {
        return;
      }
    }
    if (used_polling_command.length == 0) {
      size_t length = strlen(command) + 1;
      const char *beg = command;
      const char *end = command + length;
      used_polling_command.command = new uint8_t[length];  // NOLINT(cppcoreguidelines-owning-memory)
      size_t i = 0;
      for (; beg != end; ++beg, ++i) {
        used_polling_command.command[i] = (uint8_t) (*beg);
      }
      used_polling_command.errors = 0;
      used_polling_command.identifier = polling_command;
      used_polling_command.length = length - 1;
      return;
    }
  }
}

void ApcUps::publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state) {
  if (binary_sensor == nullptr)
    return;

  binary_sensor->publish_state(state);
}

void ApcUps::publish_state_(sensor::Sensor *sensor, float value) {
  if (sensor == nullptr)
    return;

  sensor->publish_state(value);
}

void ApcUps::publish_state_(switch_::Switch *obj, const bool &state) {
  if (obj == nullptr)
    return;

  obj->publish_state(state);
}

void ApcUps::publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state) {
  if (text_sensor == nullptr)
    return;

  text_sensor->publish_state(state);
}

}  // namespace apc_ups
}  // namespace esphome
