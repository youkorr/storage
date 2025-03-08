import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_PLATFORM, CONF_WEB_SERVER
from esphome.components import web_server_base, audio

DEPENDENCIES = ['media_player', 'web_server_base', 'audio']
CODEOWNERS = ["@votre_nom"]

# Déclaration des IDs
CONF_STORAGE = "storage"
CONF_FILES = "files"
CONF_PATH = "path"
CONF_MEDIA_FILE = "media_file"

storage_ns = cg.esphome_ns.namespace('storage')
StorageComponent = storage_ns.class_('StorageComponent', cg.Component)
StorageFile = storage_ns.class_('StorageFile', audio.AudioFile, cg.Component)

FILE_SCHEMA = cv.Schema({
    cv.Required(CONF_PATH): cv.string,
    cv.Required(CONF_ID): cv.declare_id(StorageFile),
})

CONFIG_SCHEMA = cv.Schema({
    cv.Required(CONF_PLATFORM): cv.one_of("sd_card", "flash", "inline", lower=True),
    cv.Required(CONF_ID): cv.declare_id(StorageComponent),
    cv.Required(CONF_FILES): cv.ensure_list(FILE_SCHEMA),
    cv.Optional(CONF_WEB_SERVER): cv.use_id(web_server_base.WebServerBase),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    
    # Définit la plateforme dans StorageComponent
    cg.add(var.set_platform(config[CONF_PLATFORM]))
    
    # Propage la plateforme à chaque fichier
    for file in config[CONF_FILES]:
        file_var = cg.new_Pvariable(file[CONF_ID])
        cg.add(file_var.set_path(file[CONF_PATH]))
        cg.add(file_var.set_platform(config[CONF_PLATFORM]))  # Ligne ajoutée
        cg.add(var.add_file(file_var))
        await cg.register_component(file_var, file)
    
    # Configuration du serveur web
    if CONF_WEB_SERVER in config:
        web_server = await cg.get_variable(config[CONF_WEB_SERVER])
        cg.add(var.set_web_server(web_server))






