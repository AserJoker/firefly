#include "script/lib/Module_Input.hpp"
#include "core/Singleton.hpp"
#include "input/Keyboard.hpp"
#include "input/Mouse.hpp"
#include <SDL_scancode.h>
using namespace firefly;
using namespace firefly::script;
void Module_Input::open(core::AutoPtr<Script> ctx) {
  auto exports = ctx->createValue()
                     ->setObject(ctx)
                     ->setFunctionField(ctx, captureMouse)
                     ->setFunctionField(ctx, releaseMouse)
                     ->setFunctionField(ctx, getMousePosition)
                     ->setFunctionField(ctx, getKeyState);
  auto scancodes =
      ctx->createValue()
          ->setObject(ctx)
          ->setField(ctx, "A", ctx->createValue(SDL_SCANCODE_A))
          ->setField(ctx, "B", ctx->createValue(SDL_SCANCODE_B))
          ->setField(ctx, "C", ctx->createValue(SDL_SCANCODE_C))
          ->setField(ctx, "D", ctx->createValue(SDL_SCANCODE_D))
          ->setField(ctx, "E", ctx->createValue(SDL_SCANCODE_E))
          ->setField(ctx, "F", ctx->createValue(SDL_SCANCODE_F))
          ->setField(ctx, "G", ctx->createValue(SDL_SCANCODE_G))
          ->setField(ctx, "H", ctx->createValue(SDL_SCANCODE_H))
          ->setField(ctx, "I", ctx->createValue(SDL_SCANCODE_I))
          ->setField(ctx, "J", ctx->createValue(SDL_SCANCODE_J))
          ->setField(ctx, "K", ctx->createValue(SDL_SCANCODE_K))
          ->setField(ctx, "L", ctx->createValue(SDL_SCANCODE_L))
          ->setField(ctx, "M", ctx->createValue(SDL_SCANCODE_M))
          ->setField(ctx, "N", ctx->createValue(SDL_SCANCODE_N))
          ->setField(ctx, "O", ctx->createValue(SDL_SCANCODE_O))
          ->setField(ctx, "P", ctx->createValue(SDL_SCANCODE_P))
          ->setField(ctx, "Q", ctx->createValue(SDL_SCANCODE_Q))
          ->setField(ctx, "R", ctx->createValue(SDL_SCANCODE_R))
          ->setField(ctx, "S", ctx->createValue(SDL_SCANCODE_S))
          ->setField(ctx, "T", ctx->createValue(SDL_SCANCODE_T))
          ->setField(ctx, "U", ctx->createValue(SDL_SCANCODE_U))
          ->setField(ctx, "V", ctx->createValue(SDL_SCANCODE_V))
          ->setField(ctx, "W", ctx->createValue(SDL_SCANCODE_W))
          ->setField(ctx, "X", ctx->createValue(SDL_SCANCODE_X))
          ->setField(ctx, "Y", ctx->createValue(SDL_SCANCODE_Y))
          ->setField(ctx, "Z", ctx->createValue(SDL_SCANCODE_Z))
          ->setField(ctx, "NUM1", ctx->createValue(SDL_SCANCODE_1))
          ->setField(ctx, "NUM2", ctx->createValue(SDL_SCANCODE_2))
          ->setField(ctx, "NUM3", ctx->createValue(SDL_SCANCODE_3))
          ->setField(ctx, "NUM4", ctx->createValue(SDL_SCANCODE_4))
          ->setField(ctx, "NUM5", ctx->createValue(SDL_SCANCODE_5))
          ->setField(ctx, "NUM6", ctx->createValue(SDL_SCANCODE_6))
          ->setField(ctx, "NUM7", ctx->createValue(SDL_SCANCODE_7))
          ->setField(ctx, "NUM8", ctx->createValue(SDL_SCANCODE_8))
          ->setField(ctx, "NUM9", ctx->createValue(SDL_SCANCODE_9))
          ->setField(ctx, "NUM0", ctx->createValue(SDL_SCANCODE_0))
          ->setField(ctx, "RETURN", ctx->createValue(SDL_SCANCODE_RETURN))
          ->setField(ctx, "ESCAPE", ctx->createValue(SDL_SCANCODE_ESCAPE))
          ->setField(ctx, "BACKSPACE", ctx->createValue(SDL_SCANCODE_BACKSPACE))
          ->setField(ctx, "TAB", ctx->createValue(SDL_SCANCODE_TAB))
          ->setField(ctx, "SPACE", ctx->createValue(SDL_SCANCODE_SPACE))
          ->setField(ctx, "MINUS", ctx->createValue(SDL_SCANCODE_MINUS))
          ->setField(ctx, "EQUALS", ctx->createValue(SDL_SCANCODE_EQUALS))
          ->setField(ctx, "LEFTBRACKET",
                     ctx->createValue(SDL_SCANCODE_LEFTBRACKET))
          ->setField(ctx, "RIGHTBRACKET",
                     ctx->createValue(SDL_SCANCODE_RIGHTBRACKET))
          ->setField(ctx, "BACKSLASH", ctx->createValue(SDL_SCANCODE_BACKSLASH))
          ->setField(ctx, "NONUSHASH", ctx->createValue(SDL_SCANCODE_NONUSHASH))
          ->setField(ctx, "SEMICOLON", ctx->createValue(SDL_SCANCODE_SEMICOLON))
          ->setField(ctx, "APOSTROPHE",
                     ctx->createValue(SDL_SCANCODE_APOSTROPHE))
          ->setField(ctx, "GRAVE", ctx->createValue(SDL_SCANCODE_GRAVE))
          ->setField(ctx, "COMMA", ctx->createValue(SDL_SCANCODE_COMMA))
          ->setField(ctx, "PERIOD", ctx->createValue(SDL_SCANCODE_PERIOD))
          ->setField(ctx, "SLASH", ctx->createValue(SDL_SCANCODE_SLASH))
          ->setField(ctx, "CAPSLOCK", ctx->createValue(SDL_SCANCODE_CAPSLOCK))
          ->setField(ctx, "F1", ctx->createValue(SDL_SCANCODE_F1))
          ->setField(ctx, "F2", ctx->createValue(SDL_SCANCODE_F2))
          ->setField(ctx, "F3", ctx->createValue(SDL_SCANCODE_F3))
          ->setField(ctx, "F4", ctx->createValue(SDL_SCANCODE_F4))
          ->setField(ctx, "F5", ctx->createValue(SDL_SCANCODE_F5))
          ->setField(ctx, "F6", ctx->createValue(SDL_SCANCODE_F6))
          ->setField(ctx, "F7", ctx->createValue(SDL_SCANCODE_F7))
          ->setField(ctx, "F8", ctx->createValue(SDL_SCANCODE_F8))
          ->setField(ctx, "F9", ctx->createValue(SDL_SCANCODE_F9))
          ->setField(ctx, "F10", ctx->createValue(SDL_SCANCODE_F10))
          ->setField(ctx, "F11", ctx->createValue(SDL_SCANCODE_F11))
          ->setField(ctx, "F12", ctx->createValue(SDL_SCANCODE_F12))
          ->setField(ctx, "PRINTSCREEN",
                     ctx->createValue(SDL_SCANCODE_PRINTSCREEN))
          ->setField(ctx, "SCROLLLOCK",
                     ctx->createValue(SDL_SCANCODE_SCROLLLOCK))
          ->setField(ctx, "PAUSE", ctx->createValue(SDL_SCANCODE_PAUSE))
          ->setField(ctx, "INSERT", ctx->createValue(SDL_SCANCODE_INSERT))
          ->setField(ctx, "HOME", ctx->createValue(SDL_SCANCODE_HOME))
          ->setField(ctx, "PAGEUP", ctx->createValue(SDL_SCANCODE_PAGEUP))
          ->setField(ctx, "DELETE", ctx->createValue(SDL_SCANCODE_DELETE))
          ->setField(ctx, "END", ctx->createValue(SDL_SCANCODE_END))
          ->setField(ctx, "PAGEDOWN", ctx->createValue(SDL_SCANCODE_PAGEDOWN))
          ->setField(ctx, "RIGHT", ctx->createValue(SDL_SCANCODE_RIGHT))
          ->setField(ctx, "LEFT", ctx->createValue(SDL_SCANCODE_LEFT))
          ->setField(ctx, "DOWN", ctx->createValue(SDL_SCANCODE_DOWN))
          ->setField(ctx, "UP", ctx->createValue(SDL_SCANCODE_UP))
          ->setField(ctx, "NUMLOCKCLEAR",
                     ctx->createValue(SDL_SCANCODE_NUMLOCKCLEAR))
          ->setField(ctx, "KP_DIVIDE", ctx->createValue(SDL_SCANCODE_KP_DIVIDE))
          ->setField(ctx, "KP_MULTIPLY",
                     ctx->createValue(SDL_SCANCODE_KP_MULTIPLY))
          ->setField(ctx, "KP_MINUS", ctx->createValue(SDL_SCANCODE_KP_MINUS))
          ->setField(ctx, "KP_PLUS", ctx->createValue(SDL_SCANCODE_KP_PLUS))
          ->setField(ctx, "KP_ENTER", ctx->createValue(SDL_SCANCODE_KP_ENTER))
          ->setField(ctx, "KP_1", ctx->createValue(SDL_SCANCODE_KP_1))
          ->setField(ctx, "KP_2", ctx->createValue(SDL_SCANCODE_KP_2))
          ->setField(ctx, "KP_3", ctx->createValue(SDL_SCANCODE_KP_3))
          ->setField(ctx, "KP_4", ctx->createValue(SDL_SCANCODE_KP_4))
          ->setField(ctx, "KP_5", ctx->createValue(SDL_SCANCODE_KP_5))
          ->setField(ctx, "KP_6", ctx->createValue(SDL_SCANCODE_KP_6))
          ->setField(ctx, "KP_7", ctx->createValue(SDL_SCANCODE_KP_7))
          ->setField(ctx, "KP_8", ctx->createValue(SDL_SCANCODE_KP_8))
          ->setField(ctx, "KP_9", ctx->createValue(SDL_SCANCODE_KP_9))
          ->setField(ctx, "KP_0", ctx->createValue(SDL_SCANCODE_KP_0))
          ->setField(ctx, "KP_PERIOD", ctx->createValue(SDL_SCANCODE_KP_PERIOD))
          ->setField(ctx, "NONUSBACKSLASH",
                     ctx->createValue(SDL_SCANCODE_NONUSBACKSLASH))
          ->setField(ctx, "APPLICATION",
                     ctx->createValue(SDL_SCANCODE_APPLICATION))
          ->setField(ctx, "POWER", ctx->createValue(SDL_SCANCODE_POWER))
          ->setField(ctx, "KP_EQUALS", ctx->createValue(SDL_SCANCODE_KP_EQUALS))
          ->setField(ctx, "F13", ctx->createValue(SDL_SCANCODE_F13))
          ->setField(ctx, "F14", ctx->createValue(SDL_SCANCODE_F14))
          ->setField(ctx, "F15", ctx->createValue(SDL_SCANCODE_F15))
          ->setField(ctx, "F16", ctx->createValue(SDL_SCANCODE_F16))
          ->setField(ctx, "F17", ctx->createValue(SDL_SCANCODE_F17))
          ->setField(ctx, "F18", ctx->createValue(SDL_SCANCODE_F18))
          ->setField(ctx, "F19", ctx->createValue(SDL_SCANCODE_F19))
          ->setField(ctx, "F20", ctx->createValue(SDL_SCANCODE_F20))
          ->setField(ctx, "F21", ctx->createValue(SDL_SCANCODE_F21))
          ->setField(ctx, "F22", ctx->createValue(SDL_SCANCODE_F22))
          ->setField(ctx, "F23", ctx->createValue(SDL_SCANCODE_F23))
          ->setField(ctx, "F24", ctx->createValue(SDL_SCANCODE_F24))
          ->setField(ctx, "EXECUTE", ctx->createValue(SDL_SCANCODE_EXECUTE))
          ->setField(ctx, "HELP", ctx->createValue(SDL_SCANCODE_HELP))
          ->setField(ctx, "MENU", ctx->createValue(SDL_SCANCODE_MENU))
          ->setField(ctx, "SELECT", ctx->createValue(SDL_SCANCODE_SELECT))
          ->setField(ctx, "STOP", ctx->createValue(SDL_SCANCODE_STOP))
          ->setField(ctx, "AGAIN", ctx->createValue(SDL_SCANCODE_AGAIN))
          ->setField(ctx, "UNDO", ctx->createValue(SDL_SCANCODE_UNDO))
          ->setField(ctx, "CUT", ctx->createValue(SDL_SCANCODE_CUT))
          ->setField(ctx, "COPY", ctx->createValue(SDL_SCANCODE_COPY))
          ->setField(ctx, "PASTE", ctx->createValue(SDL_SCANCODE_PASTE))
          ->setField(ctx, "FIND", ctx->createValue(SDL_SCANCODE_FIND))
          ->setField(ctx, "MUTE", ctx->createValue(SDL_SCANCODE_MUTE))
          ->setField(ctx, "VOLUMEUP", ctx->createValue(SDL_SCANCODE_VOLUMEUP))
          ->setField(ctx, "VOLUMEDOWN",
                     ctx->createValue(SDL_SCANCODE_VOLUMEDOWN))
          ->setField(ctx, "KP_COMMA", ctx->createValue(SDL_SCANCODE_KP_COMMA))
          ->setField(ctx, "KP_EQUALSAS400",
                     ctx->createValue(SDL_SCANCODE_KP_EQUALSAS400))
          ->setField(ctx, "INTERNATIONAL1",
                     ctx->createValue(SDL_SCANCODE_INTERNATIONAL1))
          ->setField(ctx, "INTERNATIONAL2",
                     ctx->createValue(SDL_SCANCODE_INTERNATIONAL2))
          ->setField(ctx, "INTERNATIONAL3",
                     ctx->createValue(SDL_SCANCODE_INTERNATIONAL3))
          ->setField(ctx, "INTERNATIONAL4",
                     ctx->createValue(SDL_SCANCODE_INTERNATIONAL4))
          ->setField(ctx, "INTERNATIONAL5",
                     ctx->createValue(SDL_SCANCODE_INTERNATIONAL5))
          ->setField(ctx, "INTERNATIONAL6",
                     ctx->createValue(SDL_SCANCODE_INTERNATIONAL6))
          ->setField(ctx, "INTERNATIONAL7",
                     ctx->createValue(SDL_SCANCODE_INTERNATIONAL7))
          ->setField(ctx, "INTERNATIONAL8",
                     ctx->createValue(SDL_SCANCODE_INTERNATIONAL8))
          ->setField(ctx, "INTERNATIONAL9",
                     ctx->createValue(SDL_SCANCODE_INTERNATIONAL9))
          ->setField(ctx, "LANG1", ctx->createValue(SDL_SCANCODE_LANG1))
          ->setField(ctx, "LANG2", ctx->createValue(SDL_SCANCODE_LANG2))
          ->setField(ctx, "LANG3", ctx->createValue(SDL_SCANCODE_LANG3))
          ->setField(ctx, "LANG4", ctx->createValue(SDL_SCANCODE_LANG4))
          ->setField(ctx, "LANG5", ctx->createValue(SDL_SCANCODE_LANG5))
          ->setField(ctx, "LANG6", ctx->createValue(SDL_SCANCODE_LANG6))
          ->setField(ctx, "LANG7", ctx->createValue(SDL_SCANCODE_LANG7))
          ->setField(ctx, "LANG8", ctx->createValue(SDL_SCANCODE_LANG8))
          ->setField(ctx, "LANG9", ctx->createValue(SDL_SCANCODE_LANG9))
          ->setField(ctx, "ALTERASE", ctx->createValue(SDL_SCANCODE_ALTERASE))
          ->setField(ctx, "SYSREQ", ctx->createValue(SDL_SCANCODE_SYSREQ))
          ->setField(ctx, "CANCEL", ctx->createValue(SDL_SCANCODE_CANCEL))
          ->setField(ctx, "CLEAR", ctx->createValue(SDL_SCANCODE_CLEAR))
          ->setField(ctx, "PRIOR", ctx->createValue(SDL_SCANCODE_PRIOR))
          ->setField(ctx, "RETURN2", ctx->createValue(SDL_SCANCODE_RETURN2))
          ->setField(ctx, "SEPARATOR", ctx->createValue(SDL_SCANCODE_SEPARATOR))
          ->setField(ctx, "OUT", ctx->createValue(SDL_SCANCODE_OUT))
          ->setField(ctx, "OPER", ctx->createValue(SDL_SCANCODE_OPER))
          ->setField(ctx, "CLEARAGAIN",
                     ctx->createValue(SDL_SCANCODE_CLEARAGAIN))
          ->setField(ctx, "CRSEL", ctx->createValue(SDL_SCANCODE_CRSEL))
          ->setField(ctx, "EXSEL", ctx->createValue(SDL_SCANCODE_EXSEL))
          ->setField(ctx, "KP_00", ctx->createValue(SDL_SCANCODE_KP_00))
          ->setField(ctx, "KP_000", ctx->createValue(SDL_SCANCODE_KP_000))
          ->setField(ctx, "THOUSANDSSEPARATOR",
                     ctx->createValue(SDL_SCANCODE_THOUSANDSSEPARATOR))
          ->setField(ctx, "DECIMALSEPARATOR",
                     ctx->createValue(SDL_SCANCODE_DECIMALSEPARATOR))
          ->setField(ctx, "CURRENCYUNIT",
                     ctx->createValue(SDL_SCANCODE_CURRENCYUNIT))
          ->setField(ctx, "CURRENCYSUBUNIT",
                     ctx->createValue(SDL_SCANCODE_CURRENCYSUBUNIT))
          ->setField(ctx, "KP_LEFTPAREN",
                     ctx->createValue(SDL_SCANCODE_KP_LEFTPAREN))
          ->setField(ctx, "KP_RIGHTPAREN",
                     ctx->createValue(SDL_SCANCODE_KP_RIGHTPAREN))
          ->setField(ctx, "KP_LEFTBRACE",
                     ctx->createValue(SDL_SCANCODE_KP_LEFTBRACE))
          ->setField(ctx, "KP_RIGHTBRACE",
                     ctx->createValue(SDL_SCANCODE_KP_RIGHTBRACE))
          ->setField(ctx, "KP_TAB", ctx->createValue(SDL_SCANCODE_KP_TAB))
          ->setField(ctx, "KP_BACKSPACE",
                     ctx->createValue(SDL_SCANCODE_KP_BACKSPACE))
          ->setField(ctx, "KP_A", ctx->createValue(SDL_SCANCODE_KP_A))
          ->setField(ctx, "KP_B", ctx->createValue(SDL_SCANCODE_KP_B))
          ->setField(ctx, "KP_C", ctx->createValue(SDL_SCANCODE_KP_C))
          ->setField(ctx, "KP_D", ctx->createValue(SDL_SCANCODE_KP_D))
          ->setField(ctx, "KP_E", ctx->createValue(SDL_SCANCODE_KP_E))
          ->setField(ctx, "KP_F", ctx->createValue(SDL_SCANCODE_KP_F))
          ->setField(ctx, "KP_XOR", ctx->createValue(SDL_SCANCODE_KP_XOR))
          ->setField(ctx, "KP_POWER", ctx->createValue(SDL_SCANCODE_KP_POWER))
          ->setField(ctx, "KP_PERCENT",
                     ctx->createValue(SDL_SCANCODE_KP_PERCENT))
          ->setField(ctx, "KP_LESS", ctx->createValue(SDL_SCANCODE_KP_LESS))
          ->setField(ctx, "KP_GREATER",
                     ctx->createValue(SDL_SCANCODE_KP_GREATER))
          ->setField(ctx, "KP_AMPERSAND",
                     ctx->createValue(SDL_SCANCODE_KP_AMPERSAND))
          ->setField(ctx, "KP_DBLAMPERSAND",
                     ctx->createValue(SDL_SCANCODE_KP_DBLAMPERSAND))
          ->setField(ctx, "KP_VERTICALBAR",
                     ctx->createValue(SDL_SCANCODE_KP_VERTICALBAR))
          ->setField(ctx, "KP_DBLVERTICALBAR",
                     ctx->createValue(SDL_SCANCODE_KP_DBLVERTICALBAR))
          ->setField(ctx, "KP_COLON", ctx->createValue(SDL_SCANCODE_KP_COLON))
          ->setField(ctx, "KP_HASH", ctx->createValue(SDL_SCANCODE_KP_HASH))
          ->setField(ctx, "KP_SPACE", ctx->createValue(SDL_SCANCODE_KP_SPACE))
          ->setField(ctx, "KP_AT", ctx->createValue(SDL_SCANCODE_KP_AT))
          ->setField(ctx, "KP_EXCLAM", ctx->createValue(SDL_SCANCODE_KP_EXCLAM))
          ->setField(ctx, "KP_MEMSTORE",
                     ctx->createValue(SDL_SCANCODE_KP_MEMSTORE))
          ->setField(ctx, "KP_MEMRECALL",
                     ctx->createValue(SDL_SCANCODE_KP_MEMRECALL))
          ->setField(ctx, "KP_MEMCLEAR",
                     ctx->createValue(SDL_SCANCODE_KP_MEMCLEAR))
          ->setField(ctx, "KP_MEMADD", ctx->createValue(SDL_SCANCODE_KP_MEMADD))
          ->setField(ctx, "KP_MEMSUBTRACT",
                     ctx->createValue(SDL_SCANCODE_KP_MEMSUBTRACT))
          ->setField(ctx, "KP_MEMMULTIPLY",
                     ctx->createValue(SDL_SCANCODE_KP_MEMMULTIPLY))
          ->setField(ctx, "KP_MEMDIVIDE",
                     ctx->createValue(SDL_SCANCODE_KP_MEMDIVIDE))
          ->setField(ctx, "KP_PLUSMINUS",
                     ctx->createValue(SDL_SCANCODE_KP_PLUSMINUS))
          ->setField(ctx, "KP_CLEAR", ctx->createValue(SDL_SCANCODE_KP_CLEAR))
          ->setField(ctx, "KP_CLEARENTRY",
                     ctx->createValue(SDL_SCANCODE_KP_CLEARENTRY))
          ->setField(ctx, "KP_BINARY", ctx->createValue(SDL_SCANCODE_KP_BINARY))
          ->setField(ctx, "KP_OCTAL", ctx->createValue(SDL_SCANCODE_KP_OCTAL))
          ->setField(ctx, "KP_DECIMAL",
                     ctx->createValue(SDL_SCANCODE_KP_DECIMAL))
          ->setField(ctx, "KP_HEXADECIMAL",
                     ctx->createValue(SDL_SCANCODE_KP_HEXADECIMAL))
          ->setField(ctx, "LCTRL", ctx->createValue(SDL_SCANCODE_LCTRL))
          ->setField(ctx, "LSHIFT", ctx->createValue(SDL_SCANCODE_LSHIFT))
          ->setField(ctx, "LALT", ctx->createValue(SDL_SCANCODE_LALT))
          ->setField(ctx, "LGUI", ctx->createValue(SDL_SCANCODE_LGUI))
          ->setField(ctx, "RCTRL", ctx->createValue(SDL_SCANCODE_RCTRL))
          ->setField(ctx, "RSHIFT", ctx->createValue(SDL_SCANCODE_RSHIFT))
          ->setField(ctx, "RALT", ctx->createValue(SDL_SCANCODE_RALT))
          ->setField(ctx, "RGUI", ctx->createValue(SDL_SCANCODE_RGUI))
          ->setField(ctx, "MODE", ctx->createValue(SDL_SCANCODE_MODE))
          ->setField(ctx, "AUDIONEXT", ctx->createValue(SDL_SCANCODE_AUDIONEXT))
          ->setField(ctx, "AUDIOPREV", ctx->createValue(SDL_SCANCODE_AUDIOPREV))
          ->setField(ctx, "AUDIOSTOP", ctx->createValue(SDL_SCANCODE_AUDIOSTOP))
          ->setField(ctx, "AUDIOPLAY", ctx->createValue(SDL_SCANCODE_AUDIOPLAY))
          ->setField(ctx, "AUDIOMUTE", ctx->createValue(SDL_SCANCODE_AUDIOMUTE))
          ->setField(ctx, "MEDIASELECT",
                     ctx->createValue(SDL_SCANCODE_MEDIASELECT))
          ->setField(ctx, "WWW", ctx->createValue(SDL_SCANCODE_WWW))
          ->setField(ctx, "MAIL", ctx->createValue(SDL_SCANCODE_MAIL))
          ->setField(ctx, "CALCULATOR",
                     ctx->createValue(SDL_SCANCODE_CALCULATOR))
          ->setField(ctx, "COMPUTER", ctx->createValue(SDL_SCANCODE_COMPUTER))
          ->setField(ctx, "AC_SEARCH", ctx->createValue(SDL_SCANCODE_AC_SEARCH))
          ->setField(ctx, "AC_HOME", ctx->createValue(SDL_SCANCODE_AC_HOME))
          ->setField(ctx, "AC_BACK", ctx->createValue(SDL_SCANCODE_AC_BACK))
          ->setField(ctx, "AC_FORWARD",
                     ctx->createValue(SDL_SCANCODE_AC_FORWARD))
          ->setField(ctx, "AC_STOP", ctx->createValue(SDL_SCANCODE_AC_STOP))
          ->setField(ctx, "AC_REFRESH",
                     ctx->createValue(SDL_SCANCODE_AC_REFRESH))
          ->setField(ctx, "AC_BOOKMARKS",
                     ctx->createValue(SDL_SCANCODE_AC_BOOKMARKS))
          ->setField(ctx, "BRIGHTNESSDOWN",
                     ctx->createValue(SDL_SCANCODE_BRIGHTNESSDOWN))
          ->setField(ctx, "BRIGHTNESSUP",
                     ctx->createValue(SDL_SCANCODE_BRIGHTNESSUP))
          ->setField(ctx, "DISPLAYSWITCH",
                     ctx->createValue(SDL_SCANCODE_DISPLAYSWITCH))
          ->setField(ctx, "KBDILLUMTOGGLE",
                     ctx->createValue(SDL_SCANCODE_KBDILLUMTOGGLE))
          ->setField(ctx, "KBDILLUMDOWN",
                     ctx->createValue(SDL_SCANCODE_KBDILLUMDOWN))
          ->setField(ctx, "KBDILLUMUP",
                     ctx->createValue(SDL_SCANCODE_KBDILLUMUP))
          ->setField(ctx, "EJECT", ctx->createValue(SDL_SCANCODE_EJECT))
          ->setField(ctx, "SLEEP", ctx->createValue(SDL_SCANCODE_SLEEP))
          ->setField(ctx, "APP1", ctx->createValue(SDL_SCANCODE_APP1))
          ->setField(ctx, "APP2", ctx->createValue(SDL_SCANCODE_APP2))
          ->setField(ctx, "AUDIOREWIND",
                     ctx->createValue(SDL_SCANCODE_AUDIOREWIND))
          ->setField(ctx, "AUDIOFASTFORWARD",
                     ctx->createValue(SDL_SCANCODE_AUDIOFASTFORWARD))
          ->setField(ctx, "SOFTLEFT", ctx->createValue(SDL_SCANCODE_SOFTLEFT))
          ->setField(ctx, "SOFTRIGHT", ctx->createValue(SDL_SCANCODE_SOFTRIGHT))
          ->setField(ctx, "CALL", ctx->createValue(SDL_SCANCODE_CALL))
          ->setField(ctx, "ENDCALL", ctx->createValue(SDL_SCANCODE_ENDCALL));
  exports->setField(ctx, "SCANCODE", scancodes);
  ctx->registerModule("input", exports);
}

FUNC_DEF(Module_Input::captureMouse) {
  auto mouse = core::Singleton<input::Mouse>::instance();
  mouse->captureMouse();
  return {};
}
FUNC_DEF(Module_Input::releaseMouse) {
  auto mouse = core::Singleton<input::Mouse>::instance();
  mouse->releaseMouse();
  return {};
}
FUNC_DEF(Module_Input::getMousePosition) {
  auto mouse = core::Singleton<input::Mouse>::instance();
  auto pos = mouse->getPosition();
  return {ctx->createValue(pos.x), ctx->createValue(pos.y)};
}
FUNC_DEF(Module_Input::getKeyState) {
  uint32_t scancode = (uint32_t)args[0]->toNumber(ctx);
  auto keyboard = core::Singleton<input::Keyboard>::instance();
  return {ctx->createValue( keyboard->getKeyState(scancode))};
}