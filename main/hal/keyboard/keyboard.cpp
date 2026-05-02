/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "keyboard.h"
#include "../hal_config.h"
#include <mooncake_log.h>

static const std::string _tag = "Keyboard";

static volatile bool _isr_flag = false;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    _isr_flag = true;
}

bool Keyboard::init()
{
    mclog::tagInfo(_tag, "init");

    _tca8418 = new Adafruit_TCA8418();
    auto ret = _tca8418->begin();
    if (!ret) {
        mclog::tagError(_tag, "init tca8418 failed");
        return false;
    }

    _tca8418->matrix(7, 8);
    _tca8418->flush();

    // Attach interrupt
    gpio_config_t io_conf;
    io_conf.intr_type    = GPIO_INTR_ANYEDGE;
    io_conf.mode         = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << HAL_PIN_KEYBOARD_INT);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en   = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    // gpio_install_isr_service(0);
    gpio_install_isr_service((int)ESP_INTR_FLAG_IRAM);
    gpio_isr_handler_add(static_cast<gpio_num_t>(HAL_PIN_KEYBOARD_INT), gpio_isr_handler, NULL);

    _tca8418->enableInterrupts();

    return true;
}

void Keyboard::update()
{
    clearKeyEvent();

    if (!_isr_flag) {
        return;
    }

    _key_event_raw_buffer = get_key_event_raw(_tca8418->getEvent());

    //  try to clear the IRQ flag
    //  if there are pending events it is not cleared
    _tca8418->writeRegister8(TCA8418_REG_INT_STAT, 1);
    int intstat = _tca8418->readRegister8(TCA8418_REG_INT_STAT);
    if ((intstat & 0x01) == 0) {
        _isr_flag = false;
    }

    remap(_key_event_raw_buffer);
    // mclog::tagDebug(_tag, "key event raw: ({}, {}): {}", _key_event_raw_buffer.row, _key_event_raw_buffer.col,
    //                 _key_event_raw_buffer.state);
    onKeyEventRaw.emit(_key_event_raw_buffer);

    update_modifier_mask(_key_event_raw_buffer);
    _key_event_buffer = convertToKeyEvent(_key_event_raw_buffer);
    // mclog::tagDebug(_tag, "key event: ({}) {} {}", (int)_key_event_buffer.keyCode, _key_event_buffer.keyName,
    //                 _key_event_buffer.state);
    onKeyEvent.emit(_key_event_buffer);
}

Keyboard::KeyEventRaw_t Keyboard::get_key_event_raw(const uint8_t& eventRaw)
{
    KeyEventRaw_t ret;
    ret.state       = eventRaw & 0x80;
    uint16_t buffer = eventRaw;
    buffer &= 0x7F;
    buffer--;
    ret.row = buffer / 10;
    ret.col = buffer % 10;
    return ret;
}

// Remap to the same as cardputer
void Keyboard::remap(KeyEventRaw_t& key)
{
    // Col
    uint8_t col = 0;
    col         = key.row * 2;
    if (key.col > 3) col++;

    // Row
    uint8_t row = 0;
    row         = (key.col + 4) % 4;

    key.row = row;
    key.col = col;
}

void Keyboard::update_modifier_mask(const KeyEventRaw_t& key)
{
    // Check control key (3, 0)
    if (key.row == 3 && key.col == 0) {
        if (key.state) {
            _modifier_mask |= KEY_MOD_LCTRL;
        } else {
            _modifier_mask &= ~KEY_MOD_LCTRL;
        }
    }

    // Check shift key (2, 1) - Aa button
    if (key.row == 2 && key.col == 1) {
        if (key.state) {
            _modifier_mask |= KEY_MOD_LSHIFT;
        } else {
            _modifier_mask &= ~KEY_MOD_LSHIFT;
        }
    }

    // Check Fn key (2, 0) - Fn button
    if (key.row == 2 && key.col == 0) {
        _fn_state = key.state;
    }

    // Check alt key (3, 2)
    if (key.row == 3 && key.col == 2) {
        if (key.state) {
            _modifier_mask |= KEY_MOD_LALT;
        } else {
            _modifier_mask &= ~KEY_MOD_LALT;
        }
    }

    // Check opt/meta key (3, 1)
    if (key.row == 3 && key.col == 1) {
        if (key.state) {
            _modifier_mask |= KEY_MOD_LMETA;
        } else {
            _modifier_mask &= ~KEY_MOD_LMETA;
        }
    }
}

struct KeyValue_t {
    const char*        firstName;
    const KeScanCode_t firstKeyCode;
    const char*        secondName;
    const KeScanCode_t secondKeyCode;
    const char*        fnName;           // nullptr = no Fn override
    const KeScanCode_t fnKeyCode;        // KEY_NONE = no Fn override
    const uint8_t      fnExtraModifiers; // extra HID modifier bits injected for this Fn key
};

// clang-format off
const KeyValue_t _key_value_map[4][14] = {
    // Row 0
    {{"`",     KEY_GRAVE,      "~",     KEY_GRAVE,      "esc",   KEY_ESC,       0              },
     {"1",     KEY_1,          "!",     KEY_1,          nullptr, KEY_NONE,      0              },
     {"2",     KEY_2,          "@",     KEY_2,          nullptr, KEY_NONE,      0              },
     {"3",     KEY_3,          "#",     KEY_3,          nullptr, KEY_NONE,      0              },
     {"4",     KEY_4,          "$",     KEY_4,          nullptr, KEY_NONE,      0              },
     {"5",     KEY_5,          "%",     KEY_5,          nullptr, KEY_NONE,      0              },
     {"6",     KEY_6,          "^",     KEY_6,          nullptr, KEY_NONE,      0              },
     {"7",     KEY_7,          "&",     KEY_7,          nullptr, KEY_NONE,      0              },
     {"8",     KEY_8,          "*",     KEY_8,          nullptr, KEY_NONE,      0              },
     {"9",     KEY_9,          "(",     KEY_9,          nullptr, KEY_NONE,      0              },
     {"0",     KEY_0,          ")",     KEY_0,          nullptr, KEY_NONE,      0              },
     {"-",     KEY_MINUS,      "_",     KEY_MINUS,      nullptr, KEY_NONE,      0              },
     {"=",     KEY_EQUAL,      "+",     KEY_EQUAL,      nullptr, KEY_NONE,      0              },
     {"del",   KEY_BACKSPACE,  "del",   KEY_BACKSPACE,  "del",   KEY_DELETE,    0              }},
    // Row 1
    {{"tab",   KEY_TAB,        "tab",   KEY_TAB,        nullptr, KEY_NONE,      0              },
     {"q",     KEY_Q,          "Q",     KEY_Q,          "Q",     KEY_Q,         KEY_MOD_LSHIFT },
     {"w",     KEY_W,          "W",     KEY_W,          "W",     KEY_W,         KEY_MOD_LSHIFT },
     {"e",     KEY_E,          "E",     KEY_E,          "E",     KEY_E,         KEY_MOD_LSHIFT },
     {"r",     KEY_R,          "R",     KEY_R,          "R",     KEY_R,         KEY_MOD_LSHIFT },
     {"t",     KEY_T,          "T",     KEY_T,          "T",     KEY_T,         KEY_MOD_LSHIFT },
     {"y",     KEY_Y,          "Y",     KEY_Y,          "Y",     KEY_Y,         KEY_MOD_LSHIFT },
     {"u",     KEY_U,          "U",     KEY_U,          "U",     KEY_U,         KEY_MOD_LSHIFT },
     {"i",     KEY_I,          "I",     KEY_I,          "I",     KEY_I,         KEY_MOD_LSHIFT },
     {"o",     KEY_O,          "O",     KEY_O,          "O",     KEY_O,         KEY_MOD_LSHIFT },
     {"p",     KEY_P,          "P",     KEY_P,          "P",     KEY_P,         KEY_MOD_LSHIFT },
     {"[",     KEY_LEFTBRACE,  "{",     KEY_LEFTBRACE,  nullptr, KEY_NONE,      0              },
     {"]",     KEY_RIGHTBRACE, "}",     KEY_RIGHTBRACE, nullptr, KEY_NONE,      0              },
     {"\\",    KEY_BACKSLASH,  "|",     KEY_BACKSLASH,  nullptr, KEY_NONE,      0              }},
    // Row 2
    {{"fn",    KEY_NONE,       "fn",    KEY_NONE,       nullptr, KEY_NONE,      0              },
     {"shift", KEY_LEFTSHIFT,  "shift", KEY_LEFTSHIFT,  nullptr, KEY_NONE,      0              },
     {"a",     KEY_A,          "A",     KEY_A,          "A",     KEY_A,         KEY_MOD_LSHIFT },
     {"s",     KEY_S,          "S",     KEY_S,          "S",     KEY_S,         KEY_MOD_LSHIFT },
     {"d",     KEY_D,          "D",     KEY_D,          "D",     KEY_D,         KEY_MOD_LSHIFT },
     {"f",     KEY_F,          "F",     KEY_F,          "F",     KEY_F,         KEY_MOD_LSHIFT },
     {"g",     KEY_G,          "G",     KEY_G,          "G",     KEY_G,         KEY_MOD_LSHIFT },
     {"h",     KEY_H,          "H",     KEY_H,          "H",     KEY_H,         KEY_MOD_LSHIFT },
     {"j",     KEY_J,          "J",     KEY_J,          "J",     KEY_J,         KEY_MOD_LSHIFT },
     {"k",     KEY_K,          "K",     KEY_K,          "K",     KEY_K,         KEY_MOD_LSHIFT },
     {"l",     KEY_L,          "L",     KEY_L,          "L",     KEY_L,         KEY_MOD_LSHIFT },
     {";",     KEY_SEMICOLON,  ":",     KEY_SEMICOLON,  "up",    KEY_UP,        0              },
     {"'",     KEY_APOSTROPHE, "\"",    KEY_APOSTROPHE, nullptr, KEY_NONE,      0              },
     {"enter", KEY_ENTER,      "enter", KEY_ENTER,      nullptr, KEY_NONE,      0              }},
    // Row 3
    {{"ctrl",  KEY_LEFTCTRL,   "ctrl",  KEY_LEFTCTRL,   nullptr, KEY_NONE,      0              },
     {"opt",   KEY_LEFTMETA,   "opt",   KEY_LEFTMETA,   nullptr, KEY_NONE,      0              },
     {"alt",   KEY_LEFTALT,    "alt",   KEY_LEFTALT,    nullptr, KEY_NONE,      0              },
     {"z",     KEY_Z,          "Z",     KEY_Z,          "Z",     KEY_Z,         KEY_MOD_LSHIFT },
     {"x",     KEY_X,          "X",     KEY_X,          "X",     KEY_X,         KEY_MOD_LSHIFT },
     {"c",     KEY_C,          "C",     KEY_C,          "C",     KEY_C,         KEY_MOD_LSHIFT },
     {"v",     KEY_V,          "V",     KEY_V,          "V",     KEY_V,         KEY_MOD_LSHIFT },
     {"b",     KEY_B,          "B",     KEY_B,          "B",     KEY_B,         KEY_MOD_LSHIFT },
     {"n",     KEY_N,          "N",     KEY_N,          "N",     KEY_N,         KEY_MOD_LSHIFT },
     {"m",     KEY_M,          "M",     KEY_M,          "M",     KEY_M,         KEY_MOD_LSHIFT },
     {",",     KEY_COMMA,      "<",     KEY_COMMA,      "left",  KEY_LEFT,      0              },
     {".",     KEY_DOT,        ">",     KEY_DOT,        "down",  KEY_DOWN,      0              },
     {"/",     KEY_SLASH,      "?",     KEY_SLASH,      "right", KEY_RIGHT,     0              },
     {" ",     KEY_SPACE,      " ",     KEY_SPACE,      nullptr, KEY_NONE,      0              }}};
// clang-format on

Keyboard::KeyEvent_t Keyboard::convertToKeyEvent(const KeyEventRaw_t& key)
{
    KeyEvent_t ret;
    ret.state = key.state;

    // Fn key itself - modifier, no keycode
    if (key.row == 2 && key.col == 0) {
        ret.keyCode    = KEY_NONE;
        ret.keyName    = "fn";
        ret.isModifier = true;
        return ret;
    }

    // Fn layer: override selected keys when Fn is held.
    // Keys with no Fn entry (fnKeyCode == KEY_NONE) fall through to the normal
    // lookup below, so they behave as if Fn were not held.  To add a new Fn
    // mapping, only the table needs updating -- no code change required.
    if (_fn_state) {
        const auto& kv = _key_value_map[key.row][key.col];
        if (kv.fnKeyCode != KEY_NONE) {
            ret.keyCode        = kv.fnKeyCode;
            ret.keyName        = kv.fnName;
            ret.isModifier     = false;
            ret.extraModifiers = kv.fnExtraModifiers;
            return ret;
        }
    }

    // Normal key lookup - shift determines upper/symbol layer
    bool use_shifted_version = (_modifier_mask & KEY_MOD_LSHIFT);

    if (use_shifted_version) {
        ret.keyCode = _key_value_map[key.row][key.col].secondKeyCode;
        ret.keyName = _key_value_map[key.row][key.col].secondName;
    } else {
        ret.keyCode = _key_value_map[key.row][key.col].firstKeyCode;
        ret.keyName = _key_value_map[key.row][key.col].firstName;
    }

    ret.isModifier = (ret.keyCode == KEY_LEFTSHIFT || ret.keyCode == KEY_LEFTCTRL ||
                      ret.keyCode == KEY_LEFTALT   || ret.keyCode == KEY_LEFTMETA);

    return ret;
}

void Keyboard::clearKeyEvent()
{
    _key_event_raw_buffer.state = false;
    _key_event_raw_buffer.row   = 233;
    _key_event_raw_buffer.col   = 233;
    _key_event_buffer.keyCode   = KEY_NONE;
}
