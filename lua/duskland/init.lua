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
    local co = require 'co'
    local now = 0
    local promise = co.start(function()
        while now < 500 do
            now = now + 1;
            co.yield()
        end
        return "data";
    end)
    promise:next(function(value) print(value[1]) end)
end)
