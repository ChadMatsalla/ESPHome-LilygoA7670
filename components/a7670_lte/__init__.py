import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import sensor
# ort EsphomeCore as core
# import esphome.core.ID
# import esphome.core as core
from esphome.const import (
    CONF_ID,
    CONF_SENSOR,
)
from esphome.core import CORE, coroutine_with_priority
from esphome.components.network import IPAddress

DEPENDENCIES = []
CODEOWNERS = ["@chadmatsalla"]
AUTO_LOAD = ["network"]
CONFLICTS_WITH = ["wifi", "captive_portal", "ethernet"]

MULTI_CONF = True

a7670_lte_ns = cg.esphome_ns.namespace("a7670_lte")
A7670LTEComponent = a7670_lte_ns.class_("A7670LTEComponent", cg.Component)

CONFIG_SCHEMA = cv.Schema({
  cv.GenerateID(): cv.declare_id(A7670LTEComponent),
  cv.Optional('use_external_mqtt'): cv.boolean,
  cv.Optional('mqtt_server'): cv.string,
  cv.Optional('mqtt_user'): cv.string,
  cv.Optional('mqtt_pass'): cv.string,
  cv.Required('deep_sleep_id'): cv.string,
  cv.Required('modem_warmup_delay'): cv.int_range(0,500),
  cv.Required('use_cellular'): cv.int_range(0,1),
  cv.Required('apn'): cv.string,
    # this should be optional while testing, because I have no thermometers attached
  cv.Optional('hive_internal_temperature_sensor_id'): cv.use_id(sensor.Sensor),
  cv.Optional('hive_external_temperature_sensor_id'): cv.use_id(sensor.Sensor),
  cv.Optional('smlm', default=10): cv.int_,
}).extend(cv.COMPONENT_SCHEMA)

# print("config schema is " + str(CONFIG_SCHEMA))

async def to_code(config):
  var = cg.new_Pvariable(config[CONF_ID])
  await cg.register_component(var, config)
  # receiver = await cg.get_variable(config['hive_internal_temperature_sensor_id'])
  # print("the type of this thing is " + str(type(config['hive_internal_temperature_sensor_id'])))
  if 1 == 2:
    internal_temperature_sensor_id = await cg.get_variable(config["hive_internal_temperature_sensor_id"])
    cg.add(var.set_internal_temperature_sensor(internal_temperature_sensor_id))
    external_temperature_sensor_id = await cg.get_variable(config["hive_external_temperature_sensor_id"])
    cg.add(var.set_external_temperature_sensor(internal_temperature_sensor_id))
  # print("foo is " + str(type(foo)))
  cg.add(var.set_use_cellular(config["use_cellular"]))
  if (isi := config.get("hive_internal_temperature_sensor_id")) is not None:
    # internal_temperature_sensor_id = await cg.get_variable(config["hive_internal_temperature_sensor_id"])
    cg.add(var.set_internal_temperature_sensor(isi))
  if (esi := config.get("hive_external_temperature_sensor_id")) is not None:
    # external_temperature_sensor_id = await cg.get_variable(config["hive_external_temperature_sensor_id"])
    cg.add(var.set_external_temperature_sensor(esi))
  if (config["use_external_mqtt"]):
    if (config["use_external_mqtt"] == "true"):
      print("Setting mqtt user to " + str(config["mqtt_user"]))
      cg.add(var.set_mqtt_username(config["mqtt_user"]))
      dfksjfkds
      print("Setting mqtt server to " + str(config["mqtt_server"]))
      cg.add(var.set_mqtt_server(config["mqtt_server"]))
      cg.add(var.set_mqtt_password(config["mqtt_pass"]))
  cg.add(var.set_apn(config["apn"]))
  if (1==2):
    gint = await cg.get_variable(config["global_temperature_internal"])
    cg.add(var.set_global("temperature_internal",gint));
    gext = await cg.get_variable(config["global_temperature_external"])
    cg.add(var.set_global("temperature_external",gext));
  
if CORE.using_arduino:
  cg.add_library("WiFi", None)
