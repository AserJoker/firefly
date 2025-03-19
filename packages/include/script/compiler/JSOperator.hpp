#pragma once
enum class JS_OPERATOR {
  BEGIN = 0,
  END,
  PUSH,
  POP,
  PUSH_VALUE,
  NIL,
  UNDEFINED,
  TRUE,
  FALSE,
  REGEX,
  CLASS,
  LOAD,
  STORE,
  REF,
  STR,
  BIGINT,
  VAR,
  CONST,
  LET,
  THIS,
  OBJECT,
  ARRAY,
  SUPER_CALL,
  SET_FUNCTION_NAME,
  FUNCTION,
  ASYNCFUNCTION,
  ARROW,
  ASYNCARROW,
  GENERATOR,
  ASYNCGENERATOR,
  ENABLE,
  DISABLE,
  GET_FIELD,
  SET_FIELD,
  SET_ACCESSOR_GETTER,
  SET_ACCESSOR_SETTER,
  SET_METHOD,
  SET_PROP_FIELD,
  SET_PROP_METHOD,
  SET_PROP_ACCESSOR_GETTER,
  SET_PROP_ACCESSOR_SETTER,
  GET_PRIVATE_FIELD,
  SET_PRIVATE_FIELD,
  SET_PRIVATE_ACCESSOR_GETTER,
  SET_PRIVATE_ACCESSOR_SETTER,
  SET_PRIVATE_METHOD,
  SET_PRIVATE_PROP_FIELD,
  SET_PRIVATE_PROP_ACCESSOR_GETTER,
  SET_PRIVATE_PROP_ACCESSOR_SETTER,
  SET_PRIVATE_PROP_METHOD,
  GET_KEYS,
  SET_SUPER_FIELD,
  GET_SUPER_FIELD,
  SET_INITIALIZER,
  SET_PRIVATE_INITIALIZER,
  CALL,
  MEMBER_CALL,
  PRIVATE_MEMBER_CALL,
  VOID,
  TYPEOF,
  NEW,
  DELETE,
  RET,
  YIELD,
  AWAIT,
  THROW,
  YIELD_DELEGATE,
  TRY_BEGIN,
  TRY_END,
  ON_FINISH,
  ON_ERROR,
  DEFER,
  BREAK_LABEL_BEGIN,
  CONTINUE_LABEL_BEGIN,
  SET_LABELE_ADDRESS,
  LABEL_END,
  BREAK,
  CONTINUE,
  JMP,
  JTRUE,
  JFALSE,
  JNULL,
  JNOT_NULL,
  UPLUS,
  UNEG,
  ADD,
  SUB,
  DIV,
  MUL,
  MOD,
  POW,
  AND,
  OR,
  NOT,
  XOR,
  SHR,
  SHL,
  USHR,
  LNOT,
  EQ,
  SEQ,
  NE,
  SNE,
  GT,
  LT,
  GE,
  LE,
  INC,
  DEC,
  UPDATE_INC,
  UPDATE_DEC,
  NEXT,
  AWAIT_NEXT,
  SPREAD,
  MERGE,
  ITERATOR,
  OBJECT_SPREAD,
  ARRAY_SPREAD,
  EMPTY_CHECK,
  ARGUMENT_SPREAD,
  HLT,
  DEBUGGER,
  WITH,
  IMPORT,
  EXPORT,
  EXPORT_ALL,
  ASSERT,
};
