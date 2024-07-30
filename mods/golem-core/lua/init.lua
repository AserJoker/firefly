require 'log'.info('golem init')
return {
    info = function(msg)
        require 'log'.info('golem: ' .. tostring(msg))
    end
}
