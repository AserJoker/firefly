--- @meta
--- @module 'co'
--- @class Promise
local Promise = {}

--- @param ... any
--- @return nil
function Promise:resolve(...) end

--- @param message string
--- @return nil
function Promise:reject(message) end

--- @return any[],nil
--- @overload fun():nil,string
function Promise:wait() end

--- @overload fun(callback:fun(value:any[]):any):Promise
--- @overload fun(callback:fun(value:nil,message:string):any):Promise
function Promise:next(callback) end

--- @return Promise
local function createPromise() end

--- @return nil
local function yield() end

--- @param callback fun():any
--- @return Promise
local function start(callback) end

return {createPromise = createPromise, yield = yield, start = start}
