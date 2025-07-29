import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

# Définition du namespace et des classes
storage_ns = cg.esphome_ns.namespace("storage")
Storage = storage_ns.class_("Storage", cg.Component)  # Hérite de Component
StorageClient = storage_ns.class_("StorageClient", cg.EntityBase)
StorageClientStatic = storage_ns.namespace("StorageClient")

# Constantes de configuration
CONF_PREFIX = "path_prefix"
CONF_SD_MMC_ID = "sd_mmc_id"

# Schéma de configuration
STORAGE_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(Storage),
        cv.Required(CONF_PREFIX): cv.string,
        cv.Optional(CONF_SD_MMC_ID): cv.use_id("sd_mmc_card"),
    }
).extend(cv.COMPONENT_SCHEMA)

# Configuration principale
CONFIG_SCHEMA = STORAGE_SCHEMA

def storage_schema(class_=None):
    """
    Fonction helper pour étendre le schéma Storage dans d'autres composants
    """
    if class_ is None:
        return STORAGE_SCHEMA
    
    return STORAGE_SCHEMA.extend({
        cv.GenerateID(): cv.declare_id(class_)
    })

async def to_code(config):
    """
    Fonction principale de génération de code
    """
    # Créer et enregistrer le composant Storage
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    
    # Configurer le préfixe de chemin
    prefix = config[CONF_PREFIX]
    cg.add(var.set_path_prefix(prefix))
    
    # Si une carte SD/MMC est spécifiée
    if CONF_SD_MMC_ID in config:
        sd_mmc = await cg.get_variable(config[CONF_SD_MMC_ID])
        cg.add(var.set_sd_mmc_card(sd_mmc))
    
    # Ajouter le storage au registre global
    cg.add(StorageClientStatic.add_storage(var, prefix))

# Fonction legacy pour compatibilité
async def storage_to_code(config):
    """
    Fonction legacy pour compatibilité avec l'ancien code
    """
    storage = await cg.get_variable(config[CONF_ID])
    prefix = config[CONF_PREFIX]
    
    if CONF_SD_MMC_ID in config:
        sd_mmc = await cg.get_variable(config[CONF_SD_MMC_ID])
        cg.add(storage.set_sd_mmc_card(sd_mmc))
    
    cg.add(StorageClientStatic.add_storage(storage, prefix))








