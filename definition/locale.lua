--- @meta
--- @module 'locale'
--- @param key string
--- @param args table<string,string>|nil
--- @return string
local function i18n(key, args) end

--- @return string
local function getLang() end

--- @param lang string
--- @return string
local function setLang(lang) end

--- @return string[]
local function getLanguages() end

return {
    i18n = i18n,
    getLang = getLang,
    setLang = setLang,
    getLanguages = getLanguages
}
