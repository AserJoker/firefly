local video = require 'video'
local input = require 'input'
local runtime = require 'runtime'
local locale = require 'locale'

require 'event'.on('gameLoaded', function()
    local title = locale.i18n("duskland.system.title")
    runtime.setWindowTitle(title)

    video.setShader("2d");
    local sprite = video.createSprite2D("001-Fighter01.png")
    local sprite2 = video.createSprite2D("001-Grassland01.png")
    local subscene = video.createScene();
    local renderTarget = video.createRenderTarget({ width = 256, height = 576 })
    local mask = video.createSprite2D(renderTarget)
    mask:setRect({ x = 0, y = 0, width = 256, height = 576 })
    subscene:appendChild(sprite2)
    renderTarget:appendChild(subscene);
    local scene = video.createScene();
    scene:appendChild(sprite)
    scene:appendChild(renderTarget)
    scene:appendChild(mask)
    video.setScene(scene)

    sprite:setZIndex(1)
    local anime_index = 0
    local anime = video.createSprite2D("particles_flipbook_example.webp")
    anime:setSourceRect({ x = 0, y = 0, width = 192, height = 192 })
    anime:setRect({ x = 512, y = 0, width = 192, height = 192 })
    scene:appendChild(anime)

    sprite:setRect({ x = 0, y = 0, width = 32, height = 48 })
    sprite:setSourceRect({ x = 0, y = 0, width = 32, height = 48 })
    local x = 0
    local y = 0
    local xframe = 0
    local yframe = 0
    local dx = 0
    local dy = 0
    local frame = 0
    require 'event'.on('tick', function()
    end)

    require 'event'.on('update', function(time)
        -- frame = frame + 1
        -- if input.getKeyState(input.SCANCODE.A) then
        --     dx = -1
        -- elseif input.getKeyState(input.SCANCODE.D) then
        --     dx = 1
        -- else
        --     dx = 0
        -- end

        -- if input.getKeyState(input.SCANCODE.W) then
        --     dy = -1
        -- elseif input.getKeyState(input.SCANCODE.S) then
        --     dy = 1
        -- else
        --     dy = 0
        -- end
        -- if dx ~= 0 or dy ~= 0 then
        --     xframe = xframe + 1
        --     if xframe >= 4 then xframe = 0 end
        -- else
        --     xframe = 0
        -- end
        -- if dx == -1 then yframe = 1 end
        -- if dx == 1 then yframe = 2 end
        -- if dy == -1 then yframe = 3 end
        -- if dy == 1 then yframe = 0 end
        -- sprite:setSourceRect({
        --     x = xframe * 32,
        --     y = yframe * 48,
        --     width = 32,
        --     height = 48
        -- })
        -- sprite:setRect({ x = x, y = y, width = 32, height = 48 })
        -- local anime_x = anime_index % 5
        -- local anime_y = (anime_index - (anime_x)) / 5
        -- anime:setSourceRect({
        --     x = anime_x * 192,
        --     y = anime_y * 192,
        --     width = 192,
        --     height = 192
        -- })
        -- anime_index = anime_index + 1
        -- if anime_index == 36 then anime_index = 0 end
        -- x = time / 10.0 * dx + x
        -- y = time / 10.0 * dy + y
    end)
end)
