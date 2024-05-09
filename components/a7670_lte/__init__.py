import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.const import (
    CONF_ID,
    CONF_TRIGGER_ID,
)
from esphome.components import uart

DEPENDENCIES = ["uart"]
CODEOWNERS = ["@chadmatsalla"]
MULTI_CONF = True

a7670_lte_ns = cg.esphome_ns.namespace("a7670_lte")
A7670LTEComponent = a7670_lte_ns.class_("A7670LTEComponent", cg.Component)

CONFIG_SCHEMA = cv.Schema({
  cv.GenerateID(): cv.declare_id(EmptyComponent),
  cv.Required('server'): cv.string,
  cv.Required('user'): cv.string,
  cv.Required('pass'): cv.string,
  cv.Optional('smlm', default=10): cv.int_,
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
  var = cg.new_Pvariable(config[CONF_ID])
  yield cg.register_component(var, config)
