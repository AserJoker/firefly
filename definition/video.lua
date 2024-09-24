--- @meta
--- @module 'video'
--- @class format
--- @field RGB number
--- @field RGBA number
--- @field GRAY number
local format = {}

--- @class Sprite2D
local Sprite2D = {}

--- @param path string
function Sprite2D:setTexture(path) end

--- @param rect {x:number,y:number,width:number,height:number}
function Sprite2D:setRect(rect) end

--- @param rect {x:number,y:number,width:number,height:number}
function Sprite2D:setSourceRect(rect) end

--- @param center {x:number,y:number}
--- @param angle number
--- @param righthandle boolean|nil
function Sprite2D:setRotation(center, angle, righthandle) end

--- @param path string
--- @return Sprite2D
function createSprite2D(path) end

return {createSprite2D = createSprite2D, format = format}
