import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import CONF_BEEPER, ENTITY_CATEGORY_CONFIG, ICON_POWER

from .. import APC_UPS_COMPONENT_SCHEMA, CONF_APC_UPS_ID, apc_ups_ns

DEPENDENCIES = ["uart"]

# CONF_BEEPER = "beeper"
CONF_QUICK_TEST = "quick_test"
CONF_DEEP_TEST = "deep_test"
CONF_TEN_MINUTES_TEST = "ten_minutes_test"

CONF_FRONT_PANEL_TEST = "front_panel_test"
CONF_SELF_TEST = "self_test"
CONF_START_RUNTIME_CALIBRATION = "start_runtime_calibration"
CONF_SIMULATE_POWER_FAILURE = "simulate_power_failure"

TYPES = {
    CONF_BEEPER: ("Q", "Q"),
    CONF_QUICK_TEST: ("T", "CT"),
    CONF_DEEP_TEST: ("TL", "CT"),
    CONF_TEN_MINUTES_TEST: ("T10", "CT"),
    CONF_FRONT_PANEL_TEST: ("A", None),
    CONF_SELF_TEST: ("W", None),
    CONF_START_RUNTIME_CALIBRATION: ("D", "D"),
    CONF_SIMULATE_POWER_FAILURE: ("U", None),
}

ApcUpsSwitch = apc_ups_ns.class_("ApcUpsSwitch", switch.Switch, cg.Component)

APC_UPS_SWITCH_SCHEMA = switch.switch_schema(
    ApcUpsSwitch, icon=ICON_POWER, block_inverted=True
).extend(cv.COMPONENT_SCHEMA)

APC_UPS_CONFIG_SWITCH_SCHEMA = switch.switch_schema(
    ApcUpsSwitch,
    icon=ICON_POWER,
    block_inverted=True,
    entity_category=ENTITY_CATEGORY_CONFIG,
).extend(cv.COMPONENT_SCHEMA)

CONFIG_SCHEMA = APC_UPS_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_BEEPER): APC_UPS_CONFIG_SWITCH_SCHEMA,
        cv.Optional(CONF_QUICK_TEST): APC_UPS_SWITCH_SCHEMA,
        cv.Optional(CONF_DEEP_TEST): APC_UPS_SWITCH_SCHEMA,
        cv.Optional(CONF_TEN_MINUTES_TEST): APC_UPS_SWITCH_SCHEMA,
        cv.Optional(CONF_FRONT_PANEL_TEST): APC_UPS_SWITCH_SCHEMA,
        cv.Optional(CONF_SELF_TEST): APC_UPS_SWITCH_SCHEMA,
        cv.Optional(CONF_START_RUNTIME_CALIBRATION): APC_UPS_SWITCH_SCHEMA,
        cv.Optional(CONF_SIMULATE_POWER_FAILURE): APC_UPS_SWITCH_SCHEMA,
    }
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
