--- @meta
--- @module 'database'
--- @param table string
--- @param query table
--- @return table
local function queryOne(table, query) end

--- @param table string
--- @param query table
--- @return table[]
local function queryList(table, query) end

return {queryOne = queryOne, queryList = queryList}
