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

--- @return {width:integer,height:integer}
function Sprite2D:getTextureSize() end

--- @param rect {x:integer,y:integer,width:integer,height:integer}
function Sprite2D:setRect(rect) end

--- @return {x:integer,y:integer,width:integer,height:integer}
function Sprite2D:getRect() end

--- @param rect {x:integer,y:integer,width:integer,height:integer}
function Sprite2D:setSourceRect(rect) end

--- @return {x:integer,y:integer,width:integer,height:integer}
function Sprite2D:getSourceRect() end

--- @param center {x:integer,y:integer}
--- @param angle number
--- @param righthandle boolean|nil
function Sprite2D:setRotation(center, angle, righthandle) end

--- @return {center:{x:integer,y:integer},angle:number,righthandle:boolean}
function Sprite2D:getRotation() end

--- @param visible boolean
function Sprite2D:setVisible(visible) end

--- @return boolean
function Sprite2D:isVisible() end

--- @param zIndex integer
function Sprite2D:setZIndex(zIndex) end

--- @return integer
function Sprite2D:getZIndex() end

--- @param path string
--- @return Sprite2D
function createSprite2D(path) end

return {createSprite2D = createSprite2D}
