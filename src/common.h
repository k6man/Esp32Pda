#pragma once
#include "imgui.h"
#include "board.h"
#include "msc_external_flash_sdcard.hpp"

#include <WiFiUdp.h>
#include <EasyNTPClient.h>
#include <UnixTime.h>

extern WiFiUDP udp;
extern EasyNTPClient ntp;

