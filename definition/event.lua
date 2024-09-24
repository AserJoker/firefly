--- @meta
--- @module 'event'
--- @overload fun(event:'gameLoaded'|'tick',callback:fun():nil):fun():nil
--- @overload fun(event:'keyUp'|'keyDown',callback:fun(scancode:integer):nil):fun():nil
--- @overload fun(event: 'update',callback:fun(timestamp:integer):nil):fun():nil
--- @overload fun(event:'mouseUp'|'mouseDown'|'click',callback:fun(button:integer):nil):fun():nil
--- @overload fun(event:'mouseMotion',callback:fun(mouse:{x:integer,y:integer,dx:integer,dy:integer}):nil):fun():nil
--- @overload fun(event:string,callback:function):fun():nil
local function on(event, callback) end

--- @param event string
--- @param arg string|nil
--- @return nil
local function emit(event, arg) end

return {on = on, emit = emit}
