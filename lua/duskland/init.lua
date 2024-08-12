local runtime = require 'runtime'
require'event'.on('gameLoaded', function()
    local locale = require 'locale'
    locale.setLang("zh_CN")

    runtime.setWindowTitle(locale.i18n("duskland.system.title"))
end)
local input = require 'input'
require'event'.on('keyDown', function(scancode)
    if scancode == input.SCANCODE.ESCAPE then input.releaseMouse() end
end)
require'event'.on('mouseDown', function() input.captureMouse() end)
require'event'.on('mouseMotion', function(motion)
    print(motion.x, motion.y, motion.dx, motion.dy)
end)
