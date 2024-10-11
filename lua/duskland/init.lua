local document = require 'document'
local input = require 'input'
local runtime = require 'runtime'
local locale = require 'locale'

require'event'.on('gameLoaded', function()
    local title = locale.i18n("duskland.system.title")
    runtime.setWindowTitle(title)

    document.setShader("2d");
    local sprite = document.createSprite2D("001-Fighter01.png")
    local scene = document.createScene();
    scene:appendChild(sprite)
    document.setScene(scene)
    local animation = document.createAnimation()

    sprite:appendChild(animation)

    animation:setAction("move", "source.x", 0, 32 * 3, 0, 3, true)
    animation:setSpeed(10)

    sprite:setZIndex(1)

    sprite:setRect({x = 0, y = 0, width = 32, height = 48})
    sprite:setSourceRect({x = 0, y = 0, width = 32, height = 48})
    sprite:beginAttrGroup("rect")
    sprite:setAttribute("x", 128)
    sprite:endAttrGroup()
    require'event'.on("keyDown", function(scancode)
        if scancode == input.SCANCODE.S then animation:start("move") end
        if scancode == input.SCANCODE.A then
            animation:removeAction("move")
        end
    end)
    require'event'.on("keyUp", function(scancode)
        if scancode == input.SCANCODE.S then animation:reset("move") end
    end)
end)
