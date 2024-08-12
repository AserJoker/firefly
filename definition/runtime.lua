--- @meta
--- @module 'runtime'
--- @param title string
--- @return nil
local function setWindowTitle(title) end

--- @return string
local function getWindowTitle() end

--- @param width integer
--- @param height integer
--- @return nil
local function setWindowSize(width, height) end

--- @return integer,integer
local function getWindowSize() end

return {
    setWindowTitle = setWindowTitle,
    getWindowTitle = getWindowTitle,
    setWindowSize = setWindowSize,
    getWindowSize = getWindowSize
}
