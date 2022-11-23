#include <HardwareSerial.h> // for Serial
#include "minikeyboard.h"
#include <Wire.h>

#ifdef BBK20KBD
#include <BBQ10Keyboard.h>

BBQ10Keyboard keyboard;

void minikeyboardSetup(ImGuiIO &io) {

    // io.KeyMap[ImGuiKey_Tab]         = '\t';
    io.KeyMap[ImGuiKey_LeftArrow]   = 6; // NSLeftArrowFunctionKey + offset_for_function_keys;
    io.KeyMap[ImGuiKey_RightArrow]  = 18; //NSRightArrowFunctionKey + offset_for_function_keys;
    // io.KeyMap[ImGuiKey_UpArrow]     = NSUpArrowFunctionKey + offset_for_function_keys;
    // io.KeyMap[ImGuiKey_DownArrow]   = NSDownArrowFunctionKey + offset_for_function_keys;
    // io.KeyMap[ImGuiKey_PageUp]      = NSPageUpFunctionKey + offset_for_function_keys;
    // io.KeyMap[ImGuiKey_PageDown]    = NSPageDownFunctionKey + offset_for_function_keys;
    // io.KeyMap[ImGuiKey_Home]        = NSHomeFunctionKey + offset_for_function_keys;
    // io.KeyMap[ImGuiKey_End]         = NSEndFunctionKey + offset_for_function_keys;
    // io.KeyMap[ImGuiKey_Insert]      = NSInsertFunctionKey + offset_for_function_keys;
    // io.KeyMap[ImGuiKey_Delete]      = NSDeleteFunctionKey + offset_for_function_keys;
    io.KeyMap[ImGuiKey_Backspace]   = 8;
    io.KeyMap[ImGuiKey_Space]       = 32;
    io.KeyMap[ImGuiKey_Enter]       = 13;
    io.KeyMap[ImGuiKey_Escape]      = 27;
    // io.KeyMap[ImGuiKey_A]           = 'A';
    // io.KeyMap[ImGuiKey_C]           = 'C';
    // io.KeyMap[ImGuiKey_V]           = 'V';
    // io.KeyMap[ImGuiKey_X]           = 'X';
    // io.KeyMap[ImGuiKey_Y]           = 'Y';
    // io.KeyMap[ImGuiKey_Z]           = 'Z';

  keyboard.begin();
  keyboard.setBacklight(0.5f);
}

void minikeyboardLoop(ImGuiIO &io) {

const int keyCount = keyboard.keyCount();
  if (keyCount == 0)
    return;

  const BBQ10Keyboard::KeyEvent key = keyboard.keyEvent();
  String state = "pressed";
  if (key.state == BBQ10Keyboard::StateLongPress)
    state = "held down";
  else if (key.state == BBQ10Keyboard::StateRelease)
    state = "released";
  
  
  Serial.printf("key: '%c' (dec %d, hex %02x) %s\r\n", key.key, key.key, key.key, state.c_str());


  if (key.state == BBQ10Keyboard::StatePress) {
    switch ( key.key ) {
      case 0x05:  io.MouseDown[0] = true; /* left */break;
      default:
      io.AddInputCharacter(key.key);
      io.KeysDown[key.key] = true;
    }
  } else if (key.state == BBQ10Keyboard::StateRelease) {
      io.KeysDown[key.key] = false;
  }

//     /*Get the last pressed or released key*/
//     data->key = key.key;            
//     if (key.state == BBQ10Keyboard::StatePress) {
//         data->state = LV_INDEV_STATE_PR; //lvgl
//     } if (key.state == BBQ10Keyboard::StateRelease) {
//         data->state = LV_INDEV_STATE_REL; /:lvgl
//     }
//   }

  // pressing 'b' turns off the backlight and pressing Shift+b turns it on
  if (key.state == BBQ10Keyboard::StatePress) {
    if (key.key == 'b') {
      keyboard.setBacklight(0);
    } else if (key.key == 'B') {
      keyboard.setBacklight(1.0);
    }
  }

}
#endif

#ifdef CARDKB

// doc / keyboard button definition: https://docs.m5stack.com/en/unit/cardkb_1.1

#define CARDKB_ADDR \
    0x5F  // Define the I2C address of CardKB.  定义CardKB的I2C地址

void minikeyboardSetup(ImGuiIO &io) {
    io.KeyMap[ImGuiKey_Tab]         = 0x09; // '\t';
    io.KeyMap[ImGuiKey_LeftArrow]   = 0xB4; // NSLeftArrowFunctionKey + offset_for_function_keys;
    io.KeyMap[ImGuiKey_RightArrow]  = 0xB7; // NSRightArrowFunctionKey + offset_for_function_keys;
    io.KeyMap[ImGuiKey_UpArrow]     = 0xB5; // NSUpArrowFunctionKey + offset_for_function_keys;
    io.KeyMap[ImGuiKey_DownArrow]   = 0xB6; //NSDownArrowFunctionKey + offset_for_function_keys;
    // io.KeyMap[ImGuiKey_PageUp]      = NSPageUpFunctionKey + offset_for_function_keys;
    // io.KeyMap[ImGuiKey_PageDown]    = NSPageDownFunctionKey + offset_for_function_keys;
    // io.KeyMap[ImGuiKey_Home]        = NSHomeFunctionKey + offset_for_function_keys;
    // io.KeyMap[ImGuiKey_End]         = NSEndFunctionKey + offset_for_function_keys;
    // io.KeyMap[ImGuiKey_Insert]      = NSInsertFunctionKey + offset_for_function_keys;
    // io.KeyMap[ImGuiKey_Delete]      = NSDeleteFunctionKey + offset_for_function_keys;
    io.KeyMap[ImGuiKey_Backspace]   = 0x08;
    io.KeyMap[ImGuiKey_Space]       = 0x20;
    io.KeyMap[ImGuiKey_Enter]       = 0x0D;
    io.KeyMap[ImGuiKey_Escape]      = 0x1B;
    // io.KeyMap[ImGuiKey_A]           = 'A';
    // io.KeyMap[ImGuiKey_C]           = 'C';
    // io.KeyMap[ImGuiKey_V]           = 'V';
    // io.KeyMap[ImGuiKey_X]           = 'X';
    // io.KeyMap[ImGuiKey_Y]           = 'Y';
    // io.KeyMap[ImGuiKey_Z]           = 'Z';
}

void minikeyboardLoop(ImGuiIO &io) {

  for (int n = 0; n < IM_ARRAYSIZE(io.KeysDown); n++)
    io.KeysDown[n] = false;


  Wire.requestFrom(
          CARDKB_ADDR,
          1);  // Request 1 byte from the slave device.  向从设备请求1字节
  while (
      Wire.available())  // If received data is detected.  如果检测到收到数据
  {
      char c = Wire.read();  // Store the received data.  将接收到的数据存储
      if (c != 0) {
        io.AddInputCharacter(c);
        io.KeysDown[c] = true;
        Serial.printf("key: '%c' (dec %d, hex %02x) \r\n", c, c, c);
      }
  }
}

#endif