print('golem init')
return {
    info = function(msg)
        require 'duskland.api'.info('golem: ' .. msg)
    end
}
