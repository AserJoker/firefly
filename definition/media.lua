--- @meta
--- @module 'media'
--- @class Buffer
local Buffer = {};

--- @return integer
function Buffer:getLength() end

--- @param offset integer
--- @return integer
function Buffer:readUint8(offset) end

--- @param offset integer
--- @return integer
function Buffer:readUint16(offset) end

--- @param offset integer
--- @return integer
function Buffer:readUint32(offset) end

--- @param offset integer
--- @param value integer
--- @return nil
function Buffer:writeUint8(offset, value) end

--- @param offset integer
--- @param value integer
--- @return nil
function Buffer:writeUint16(offset, value) end

--- @param offset integer
--- @param value integer
--- @return nil
function Buffer:writeUint32(offset, value) end

--- @return integer[]
function Buffer:toUint8Array() end

--- @return integer[]
function Buffer:toUint16Array() end

--- @return integer[]
function Buffer:toUint32Array() end

--- @return string
function Buffer:toString() end

--- @class Resource
local Resource = {}

--- @param size integer|nil
--- @return Buffer
function Resource:read(size) end

--- @param name string
--- @return Resource
local function query(name) end
return {query = query}
