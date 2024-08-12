local runtime = require 'runtime'
require'event'.on('gameLoaded', function()
    local locale = require 'locale'
    locale.setLang("zh_CN")

    runtime.setWindowTitle(locale.i18n("duskland.system.title"))
end)
