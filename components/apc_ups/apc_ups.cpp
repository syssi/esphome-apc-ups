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
    // 0x21 symbol !
    if (byte == 0x21) {
      ESP_LOGI(TAG, "Power failure");
      queue_command_("Q", 1);
    }
    // 0x24 symbol $
    if (byte == 0x24) {
      ESP_LOGI(TAG, "Power has been restored");
      queue_command_("Q", 1);
    }
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
      ESP_LOGI(TAG, "Command successful");
    } else {
      ESP_LOGE(TAG, "Command not successful");
    }

    switch (this->command_queue_[this->command_queue_position_][0]) {
      case 0x41:
        ESP_LOGD(TAG, "Decode A");
        // "OK\r\n"
        this->value_front_panel_test_ = (this->read_buffer_[0] == 'O' && this->read_buffer_[1] == 'K');
        this->publish_state_(this->front_panel_test_, !value_front_panel_test_);
        break;
      case 0x44:
        ESP_LOGD(TAG, "Decode D");
        // "OK\r\n!"
        this->value_start_runtime_calibration_ = (this->read_buffer_[0] == 'O' && this->read_buffer_[1] == 'K');
        this->publish_state_(this->start_runtime_calibration_, !value_start_runtime_calibration_);
        break;
      case 0x55:
        ESP_LOGD(TAG, "Decode U");
        // "OK\r\n!"
        this->value_simulate_power_failure_ = (this->read_buffer_[0] == 'O' && this->read_buffer_[1] == 'K');
        this->publish_state_(this->simulate_power_failure_, !value_simulate_power_failure_);
        break;
      case 0x57:
        ESP_LOGD(TAG, "Decode W");
        // "OK\r\n!"
        this->value_self_test_ = (this->read_buffer_[0] == 'O' && this->read_buffer_[1] == 'K');
        this->publish_state_(this->self_test_, !value_self_test_);
        break;
    }
    this->command_queue_[this->command_queue_position_] = std::string("");
    this->command_queue_position_ = (command_queue_position_ + 1) % COMMAND_QUEUE_LENGTH;
    this->state_ = STATE_IDLE;
  }

  if (this->state_ == STATE_POLL_DECODED) {
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
      case POLLING_M:
        this->publish_state_(this->max_grid_voltage_, value_max_grid_voltage_);
        break;
      case POLLING_N:
        this->publish_state_(this->min_grid_voltage_, value_min_grid_voltage_);
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
      case POLLING_X:
        this->publish_state_(this->self_test_results_, value_self_test_results_);
        break;
      case POLLING_V:
        this->publish_state_(this->old_firmware_version_, value_old_firmware_version_);
        break;
      case POLLING_LOWER_A:
        this->publish_state_(this->protocol_info_, value_protocol_info_);
        break;
      case POLLING_LOWER_B:
        this->publish_state_(this->firmware_revision_, value_firmware_revision_);
        break;
      case POLLING_LOWER_C:
        this->publish_state_(this->local_identifier_, value_local_identifier_);
        break;
      case POLLING_LOWER_E:
        this->publish_state_(this->return_threshold_, value_return_threshold_);
        break;
      case POLLING_LOWER_F:
        this->publish_state_(this->state_of_charge_, value_state_of_charge_);
        break;
      case POLLING_LOWER_G:
        this->publish_state_(this->nominal_battery_voltage_, value_nominal_battery_voltage_);
        break;
      case POLLING_LOWER_H:
        this->publish_state_(this->ambient_humidity_, value_ambient_humidity_);
        break;
      case POLLING_LOWER_J:
        this->publish_state_(this->estimated_runtime_, value_estimated_runtime_);
        break;
      case POLLING_LOWER_K:
        this->publish_state_(this->alarm_delay_, value_alarm_delay_);
        break;
      case POLLING_LOWER_L:
        this->publish_state_(this->low_transfer_voltage_, value_low_transfer_voltage_);
        break;
      case POLLING_LOWER_M:
        this->publish_state_(this->manufacture_date_, value_manufacture_date_);
        break;
      case POLLING_LOWER_N:
        this->publish_state_(this->serial_number_, value_serial_number_);
        break;
      case POLLING_LOWER_O:
        this->publish_state_(this->nominal_output_voltage_, value_nominal_output_voltage_);
        break;
      case POLLING_LOWER_T:
        this->publish_state_(this->ambient_temperature_, value_ambient_temperature_);
        break;
      case POLLING_LOWER_U:
        this->publish_state_(this->upper_transfer_voltage_, value_upper_transfer_voltage_);
        break;
      case POLLING_LOWER_V:
        this->publish_state_(this->measure_upc_firmware_, value_measure_upc_firmware_);
        break;
      case POLLING_LOWER_X:
        this->publish_state_(this->last_battery_change_date_, value_last_battery_change_date_);
        break;
      case POLLING_LOWER_Y:
        this->publish_state_(this->alarm_delay_, value_alarm_delay_);
        break;
      case POLLING_9:
        this->publish_state_(this->line_quality_, value_line_quality_);
        break;
      case POLLING_CTRL_A:
        this->publish_state_(this->model_name_, value_model_name_);
        break;
      default:
        ESP_LOGD(TAG, "Response not implemented");
        break;
    }
    this->state_ = STATE_IDLE;
  }

  if (this->state_ == STATE_POLL_CHECKED) {
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
      case POLLING_M:
        ESP_LOGD(TAG, "Decode M");
        // "231.8\r\n"
        sscanf(tmp, "%f", &value_max_grid_voltage_);  // NOLINT
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_N:
        ESP_LOGD(TAG, "Decode N");
        // "231.8\r\n"
        sscanf(tmp, "%f", &value_min_grid_voltage_);  // NOLINT
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
      case POLLING_V:
        ESP_LOGD(TAG, "Decode V");
        // "FWI\r\n"
        this->value_old_firmware_version_ = tmp;
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_Q:
        ESP_LOGD(TAG, "Decode Q");
        // "08\r\n"
        sscanf(tmp, "%x", &value_status_bitmask_);  // NOLINT
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_X:
        ESP_LOGD(TAG, "Decode X");
        // OK
        this->value_self_test_results_ = tmp;
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
      case POLLING_LOWER_C:
        ESP_LOGD(TAG, "Decode c");
        // "UPS_IDEN\r\n"
        this->value_local_identifier_ = tmp;
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_LOWER_E:
        ESP_LOGD(TAG, "Decode e");
        // "53.8\r\n"
        sscanf(tmp, "%f", &value_return_threshold_);  // NOLINT
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_LOWER_F:
        ESP_LOGD(TAG, "Decode f");
        // "100.0\r\n"
        sscanf(tmp, "%f", &value_state_of_charge_);  // NOLINT
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_LOWER_G:
        ESP_LOGD(TAG, "Decode g");
        // "53.8\r\n"
        sscanf(tmp, "%f", &value_nominal_battery_voltage_);  // NOLINT
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_LOWER_H:
        ESP_LOGD(TAG, "Decode h");
        // "100.0\r\n"
        sscanf(tmp, "%f", &value_ambient_humidity_);  // NOLINT
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_LOWER_J:
        ESP_LOGD(TAG, "Decode j");
        // "0042:\r\n"
        sscanf(tmp, "%f:", &value_estimated_runtime_);  // NOLINT
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_LOWER_K:
        ESP_LOGD(TAG, "Decode k");
        // alarm
        this->value_alarm_delay_ = tmp;
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_LOWER_L:
        ESP_LOGD(TAG, "Decode l");
        // "80.5\r\n"
        sscanf(tmp, "%f", &value_low_transfer_voltage_);  // NOLINT
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_LOWER_M:
        ESP_LOGD(TAG, "Decode m");
        // "11/29/96\r\n"
        this->value_manufacture_date_ = tmp;
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_LOWER_N:
        ESP_LOGD(TAG, "Decode n");
        // "11/29/96\r\n"
        this->value_serial_number_ = tmp;
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_LOWER_O:
        ESP_LOGD(TAG, "Decode o");
        // "80.5\r\n"
        sscanf(tmp, "%f", &value_nominal_output_voltage_);  // NOLINT
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_LOWER_T:
        ESP_LOGD(TAG, "Decode t");
        // "80.5\r\n"
        sscanf(tmp, "%f", &value_ambient_temperature_);  // NOLINT
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_LOWER_U:
        ESP_LOGD(TAG, "Decode u");
        // "80.5\r\n"
        sscanf(tmp, "%f", &value_upper_transfer_voltage_);  // NOLINT
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_LOWER_V:
        ESP_LOGD(TAG, "Decode v");
        // alarm
        this->value_alarm_delay_ = tmp;
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_LOWER_X:
        ESP_LOGD(TAG, "Decode x");
        // "11/29/96\r\n"
        this->value_last_battery_change_date_ = tmp;
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_LOWER_Y:
        ESP_LOGD(TAG, "Decode y");
        // copyright_notice
        this->value_copyright_notice_ = tmp;
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_9:
        ESP_LOGD(TAG, "Decode 9");
        // line_quality
        this->value_line_quality_ = tmp;
        this->state_ = STATE_POLL_DECODED;
        break;
      case POLLING_CTRL_A:
        ESP_LOGD(TAG, "Decode Ctrl+A");
        // "SMART-UPS 700\r\n"
        this->value_model_name_ = tmp;
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
  if (!this->command_queue_[this->command_queue_position_].empty()) {
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
  this->last_polling_command_ = (this->last_polling_command_ + 1) % 32;
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
      if (strcmp(command, "A") == 0 || strcmp(command, "D") == 0 || strcmp(command, "U") == 0 ||
          strcmp(command, "W") == 0) {
        return;
      };  // Exclusion from the command queue
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
