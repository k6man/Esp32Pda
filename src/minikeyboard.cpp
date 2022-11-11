#include <HardwareSerial.h> // for Serial
#include "minikeyboard.h"


BBQ10Keyboard keyboard;

void minikeyboardSetup(void) {

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
