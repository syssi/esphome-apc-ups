# esphome-apc-ups

![GitHub actions](https://github.com/syssi/esphome-apc-ups/actions/workflows/ci.yaml/badge.svg)
![GitHub stars](https://img.shields.io/github/stars/syssi/esphome-apc-ups)
![GitHub forks](https://img.shields.io/github/forks/syssi/esphome-apc-ups)
![GitHub watchers](https://img.shields.io/github/watchers/syssi/esphome-apc-ups)
[!["Buy Me A Coffee"](https://img.shields.io/badge/buy%20me%20a%20coffee-donate-yellow.svg)](https://www.buymeacoffee.com/syssi)

ESPHome component to monitor and control a APC UPS via RS232

## Supported devices

* APC SU420INET (firmware `21.3.I`)
* APC SUVS420I (firmware `42.L.I`)
* APC SUA1000I (firmware `652.13.I`)
* APC SUA1500MRI2U
* APC SUA2200I

## Requirements

* [ESPHome 2024.6.0 or higher](https://github.com/esphome/esphome/releases).
* Generic ESP32/ESP8266 board

## Schematics

```
                 RS232                     UART-TTL
┌───────────┐              ┌──────────┐                ┌─────────┐
│           │              │          │<----- RX ----->│         │
│           │<---- TX ---->│  RS232   │<----- TX ----->│ ESP32/  │
│  APC UPS  │<---- RX ---->│  to TTL  │<----- GND ---->│ ESP8266 │
│           │<---- GND --->│  module  │<-- 3.3V VCC -->│         │<--- VCC
│           │              │          │                │         │<--- GND
└───────────┘              └──────────┘                └─────────┘
```

### D-SUB 9P connector

| Pin | Purpose      | MAX3232 pin       |
|:---:| :----------- | :---------------- |
|  1  | RX           | P14 (DOUT1)       |
|  2  | TX           | P13 (RIN1)        |
|  3  |              |                   |
|  4  |              |                   |
|  5  |              |                   |
|  6  |              |                   |
|  7  |              |                   |
|  8  |              |                   |
|  9  | GND          | P15 (GND)         |

### MAX3232

| Pin          | Label        | ESPHome     | ESP8266 example  | ESP32 example |
| :----------- | :----------- | :---------- | :--------------- | :------------ |
| P11 (DIN1)   | TXD          | `tx_pin`    | `GPIO4`          | `GPIO16`      |
| P12 (ROUT1)  | RXD          | `rx_pin`    | `GPIO5`          | `GPIO17`      |
| P16 (VCC)    | VCC          |             |                  |               |
| P15 (GND)    | GND          |             |                  |               |

## Installation

You can install this component with [ESPHome external components feature](https://esphome.io/components/external_components.html) like this:
```yaml
external_components:
  - source: github://syssi/esphome-apc-ups@main
```

or just use the `esp32-example.yaml` as proof of concept:

```bash
# Install esphome
pip3 install esphome

# Clone this external component
git clone https://github.com/syssi/esphome-apc-ups.git
cd esphome-apc-ups

# Create a secrets.yaml containing some setup specific secrets
cat > secrets.yaml <<EOF
wifi_ssid: MY_WIFI_SSID
wifi_password: MY_WIFI_PASSWORD

mqtt_host: MY_MQTT_HOST
mqtt_username: MY_MQTT_USERNAME
mqtt_password: MY_MQTT_PASSWORD
EOF

# Validate the configuration, create a binary, upload it, and start logs
# If you use a esp8266 run the esp8266-examle.yaml
esphome run esp32-example.yaml

```

## Example response all sensors enabled

```
[apc_ups:286]: Sending polling command : Y with length 1
[apc_ups:112]: Decode Y
[apc_ups:286]: Sending polling command : Q with length 1
[apc_ups:153]: Decode Q
[sensor:094]: 'apc-ups status bitmask': Sending state 8.00000  with 0 decimals of accuracy
[binary_sensor:036]: 'apc-ups runtime calibration': Sending state OFF
[binary_sensor:036]: 'apc-ups smart trim': Sending state OFF
[binary_sensor:036]: 'apc-ups smart boost': Sending state OFF
[binary_sensor:036]: 'apc-ups on line': Sending state ON
[binary_sensor:036]: 'apc-ups on battery': Sending state OFF
[binary_sensor:036]: 'apc-ups output overloaded': Sending state OFF
[binary_sensor:036]: 'apc-ups battery low': Sending state OFF
[binary_sensor:036]: 'apc-ups replace battery': Sending state OFF
[apc_ups:286]: Sending polling command : B with length 1
[apc_ups:117]: Decode B
[sensor:094]: 'apc-ups battery voltage': Sending state 13.61000 V with 2 decimals of accuracy
[apc_ups:286]: Sending polling command : F with length 1
[apc_ups:123]: Decode F
[sensor:094]: 'apc-ups grid frequency': Sending state 50.00000 Hz with 2 decimals of accuracy
[apc_ups:286]: Sending polling command : L with length 1
[apc_ups:135]: Decode L
[sensor:094]: 'apc-ups grid voltage': Sending state 231.80000 V with 1 decimals of accuracy
[apc_ups:286]: Sending polling command : O with length 1
[apc_ups:141]: Decode O
[sensor:094]: 'apc-ups ac output voltage': Sending state 231.80000 V with 1 decimals of accuracy
[apc_ups:286]: Sending polling command : P with length 1
[apc_ups:147]: Decode P
[sensor:094]: 'apc-ups ac output load': Sending state 9.10000 % with 1 decimals of accuracy
[apc_ups:286]: Sending polling command : f with length 1
[apc_ups:171]: Decode f
[sensor:094]: 'apc-ups state of charge': Sending state 100.00000 % with 1 decimals of accuracy
[apc_ups:286]: Sending polling command : j with length 1
[apc_ups:177]: Decode j
[sensor:094]: 'apc-ups estimated runtime': Sending state 42.00000 min with 0 decimals of accuracy
[apc_ups:286]: Sending polling command : G with length 1
[apc_ups:129]: Decode G
[text_sensor:064]: 'apc-ups cause of last transfer': Sending state 'S'
```

## Protocol

See [kirbah.github.io/apc-ups/](https://kirbah.github.io/apc-ups/).

## Known issues

None.

## Debugging

If this component doesn't work out of the box for your device please update your configuration to increase the log level to see details about the UART traffic:

```
logger:
  level: DEBUG

uart:
  - id: uart_0
    baud_rate: 2400
    tx_pin: ${tx_pin}
    rx_pin: ${rx_pin}
    debug:
      direction: BOTH
      dummy_receiver: false
      after:
        delimiter: "\r"
      sequence:
        - lambda: UARTDebug::log_string(direction, bytes);
```

## References

* https://github.com/kirbah/apc-ups
* http://www.apcupsd.org/manual/#apc-smart-protocol
* https://github.com/ssieb/custom_components/tree/master/components/apcups
