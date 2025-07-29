import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.cpp_generator import MockObjClass

# Définition du namespace et des classes
storage_ns = cg.esphome_ns.namespace("storage")
Storage = storage_ns.class_("Storage", cg.EntityBase)
StorageClient = storage_ns.class_("StorageClient", cg.EntityBase)
StorageClientStatic = storage_ns.namespace("StorageClient")

# Configuration du composant
IS_PLATFORM_COMPONENT = True

# Constantes de configuration
CONF_PREFIX = "path_prefix"
CONF_SD_MMC_ID = "sd_mmc_id"

# Schéma de base pour Storage
STORAGE_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(Storage),
        cv.Required(CONF_PREFIX): cv.string,
        cv.Optional(CONF_SD_MMC_ID): cv.use_id("sd_mmc_card"),
    }
)

# Schéma de configuration principal
CONFIG_SCHEMA = cv.All(
    cv.ensure_list(STORAGE_SCHEMA),
    cv.Length(min=1)
)

def storage_schema(
    class_: MockObjClass = cv.UNDEFINED,
) -> cv.Schema:
    """
    Fonction helper pour étendre le schéma Storage dans d'autres composants
    """
    schema = {}
    if class_ is not cv.UNDEFINED:
        schema[cv.GenerateID()] = cv.declare_id(class_)
    return STORAGE_SCHEMA.extend(schema)

async def to_code(config):
    """
    Fonction principale de génération de code
    """
    for conf in config:
        # Créer l'instance Storage
        var = cg.new_Pvariable(conf[CONF_ID])
        await cg.register_component(var, conf)
        
        # Configurer le préfixe de chemin
        prefix = conf[CONF_PREFIX]
        cg.add(var.set_path_prefix(prefix))
        
        # Si une carte SD/MMC est spécifiée, l'associer
        if CONF_SD_MMC_ID in conf:
            sd_mmc = await cg.get_variable(conf[CONF_SD_MMC_ID])
            cg.add(var.set_sd_mmc_card(sd_mmc))
            cg.add(StorageClientStatic.add_storage_with_sd(var, prefix, sd_mmc))
        else:
            # Stockage sans carte SD (utilise le stockage flash interne)
            cg.add(StorageClientStatic.add_storage(var, prefix))

# Fonction legacy pour compatibilité (optionnelle)
async def storage_to_code(config):
    """
    Fonction legacy maintenue pour compatibilité avec l'ancien code
    """
    storage = await cg.get_variable(config[CONF_ID])
    prefix = config[CONF_PREFIX]
    
    if CONF_SD_MMC_ID in config:
        sd_mmc = await cg.get_variable(config[CONF_SD_MMC_ID])
        cg.add(StorageClientStatic.add_storage_with_sd(storage, prefix, sd_mmc))
    else:
        cg.add(StorageClientStatic.add_storage(storage, prefix))








