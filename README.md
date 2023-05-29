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

## Untested devices

* TBD.

## Requirements

* [ESPHome 2023.3.0 or higher](https://github.com/esphome/esphome/releases).
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
TBD.
```

## Protocol

See [https://github.com/kirbah/apc-ups](github.com/kirbah/apc-ups).

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
