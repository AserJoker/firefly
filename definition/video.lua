--- @meta
--- @module 'video'
--- @class format
--- @field RGB number
--- @field RGBA number
--- @field GRAY number
local format = {}

--- @class Rect
--- @field x integer
--- @field y integer
--- @field width integer
--- @field height integer
local Rect = {}

--- @class Sprite2D
local Sprite2D = {}

--- @param path string
function Sprite2D:setTexture(path) end

--- @return {width:integer,height:integer}
function Sprite2D:getTextureSize() end

--- @param rect Rect
function Sprite2D:setRect(rect) end

--- @return Rect
function Sprite2D:getRect() end

--- @param rect Rect
function Sprite2D:setSourceRect(rect) end

--- @return Rect
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

--- @param alpha number
function Sprite2D:setBlend(alpha) end

--- @return number
function Sprite2D:getBlend() end

--- @param path string
--- @return Sprite2D
function createSprite2D(path) end

--- @param shaderName string
function setShader(shaderName) end

--- @class Node
local Node = {};

---@param child Node
function Node:appendChild(child) end

---@param child Node
function Node:removeChild(child) end

---@return Node
function Node:getParent() end

--- @class Camera:Node
local Camera = {};

--- @return Camera
function createOrthoCamera() end

return {
    createSprite2D = createSprite2D,
    setShader = setShader,
    createOrthoCamera = createOrthoCamera
}
