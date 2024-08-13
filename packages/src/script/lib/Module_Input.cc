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
          ->setField(ctx, "A", createNumber(ctx, SDL_SCANCODE_A))
          ->setField(ctx, "B", createNumber(ctx, SDL_SCANCODE_B))
          ->setField(ctx, "C", createNumber(ctx, SDL_SCANCODE_C))
          ->setField(ctx, "D", createNumber(ctx, SDL_SCANCODE_D))
          ->setField(ctx, "E", createNumber(ctx, SDL_SCANCODE_E))
          ->setField(ctx, "F", createNumber(ctx, SDL_SCANCODE_F))
          ->setField(ctx, "G", createNumber(ctx, SDL_SCANCODE_G))
          ->setField(ctx, "H", createNumber(ctx, SDL_SCANCODE_H))
          ->setField(ctx, "I", createNumber(ctx, SDL_SCANCODE_I))
          ->setField(ctx, "J", createNumber(ctx, SDL_SCANCODE_J))
          ->setField(ctx, "K", createNumber(ctx, SDL_SCANCODE_K))
          ->setField(ctx, "L", createNumber(ctx, SDL_SCANCODE_L))
          ->setField(ctx, "M", createNumber(ctx, SDL_SCANCODE_M))
          ->setField(ctx, "N", createNumber(ctx, SDL_SCANCODE_N))
          ->setField(ctx, "O", createNumber(ctx, SDL_SCANCODE_O))
          ->setField(ctx, "P", createNumber(ctx, SDL_SCANCODE_P))
          ->setField(ctx, "Q", createNumber(ctx, SDL_SCANCODE_Q))
          ->setField(ctx, "R", createNumber(ctx, SDL_SCANCODE_R))
          ->setField(ctx, "S", createNumber(ctx, SDL_SCANCODE_S))
          ->setField(ctx, "T", createNumber(ctx, SDL_SCANCODE_T))
          ->setField(ctx, "U", createNumber(ctx, SDL_SCANCODE_U))
          ->setField(ctx, "V", createNumber(ctx, SDL_SCANCODE_V))
          ->setField(ctx, "W", createNumber(ctx, SDL_SCANCODE_W))
          ->setField(ctx, "X", createNumber(ctx, SDL_SCANCODE_X))
          ->setField(ctx, "Y", createNumber(ctx, SDL_SCANCODE_Y))
          ->setField(ctx, "Z", createNumber(ctx, SDL_SCANCODE_Z))
          ->setField(ctx, "NUM1", createNumber(ctx, SDL_SCANCODE_1))
          ->setField(ctx, "NUM2", createNumber(ctx, SDL_SCANCODE_2))
          ->setField(ctx, "NUM3", createNumber(ctx, SDL_SCANCODE_3))
          ->setField(ctx, "NUM4", createNumber(ctx, SDL_SCANCODE_4))
          ->setField(ctx, "NUM5", createNumber(ctx, SDL_SCANCODE_5))
          ->setField(ctx, "NUM6", createNumber(ctx, SDL_SCANCODE_6))
          ->setField(ctx, "NUM7", createNumber(ctx, SDL_SCANCODE_7))
          ->setField(ctx, "NUM8", createNumber(ctx, SDL_SCANCODE_8))
          ->setField(ctx, "NUM9", createNumber(ctx, SDL_SCANCODE_9))
          ->setField(ctx, "NUM0", createNumber(ctx, SDL_SCANCODE_0))
          ->setField(ctx, "RETURN", createNumber(ctx, SDL_SCANCODE_RETURN))
          ->setField(ctx, "ESCAPE", createNumber(ctx, SDL_SCANCODE_ESCAPE))
          ->setField(ctx, "BACKSPACE",
                     createNumber(ctx, SDL_SCANCODE_BACKSPACE))
          ->setField(ctx, "TAB", createNumber(ctx, SDL_SCANCODE_TAB))
          ->setField(ctx, "SPACE", createNumber(ctx, SDL_SCANCODE_SPACE))
          ->setField(ctx, "MINUS", createNumber(ctx, SDL_SCANCODE_MINUS))
          ->setField(ctx, "EQUALS", createNumber(ctx, SDL_SCANCODE_EQUALS))
          ->setField(ctx, "LEFTBRACKET",
                     createNumber(ctx, SDL_SCANCODE_LEFTBRACKET))
          ->setField(ctx, "RIGHTBRACKET",
                     createNumber(ctx, SDL_SCANCODE_RIGHTBRACKET))
          ->setField(ctx, "BACKSLASH",
                     createNumber(ctx, SDL_SCANCODE_BACKSLASH))
          ->setField(ctx, "NONUSHASH",
                     createNumber(ctx, SDL_SCANCODE_NONUSHASH))
          ->setField(ctx, "SEMICOLON",
                     createNumber(ctx, SDL_SCANCODE_SEMICOLON))
          ->setField(ctx, "APOSTROPHE",
                     createNumber(ctx, SDL_SCANCODE_APOSTROPHE))
          ->setField(ctx, "GRAVE", createNumber(ctx, SDL_SCANCODE_GRAVE))
          ->setField(ctx, "COMMA", createNumber(ctx, SDL_SCANCODE_COMMA))
          ->setField(ctx, "PERIOD", createNumber(ctx, SDL_SCANCODE_PERIOD))
          ->setField(ctx, "SLASH", createNumber(ctx, SDL_SCANCODE_SLASH))
          ->setField(ctx, "CAPSLOCK", createNumber(ctx, SDL_SCANCODE_CAPSLOCK))
          ->setField(ctx, "F1", createNumber(ctx, SDL_SCANCODE_F1))
          ->setField(ctx, "F2", createNumber(ctx, SDL_SCANCODE_F2))
          ->setField(ctx, "F3", createNumber(ctx, SDL_SCANCODE_F3))
          ->setField(ctx, "F4", createNumber(ctx, SDL_SCANCODE_F4))
          ->setField(ctx, "F5", createNumber(ctx, SDL_SCANCODE_F5))
          ->setField(ctx, "F6", createNumber(ctx, SDL_SCANCODE_F6))
          ->setField(ctx, "F7", createNumber(ctx, SDL_SCANCODE_F7))
          ->setField(ctx, "F8", createNumber(ctx, SDL_SCANCODE_F8))
          ->setField(ctx, "F9", createNumber(ctx, SDL_SCANCODE_F9))
          ->setField(ctx, "F10", createNumber(ctx, SDL_SCANCODE_F10))
          ->setField(ctx, "F11", createNumber(ctx, SDL_SCANCODE_F11))
          ->setField(ctx, "F12", createNumber(ctx, SDL_SCANCODE_F12))
          ->setField(ctx, "PRINTSCREEN",
                     createNumber(ctx, SDL_SCANCODE_PRINTSCREEN))
          ->setField(ctx, "SCROLLLOCK",
                     createNumber(ctx, SDL_SCANCODE_SCROLLLOCK))
          ->setField(ctx, "PAUSE", createNumber(ctx, SDL_SCANCODE_PAUSE))
          ->setField(ctx, "INSERT", createNumber(ctx, SDL_SCANCODE_INSERT))
          ->setField(ctx, "HOME", createNumber(ctx, SDL_SCANCODE_HOME))
          ->setField(ctx, "PAGEUP", createNumber(ctx, SDL_SCANCODE_PAGEUP))
          ->setField(ctx, "DELETE", createNumber(ctx, SDL_SCANCODE_DELETE))
          ->setField(ctx, "END", createNumber(ctx, SDL_SCANCODE_END))
          ->setField(ctx, "PAGEDOWN", createNumber(ctx, SDL_SCANCODE_PAGEDOWN))
          ->setField(ctx, "RIGHT", createNumber(ctx, SDL_SCANCODE_RIGHT))
          ->setField(ctx, "LEFT", createNumber(ctx, SDL_SCANCODE_LEFT))
          ->setField(ctx, "DOWN", createNumber(ctx, SDL_SCANCODE_DOWN))
          ->setField(ctx, "UP", createNumber(ctx, SDL_SCANCODE_UP))
          ->setField(ctx, "NUMLOCKCLEAR",
                     createNumber(ctx, SDL_SCANCODE_NUMLOCKCLEAR))
          ->setField(ctx, "KP_DIVIDE",
                     createNumber(ctx, SDL_SCANCODE_KP_DIVIDE))
          ->setField(ctx, "KP_MULTIPLY",
                     createNumber(ctx, SDL_SCANCODE_KP_MULTIPLY))
          ->setField(ctx, "KP_MINUS", createNumber(ctx, SDL_SCANCODE_KP_MINUS))
          ->setField(ctx, "KP_PLUS", createNumber(ctx, SDL_SCANCODE_KP_PLUS))
          ->setField(ctx, "KP_ENTER", createNumber(ctx, SDL_SCANCODE_KP_ENTER))
          ->setField(ctx, "KP_1", createNumber(ctx, SDL_SCANCODE_KP_1))
          ->setField(ctx, "KP_2", createNumber(ctx, SDL_SCANCODE_KP_2))
          ->setField(ctx, "KP_3", createNumber(ctx, SDL_SCANCODE_KP_3))
          ->setField(ctx, "KP_4", createNumber(ctx, SDL_SCANCODE_KP_4))
          ->setField(ctx, "KP_5", createNumber(ctx, SDL_SCANCODE_KP_5))
          ->setField(ctx, "KP_6", createNumber(ctx, SDL_SCANCODE_KP_6))
          ->setField(ctx, "KP_7", createNumber(ctx, SDL_SCANCODE_KP_7))
          ->setField(ctx, "KP_8", createNumber(ctx, SDL_SCANCODE_KP_8))
          ->setField(ctx, "KP_9", createNumber(ctx, SDL_SCANCODE_KP_9))
          ->setField(ctx, "KP_0", createNumber(ctx, SDL_SCANCODE_KP_0))
          ->setField(ctx, "KP_PERIOD",
                     createNumber(ctx, SDL_SCANCODE_KP_PERIOD))
          ->setField(ctx, "NONUSBACKSLASH",
                     createNumber(ctx, SDL_SCANCODE_NONUSBACKSLASH))
          ->setField(ctx, "APPLICATION",
                     createNumber(ctx, SDL_SCANCODE_APPLICATION))
          ->setField(ctx, "POWER", createNumber(ctx, SDL_SCANCODE_POWER))
          ->setField(ctx, "KP_EQUALS",
                     createNumber(ctx, SDL_SCANCODE_KP_EQUALS))
          ->setField(ctx, "F13", createNumber(ctx, SDL_SCANCODE_F13))
          ->setField(ctx, "F14", createNumber(ctx, SDL_SCANCODE_F14))
          ->setField(ctx, "F15", createNumber(ctx, SDL_SCANCODE_F15))
          ->setField(ctx, "F16", createNumber(ctx, SDL_SCANCODE_F16))
          ->setField(ctx, "F17", createNumber(ctx, SDL_SCANCODE_F17))
          ->setField(ctx, "F18", createNumber(ctx, SDL_SCANCODE_F18))
          ->setField(ctx, "F19", createNumber(ctx, SDL_SCANCODE_F19))
          ->setField(ctx, "F20", createNumber(ctx, SDL_SCANCODE_F20))
          ->setField(ctx, "F21", createNumber(ctx, SDL_SCANCODE_F21))
          ->setField(ctx, "F22", createNumber(ctx, SDL_SCANCODE_F22))
          ->setField(ctx, "F23", createNumber(ctx, SDL_SCANCODE_F23))
          ->setField(ctx, "F24", createNumber(ctx, SDL_SCANCODE_F24))
          ->setField(ctx, "EXECUTE", createNumber(ctx, SDL_SCANCODE_EXECUTE))
          ->setField(ctx, "HELP", createNumber(ctx, SDL_SCANCODE_HELP))
          ->setField(ctx, "MENU", createNumber(ctx, SDL_SCANCODE_MENU))
          ->setField(ctx, "SELECT", createNumber(ctx, SDL_SCANCODE_SELECT))
          ->setField(ctx, "STOP", createNumber(ctx, SDL_SCANCODE_STOP))
          ->setField(ctx, "AGAIN", createNumber(ctx, SDL_SCANCODE_AGAIN))
          ->setField(ctx, "UNDO", createNumber(ctx, SDL_SCANCODE_UNDO))
          ->setField(ctx, "CUT", createNumber(ctx, SDL_SCANCODE_CUT))
          ->setField(ctx, "COPY", createNumber(ctx, SDL_SCANCODE_COPY))
          ->setField(ctx, "PASTE", createNumber(ctx, SDL_SCANCODE_PASTE))
          ->setField(ctx, "FIND", createNumber(ctx, SDL_SCANCODE_FIND))
          ->setField(ctx, "MUTE", createNumber(ctx, SDL_SCANCODE_MUTE))
          ->setField(ctx, "VOLUMEUP", createNumber(ctx, SDL_SCANCODE_VOLUMEUP))
          ->setField(ctx, "VOLUMEDOWN",
                     createNumber(ctx, SDL_SCANCODE_VOLUMEDOWN))
          ->setField(ctx, "KP_COMMA", createNumber(ctx, SDL_SCANCODE_KP_COMMA))
          ->setField(ctx, "KP_EQUALSAS400",
                     createNumber(ctx, SDL_SCANCODE_KP_EQUALSAS400))
          ->setField(ctx, "INTERNATIONAL1",
                     createNumber(ctx, SDL_SCANCODE_INTERNATIONAL1))
          ->setField(ctx, "INTERNATIONAL2",
                     createNumber(ctx, SDL_SCANCODE_INTERNATIONAL2))
          ->setField(ctx, "INTERNATIONAL3",
                     createNumber(ctx, SDL_SCANCODE_INTERNATIONAL3))
          ->setField(ctx, "INTERNATIONAL4",
                     createNumber(ctx, SDL_SCANCODE_INTERNATIONAL4))
          ->setField(ctx, "INTERNATIONAL5",
                     createNumber(ctx, SDL_SCANCODE_INTERNATIONAL5))
          ->setField(ctx, "INTERNATIONAL6",
                     createNumber(ctx, SDL_SCANCODE_INTERNATIONAL6))
          ->setField(ctx, "INTERNATIONAL7",
                     createNumber(ctx, SDL_SCANCODE_INTERNATIONAL7))
          ->setField(ctx, "INTERNATIONAL8",
                     createNumber(ctx, SDL_SCANCODE_INTERNATIONAL8))
          ->setField(ctx, "INTERNATIONAL9",
                     createNumber(ctx, SDL_SCANCODE_INTERNATIONAL9))
          ->setField(ctx, "LANG1", createNumber(ctx, SDL_SCANCODE_LANG1))
          ->setField(ctx, "LANG2", createNumber(ctx, SDL_SCANCODE_LANG2))
          ->setField(ctx, "LANG3", createNumber(ctx, SDL_SCANCODE_LANG3))
          ->setField(ctx, "LANG4", createNumber(ctx, SDL_SCANCODE_LANG4))
          ->setField(ctx, "LANG5", createNumber(ctx, SDL_SCANCODE_LANG5))
          ->setField(ctx, "LANG6", createNumber(ctx, SDL_SCANCODE_LANG6))
          ->setField(ctx, "LANG7", createNumber(ctx, SDL_SCANCODE_LANG7))
          ->setField(ctx, "LANG8", createNumber(ctx, SDL_SCANCODE_LANG8))
          ->setField(ctx, "LANG9", createNumber(ctx, SDL_SCANCODE_LANG9))
          ->setField(ctx, "ALTERASE", createNumber(ctx, SDL_SCANCODE_ALTERASE))
          ->setField(ctx, "SYSREQ", createNumber(ctx, SDL_SCANCODE_SYSREQ))
          ->setField(ctx, "CANCEL", createNumber(ctx, SDL_SCANCODE_CANCEL))
          ->setField(ctx, "CLEAR", createNumber(ctx, SDL_SCANCODE_CLEAR))
          ->setField(ctx, "PRIOR", createNumber(ctx, SDL_SCANCODE_PRIOR))
          ->setField(ctx, "RETURN2", createNumber(ctx, SDL_SCANCODE_RETURN2))
          ->setField(ctx, "SEPARATOR",
                     createNumber(ctx, SDL_SCANCODE_SEPARATOR))
          ->setField(ctx, "OUT", createNumber(ctx, SDL_SCANCODE_OUT))
          ->setField(ctx, "OPER", createNumber(ctx, SDL_SCANCODE_OPER))
          ->setField(ctx, "CLEARAGAIN",
                     createNumber(ctx, SDL_SCANCODE_CLEARAGAIN))
          ->setField(ctx, "CRSEL", createNumber(ctx, SDL_SCANCODE_CRSEL))
          ->setField(ctx, "EXSEL", createNumber(ctx, SDL_SCANCODE_EXSEL))
          ->setField(ctx, "KP_00", createNumber(ctx, SDL_SCANCODE_KP_00))
          ->setField(ctx, "KP_000", createNumber(ctx, SDL_SCANCODE_KP_000))
          ->setField(ctx, "THOUSANDSSEPARATOR",
                     createNumber(ctx, SDL_SCANCODE_THOUSANDSSEPARATOR))
          ->setField(ctx, "DECIMALSEPARATOR",
                     createNumber(ctx, SDL_SCANCODE_DECIMALSEPARATOR))
          ->setField(ctx, "CURRENCYUNIT",
                     createNumber(ctx, SDL_SCANCODE_CURRENCYUNIT))
          ->setField(ctx, "CURRENCYSUBUNIT",
                     createNumber(ctx, SDL_SCANCODE_CURRENCYSUBUNIT))
          ->setField(ctx, "KP_LEFTPAREN",
                     createNumber(ctx, SDL_SCANCODE_KP_LEFTPAREN))
          ->setField(ctx, "KP_RIGHTPAREN",
                     createNumber(ctx, SDL_SCANCODE_KP_RIGHTPAREN))
          ->setField(ctx, "KP_LEFTBRACE",
                     createNumber(ctx, SDL_SCANCODE_KP_LEFTBRACE))
          ->setField(ctx, "KP_RIGHTBRACE",
                     createNumber(ctx, SDL_SCANCODE_KP_RIGHTBRACE))
          ->setField(ctx, "KP_TAB", createNumber(ctx, SDL_SCANCODE_KP_TAB))
          ->setField(ctx, "KP_BACKSPACE",
                     createNumber(ctx, SDL_SCANCODE_KP_BACKSPACE))
          ->setField(ctx, "KP_A", createNumber(ctx, SDL_SCANCODE_KP_A))
          ->setField(ctx, "KP_B", createNumber(ctx, SDL_SCANCODE_KP_B))
          ->setField(ctx, "KP_C", createNumber(ctx, SDL_SCANCODE_KP_C))
          ->setField(ctx, "KP_D", createNumber(ctx, SDL_SCANCODE_KP_D))
          ->setField(ctx, "KP_E", createNumber(ctx, SDL_SCANCODE_KP_E))
          ->setField(ctx, "KP_F", createNumber(ctx, SDL_SCANCODE_KP_F))
          ->setField(ctx, "KP_XOR", createNumber(ctx, SDL_SCANCODE_KP_XOR))
          ->setField(ctx, "KP_POWER", createNumber(ctx, SDL_SCANCODE_KP_POWER))
          ->setField(ctx, "KP_PERCENT",
                     createNumber(ctx, SDL_SCANCODE_KP_PERCENT))
          ->setField(ctx, "KP_LESS", createNumber(ctx, SDL_SCANCODE_KP_LESS))
          ->setField(ctx, "KP_GREATER",
                     createNumber(ctx, SDL_SCANCODE_KP_GREATER))
          ->setField(ctx, "KP_AMPERSAND",
                     createNumber(ctx, SDL_SCANCODE_KP_AMPERSAND))
          ->setField(ctx, "KP_DBLAMPERSAND",
                     createNumber(ctx, SDL_SCANCODE_KP_DBLAMPERSAND))
          ->setField(ctx, "KP_VERTICALBAR",
                     createNumber(ctx, SDL_SCANCODE_KP_VERTICALBAR))
          ->setField(ctx, "KP_DBLVERTICALBAR",
                     createNumber(ctx, SDL_SCANCODE_KP_DBLVERTICALBAR))
          ->setField(ctx, "KP_COLON", createNumber(ctx, SDL_SCANCODE_KP_COLON))
          ->setField(ctx, "KP_HASH", createNumber(ctx, SDL_SCANCODE_KP_HASH))
          ->setField(ctx, "KP_SPACE", createNumber(ctx, SDL_SCANCODE_KP_SPACE))
          ->setField(ctx, "KP_AT", createNumber(ctx, SDL_SCANCODE_KP_AT))
          ->setField(ctx, "KP_EXCLAM",
                     createNumber(ctx, SDL_SCANCODE_KP_EXCLAM))
          ->setField(ctx, "KP_MEMSTORE",
                     createNumber(ctx, SDL_SCANCODE_KP_MEMSTORE))
          ->setField(ctx, "KP_MEMRECALL",
                     createNumber(ctx, SDL_SCANCODE_KP_MEMRECALL))
          ->setField(ctx, "KP_MEMCLEAR",
                     createNumber(ctx, SDL_SCANCODE_KP_MEMCLEAR))
          ->setField(ctx, "KP_MEMADD",
                     createNumber(ctx, SDL_SCANCODE_KP_MEMADD))
          ->setField(ctx, "KP_MEMSUBTRACT",
                     createNumber(ctx, SDL_SCANCODE_KP_MEMSUBTRACT))
          ->setField(ctx, "KP_MEMMULTIPLY",
                     createNumber(ctx, SDL_SCANCODE_KP_MEMMULTIPLY))
          ->setField(ctx, "KP_MEMDIVIDE",
                     createNumber(ctx, SDL_SCANCODE_KP_MEMDIVIDE))
          ->setField(ctx, "KP_PLUSMINUS",
                     createNumber(ctx, SDL_SCANCODE_KP_PLUSMINUS))
          ->setField(ctx, "KP_CLEAR", createNumber(ctx, SDL_SCANCODE_KP_CLEAR))
          ->setField(ctx, "KP_CLEARENTRY",
                     createNumber(ctx, SDL_SCANCODE_KP_CLEARENTRY))
          ->setField(ctx, "KP_BINARY",
                     createNumber(ctx, SDL_SCANCODE_KP_BINARY))
          ->setField(ctx, "KP_OCTAL", createNumber(ctx, SDL_SCANCODE_KP_OCTAL))
          ->setField(ctx, "KP_DECIMAL",
                     createNumber(ctx, SDL_SCANCODE_KP_DECIMAL))
          ->setField(ctx, "KP_HEXADECIMAL",
                     createNumber(ctx, SDL_SCANCODE_KP_HEXADECIMAL))
          ->setField(ctx, "LCTRL", createNumber(ctx, SDL_SCANCODE_LCTRL))
          ->setField(ctx, "LSHIFT", createNumber(ctx, SDL_SCANCODE_LSHIFT))
          ->setField(ctx, "LALT", createNumber(ctx, SDL_SCANCODE_LALT))
          ->setField(ctx, "LGUI", createNumber(ctx, SDL_SCANCODE_LGUI))
          ->setField(ctx, "RCTRL", createNumber(ctx, SDL_SCANCODE_RCTRL))
          ->setField(ctx, "RSHIFT", createNumber(ctx, SDL_SCANCODE_RSHIFT))
          ->setField(ctx, "RALT", createNumber(ctx, SDL_SCANCODE_RALT))
          ->setField(ctx, "RGUI", createNumber(ctx, SDL_SCANCODE_RGUI))
          ->setField(ctx, "MODE", createNumber(ctx, SDL_SCANCODE_MODE))
          ->setField(ctx, "AUDIONEXT",
                     createNumber(ctx, SDL_SCANCODE_AUDIONEXT))
          ->setField(ctx, "AUDIOPREV",
                     createNumber(ctx, SDL_SCANCODE_AUDIOPREV))
          ->setField(ctx, "AUDIOSTOP",
                     createNumber(ctx, SDL_SCANCODE_AUDIOSTOP))
          ->setField(ctx, "AUDIOPLAY",
                     createNumber(ctx, SDL_SCANCODE_AUDIOPLAY))
          ->setField(ctx, "AUDIOMUTE",
                     createNumber(ctx, SDL_SCANCODE_AUDIOMUTE))
          ->setField(ctx, "MEDIASELECT",
                     createNumber(ctx, SDL_SCANCODE_MEDIASELECT))
          ->setField(ctx, "WWW", createNumber(ctx, SDL_SCANCODE_WWW))
          ->setField(ctx, "MAIL", createNumber(ctx, SDL_SCANCODE_MAIL))
          ->setField(ctx, "CALCULATOR",
                     createNumber(ctx, SDL_SCANCODE_CALCULATOR))
          ->setField(ctx, "COMPUTER", createNumber(ctx, SDL_SCANCODE_COMPUTER))
          ->setField(ctx, "AC_SEARCH",
                     createNumber(ctx, SDL_SCANCODE_AC_SEARCH))
          ->setField(ctx, "AC_HOME", createNumber(ctx, SDL_SCANCODE_AC_HOME))
          ->setField(ctx, "AC_BACK", createNumber(ctx, SDL_SCANCODE_AC_BACK))
          ->setField(ctx, "AC_FORWARD",
                     createNumber(ctx, SDL_SCANCODE_AC_FORWARD))
          ->setField(ctx, "AC_STOP", createNumber(ctx, SDL_SCANCODE_AC_STOP))
          ->setField(ctx, "AC_REFRESH",
                     createNumber(ctx, SDL_SCANCODE_AC_REFRESH))
          ->setField(ctx, "AC_BOOKMARKS",
                     createNumber(ctx, SDL_SCANCODE_AC_BOOKMARKS))
          ->setField(ctx, "BRIGHTNESSDOWN",
                     createNumber(ctx, SDL_SCANCODE_BRIGHTNESSDOWN))
          ->setField(ctx, "BRIGHTNESSUP",
                     createNumber(ctx, SDL_SCANCODE_BRIGHTNESSUP))
          ->setField(ctx, "DISPLAYSWITCH",
                     createNumber(ctx, SDL_SCANCODE_DISPLAYSWITCH))
          ->setField(ctx, "KBDILLUMTOGGLE",
                     createNumber(ctx, SDL_SCANCODE_KBDILLUMTOGGLE))
          ->setField(ctx, "KBDILLUMDOWN",
                     createNumber(ctx, SDL_SCANCODE_KBDILLUMDOWN))
          ->setField(ctx, "KBDILLUMUP",
                     createNumber(ctx, SDL_SCANCODE_KBDILLUMUP))
          ->setField(ctx, "EJECT", createNumber(ctx, SDL_SCANCODE_EJECT))
          ->setField(ctx, "SLEEP", createNumber(ctx, SDL_SCANCODE_SLEEP))
          ->setField(ctx, "APP1", createNumber(ctx, SDL_SCANCODE_APP1))
          ->setField(ctx, "APP2", createNumber(ctx, SDL_SCANCODE_APP2))
          ->setField(ctx, "AUDIOREWIND",
                     createNumber(ctx, SDL_SCANCODE_AUDIOREWIND))
          ->setField(ctx, "AUDIOFASTFORWARD",
                     createNumber(ctx, SDL_SCANCODE_AUDIOFASTFORWARD))
          ->setField(ctx, "SOFTLEFT", createNumber(ctx, SDL_SCANCODE_SOFTLEFT))
          ->setField(ctx, "SOFTRIGHT",
                     createNumber(ctx, SDL_SCANCODE_SOFTRIGHT))
          ->setField(ctx, "CALL", createNumber(ctx, SDL_SCANCODE_CALL))
          ->setField(ctx, "ENDCALL", createNumber(ctx, SDL_SCANCODE_ENDCALL));
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
  return {createNumber(ctx, pos.x), createNumber(ctx, pos.y)};
}
FUNC_DEF(Module_Input::getKeyState) {
  uint32_t scancode = args[0]->toNumber(ctx);
  auto keyboard = core::Singleton<input::Keyboard>::instance();
  return {createBoolean(ctx, keyboard->getKeyState(scancode))};
}