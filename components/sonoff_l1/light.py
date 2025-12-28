import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import light
from esphome.const import CONF_ID

sonoff_l1_ns = cg.esphome_ns.namespace("sonoff_l1")
SonoffL1Light = sonoff_l1_ns.class_("SonoffL1Light", cg.Component, light.LightOutput)

CONFIG_SCHEMA = light.RGB_LIGHT_SCHEMA.extend(
    {
        cv.GenerateID(CONF_ID): cv.declare_id(SonoffL1Light),
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await light.register_light(var, config)
