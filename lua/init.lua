local function onLoop(msg)
    print(msg)
    require 'event'.offEvent("loop", onLoop)
end
require 'event'.onEvent('loop', onLoop)
