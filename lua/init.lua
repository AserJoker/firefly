require 'event'.onEvent('gameLoaded', function()
    require 'system'.setWindowTitle('duskland')
    require 'system'.setWindowSize(1024, 768)
    local languages = require 'locale'.getLanguages()
    for _, v in ipairs(languages) do
        require 'log'.info(v)
    end
end)
