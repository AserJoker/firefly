--- @meta
--- @module 'serialization'
--- @param data any
--- @return string
local function stringify(data) end

--- @param source string
--- @return any
local function parse(source) end

return {
    JSON = {stringify = stringify, parse = parse},
    YAML = {stringify = stringify, parse = parse}
}
