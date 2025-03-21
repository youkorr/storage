import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_PLATFORM, CONF_WEB_SERVER
from esphome.components import web_server_base, audio

DEPENDENCIES = ['media_player', 'web_server_base', 'audio', 'sd_mmc_card'] #Add sd_mmc_card dependency
CODEOWNERS = ["@votre_nom"]

CONF_STORAGE = "storage"
CONF_FILES = "files"
CONF_PATH = "path"
CONF_MEDIA_FILE = "media_file"
CONF_CHUNK_SIZE = "chunk_size" #Add chunksize option

storage_ns = cg.esphome_ns.namespace('storage')
StorageComponent = storage_ns.class_('StorageComponent', cg.Component)
StorageFile = storage_ns.class_('StorageFile', audio.AudioFile, cg.Component)

FILE_SCHEMA = cv.Schema({
    cv.Required(CONF_PATH): cv.string,
    cv.Required(CONF_ID): cv.declare_id(StorageFile),
    cv.Optional(CONF_CHUNK_SIZE, default="512"): cv.positive_int, #Default chunk size
})

CONFIG_SCHEMA = cv.Schema({
    cv.Required(CONF_PLATFORM): cv.one_of("sd_card", "flash", "inline", lower=True),
    cv.Required(CONF_ID): cv.declare_id(StorageComponent),
    cv.Required(CONF_FILES): cv.ensure_list(FILE_SCHEMA),
    cv.Optional(CONF_WEB_SERVER): cv.use_id(web_server_base.WebServerBase),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config): #Replaced @cg.coroutine with async def
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config) #Replaced yield with await

    cg.add(var.set_platform(config[CONF_PLATFORM]))

    for file in config[CONF_FILES]:
        file_var = cg.new_Pvariable(file[CONF_ID])
        cg.add(file_var.set_path(file[CONF_PATH]))
        cg.add(file_var.set_platform(config[CONF_PLATFORM]))
        cg.add(file_var.set_chunk_size(file[CONF_CHUNK_SIZE]))  # Set Chunk Size
        await cg.register_component(file_var, file) #Replaced yield with await
        cg.add(var.add_file(file_var))

    if CONF_WEB_SERVER in config:
        web_server = await cg.get_variable(config[CONF_WEB_SERVER]) #Replaced yield with await
        cg.add(var.set_web_server(web_server))








