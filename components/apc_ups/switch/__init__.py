import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import CONF_BEEPER, ICON_POWER

from .. import APC_UPS_COMPONENT_SCHEMA, CONF_APC_UPS_ID, apc_ups_ns

DEPENDENCIES = ["uart"]

CONF_FRONT_PANEL_TEST = "front_panel_test"
CONF_SELF_TEST = "self_test"
CONF_START_RUNTIME_CALIBRATION = "start_runtime_calibration"
CONF_SIMULATE_POWER_FAILURE = "simulate_power_failure"
CONF_SAVE_LAST_BATTERY_CHANGE_DATE = "save_last_battery_change_date"
CONF_SHUTDOWN_WITH_GRACE_PERIOD = "shutdown_with_grace_period"
CONF_SOFT_SHUTDOWN = "soft_shutdown"
CONF_SHUTDOWN_IMMEDIATELY = "shutdown_immediately"
CONF_TURN_ON = "turn_on"

TYPES = {
    CONF_FRONT_PANEL_TEST: ("A", None),
    CONF_SELF_TEST: ("W", None),
    CONF_START_RUNTIME_CALIBRATION: ("D", "D"),
    CONF_SIMULATE_POWER_FAILURE: ("U", None),
    CONF_SAVE_LAST_BATTERY_CHANGE_DATE: ("SDATE", None),
    CONF_SHUTDOWN_WITH_GRACE_PERIOD: ("K", None),
    CONF_SOFT_SHUTDOWN: ("S", None),
    CONF_SHUTDOWN_IMMEDIATELY: ("Z", None),
    CONF_TURN_ON: ("CTRL_N", None),
}

ApcUpsSwitch = apc_ups_ns.class_("ApcUpsSwitch", switch.Switch, cg.Component)

PIPSWITCH_SCHEMA = switch.switch_schema(
    ApcUpsSwitch, icon=ICON_POWER, block_inverted=True
).extend(cv.COMPONENT_SCHEMA)

CONFIG_SCHEMA = APC_UPS_COMPONENT_SCHEMA.extend(
    {cv.Optional(type): PIPSWITCH_SCHEMA for type in TYPES}
)


async def to_code(config):
    paren = await cg.get_variable(config[CONF_APC_UPS_ID])

    for type, (on, off) in TYPES.items():
        if type in config:
            conf = config[type]
            var = await switch.new_switch(conf)
            await cg.register_component(var, conf)
            cg.add(getattr(paren, f"set_{type}")(var))
            cg.add(var.set_parent(paren))
            cg.add(var.set_on_command(on))
            if off is not None:
                cg.add(var.set_off_command(off))
