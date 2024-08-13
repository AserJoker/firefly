--- @meta
--- @module 'runtime'
--- @class SaveManifest
--- @field playerName string
--- @field wordName string
--- @field icon string
--- @field createTime integer
--- @field updateTime integer
local SaveManifest = {}

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

--- @class Properties
local Properties = {}

--- @param key string
--- @return string
function Properties:getKey(key) end

--- @param key  string
--- @param value string
--- @return nil
function Properties:setKey(key, value) end

--- @param key string
--- @return nil
function Properties:removeKey(key) end

--- @return string
function Properties:store() end

--- @param name string
--- @param file string
--- @param config Properties
local function saveConfig(name, file, config) end

--- @param name string
--- @param file string
--- @return Properties
local function loadConfig(name, file) end

--- @param name string
--- @param file string
--- @param data Buffer
--- @return nil
local function save(name, file, data) end

--- @return string[]
local function getSaveInfo() end

--- @param name string
--- @param file string
--- @return Buffer
local function load(name, file) end

--- @return Properties
local function createProperties() end

return {
    setWindowTitle = setWindowTitle,
    getWindowTitle = getWindowTitle,
    setWindowSize = setWindowSize,
    getWindowSize = getWindowSize,
    saveConfig = saveConfig,
    loadConfig = loadConfig,
    save = save,
    load = load,
    getSaveInfo = getSaveInfo,
    createProperties = createProperties
}
