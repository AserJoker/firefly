--- @meta
--- @module 'video'
--- @class Image
local Image = {}

---@return number,number
function Image:getSize() end

--- @param name string
--- @return Image
--- @overload fun(width:number,height:number,format:number):Image
local createImage = function(name) end

--- @class format
--- @field RGB number
--- @field RGBA number
--- @field GRAY number
local format = {}

return {createImage = createImage, format = format}
