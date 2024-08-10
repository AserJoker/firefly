--- @meta
--- @module 'log'
--- @param msg string
--- @return nil
local function debug(msg) end

--- @param msg string
--- @return nil
local function info(msg) end

--- @param msg string
--- @return nil
local function log(msg) end

--- @param msg string
--- @return nil
local function warn(msg) end

--- @param msg string
--- @return nil
local function error(msg) end

--- @param msg string
--- @return nil
local function panic(msg) end

return {
    debug = debug,
    info = info,
    log = log,
    warn = warn,
    error = error,
    panic = panic
}
