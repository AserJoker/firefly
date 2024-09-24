local video = require 'video'
local input = require 'input'
local runtime = require 'runtime'
local locale = require 'locale'

require'event'.on('gameLoaded', function()
    local title = locale.i18n("duskland.system.title")
    runtime.setWindowTitle(title)
    local sprite = video.createSprite2D("001-Fighter01.png")
    local tile = video.createSprite2D("001-Grassland01.png")

    sprite:setZIndex(1)

    sprite:setRect({x = 0, y = 0, width = 32, height = 48})
    sprite:setSourceRect({x = 0, y = 0, width = 32, height = 48})
    local x = 0
    local y = 0
    local xframe = 0
    local yframe = 0
    local dx = 0
    local dy = 0

    require'event'.on('tick', function()
        if input.getKeyState(input.SCANCODE.A) then
            dx = -1
        elseif input.getKeyState(input.SCANCODE.D) then
            dx = 1
        else
            dx = 0
        end

        if input.getKeyState(input.SCANCODE.W) then
            dy = -1
        elseif input.getKeyState(input.SCANCODE.S) then
            dy = 1
        else
            dy = 0
        end
        if dx ~= 0 or dy ~= 0 then
            xframe = xframe + 1
            if xframe >= 4 then xframe = 0 end
        else
            xframe = 0
        end
        if dx == -1 then yframe = 1 end
        if dx == 1 then yframe = 2 end
        if dy == -1 then yframe = 3 end
        if dy == 1 then yframe = 0 end
        sprite:setSourceRect({
            x = xframe * 32,
            y = yframe * 48,
            width = 32,
            height = 48
        })
        sprite:setRect({x = x, y = y, width = 32, height = 48})
        tile:getZIndex()
    end)

    require'event'.on('update', function(time)
        x = time / 10.0 * dx + x
        y = time / 10.0 * dy + y
    end)
end)
