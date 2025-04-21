import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.const import (
    CONF_ID,
    CONF_TRIGGER_ID,
)

DEPENDENCIES = []
CODEOWNERS = ["@chino-lu"]
MULTI_CONF = True

a7670_ns = cg.esphome_ns.namespace("a7670")
a7670Component = a7670_ns.class_("a7670Component", cg.Component)

a7670ReceivedMessageTrigger = a7670_ns.class_(
    "a7670ReceivedMessageTrigger",
    automation.Trigger.template(cg.std_string, cg.std_string),
)
a7670IncomingCallTrigger = a7670_ns.class_(
    "a7670IncomingCallTrigger",
    automation.Trigger.template(cg.std_string),
)
a7670CallConnectedTrigger = a7670_ns.class_(
    "a7670CallConnectedTrigger",
    automation.Trigger.template(),
)
a7670CallDisconnectedTrigger = a7670_ns.class_(
    "a7670CallDisconnectedTrigger",
    automation.Trigger.template(),
)

a7670ReceivedUssdTrigger = a7670_ns.class_(
    "a7670ReceivedUssdTrigger",
    automation.Trigger.template(cg.std_string),
)

# Actions
a7670SendSmsAction = a7670_ns.class_("a7670SendSmsAction", automation.Action)
a7670SendUssdAction = a7670_ns.class_("a7670SendUssdAction", automation.Action)
a7670DialAction = a7670_ns.class_("a7670DialAction", automation.Action)
a7670ConnectAction = a7670_ns.class_("a7670ConnectAction", automation.Action)
a7670DisconnectAction = a7670_ns.class_(
    "a7670DisconnectAction", automation.Action
)

CONF_a7670_ID = "a7670_id"
CONF_ON_SMS_RECEIVED = "on_sms_received"
CONF_ON_USSD_RECEIVED = "on_ussd_received"
CONF_ON_INCOMING_CALL = "on_incoming_call"
CONF_ON_CALL_CONNECTED = "on_call_connected"
CONF_ON_CALL_DISCONNECTED = "on_call_disconnected"
CONF_RECIPIENT = "recipient"
CONF_MESSAGE = "message"
CONF_USSD = "ussd"

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(a7670Component),
            cv.Optional(CONF_ON_SMS_RECEIVED): automation.validate_automation(
                {
                    cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(
                        a7670ReceivedMessageTrigger
                    ),
                }
            ),
            cv.Optional(CONF_ON_INCOMING_CALL): automation.validate_automation(
                {
                    cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(
                        a7670IncomingCallTrigger
                    ),
                }
            ),
            cv.Optional(CONF_ON_CALL_CONNECTED): automation.validate_automation(
                {
                    cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(
                        a7670CallConnectedTrigger
                    ),
                }
            ),
            cv.Optional(CONF_ON_CALL_DISCONNECTED): automation.validate_automation(
                {
                    cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(
                        a7670CallDisconnectedTrigger
                    ),
                }
            ),
            cv.Optional(CONF_ON_USSD_RECEIVED): automation.validate_automation(
                {
                    cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(
                        a7670ReceivedUssdTrigger
                    ),
                }
            ),
        }
    )
    .extend(cv.polling_component_schema("5s"))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    for conf in config.get(CONF_ON_SMS_RECEIVED, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(
            trigger, [(cg.std_string, "message"), (cg.std_string, "sender")], conf
        )
    for conf in config.get(CONF_ON_INCOMING_CALL, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(cg.std_string, "caller_id")], conf)
    for conf in config.get(CONF_ON_CALL_CONNECTED, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)
    for conf in config.get(CONF_ON_CALL_DISCONNECTED, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)

    for conf in config.get(CONF_ON_USSD_RECEIVED, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(cg.std_string, "ussd")], conf)


a7670_SEND_SMS_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.use_id(a7670Component),
        cv.Required(CONF_RECIPIENT): cv.templatable(cv.string_strict),
        cv.Required(CONF_MESSAGE): cv.templatable(cv.string),
    }
)


@automation.register_action(
    "a7670.send_sms", a7670SendSmsAction, a7670_SEND_SMS_SCHEMA
)
async def a7670_send_sms_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    template_ = await cg.templatable(config[CONF_RECIPIENT], args, cg.std_string)
    cg.add(var.set_recipient(template_))
    template_ = await cg.templatable(config[CONF_MESSAGE], args, cg.std_string)
    cg.add(var.set_message(template_))
    return var


a7670_DIAL_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.use_id(a7670Component),
        cv.Required(CONF_RECIPIENT): cv.templatable(cv.string_strict),
    }
)


@automation.register_action("a7670.dial", a7670DialAction, a7670_DIAL_SCHEMA)
async def a7670_dial_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    template_ = await cg.templatable(config[CONF_RECIPIENT], args, cg.std_string)
    cg.add(var.set_recipient(template_))
    return var


@automation.register_action(
    "a7670.connect",
    a7670ConnectAction,
    cv.Schema({cv.GenerateID(): cv.use_id(a7670Component)}),
)
async def a7670_connect_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    return var


a7670_SEND_USSD_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.use_id(a7670Component),
        cv.Required(CONF_USSD): cv.templatable(cv.string_strict),
    }
)


@automation.register_action(
    "a7670.send_ussd", a7670SendUssdAction, a7670_SEND_USSD_SCHEMA
)
async def a7670_send_ussd_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    template_ = await cg.templatable(config[CONF_USSD], args, cg.std_string)
    cg.add(var.set_ussd(template_))
    return var


@automation.register_action(
    "a7670.disconnect",
    a7670DisconnectAction,
    cv.Schema({cv.GenerateID(): cv.use_id(a7670Component)}),
)
async def a7670_disconnect_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    return var
