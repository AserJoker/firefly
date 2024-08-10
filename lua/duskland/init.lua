local logger = require 'log'
logger.info("Hello world")
require'event'.on('gameLoaded', function()
    local locale = require 'locale'
    local languages = locale.getLanguages()
    for i = 1, #languages, 1 do logger.info(languages[i]) end

    local media = require 'media'
    local res = media.query("lang::zh_CN.lang")
    local buffer = res:read()
    local str = buffer:toString()
    logger.info(str)
end)
