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

--- @return string
function Node:getId() end

--- @param id string
function Node:setId(id) end

--- @param id string
--- @return Node
function Node:getChild(id) end

--- @class Scene:Node
local Scene = {};

--- @param type "ortho"|"perspective"
function Scene:setCamera(type) end

--- @class RenderTarget:Node
local RenderTarget = {}

--- @class Sprite2D:Node
local Sprite2D = {}

--- @param src string
function Sprite2D:setTexture(src) end

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

--- @param path string|RenderTarget
--- @return Sprite2D
function createSprite2D(path) end

--- @return Scene
function createScene() end

--- @param scene Scene
function setScene(scene) end

--- @return Scene
function getScene() end

--- @param size {width:integer,height:integer}
--- @return RenderTarget
function createRenderTarget(size) end

return {
    createSprite2D = createSprite2D,
    setShader = setShader,
    createScene = createScene,
    setScene = setScene,
    getScene = getScene,
    createRenderTarget = createRenderTarget
}
