--- @meta
--- @module 'document'
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

--- @param key string
--- @param value any
--- @return boolean
function Node:setAttribute(key, value) end

--- @param key string
--- @return any
function Node:getAttribute(key) end

--- @param name string
function Node:beginAttrGroup(name) end

function Node:endAttrGroup() end

--- @param name string
--- @param host Node
--- @param source string
function Node:bindAttribute(name, host, source) end

--- @return {[string]:"string"|"int32"|"uint32"|"float32"|"boolean"|"nil"}
function Node:getAttributes() end

--- @return {[string]:string[]}
function Node:getAttributeGroups() end

--- @class Scene:Node
local Scene = {};

--- @param type "ortho"|"perspective"
function Scene:setCamera(type) end

--- @class RenderTarget:Node
local RenderTarget = {}

--- @param size Size
function RenderTarget:resize(size) end

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

--- @class Animation:Node
local Animation = {}

--- @param name string
--- @param attr string
--- @param step number
--- @param beginframe integer
--- @param endframe integer
--- @param loop boolean
function Animation:setAction(name, attr, step, beginframe, endframe, loop) end

--- @param name string
--- @param attr string
--- @param startvalue number
--- @param endvalue number
--- @param beginframe integer
--- @param endframe integer
--- @param loop boolean
function Animation:setAction(name, attr, startvalue, endvalue, beginframe,
                             endframe, loop) end

--- @param name string
function Animation:removeAction(name) end

--- @param name string
function Animation:setGroup(name) end

--- @param speed number
function Animation:setSpeed(speed) end

--- @param name string
function Animation:start(name) end

--- @param name string
function Animation:stop(name) end

--- @param name string
function Animation:resume(name) end

--- @param name string
function Animation:reset(name) end

--- @param path string
--- @return Sprite2D
--- @overload fun(renderTarget:RenderTarget,attachement:integer|nil):Sprite2D
function createSprite2D(path) end

--- @return Scene
function createScene() end

--- @param scene Scene
function setScene(scene) end

--- @return Scene
function getScene() end

--- @class Size
--- @field width integer
--- @field height integer
local Size = {};

--- @param size Size
--- @return RenderTarget
--- @overload fun(size:Size,attachment:integer):RenderTarget
--- @overload fun(size:Size,stage:string):RenderTarget
--- @overload fun(size:Size,stage:string,attachment:integer):RenderTarget
function createRenderTarget(size) end

--- @return Animation
function createAnimation() end

return {
    createSprite2D = createSprite2D,
    setShader = setShader,
    createScene = createScene,
    setScene = setScene,
    getScene = getScene,
    createRenderTarget = createRenderTarget,
    createAnimation = createAnimation
}
