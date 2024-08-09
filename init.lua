for k, v in pairs(_G['$objects']) do
    print(tostring(k) .. ":" .. tostring(v))
end
for k, v in pairs(_G['$functions']) do
    print(tostring(k) .. ":" .. tostring(v))
end
local native = require('native');
native.print()
