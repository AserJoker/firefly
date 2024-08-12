local runtime = require 'runtime'
require'event'.on('gameLoaded', function()
    local locale = require 'locale'
    locale.setLang("zh_CN")

    runtime.setWindowTitle(locale.i18n("duskland.system.title"))

    local db = require 'database'
    local record = db.queryOne("firefly.table", {id = "firefly.table"})
    for k, v in pairs(record) do print(k .. ":" .. tostring(v)) end
end)
