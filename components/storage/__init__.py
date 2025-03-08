import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_PLATFORM, CONF_WEB_SERVER
from esphome.components import web_server_base

DEPENDENCIES = ['media_player', 'web_server_base']
CODEOWNERS = ["@votre_nom"]

CONF_STORAGE = "storage"
CONF_FILES = "files"
CONF_DATA = "data"
CONF_MEDIA_FILE = "media_file"

storage_ns = cg.esphome_ns.namespace('storage')
StorageComponent = storage_ns.class_('StorageComponent', cg.Component)

def validate_data(value):
  if isinstance(value, str):
    return cv.string(value)
  if isinstance(value, dict) and 'lambda' in value:
    return cv.templatable(value['lambda'])
  raise cv.Invalid("Data must be either a string or a lambda")

FILE_SCHEMA = cv.Schema({
    cv.Required(CONF_DATA): validate_data,
    cv.Required(CONF_ID): cv.string,
})

CONFIG_SCHEMA = cv.Schema({
    cv.Required(CONF_PLATFORM): cv.one_of("sd_card", "flash", "inline", lower=True),
    cv.Required(CONF_ID): cv.declare_id(StorageComponent),
    cv.Required(CONF_FILES): cv.ensure_list(FILE_SCHEMA),
    cv.Optional(CONF_WEB_SERVER): cv.use_id(web_server_base.WebServerBase),
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    
    cg.add(var.set_platform(config[CONF_PLATFORM]))
    
    if CONF_WEB_SERVER in config:
        web_server = yield cg.get_variable(config[CONF_WEB_SERVER])
        cg.add(var.set_web_server(web_server))
    
    for file in config[CONF_FILES]:
        if isinstance(file[CONF_DATA], str):
            cg.add(var.add_file(
                lambda path=file[CONF_DATA]: id(var).read_file(path),
                file[CONF_ID]
            ))
        else:
            cg.add(var.add_file(file[CONF_DATA], file[CONF_ID]))
        cg.register_variable(file[CONF_ID], var)







