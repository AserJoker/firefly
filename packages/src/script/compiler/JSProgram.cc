#include "script/compiler/JSProgram.hpp"
#include "script/compiler/JSOperator.hpp"
#include <sstream>

std::wstring JSProgram::toString() {
  std::wstringstream ss;
  ss << L"[.section text]" << std::endl;
  for (size_t index = 0; index < constants.size(); index++) {
    ss << L"." << index << ": \"";
    for (auto &ch : constants[index]) {
      if (ch == '\n') {
        ss << L"\\n";
      } else if (ch == L'\r') {
        ss << L"\\r";
      } else if (ch == '\t') {
        ss << L"\\t";
      } else if (ch == '\"') {
        ss << L"\\\"";
      } else if (ch == '\\') {
        ss << L"\\\\";
      } else {
        ss << ch;
      }
    }
    ss << L"\"" << std::endl;
  }
  size_t offset = 0;
  ss << L"[.section code]" << std::endl;
  while (offset < codes.size()) {
    ss << offset << ": ";
    auto opt = (JS_OPERATOR) * (codes.data() + offset);
    offset++;
    switch (opt) {
    case JS_OPERATOR::BEGIN: {
      ss << L"BEGIN";
      break;
    }
    case JS_OPERATOR::END: {
      ss << L"END";
      break;
    }
    case JS_OPERATOR::PUSH: {
      ss << L"PUSH " << *(double *)(codes.data() + offset);
      offset += 4;
      break;
    }
    case JS_OPERATOR::POP: {
      ss << L"POP";
      break;
    }
    case JS_OPERATOR::PUSH_VALUE: {
      ss << L"PUSH_VALUE " << *(uint32_t *)(codes.data() + offset);
      offset += 2;
      break;
    }
    case JS_OPERATOR::NIL: {
      ss << L"NIL";
      break;
    }
    case JS_OPERATOR::UNDEFINED: {
      ss << L"UNDEFINED";
      break;
    }
    case JS_OPERATOR::TRUE: {
      ss << L"TRUE";
      break;
    }
    case JS_OPERATOR::FALSE: {
      ss << L"FALSE";
      break;
    }
    case JS_OPERATOR::REGEX: {
      auto idx = *(uint32_t *)(codes.data() + offset);
      ss << L"REGEX \"" << constants[idx] << L"\"";
      offset += 2;
      break;
    }
    case JS_OPERATOR::LOAD: {
      auto idx = *(uint32_t *)(codes.data() + offset);
      ss << L"LOAD \"" << constants[idx] << L"\"";
      offset += 2;
      break;
    }
    case JS_OPERATOR::STORE: {
      auto idx = *(uint32_t *)(codes.data() + offset);
      ss << L"STORE \"" << constants[idx] << L"\"";
      offset += 2;
      break;
    }
    case JS_OPERATOR::STR: {
      auto idx = *(uint32_t *)(codes.data() + offset);
      ss << L"STR \"" << constants[idx] << L"\"";
      offset += 2;
      break;
    }
    case JS_OPERATOR::REF: {
      auto idx = *(uint32_t *)(codes.data() + offset);
      ss << L"REF \"" << constants[idx] << L"\"";
      offset += 2;
      break;
    }
    case JS_OPERATOR::ENABLE: {
      auto idx = *(uint32_t *)(codes.data() + offset);
      ss << L"ENABLE \"" << constants[idx] << L"\"";
      offset += 2;
      break;
    }
    case JS_OPERATOR::DISABLE: {
      auto idx = *(uint32_t *)(codes.data() + offset);
      ss << L"DISABLE \"" << constants[idx] << L"\"";
      offset += 2;
      break;
    }
    case JS_OPERATOR::THIS: {
      ss << L"THIS";
      break;
    }
    case JS_OPERATOR::SUPER_CALL: {
      ss << L"SUPER_CALL";
      break;
    }
    case JS_OPERATOR::GET_FIELD: {
      ss << L"GET_FIELD";
      break;
    }
    case JS_OPERATOR::SET_FIELD: {
      ss << L"SET_FIELD";
      break;
    }
    case JS_OPERATOR::SET_ACCESSOR_GETTER: {
      ss << L"SET_ACCESSOR_GETTER";
      break;
    }
    case JS_OPERATOR::SET_ACCESSOR_SETTER: {
      ss << L"SET_ACCESSOR_SETTER";
      break;
    }
    case JS_OPERATOR::SET_METHOD: {
      ss << L"SET_METHOD";
      break;
    }
    case JS_OPERATOR::SET_PROP_FIELD: {
      ss << L"SET_PROP_FIELD";
      break;
    }
    case JS_OPERATOR::SET_PROP_ACCESSOR_GETTER: {
      ss << L"SET_PROP_ACCESSOR_GETTER";
      break;
    }
    case JS_OPERATOR::SET_PROP_ACCESSOR_SETTER: {
      ss << L"SET_PROP_ACCESSOR_SETTER";
      break;
    }
    case JS_OPERATOR::SET_PROP_METHOD: {
      ss << L"SET_PROP_METHOD";
      break;
    }
    case JS_OPERATOR::GET_SUPER_FIELD: {
      ss << L"GET_SUPER_FIELD";
      break;
    }
    case JS_OPERATOR::SET_SUPER_FIELD: {
      ss << L"SET_SUPER_FIELD";
      break;
    }
    case JS_OPERATOR::CALL: {
      ss << L"CALL";
      break;
    }
    case JS_OPERATOR::MEMBER_CALL: {
      ss << L"MEMBER_CALL";
      break;
    }
    case JS_OPERATOR::PRIVATE_MEMBER_CALL: {
      ss << L"PRIVATE_MEMBER_CALL";
      break;
    }
    case JS_OPERATOR::VOID: {
      ss << L"VOID";
      break;
    }
    case JS_OPERATOR::TYPEOF: {
      ss << L"TYPEOF";
      break;
    }
    case JS_OPERATOR::NEW: {
      ss << L"NEW " << *(uint32_t *)(codes.data() + offset);
      offset += 2;
      break;
    }
    case JS_OPERATOR::DELETE: {
      ss << L"DELETE";
      break;
    }
    case JS_OPERATOR::RET: {
      ss << L"RET";
      break;
    }
    case JS_OPERATOR::YIELD: {
      ss << L"YIELD";
      break;
    }
    case JS_OPERATOR::AWAIT: {
      ss << L"AWAIT";
      break;
    }
    case JS_OPERATOR::ARROW: {
      ss << L"ARROW " << *(uint64_t *)(codes.data() + offset);
      offset += 4;
      break;
    }
    case JS_OPERATOR::ASYNCARROW: {
      ss << L"ASYNCARROW " << *(uint64_t *)(codes.data() + offset);
      offset += 4;
      break;
    }
    case JS_OPERATOR::YIELD_DELEGATE: {
      ss << L"YIELD_DELEGATE";
      break;
    }
    case JS_OPERATOR::JMP: {
      ss << L"JMP " << *(uint64_t *)(codes.data() + offset);
      offset += 4;
      break;
    }
    case JS_OPERATOR::JTRUE: {
      ss << L"JTRUE " << *(uint64_t *)(codes.data() + offset);
      offset += 4;
      break;
    }
    case JS_OPERATOR::JFALSE: {
      ss << L"JFALSE " << *(uint64_t *)(codes.data() + offset);
      offset += 4;
      break;
    }
    case JS_OPERATOR::JNULL: {
      ss << L"JNULL " << *(uint64_t *)(codes.data() + offset);
      offset += 4;
      break;
    }
    case JS_OPERATOR::JNOT_NULL: {
      ss << L"JNOT_NULL " << *(uint64_t *)(codes.data() + offset);
      offset += 4;
      break;
    }
    case JS_OPERATOR::UPLUS: {
      ss << L"UPLUS";
      break;
    }
    case JS_OPERATOR::UNEG: {
      ss << L"UNEG";
      break;
    }
    case JS_OPERATOR::ADD: {
      ss << L"ADD";
      break;
    }
    case JS_OPERATOR::SUB: {
      ss << L"SUB";
      break;
    }
    case JS_OPERATOR::DIV: {
      ss << L"DIV";
      break;
    }
    case JS_OPERATOR::MUL: {
      ss << L"MUL";
      break;
    }
    case JS_OPERATOR::MOD: {
      ss << L"MOD";
      break;
    }
    case JS_OPERATOR::POW: {
      ss << L"POW";
      break;
    }
    case JS_OPERATOR::AND: {
      ss << L"AND";
      break;
    }
    case JS_OPERATOR::OR: {
      ss << L"OR";
      break;
    }
    case JS_OPERATOR::NOT: {
      ss << L"NOT";
      break;
    }
    case JS_OPERATOR::LNOT: {
      ss << L"LNOT";
      break;
    }
    case JS_OPERATOR::XOR: {
      ss << L"XOR";
      break;
    }
    case JS_OPERATOR::SHR: {
      ss << L"SHR";
      break;
    }
    case JS_OPERATOR::SHL: {
      ss << L"SHL";
      break;
    }
    case JS_OPERATOR::USHR: {
      ss << L"USHR";
      break;
    }
    case JS_OPERATOR::EQ: {
      ss << L"EQ";
      break;
    }
    case JS_OPERATOR::SEQ: {
      ss << L"SEQ";
      break;
    }
    case JS_OPERATOR::NE: {
      ss << L"NE";
      break;
    }
    case JS_OPERATOR::SNE: {
      ss << L"SNE";
      break;
    }
    case JS_OPERATOR::GT: {
      ss << L"GT";
      break;
    }
    case JS_OPERATOR::LT: {
      ss << L"LT";
      break;
    }
    case JS_OPERATOR::GE: {
      ss << L"GE";
      break;
    }
    case JS_OPERATOR::LE: {
      ss << L"LE";
      break;
    }
    case JS_OPERATOR::INC: {
      ss << L"INC";
      break;
    }
    case JS_OPERATOR::DEC: {
      ss << L"DEC";
      break;
    }
    case JS_OPERATOR::UPDATE_INC: {
      ss << L"UPDATE_INC";
      break;
    }
    case JS_OPERATOR::UPDATE_DEC: {
      ss << L"UPDATE_DEC";
      break;
    }
    case JS_OPERATOR::NEXT: {
      ss << L"NEXT";
      break;
    }
    case JS_OPERATOR::AWAIT_NEXT: {
      ss << L"AWAIT_NEXT";
      break;
    }
    case JS_OPERATOR::OBJECT_SPREAD: {
      ss << L"OBJECT_SPREAD " << *(uint32_t *)(codes.data() + offset);
      offset += 2;
      break;
    }
    case JS_OPERATOR::ARRAY_SPREAD: {
      ss << L"ARRAY_SPREAD";
      break;
    }
    case JS_OPERATOR::ARGUMENT_SPREAD: {
      ss << L"ARGUMENT_SPREAD";
      break;
    }
    case JS_OPERATOR::VAR: {
      auto idx = *(uint32_t *)(codes.data() + offset);
      ss << L"VAR \"" << constants[idx] << L"\"";
      offset += 2;
      break;
    }
    case JS_OPERATOR::CONST: {
      auto idx = *(uint32_t *)(codes.data() + offset);
      ss << L"CONST \"" << constants[idx] << L"\"";
      offset += 2;
      break;
    }
    case JS_OPERATOR::LET: {
      auto idx = *(uint32_t *)(codes.data() + offset);
      ss << L"LET \"" << constants[idx] << L"\"";
      offset += 2;
      break;
    }
    case JS_OPERATOR::FUNCTION: {
      ss << L"FUNCTION " << *(uint64_t *)(codes.data() + offset);
      offset += 4;
      break;
    }
    case JS_OPERATOR::ASYNCFUNCTION: {
      ss << L"ASYNCFUNCTION " << *(uint64_t *)(codes.data() + offset);
      offset += 4;
      break;
    }
    case JS_OPERATOR::GENERATOR: {
      ss << L"GENERATOR " << *(uint64_t *)(codes.data() + offset);
      offset += 4;
      break;
    }
    case JS_OPERATOR::ASYNCGENERATOR: {
      ss << L"ASYNCGENERATOR " << *(uint64_t *)(codes.data() + offset);
      offset += 4;
      break;
    }
    case JS_OPERATOR::HLT: {
      ss << L"HLT";
      break;
    }
    case JS_OPERATOR::DEBUGGER: {
      ss << L"DEBUGGER";
      break;
    }

    case JS_OPERATOR::BREAK_LABEL_BEGIN: {
      auto idx = *(uint32_t *)(codes.data() + offset);
      ss << L"BREAK_LABEL_BEGIN \"" << constants[idx] << L"\"";
      offset += 2;
      break;
    }
    case JS_OPERATOR::CONTINUE_LABEL_BEGIN: {
      auto idx = *(uint32_t *)(codes.data() + offset);
      ss << L"CONTINUE_LABEL_BEGIN \"" << constants[idx] << L"\"";
      offset += 2;
      break;
    }
    case JS_OPERATOR::LABEL_END: {
      ss << L"LABEL_END";
      break;
    }
    case JS_OPERATOR::BREAK: {
      auto idx = *(uint32_t *)(codes.data() + offset);
      ss << L"BREAK \"" << constants[idx] << L"\"";
      offset += 2;
      break;
    }
    case JS_OPERATOR::CONTINUE: {
      auto idx = *(uint32_t *)(codes.data() + offset);
      ss << L"CONTINUE \"" << constants[idx] << L"\"";
      offset += 2;
      break;
    }
    case JS_OPERATOR::SET_LABELE_ADDRESS: {
      ss << L"SET_LABELE_ADDRESS " << *(uint64_t *)(codes.data() + offset);
      offset += 4;
      break;
    }
    case JS_OPERATOR::SPREAD: {
      ss << L"SPREAD " << *(uint32_t *)(codes.data() + offset);
      offset += 2;
      break;
    }
    case JS_OPERATOR::MERGE: {
      ss << L"MERGE";
      break;
    }
    case JS_OPERATOR::GET_KEYS: {
      ss << L"GET_KEYS";
      break;
    }
    case JS_OPERATOR::TRY_BEGIN: {
      ss << L"TRY_BEGIN";
      break;
    }
    case JS_OPERATOR::TRY_END: {
      ss << L"TRY_END";
      break;
    }
    case JS_OPERATOR::DEFER: {
      ss << L"DEFER";
      break;
    }
    case JS_OPERATOR::ON_FINISH: {
      ss << L"ON_FINISH " << *(uint64_t *)(codes.data() + offset);
      offset += 4;
      break;
    }
    case JS_OPERATOR::ON_ERROR: {
      ss << L"ON_ERROR " << *(uint64_t *)(codes.data() + offset);
      offset += 4;
      break;
    }
    case JS_OPERATOR::THROW: {
      ss << L"THROW";
      break;
    }
    case JS_OPERATOR::BIGINT: {
      auto idx = *(uint32_t *)(codes.data() + offset);
      ss << L"BIGINT \"" << constants[idx] << L"\"";
      offset += 2;
      break;
    }
    case JS_OPERATOR::OBJECT: {
      ss << L"OBJECT";
      break;
    }
    case JS_OPERATOR::ARRAY: {
      ss << L"ARRAY";
      break;
    }
    case JS_OPERATOR::EMPTY_CHECK: {
      ss << L"EMPTY_CHECK";
      break;
    }
    case JS_OPERATOR::CLASS: {
      ss << L"CLASS";
      break;
    }
    case JS_OPERATOR::WITH: {
      ss << L"WITH";
      break;
    }
    case JS_OPERATOR::SET_INITIALIZER: {
      ss << L"SET_INITIALIZER " << *(uint64_t *)(codes.data() + offset);
      offset += 4;
      break;
    }
    case JS_OPERATOR::IMPORT: {
      auto idx = *(uint32_t *)(codes.data() + offset);
      ss << L"IMPORT \"" << constants[idx] << L"\"";
      offset += 2;
      break;
    }
    case JS_OPERATOR::EXPORT: {
      auto idx = *(uint32_t *)(codes.data() + offset);
      ss << L"EXPORT \"" << constants[idx] << L"\"";
      offset += 2;
      break;
    }
    case JS_OPERATOR::ASSERT: {
      auto idx = *(uint32_t *)(codes.data() + offset);
      ss << L"ASSERT \"" << constants[idx] << L"\"";
      offset += 2;
      break;
    }
    case JS_OPERATOR::EXPORT_ALL:
      ss << L"EXPORT_ALL";
      break;
    case JS_OPERATOR::ITERATOR:
      ss << L"ITERATOR";
      break;
    case JS_OPERATOR::GET_PRIVATE_FIELD: {
      ss << L"GET_PRIVATE_FIELD";
      break;
    }
    case JS_OPERATOR::SET_PRIVATE_FIELD: {
      ss << L"SET_PRIVATE_FIELD";
      break;
    }
    case JS_OPERATOR::SET_PRIVATE_ACCESSOR_GETTER: {
      ss << L"SET_PRIVATE_ACCESSOR_GETTER";
      break;
    }
    case JS_OPERATOR::SET_PRIVATE_ACCESSOR_SETTER: {
      ss << L"SET_PRIVATE_ACCESSOR_SETTER";
      break;
    }
    case JS_OPERATOR::SET_PRIVATE_METHOD: {
      ss << L"SET_PRIVATE_METHOD";
      break;
    }
    case JS_OPERATOR::SET_PRIVATE_PROP_FIELD: {
      ss << L"SET_PRIVATE_PROP_FIELD";
      break;
    }
    case JS_OPERATOR::SET_PRIVATE_PROP_ACCESSOR_GETTER: {
      ss << L"SET_PRIVATE_PROP_ACCESSOR_GETTER";
      break;
    }
    case JS_OPERATOR::SET_PRIVATE_PROP_ACCESSOR_SETTER: {
      ss << L"SET_PRIVATE_PROP_ACCESSOR_SETTER";
      break;
    }
    case JS_OPERATOR::SET_PRIVATE_PROP_METHOD: {
      ss << L"SET_PRIVATE_PROP_METHOD";
      break;
    }
    case JS_OPERATOR::SET_PRIVATE_INITIALIZER: {
      ss << L"SET_PRIVATE_INITIALIZER " << *(uint64_t *)(codes.data() + offset);
      offset += 4;
      break;
    }
    case JS_OPERATOR::SET_FUNCTION_NAME: {
      auto idx = *(uint32_t *)(codes.data() + offset);
      ss << L"SET_FUNCTION_NAME \"" << constants[idx] << L"\"";
      offset += 2;
      break;
    }
    }
    ss << std::endl;
  }
  ss << L"[.section stack]" << std::endl;
  for (auto &[offset, frame] : stacks) {
    ss << L"." << offset << L": \"" << frame.position.funcname << L" ( "
       << frame.filename << L":" << frame.position.line << L":"
       << frame.position.column << L" )\"" << std::endl;
  }
  return ss.str();
}