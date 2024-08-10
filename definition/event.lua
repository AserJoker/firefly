--- @meta
--- @module 'event'
--- @param event 'gameLoaded'|'tick'|string
--- @param callback function
--- @return fun():nil
local function on(event, callback) end
--- @param event string
--- @param arg string|nil
--- @return nil
local function emit(event, arg) end
return {on = on, emit = emit}
