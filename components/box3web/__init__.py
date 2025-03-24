import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sd_mmc_card
from esphome.const import CONF_ID, CONF_URL, CONF_PORT

# Nouvelles constantes pour ESPAsyncWebServer
CONF_ASYNC_WEB_SERVER_ID = "async_web_server_id"

# Espace de noms pour le composant
Box3Web_ns = cg.esphome_ns.namespace("box3web")
Box3Web = Box3Web_ns.class_("Box3Web", cg.Component)

# Schéma de validation de la configuration
CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(Box3Web),
            cv.GenerateID(CONF_ASYNC_WEB_SERVER_ID): cv.use_id(AsyncWebServer),  # Utiliser AsyncWebServer
            cv.GenerateID(sd_mmc_card.CONF_SD_MMC_CARD_ID): cv.use_id(sd_mmc_card.SdMmc),
            cv.Optional(CONF_URL_PREFIX, default="file"): cv.string_strict,
            cv.Optional(CONF_ROOT_PATH, default="/"): cv.string_strict,
            cv.Optional(CONF_ENABLE_DELETION, default=False): cv.boolean,
            cv.Optional(CONF_ENABLE_DOWNLOAD, default=False): cv.boolean,
            cv.Optional(CONF_ENABLE_UPLOAD, default=False): cv.boolean,
        }
    ).extend(cv.COMPONENT_SCHEMA),
)

# Priorité de génération du code
@coroutine_with_priority(45.0)
async def to_code(config):
    # Récupérer l'instance de AsyncWebServer
    server_var = await cg.get_variable(config[CONF_ASYNC_WEB_SERVER_ID])

    # Créer une instance de Box3Web
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    # Configurer la carte SD
    sdmmc = await cg.get_variable(config[sd_mmc_card.CONF_SD_MMC_CARD_ID])
    cg.add(var.set_sd_mmc_card(sdmmc))

    # Configurer les options
    cg.add(var.set_url_prefix(config[CONF_URL_PREFIX]))
    cg.add(var.set_root_path(config[CONF_ROOT_PATH]))
    cg.add(var.set_deletion_enabled(config[CONF_ENABLE_DELETION]))
    cg.add(var.set_download_enabled(config[CONF_ENABLE_DOWNLOAD]))
    cg.add(var.set_upload_enabled(config[CONF_ENABLE_UPLOAD]))

    # Ajouter Box3Web comme gestionnaire au serveur AsyncWebServer
    cg.add(server_var.add_handler(var))

    # Définir une macro pour indiquer que nous utilisons ESPAsyncWebServer
    cg.add_define("USE_ESPASYNCWEBSERVER")
    
