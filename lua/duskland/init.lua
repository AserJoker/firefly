local logger = require 'log'
require'event'.on('gameLoaded', function()
    local locale = require 'locale'
    locale.setLang("zh_CN")
    logger.info(locale.i18n("duskland.system.title"))
end)
