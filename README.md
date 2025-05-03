# esphome-apc-ups

ESPHome component to monitor and control a APC UPS via RS232 with MQTT

* Fixed switches. They're working now.
* When power is lost or restored, an external command (Q-status flag) is sent to update the status (on_line and on_battery)
* Added a record of the date of battery replacement
* Added switches: shutdown with grace period, soft shutdown, shutdown immediately, turn on

## Tested on the device

* APC SUA1000I (firmware `652.13.I`)

## Requirements

* [ESPHome 2024.5.0 or higher](https://github.com/esphome/esphome/releases).
* Generic ESP32/ESP8266 board

## Schematics

```
                 RS232                     UART-TTL
┌───────────┐             ┌──────────┐               ┌─────────┐
│           │<--- TX -----│  RS232   │<---- TX ------│         │
│           │---- RX ---->│  to TTL  │----- RX ----->│ ESP32/  │
│  APC UPS  │<--- GND --->│  module  │<---- GND ---->│ ESP8266 │
│           │             |          |<-- 3.3V VCC --│         │
│           │             └──────────┘               │         │
│           │             ┌──────────┐               │         │
│           │<--- GND --->│  DC-DC   │<---- GND ---->│         │
│           │---- 24V --->|  XL1509  │-- 3.3V VCC -->│         │
└───────────┘             └──────────┘               └─────────┘
```

### D-SUB 9P connector

| UPS pin            | RS232 to TTL pin | MAX3232 pin        |
| :----------------- | :--------------- | :----------------- |
| RX (pin 1)         | RX (pin 2)       | P14 (DOUT1)        |
| TX (pin 2)         | TX (pin 3)       | P13 (RIN1)         |
| GND (pin 4, 9)     | GND (pin 5)      | P15 (GND)          |
| 24V battery (pin8) | 24V (free pin 9) | connected to DC-DC |

### MAX3232

| Pin          | Label        | ESPHome     | ESP8266 | ESP32    | ESP-01   |
| :----------- | :----------- | :---------- | :------ | :------- | :---     |
| P11 (DIN1)   | TXD          | `tx_pin`    | `GPIO4` | `GPIO16` | `GPIO1`  |
| P12 (ROUT1)  | RXD          | `rx_pin`    | `GPIO5` | `GPIO17` | `GPIO3`  |
| P16 (VCC)    | VCC          |             |         |          |          |
| P15 (GND)    | GND          |             |         |          |          |

## Installation

You can install this component with [ESPHome external components feature](https://esphome.io/components/external_components.html) like this:
```yaml
external_components:
  - source: github://samoswall/esphome-apc-ups@main
```

## Configuration info

| Type | Symbol | Variable | Info
|-|-|-|-
| Binary_Sensor | Y | smart_mode | Enable smart mode
| Switch | A | front_panel_test | Front panel test
| Sensor | B | battery_voltage | Battery voltage
| Sensor | C | internal_temperature | Internal temperature
| Switch | D | start_runtime_calibration | Start runtime calibration
| Text_Sensor (writable) | E |  | Automatic self test interval @TODO
| Sensor | F | grid_frequency | Input line frequency
| Text_Sensor | G | cause_of_last_transfer | Cause of last transfer to battery
| Text_Sensor | I |  | Measure-UPS alarm enable @TODO (no decode)
| Text_Sensor | J |  | Measure-UPS alarm status @TODO (no decode)
| Switch | K | shutdown_with_grace_period | Shutdown with grace period
| Sensor | L | grid_voltage | Input line voltage
| Sensor | M | max_grid_voltage | Maximum line voltage
| Sensor | N | min_grid_voltage | Minimum line voltage
| Sensor | O | ac_output_voltage | Output voltage
| Sensor | P | ac_output_load | Power load in %
| Sensor | Q | status_bitmask | Status flags. (See table)
| Switch | R |  | Turn dumb @TODO
| Switch | S | soft_shutdown | Soft shutdown
| Switch | U | simulate_power_failure | Simulate power failure
| Text_Sensor | V | old_firmware_version | Old firmware version
| Switch | W | self_test | Self test
| Text_Sensor | X | self_test_results | Self test results
| Switch | Z | shutdown_immediately | Shutdown immediately
| Text_Sensor | a | protocol_info | Protocol info
| Text_Sensor | b | firmware_revision | Firmware revision
| Text_Sensor | c | local_identifier | UPS local identifier
| Sensor | e | return_threshold | Return threshold
| Sensor | f | state_of_charge | Battery level
| Sensor | g | nominal_battery_voltage | Nominal battery voltage
| Sensor | h | ambient_humidity | Measure-UPS ambient humidity
| Text_Sensor | i | | Measure-UPS dry contacts @TODO
| Sensor | j | estimated_runtime | Estimated runtime
| Text_Sensor | k | alarm_delay | Alarm delay
| Sensor | l | low_transfer_voltage | Low transfer voltage
| Text_Sensor | m | manufacture_date | Manufacture date
| Text_Sensor | n | serial_number | Serial number
| Sensor | o | nominal_output_voltage | Nominal Output Voltage
| Text_Sensor (writable) | p |  | Shutdown grace delay @TODO
| Text_Sensor (writable) | q |  | Low battery warning @TODO
| Text_Sensor (writable) | r |  | Wakeup delay @TODO
| Text_Sensor (writable) | s |  | Sensitivity @TODO
| Sensor | t | ambient_temperature | Measure-UPS ambient temperature
| Sensor | u | upper_transfer_voltage | Upper transfer voltage
| Text_Sensor | v | measure_upc_firmware | Measure-UPS firmware
| Text_Sensor | x | last_battery_change_date | Last battery change date
| Switch | x | save_last_battery_change_date | Save last battery change date
| Text_Sensor | y | copyright_notice | Copyright notice
| Switch | z |  | Reset to factory settings @TODO
| Text_Sensor | 9 | line_quality | Line quality @TODO
| Text_Sensor | 0x01 | model_name | Model name
| Switch | 0x0E | turn_on | Turn ON UPS

## Status flags

| Type | Bit | Variable
|-|-|-
| Binary_Sensor | 0 | runtime_calibration
| Binary_Sensor | 1 | smart_trim
| Binary_Sensor | 2 | smart_boost
| Binary_Sensor | 3 | on_line
| Binary_Sensor | 4 | on_battery
| Binary_Sensor | 5 | output_overloaded
| Binary_Sensor | 6 | battery_low
| Binary_Sensor | 7 | replace_battery

## Protocol

See [kirbah.github.io/apc-ups/](https://kirbah.github.io/apc-ups/).

## References

* https://github.com/kirbah/apc-ups
* http://www.apcupsd.org/manual/#apc-smart-protocol
* https://github.com/ssieb/custom_components/tree/master/components/apcups

## Forked from

[https://github.com/syssi/esphome-apc-ups](https://github.com/syssi/esphome-apc-ups)
