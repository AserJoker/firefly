require 'scene'.setScene(function()
    require 'system'.setWindowTitle('duskland')
    require 'system'.setWindowSize(1024, 768)
    local video = require 'video'
    video.loadTexture("wall", "texture::wall.jpg")
    local sprite = video.createSprite2D()
    sprite:setTexture("wall")
    sprite:enable()
    local angle = 0;
    return {
        tick = function()
            angle = angle + 1
            sprite:setRotation({ x = 256, y = 256, z = 1, angle = angle })
        end
    }
end)
