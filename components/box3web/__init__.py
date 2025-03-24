import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sd_mmc_card

# Import AsyncWebServer
from esphome.components.async_web_server import AsyncWebServer

# Define constants
CONF_ASYNC_WEB_SERVER_ID = "async_web_server_id"
CONF_URL_PREFIX = "url_prefix"
CONF_ROOT_PATH = "root_path"
CONF_ENABLE_DELETION = "enable_deletion"
CONF_ENABLE_DOWNLOAD = "enable_download"
CONF_ENABLE_UPLOAD = "enable_upload"

# Namespace
Box3Web_ns = cg.esphome_ns.namespace("box3web")
Box3Web = Box3Web_ns.class_("Box3Web", cg.Component)

# Configuration schema
CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(Box3Web),
            cv.GenerateID(CONF_ASYNC_WEB_SERVER_ID): cv.use_id(AsyncWebServer),  # Use AsyncWebServer
            cv.GenerateID(sd_mmc_card.CONF_SD_MMC_CARD_ID): cv.use_id(sd_mmc_card.SdMmc),
            cv.Optional(CONF_URL_PREFIX, default="file"): cv.string_strict,
            cv.Optional(CONF_ROOT_PATH, default="/"): cv.string_strict,
            cv.Optional(CONF_ENABLE_DELETION, default=False): cv.boolean,
            cv.Optional(CONF_ENABLE_DOWNLOAD, default=False): cv.boolean,
            cv.Optional(CONF_ENABLE_UPLOAD, default=False): cv.boolean,
        }
    ).extend(cv.COMPONENT_SCHEMA),
)

@coroutine_with_priority(45.0)
async def to_code(config):
    # Get the AsyncWebServer instance
    server_var = await cg.get_variable(config[CONF_ASYNC_WEB_SERVER_ID])

    # Create Box3Web instance
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    # Configure SD card
    sdmmc = await cg.get_variable(config[sd_mmc_card.CONF_SD_MMC_CARD_ID])
    cg.add(var.set_sd_mmc_card(sdmmc))

    # Set URL prefix and root path
    cg.add(var.set_url_prefix(config[CONF_URL_PREFIX]))
    cg.add(var.set_root_path(config[CONF_ROOT_PATH]))

    # Enable features
    cg.add(var.set_deletion_enabled(config[CONF_ENABLE_DELETION]))
    cg.add(var.set_download_enabled(config[CONF_ENABLE_DOWNLOAD]))
    cg.add(var.set_upload_enabled(config[CONF_ENABLE_UPLOAD]))

    # Add Box3Web as a handler to the AsyncWebServer
    cg.add(server_var.add_handler(var))

    # Define macro for ESPAsyncWebServer usage
    cg.add_define("USE_ESPASYNCWEBSERVER")
    
