--- @meta
--- @module 'media'
--- @class Buffer
local Buffer = {};

--- @return number
function Buffer:getLength() end

--- @param offset number
--- @return number
function Buffer:readUint8(offset) end

--- @param offset number
--- @return number
function Buffer:readUint16(offset) end

--- @param offset number
--- @return number
function Buffer:readUint32(offset) end

--- @param offset number
--- @param value number
--- @return nil
function Buffer:writeUint8(offset, value) end

--- @param offset number
--- @param value number
--- @return nil
function Buffer:writeUint16(offset, value) end

--- @param offset number
--- @param value number
--- @return nil
function Buffer:writeUint32(offset, value) end

--- @return number[]
function Buffer:toUint8Array() end

--- @return number[]
function Buffer:toUint16Array() end

--- @return number[]
function Buffer:toUint32Array() end

--- @return string
function Buffer:toString() end

--- @class Resource
local Resource = {}

--- @param size number|nil
--- @return Buffer
function Resource:read(size) end

--- @param name string
--- @return Resource
local function query(name) end
return {query = query}
