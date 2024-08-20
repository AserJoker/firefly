local runtime = require 'runtime'
require'event'.on('gameLoaded', function()
    local config = runtime.loadConfig("duskland", "options")
    if config == nil then
        config = runtime.createProperties()
        config:setKey("options.locale", "zh_CN")
        runtime.saveConfig("duskland", "options", config)
    end
    local locale = require 'locale'
    locale.setLang(config:getKey("options.locale"))
    runtime.setWindowTitle(locale.i18n("duskland.system.title"))
    local obj = {
        arr = require'array'.createArray({1, 2, 3}),
        str = "Hello world",
        num = 123
    }
    local str = require'serialization'.JSON.stringify(obj)
    print(str)
end)
