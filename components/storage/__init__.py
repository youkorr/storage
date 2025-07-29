import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

# Définition du namespace et des classes
storage_ns = cg.esphome_ns.namespace("storage")
Storage = storage_ns.class_("Storage", cg.Component)
SDStorage = storage_ns.class_("SDStorage", Storage)
StorageClient = storage_ns.class_("StorageClient", cg.EntityBase)
StorageClientStatic = storage_ns.namespace("StorageClient")

# Constantes de configuration
CONF_PREFIX = "path_prefix"
CONF_SD_MMC_ID = "sd_mmc_id"

# Schéma de configuration
STORAGE_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(SDStorage),
        cv.Required(CONF_PREFIX): cv.string,
        cv.Optional(CONF_SD_MMC_ID): cv.use_id("sd_mmc_card"),  # Optionnel pour compatibilité
    }
).extend(cv.COMPONENT_SCHEMA)

# Configuration principale - liste de composants storage
CONFIG_SCHEMA = cv.All(
    cv.ensure_list(STORAGE_SCHEMA),
    cv.Length(min=1)
)

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
    for conf in config:
        # Créer et enregistrer le composant SDStorage
        var = cg.new_Pvariable(conf[CONF_ID])
        await cg.register_component(var, conf)
        
        # Configurer le préfixe de chemin
        prefix = conf[CONF_PREFIX]
        cg.add(var.set_path_prefix(cg.RawExpression(f'"{prefix}"')))
        
        # Note: On ne configure plus sd_mmc_card directement
        # La carte SD est initialisée automatiquement dans SDStorage::setup()
        
        # Ajouter le storage au registre global
        cg.add(StorageClientStatic.add_storage(var, cg.RawExpression(f'"{prefix}"')))

# Fonction pour valider la configuration
def validate_config(config):
    """
    Validation supplémentaire de la configuration
    """
    if not config:
        raise cv.Invalid("Au moins un composant storage doit être configuré")
    
    # Vérifier les doublons de préfixes
    prefixes = []
    for conf in config:
        prefix = conf[CONF_PREFIX]
        if prefix in prefixes:
            raise cv.Invalid(f"Le préfixe '{prefix}' est utilisé plusieurs fois")
        prefixes.append(prefix)
    
    return config

# Appliquer la validation
CONFIG_SCHEMA = cv.All(CONFIG_SCHEMA, validate_config)









