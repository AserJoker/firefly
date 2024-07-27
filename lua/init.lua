require 'event'.onEvent('init', function(msg)
    require 'log'.info(tostring(msg))
end)
