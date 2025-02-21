[ generator ]->NEXT->[ value, done ]
[ generator ]->AWAIT_NEXT->[ value, done ]
[ value,key,object ] ->SET_FIELD-> [ result ]
[ key,object ] -> GET_FIELD-> [ value ]
[] -> BEGIN -> []
[] -> END -> []
[] -> PUSH(number) -> [ number ]
[ val ] -> POP -> []
[ val ] -> PUSH_VALUE(offset) -> [ val, stack(-offset) ]
[] -> NIL -> [ null ]
[] -> UNDEFINED -> [ undefined ]
[] -> TRUE -> [ true ]
[] -> FALSE -> [ false ]
[] -> REGEX(init) -> [ regex ]
[] -> CLASS -> [ class ]
[] -> LOAD(id) -> [ id ]
[ val ] -> STORE(id) -> [ val ]
[ func ] -> REF -> [ func ]
[] -> STR(idx) -> [ str ]
[] -> BIGINT(init) -> [ bigint ]
[] -> VAR(idx) -> []
[] -> CONST(idx) -> []
[] -> LET(idx) -> []
[] -> THIS -> [ this ]
[] -> OBJECT -> [ obj ]
[] -> ARRAY -> [ arr ]
[ ...args,id ] -> SUPER_CALL(num) -> [ ret ]
[] -> FUNCTION(entry) -> [ function ]
[] -> ASYNCFUNCTION(entry) -> [ asyncfunction ]
[] -> ARROW(entry) -> [ arrow ]
[] -> ASYNCARROW(entry) -> [ asyncarrow ]
[] -> GENERATOR(entry) -> [ generator ]
[] -> ASYNCGENERATOR(entry) -> [ asyncgenerator ]
[] -> ENABLE(directive) -> []
[] -> DISABLE -> []
[ id, obj ] -> GET_FIELD -> [ val ]
[ val, id, obj ] -> SET_FIELD -> [ val ]
[ getter, id, obj ] -> SET_GETTER -> [ getter ]
[ setter, id, obj ] -> SET_SETTER -> [ setter ]
[ getter, id, class, static ] -> SET_ACCESSOR_GETTER -> [ getter ]
[ setter, id, class, static ] -> SET_ACCESSOR_SETTER -> [ setter ]
[ method, id, class, static ] -> SET_METHOD -> [ method ]
[ id, obj ] -> GET_PRIVATE_FIELD -> [ val ]
[ val, id, obj] -> SET_PRIVATE_FIELD -> [ val ]
[ getter, id, class, static ] -> SET_PRIVATE_ACCESSOR_GETTER -> [ getter ]
[ setter, id, class, static ] -> SET_PRIVATE_ACCESSOR_SETTER -> [ setter ]
[ method, id, class, static ] -> SET_PRIVATE_METHOD -> [ method ]
[ obj ] -> GET_KEYS -> [ obj, keys ]
[ val, id ] -> SET_SUPER_FIELD -> [ val ]
[ id ] -> GET_SUPER_FIELD -> [ val ]
[ id, class ] -> SET_INITIALIZER(entry) -> []
[ id, class ] -> SET_PRIVATE_INITIALIZER(entry) -> []
[ ...args, callee] -> CALL(num) -> [ ret ]
[ ...args, id, callee] -> MEMBER_CALL(num) -> [ ret ]
[ ...args, id, callee] -> PRIVATE_MEMBER_CALL(num) -> [ ret ]
[ val ] -> VOID -> [ undefined ]
TYPEOF
NEW
DELETE
RET
[ val ] -> YIELD -> [ next ]
AWAIT
THROW
[ it ] -> YIELD_DELEGATE -> [ it, val ]
TRY_BEGIN
TRY_END
DEFER
JMP
JTRUE
JFALSE
JNULL
JNOT_NULL
ADD
SUB
DIV
MUL
MOD
POW
AND
OR
NOT
XOR
SHR
SHL
USHR
EQ
SEQ
NE
SNE
GT
LT
GE
LE
INC
DEC
UPDATE_INC
UPDATE_DEC
[ it ] -> NEXT -> [ value, done ]
[ asyncit ] -> AWAIT_NEXT -> [ value, done ]
SPREAD
MERGE
[ val ] -> ITERATOR -> [ it ]
OBJECT_SPREAD
ARRAY_SPREAD
EMPTY_CHECK
ARGUMENT_SPREAD
HLT
DEBUGGER
WITH
IMPORT
EXPORT
EXPORT_ALL
ASSERT