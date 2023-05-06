import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv

from .. import APC_UPS_COMPONENT_SCHEMA, CONF_APC_UPS_ID

DEPENDENCIES = ["uart"]

CONF_FAULTS_PRESENT = "faults_present"

CONF_SMART_MODE = "smart_mode"

CONF_UTILITY_FAIL = "utility_fail"
CONF_BATTERY_LOW = "battery_low"
CONF_BYPASS_ACTIVE = "bypass_active"
CONF_UPS_FAILED = "ups_failed"
CONF_UPS_TYPE_STANDBY = "ups_type_standby"
CONF_TEST_IN_PROGRESS = "test_in_progress"
CONF_SHUTDOWN_ACTIVE = "shutdown_active"
CONF_BEEPER_ON = "beeper_on"

TYPES = [
    CONF_SMART_MODE,
    CONF_UTILITY_FAIL,
    CONF_BATTERY_LOW,
    CONF_BYPASS_ACTIVE,
    CONF_UPS_FAILED,
    CONF_UPS_TYPE_STANDBY,
    CONF_TEST_IN_PROGRESS,
    CONF_SHUTDOWN_ACTIVE,
    CONF_BEEPER_ON,
]

CONFIG_SCHEMA = APC_UPS_COMPONENT_SCHEMA.extend(
    {cv.Optional(type): binary_sensor.binary_sensor_schema() for type in TYPES}
)


async def to_code(config):
    paren = await cg.get_variable(config[CONF_APC_UPS_ID])
    for type in TYPES:
        if type in config:
            conf = config[type]
            var = await binary_sensor.new_binary_sensor(conf)
            cg.add(getattr(paren, f"set_{type}")(var))
