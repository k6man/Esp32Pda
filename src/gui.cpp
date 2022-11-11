#include <BBQ10Keyboard.h>

#include "gui.h"
#include "SPI.h"
#include <LovyanGFX.hpp>
#include "minikeyboard.h"
// #include "esp32notifications.h"

#include "weather.h"


// // Create an interface to the BLE notification library at the top of your sketch
// BLENotifications notifications;

#define I2C_SCL 39
#define I2C_SDA 38

//#define NS2009_TOUCH  //Resistive screen driver
#define FT6236_TOUCH //Capacitive screen driver

#ifdef NS2009_TOUCH
#include "NS2009.h"
const int i2c_touch_addr = NS2009_ADDR;
#define get_pos ns2009_pos
#endif

#ifdef FT6236_TOUCH
#include "FT6236.h"
const int i2c_touch_addr = TOUCH_I2C_ADD;
#define get_pos ft6236_pos
#endif

#define SPI_MOSI 2 //1
#define SPI_MISO 41
#define SPI_SCK 42
#define SD_CS 1 //2

#define LCD_CS 37
#define LCD_BLK 45

/*Change to your screen resolution*/
static const uint16_t screenWidth  = 480;
static const uint16_t screenHeight = 320;

// static lv_disp_draw_buf_t draw_buf;
// static lv_color_t buf[ screenWidth * 10 ];

class LGFX : public lgfx::LGFX_Device
{
    // lgfx::Panel_ILI9341 _panel_instance;
    lgfx::Panel_ILI9488 _panel_instance;
    lgfx::Bus_Parallel16 _bus_instance; // 8ビットパラレルバスのインスタンス (ESP32のみ)

public:
    // コンストラクタを作成し、ここで各種設定を行います。
    // クラス名を変更した場合はコンストラクタも同じ名前を指定してください。
    LGFX(void)
    {
        {                                      // バス制御の設定を行います。
            auto cfg = _bus_instance.config(); // バス設定用の構造体を取得します。

            // 16位设置
            cfg.port = 0;              // 使用するI2Sポートを選択 (0 or 1) (ESP32のI2S LCDモードを使用します)
            cfg.freq_write = 20000000; // 送信クロック (最大20MHz, 80MHzを整数で割った値に丸められます)
            cfg.pin_wr = 35;           // WR を接続しているピン番号
            cfg.pin_rd = 48;           // RD を接続しているピン番号
            cfg.pin_rs = 36;           // RS(D/C)を接続しているピン番号

            cfg.pin_d0 = 47;
            cfg.pin_d1 = 21;
            cfg.pin_d2 = 14;
            cfg.pin_d3 = 13;
            cfg.pin_d4 = 12;
            cfg.pin_d5 = 11;
            cfg.pin_d6 = 10;
            cfg.pin_d7 = 9;
            cfg.pin_d8 = 3;
            cfg.pin_d9 = 8;
            cfg.pin_d10 = 16;
            cfg.pin_d11 = 15;
            cfg.pin_d12 = 7;
            cfg.pin_d13 = 6;
            cfg.pin_d14 = 5;
            cfg.pin_d15 = 4;

            _bus_instance.config(cfg);              // 設定値をバスに反映します。
            _panel_instance.setBus(&_bus_instance); // バスをパネルにセットします。
        }

        {                                        // 表示パネル制御の設定を行います。
            auto cfg = _panel_instance.config(); // 表示パネル設定用の構造体を取得します。

            cfg.pin_cs = -1;   // CS要拉低
            cfg.pin_rst = -1;  // RST和开发板RST相连
            cfg.pin_busy = -1; // BUSYが接続されているピン番号 (-1 = disable)

            // ※ 以下の設定値はパネル毎に一般的な初期値が設定されていますので、不明な項目はコメントアウトして試してみてください。

            cfg.memory_width = 320;   // ドライバICがサポートしている最大の幅
            cfg.memory_height = 480;  // ドライバICがサポートしている最大の高さ
            cfg.panel_width = 320;    // 実際に表示可能な幅
            cfg.panel_height = 480;   // 実際に表示可能な高さ
            cfg.offset_x = 0;         // パネルのX方向オフセット量
            cfg.offset_y = 0;         // パネルのY方向オフセット量
            cfg.offset_rotation = 0;  // 回転方向の値のオフセット 0~7 (4~7は上下反転)
            cfg.dummy_read_pixel = 8; // ピクセル読出し前のダミーリードのビット数
            cfg.dummy_read_bits = 1;  // ピクセル以外のデータ読出し前のダミーリードのビット数
            cfg.readable = true;      // データ読出しが可能な場合 trueに設定
            cfg.invert = false;       // パネルの明暗が反転してしまう場合 trueに設定
            cfg.rgb_order = false;    // パネルの赤と青が入れ替わってしまう場合 trueに設定
            cfg.dlen_16bit = true;    // データ長を16bit単位で送信するパネルの場合 trueに設定
            cfg.bus_shared = true;    // SDカードとバスを共有している場合 trueに設定(drawJpgFile等でバス制御を行います)

            _panel_instance.config(cfg);
        }

        setPanel(&_panel_instance); // 使用するパネルをセットします。
    }
};

LGFX lcd;
//int pos[2] = {0, 0};
int err_code = 0;

// transform touch screen pos
void pos_rotation(int *l_pos, int rotation)
{
    if (l_pos[0] == -1)
        return;
    if (rotation == 0)
    {
        return;
    }
    if (rotation == 1)
    {
        l_pos[0] = 320-l_pos[0];
        //l_pos[1] = tempx;
    }
    if (rotation == 3)
    {
        int tempx = 480 - l_pos[1];
        int tempy = l_pos[0];
        l_pos[0] = tempy;
        l_pos[1] = tempx;
    }
}


//texture_color16_t screen;
texture_color24_t screen;
texture_alpha8_t fontAtlas;

ImGuiContext *context;

void screen_draw()
{

   lcd.startWrite();  // SPIバス確保
  #if 0
   for (uint32_t x = 0; x < screenWidth; ++x) {
     for (uint32_t y = 0; y < screenHeight; ++y) {
      color24_t pixel = screen.at(x,y); lcd.writePixel(x, y, lcd.color888( pixel.R(), pixel.G(), pixel.B()) );
     }
   }
  #endif

    lcd.setAddrWindow(0, 0, screenWidth, screenHeight);
    lcd.writePixels((void*)screen.pixels, screenWidth *screenHeight, false); // OK  // 24bit
    //lcd.pushPixels((void*)screen.pixels, screenWidth *screenHeight, false);  // 24bit
    //lcd.writePixelsDMA((void*)screen.pixels, screenWidth *screenHeight, false);  // 24bit

   lcd.endWrite();    // SPIバス解放
}

// // This callback will be called when a Bluetooth LE connection is made or broken.
// // You can update the ESP 32's UI or take other action here.
// void onBLEStateChanged(BLENotifications::State state) {
//   switch(state) {
//       case BLENotifications::StateConnected:
//           Serial.println("StateConnected - connected to a phone or tablet"); 
//           break;

//       case BLENotifications::StateDisconnected:
//           Serial.println("StateDisconnected - disconnected from a phone or tablet"); 
//           notifications.startAdvertising(); 
//           break; 
//   }
// }

// // Setup a callback for when a notification arrives
// void onNotificationArrived(const ArduinoNotification * notification, const Notification * rawNotificationData) {
//     Serial.println(notification->title);
// }

void guiSetup( void ){

  pinMode(LCD_CS, OUTPUT);
    pinMode(LCD_BLK, OUTPUT);

    digitalWrite(LCD_CS, LOW);
    digitalWrite(LCD_BLK, HIGH);

    Serial.begin(115200);
    // // Start looking for a device connection
    // notifications.begin("BLEConnection device name");
    // // Register the callback to be informed when a notification arrives
    // notifications.setConnectionStateChangedCallback(onBLEStateChanged);
    // notifications.setNotificationCallback(onNotificationArrived);

    lcd.init();
    lcd.fillScreen(TFT_BLUE);
//    lcd.setTextColor(TFT_YELLOW);
//    lcd.setTextSize(2);
//    lcd.setCursor(0, 0);
//    lcd.print("Makerfabs ESP32-S3");
//    lcd.setCursor(0, 16);
//    lcd.print("Parallel TFT with Touch");

    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);

//    lcd.setCursor(0, 32);
//    if (SD_init())
//    {
//        lcd.print("ERROR:   SD");
//        err_code += 1;
//    }
//    else
//        lcd.print("SD INIT OVER");

    //I2C init
    Wire.begin(I2C_SDA, I2C_SCL);
    byte error, address;

    Wire.beginTransmission(i2c_touch_addr);
    error = Wire.endTransmission();
    lcd.setRotation(ROTATION);
    if (error == 0)
    {
      // lcd.setCursor(0, 16);
      // Serial.print("I2C device found at address 0x");
      //   Serial.print(i2c_touch_addr, HEX);
      //   Serial.println("  !");
      //   lcd.print("TOUCH INIT OVER");
    }
    else
    {
      lcd.setCursor(0, 16);
      Serial.print("Unknown error at address 0x");
      Serial.println(i2c_touch_addr, HEX);
      lcd.print("ERROR:   TOUCH");
      err_code += 1;
    }

    if (err_code)
    {
      lcd.setCursor(0, 64);
      lcd.print("SOMETHING WRONG");
      while (1);
    }
    // else
    //     lcd.print("ALL SUCCESS");

    lcd.setColorDepth(lgfx::rgb888_3Byte);
    //lcd.setColorDepth(lgfx::rgb565_2Byte);
    //lcd.startWrite();  // SPIバス確保

  /* initilise  physical Keyboard */
  minikeyboardSetup();

  context = ImGui::CreateContext();

  ImGui_ImplSoftraster_Init(&screen);

  ImGuiStyle &style      = ImGui::GetStyle();
  style.AntiAliasedLines = false;
  style.AntiAliasedFill  = false;
  style.WindowRounding   = 0.0f;

  ImGuiIO &io = ImGui::GetIO();
  io.Fonts->Flags |=
    ImFontAtlasFlags_NoPowerOfTwoHeight /* | ImFontAtlasFlags_NoMouseCursors */;

  uint8_t *pixels;
  int width, height;
  io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);
  fontAtlas.init(width, height, (alpha8_t *)pixels);
  io.Fonts->TexID = &fontAtlas;

  screen.init(screenWidth, screenHeight);

  io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
  io.MouseDown[0] = false; // left
  io.MouseDown[1] = false; // right
  io.MouseDown[2] = false; // middle + extras
  io.MouseDown[3] = false; // 
  io.MouseDown[4] = false; // 

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






}

unsigned long drawTime;
unsigned long renderTime;
unsigned long rasterTime;
unsigned long lastTime = 0;

static float f         = 0.0f;
static unsigned long t = 0;
// static uint16_t touchX = 0;
// static uint16_t touchY = 0;
static float touchX = 0;
static float touchY = 0;

void guiLoop()
{

  ImGuiIO &io  = ImGui::GetIO();
  unsigned long current_time = millis();
  io.DeltaTime =  (static_cast<float>(current_time - lastTime)) / 1000;
  lastTime = current_time;
  minikeyboardLoop(io);

{
    bool touched;
    int l_pos[2] = {0, 0};
    get_pos(l_pos);
    pos_rotation(l_pos, ROTATION);
    //touched = !((l_pos[0] == screenHeight) && ( l_pos[1] == 0));
    touched = (l_pos[0] != -1);

    if( touched == true ) {
      touchY = (float)l_pos[0];
      touchX = (float)l_pos[1];
      io.MousePos = ImVec2(touchX, touchY);

      //Mouse buttons: 0=left, 1=right, 2=middle + extras. ImGui itself mostly only uses left button (BeginPopupContext** are using right button). Others buttons allows us to track if the mouse is being used by your application + available to user as a convenience via IsMouse** API.
      io.MouseDown[0] = true; // left
      io.MouseDown[1] = false; // right
      io.MouseDown[2] = false; // middle + extras
      io.MouseDown[3] = false; // 
      io.MouseDown[4] = false; // 

      Serial.printf( "Data x %f %x\r\n", touchX, l_pos[1] );
      Serial.printf( "Data y %f %x\r\n", touchY, l_pos[0] );

    } else if (io.MouseDown[0] == true || 
              io.MouseDown[1] == true || 
              io.MouseDown[2] == true || 
              io.MouseDown[3] == true ||                             
              io.MouseDown[4] == true 
              ) {
      io.MouseDown[0] = false; // left
      io.MouseDown[1] = false; // right
      io.MouseDown[2] = false; // middle + extras
      io.MouseDown[3] = false; // 
      io.MouseDown[4] = false; // 
    } else {
     //Mouse buttons: 0=left, 1=right, 2=middle + extras. ImGui itself mostly only uses left button (BeginPopupContext** are using right button). Others buttons allows us to track if the mouse is being used by your application + available to user as a convenience via IsMouse** API.
      io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
    }

}

  /* [0.0f - 1.0f] */
  io.NavInputs[ImGuiNavInput_Activate] =
    0.0f; // activate / open / toggle / tweak value       // e.g. Circle (PS4),
          // A (Xbox), B (Switch), Space (Keyboard)
  io.NavInputs[ImGuiNavInput_Cancel] =
    0.0f; // cancel / close / exit                        // e.g. Cross (PS4),
          // B (Xbox), A (Switch), Escape (Keyboard)
  io.NavInputs[ImGuiNavInput_Input] =
    0.0f; // text input / on-screen keyboard              // e.g. Triang.(PS4),
          // Y (Xbox), X (Switch), Return (Keyboard)
  io.NavInputs[ImGuiNavInput_Menu] =
    0.0f; // tap: toggle menu / hold: focus, move, resize // e.g. Square (PS4),
          // X (Xbox), Y (Switch), Alt (Keyboard)
  io.NavInputs[ImGuiNavInput_DpadLeft] =
    0.0f; // move / tweak / resize window (w/ PadMenu)    // e.g. D-pad
          // Left/Right/Up/Down (Gamepads), Arrow keys (Keyboard)
  io.NavInputs[ImGuiNavInput_DpadRight] = 0.0f;
  io.NavInputs[ImGuiNavInput_DpadUp]    = 0.0f;
  io.NavInputs[ImGuiNavInput_DpadDown]  = 0.0f;
  io.NavInputs[ImGuiNavInput_TweakSlow] =
    0.0f; // slower tweaks                                // e.g. L1 or L2
          // (PS4), LB or LT (Xbox), L or ZL (Switch)
  io.NavInputs[ImGuiNavInput_TweakFast] =
    0.0f; // faster tweaks                                // e.g. R1 or R2
          // (PS4), RB or RT (Xbox), R or ZL (Switch)

  ImGui_ImplSoftraster_NewFrame();
  ImGui::NewFrame();

  ImGuiWindowFlags window_flags = 0;
  window_flags |= ImGuiWindowFlags_NoMove;
  window_flags |= ImGuiWindowFlags_NoResize;
  ImGui::Begin("Launcher", nullptr, window_flags);

  ImGuiStyle& style = ImGui::GetStyle();
  style.TouchExtraPadding = ImVec2(5.0, 5.0);
  ImGui::SetWindowPos(ImVec2(0.0, 0.0));
  // ImGui::SetWindowSize(ImVec2(SCREENX, SCREENY));
  ImGui::SetWindowSize(ImVec2(screenWidth, screenHeight));

  f += 0.05;
  if (f > 1.0f) f = 0.0f;

  unsigned int deltaTime = millis() - t;
  t += deltaTime;

  deltaTime -= (drawTime + renderTime + rasterTime);

  ImGui::Text("Frame rate: %f", ImGui::GetIO().Framerate);
  ImGui::Text("Render time %d ms", renderTime);
  ImGui::Text("Raster time %d ms", rasterTime);
  ImGui::Text("Hardware write time %d ms", drawTime);
  ImGui::Text("Remaining time %d ms", deltaTime);
  ImGui::SliderFloat("SliderFloat", &f, 0.0f, 1.0f);

  // ImGui::Text("mouse x:%f y:%f ", io.MousePos[0], io.MousePos[1] );
  // ImGui::Text("io.MouseDown[0]:%s left", io.MouseDown[0] == true?"true":"false");
  // ImGui::Text("io.MouseDown[1]:%s right", io.MouseDown[1] == true?"true":"false");
  // ImGui::Text("io.MouseDown[2]:%s center", io.MouseDown[2] == true?"true":"false");
        static int counter = 0;
        float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
        ImGui::PushButtonRepeat(true);
        if (ImGui::ArrowButton("##left", ImGuiDir_Left)) { counter--; }
        ImGui::SameLine(0.0f, spacing);
        if (ImGui::ArrowButton("##right", ImGuiDir_Right)) { counter++; }
        ImGui::PopButtonRepeat();
        ImGui::SameLine();
        ImGui::Text("%d", counter);

  static char str0[128] = "Hello, world!";
  ImGui::InputText("input text", str0, IM_ARRAYSIZE(str0));

  static int clicked = 0;
  if (ImGui::Button("Button"))
      clicked++;
  // if (clicked & 1)
  // {
      ImGui::SameLine();
      ImGui::Text(" %u ",clicked);
  // }

  static bool check = true;
  ImGui::Checkbox("checkbox", &check);

  static int e = 0;
  ImGui::RadioButton("radio a", &e, 0); ImGui::SameLine();
  ImGui::RadioButton("radio b", &e, 1); ImGui::SameLine();
  ImGui::RadioButton("radio c", &e, 2);
  ImGui::End(); // launcher

  renderTime = millis();
  ImGui::Render();
  renderTime = millis() - renderTime;

  rasterTime = millis();
  ImGui_ImplSoftraster_RenderDrawData(ImGui::GetDrawData());
  rasterTime = millis() - rasterTime;

  drawTime = millis();
  screen_draw();
  drawTime = millis() - drawTime;
}
