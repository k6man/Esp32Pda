#pragma once

#include "SPI.h"


//----------------------------------------------------------------
#ifdef MAKERFABS_PARALLEL_TFT

#define TOUCH

#define LGFX_USE_V1
#include <LovyanGFX.hpp>


#define I2C_SCL 39
#define I2C_SDA 38


//#define NS2009_TOUCH  //Resistive screen driver
#define FT6236_TOUCH //Capacitive screen driver

#ifdef NS2009_TOUCH
#include "drivers/NS2009.h"
const int i2c_touch_addr = NS2009_ADDR;
#define get_pos ns2009_pos
#endif

#ifdef FT6236_TOUCH
#include "drivers/FT6236.h"
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
const uint16_t screenWidth  = 480;
const uint16_t screenHeight = 320;

#define ROTATION 1

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

extern LGFX lcd;
extern texture_color24_t screen;
//texture_value8_t screen;
//extern texture_alpha8_t fontAtlas;

inline void pos_rotation(int *l_pos, int rotation)
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

inline void screen_draw()
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

inline void lcdSetup() {
    int err_code = 0;

    pinMode(LCD_CS, OUTPUT);
    pinMode(LCD_BLK, OUTPUT);

    digitalWrite(LCD_CS, LOW);
    digitalWrite(LCD_BLK, HIGH);

    Serial.begin(115200);

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
    //lcd.setColorDepth(lgfx::grayscale_8bit);

    
    //lcd.setColorDepth(lgfx::rgb565_2Byte);
    //lcd.startWrite();  // SPIバス確保

}

inline void lcdLoop(ImGuiIO &io) {
// static uint16_t touchX = 0;
// static uint16_t touchY = 0;
static float touchX = 0;
static float touchY = 0;

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
#endif



#ifdef SHARPMEMORYDISPLAY

#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>

// any pins can be used
#define SHARP_SCK  36
#define SHARP_MOSI 35
#define SHARP_SS   44

const uint16_t screenWidth  = 400;
const uint16_t screenHeight = 240;

extern Adafruit_SharpMem display;

extern texture_value8_t screen;

inline void screen_draw()
{
   for (uint32_t x = 0; x < screenWidth; ++x) {
     for (uint32_t y = 0; y < screenHeight; ++y) {
      value8_t pixel = screen.at(x,y);
      display.drawPixel(x, y, pixel.V());
     }
   }
   display.refresh();
}


#define BLACK 0
#define WHITE 1

#ifdef IMGUIMONOCHROME
inline void imguiMonochromeTheme () {
  ImGuiStyle& style = ImGui::GetStyle();
  style.Alpha = 1.0;
  // style.WindowFillAlphaDefault = 0.83;
  // style.ChildWindowRounding = 3;
  style.WindowRounding = 3;
  style.GrabRounding = 1;
  style.GrabMinSize = 20;
  style.FrameRounding = 3;


  style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.00f, 0.40f, 0.41f, 1.00f);
  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 1.00f, 1.00f, 0.65f);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  style.Colors[ImGuiCol_FrameBg] = ImVec4(0.44f, 0.80f, 0.80f, 0.18f);
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.44f, 0.80f, 0.80f, 0.27f);
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.44f, 0.81f, 0.86f, 0.66f);
  style.Colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.18f, 0.21f, 0.73f);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.27f);
  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.22f, 0.29f, 0.30f, 0.71f);
  style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.44f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
  // style.Colors[ImGuiCol_ComboBg] = ImVec4(0.16f, 0.24f, 0.22f, 0.60f);
  style.Colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 1.00f, 1.00f, 0.68f);
  style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.36f);
  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.76f);
  style.Colors[ImGuiCol_Button] = ImVec4(0.00f, 0.65f, 0.65f, 0.46f);
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.01f, 1.00f, 1.00f, 0.43f);
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.62f);
  style.Colors[ImGuiCol_Header] = ImVec4(0.00f, 1.00f, 1.00f, 0.33f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.42f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
  style.Colors[ImGuiCol_Column] = ImVec4(0.00f, 0.50f, 0.50f, 0.33f);
  style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.00f, 0.50f, 0.50f, 0.47f);
  style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.00f, 0.70f, 0.70f, 1.00f);
  style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
  // style.Colors[ImGuiCol_CloseButton] = ImVec4(0.00f, 0.78f, 0.78f, 0.35f);
  // style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.00f, 0.78f, 0.78f, 0.47f);
  // style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.00f, 0.78f, 0.78f, 1.00f);
  style.Colors[ImGuiCol_PlotLines] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 1.00f, 1.00f, 0.22f);
  // style.Colors[ImGuiCol_TooltipBg] = ImVec4(0.00f, 0.13f, 0.13f, 0.90f);
  style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.04f, 0.10f, 0.09f, 0.51f);
}
#endif

inline void lcdSetup() {
  // start & clear the display
  display.begin();
  display.clearDisplay();

  // text display tests
  // display.setTextSize(1);
  // display.setTextColor(BLACK);
  // display.setCursor(0,0);
  // display.println("Hello, world!");
  // display.setCursor(20,20);
  // display.println("Hello, world!");

}

inline void lcdLoop(ImGuiIO &io) {
  // // Screen must be refreshed at least once per second
  // for(int j=0; j<4; j++) {
  // display.refresh();
  //   delay(500); // 1/2 sec delay
  // } // x4 = 2 second pause between rotations    
}
#endif