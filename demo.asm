const_pool = ["Hello world",...]

pushsoop

const 1
import aa 
store bb
importall 
store cc
importdefault 
store dd
pop  // import dd,{aa as bb},* as cc from const_pool[1]

pushundefined
store data 
pop // var data = undefined

load a
load b
add
pop //a+b

load print
load add
push 1
push 2
call 2
call 1
pop // print(add(1,2))

load a
const 0
push "data"
setfield 
pop // a.data = const_pool[0]

push a
push b
sub
pop
jae 2
push b
store a 
pop // if(a<b) a=b

jmp 6
arg a
arg b
load a
add b
ret
func 5
store add 
export add
pop //export function add(a,b){return a+b}

pushclass
load Base
extends
push 123
push "data"
setfield data
jmp 6
arg a
arg b
load a
add b
ret
func 5
push "add"
setfield
store Test
pop // class Test extends Base{data = 123,add(a,b){return a+b}}

popscoop