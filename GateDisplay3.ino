// GateDisplay
// Version 1.2.6 By Chris Diphoorn April 2026
//
// 1.2.6 Fixed reregister conversation
// 1.3.2 Moved RequestQueue to a deque faster than vector

//ESP-ROM:esp32s3-20210327
// https://github.com/Makerfabs/ESP32-S3-Round-SPI-TFT-with-Touch-1.28

// GC9A01 / ILI9341 LCD Controller Commands Comparison / Mapping by allObjects
// https://espruino.microcosm.app/api/v1/files/9dc1b976d621a2ab3854312cce862c4a9a50dc1b.html

// ** NB ** use USB CDC enabled on Boot to get Serial Debug to work.

// ESP32S3 Dev Module
// Arduino IDE 2.x Settings for ESP32-S3
// Board: ESP32S3 Dev Module
// USB CDC On Boot: Enabled (for debugging via USB)
// Flash Size: 16MB (128Mb)
// Partition Scheme: 16M Flash (3M APP/9.9M FATFS) or "Huge APP"
// PSRAM: OPI PSRAM (for modules with 8MB PSRAM)
// Events run on CORE 0
// Arduino runs on CORE 1
// Flash Mode: QIO 80MHz
// Flash Frequency: 80MHz
// Upload Speed: 921600 or 115200

// COM: ESP32 Family Device


#include <Arduino.h>
#include <esp_system.h>  // Required for esp_random() in ESP-IDF
#include "esp_random.h"
#include <Semaphore.h>  // Required for Multi CPU
#include <ctype.h>
#include <Ticker.h>
Ticker scheduler;                                 // A Scheduler to perfom some actions at a set time of the day.

#include <Arduino_GFX_Library.h>                  // The TFT_eSPI project is forket from this library, so they are compatable
#include <Wire.h>
#include <TFT_eSPI.h>                             //User_Setup_Select.h = FOR other display Drivers supported by TFT_eSPI EG: #include <../TFT_eSPI_Setups/Setup302_Waveshare_ESP32S3_GC9A01.h>
#include "TouchDrvCSTXXX.hpp"  // Part of SensorLib

/* #define USER_SETUP_LOADED 1 // Prevents other setup files from loading


#define ST7789_DRIVER   
#define Generic_Driver      // Forces TFT_eSPI into a pure memory buffer state

#define TFT_WIDTH  466      // Matches the exact dimensions of your AMOLED panel
#define TFT_HEIGHT 466

// Load the standard font libraries for text generation
#define LOAD_GLCD   
#define LOAD_FONT2  
#define LOAD_FONT4  
#define LOAD_FONT6  
#define LOAD_FONT7  
#define LOAD_FONT8  
#define LOAD_GFXFF  

#define SMOOTH_FONT
*/
#include <SPI.h>

#include "130ptFont.h" 
#include "110ptFont.h" 
#include "64ptFont.h"  
#include "54ptFont.h"  
#include "44ptFont.h"  
#include "32ptFont.h"  

#include <WiFi.h>
#include <ESPmDNS.h>
#include <AsyncTCP.h>
#include "AsyncUDP.h"
#include <ESPAsyncWebServer.h>

#include <WiFiClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <NetworkClient.h>
#include <ESP32Ping.h>
#include <DNSServer.h>

#include <Timezone.h>
#include "time.h"
#include "timezones.h"
#include "Base64.h"

#include "favicon.h"  // FavIcon Image Data -(Served from Memory)
#include "html.h"
#include "crypt.h"

#include <EEPROM.h>
#include "esp_chip_info.h"
#include "esp_flash.h"

#define BUTTON_1_PIN  0   // The physical BOOT button
//#define BUTTON_2_PIN  14  // The physical KEY / USER button

#define SCREEN_WIDTH 466
#define SCREEN_HEIGHT 466
#define SCREEN_CENTER 233
#define PRESS_Y 200
#define PRESS_H 310
#define PRESS_X 86
#define PRESS_W 390

// The large colored box indicating red = open, green = closed, or blue = open for the Box
#define BOX_Y 120             
#define BOX_X 40
#define BOX_HEIGHT 240
#define BOX_WIDTH 386                 //  So the box can fit inside the Screens display.
#define BOX_CENTER 193                //  Box Width /2
#define HEADING_HEIGHT 119            // Height of menu headings (top of screen)

TFT_eSPI tft = TFT_eSPI();

TFT_eSprite canvas = TFT_eSprite(&tft);       // All the display when starting up, then only The Top Part of the display (Heading)
TFT_eSprite display = TFT_eSprite(&tft);      // Middle Box Part of the Display
TFT_eSprite wifibars = TFT_eSprite(&tft);     // Green bars at bottom indicating wifi strength

#define WIFIPOS_Y 410
#define WIFIPOS_X 190

TFT_eSprite menuPosition = TFT_eSprite(&tft);

#define MENUPOS_HEIGHT 19                     // Includes 3 extra Pixels  (radius is 8)
#define MENUPOS_Y 370

// ==== LCD PINS ====
#define LCD_SDIO0 4
#define LCD_SDIO1 5
#define LCD_SDIO2 6
#define LCD_SDIO3 7
#define LCD_SCLK  38
#define LCD_RESET 2
#define LCD_CS    12

#define LCD_WIDTH  466
#define LCD_HEIGHT 466

// ==== TOUCH (I2C) ====
#define IIC_SDA 15
#define IIC_SCL 14
#define TOUCH_INT 11  // Active-LOW interrupt pin signaling a user tap
volatile bool alreadyReading = false;
bool lastPressedState = false;

// Waveshare 1.75 AMOLED Pin Matrix
#define I2C_SDA_PIN         15
#define I2C_SCL_PIN         14
#define TOUCH_INT_PIN       11
#define TOUCH_RST_PIN       -1   // Handled by power management, leave as -1

// Structural tracking fields for normalized coordinates
struct CustomTouchPoint { bool isPressed; int16_t x; int16_t y;};

TouchDrvCSTXXX touch;

// ==== DISPLAY BUS ====
Arduino_DataBus *bus = new Arduino_ESP32QSPI(
  LCD_CS, 
  LCD_SCLK, 
  LCD_SDIO0, 
  LCD_SDIO1,
  LCD_SDIO2, 
  LCD_SDIO3
);

// ==== DISPLAY DRIVER ====
Arduino_CO5300 *gfx = new Arduino_CO5300(
  bus, 
  LCD_RESET, 
  0,              // rotation
  LCD_WIDTH, 
  LCD_HEIGHT, 
  6, 0, 0, 0
);

#define MAX_SESSIONS 10  // Max Unique Web Connections
#define MAX_DEVICES 11   // Max Unique Devices
#define TOKEN_SIZE 11    // 10 chars + null terminator
#define IP_CHAR 46       // IPv6 = 8 groups of 4 hex digits (32 hex digits + 7 colons) = 39 characters.

struct mDNSJob {
    char hostname[27];
};

struct DeviceRegister {
  char token[TOKEN_SIZE];
  char ip[IP_CHAR];
  char hostname[27];
  char hostnamelocal[33];
  char type[16];
  int8_t dBm = 0;

  bool monitored = false;                 // Only really worry about monitored devices
  bool failed = false;                    // Failed monitored devices are then searched for in the network
  bool dead = false;                      // Cant Find it on the network again.
  bool unstable = false;                  // Deemed network connection is bad
  uint8_t pingsSent = 0;                  // Ping Pong
  uint8_t pingsFailures = 0;              // Ping Pong

  unsigned long lastRecoveryAttempt = 0;  // Timestamp of last mDNS search
  unsigned long nextRecoveryAllowed = 0;  // Timestamp for mDNS
  bool isRegistering = false;             // Prevents duplicate triggers during jitter/delay
  bool mDNSTaskRunning = false;           // mDNS is currently running or not.

  float packetLossPercent = 0;
  float lastLatency = 0;                  // Keeps Track of the Last latency value.
  float avgLatency = 0;
  float latencyHistory[5];                // Fixed array for 5 checks
  int historyIdx = 0;                     // History Position in array
  int failCount = 0;                      // How many Pings Failed - (NumPings is max)
  unsigned long lastSent;                 // Last Ping Sent
  unsigned long lastReplyTime;            // Last time ping was sent. You not allowed to flood a device with pings
};
std::vector<DeviceRegister*> active_devices;
// Shadow Index: Maps a hostname string directly to its index slot inside active_devices
std::unordered_map<std::string, DeviceRegister*> device_lookup_map;

// The RequestQueue is to provide the ability only run one request transaction every loop (slows things down a little and allows async requests to be easily processeed)
struct RequestQueue {
  char ip[IP_CHAR];
  char hostname[27];
  char request[20];
  char message[32];
  uint16_t clientPort;
  unsigned int position;  // Queue Position Number
};
std::deque<RequestQueue> request_queue;
const size_t MAX_QUEUE_SIZE = 30;  // Set your maximum allowed limit

unsigned long lastHourlySync = 0;
unsigned long nextSyncInterval = 3600000;  // Start with exactly 1 hour

int8_t currentRSSI = 0;

// Keep Track of the memory
uint32_t totalHeapBytes = 0;
uint32_t maxBlockBytes = 0;

unsigned long previousWiFiMillis = 0;       // Keep track of wifi connectivity
unsigned long WiFiInterval = 30000;         // If Disconnected Check Wifi Status every 30 Secs and reconnect.

#define CONFIG_ASYNC_TCP_MAX_ACK_TIME 5000  // (keep default)
#define CONFIG_ASYNC_TCP_PRIORITY 10        // (keep default)
#define CONFIG_ASYNC_TCP_QUEUE_SIZE 64      // (keep default)
#define CONFIG_ASYNC_TCP_RUNNING_CORE 0     // force async_tcp task to be on same core as the app (default is core 0)
#define CONFIG_ASYNC_TCP_STACK_SIZE 4096    // reduce the stack size (default is 16K)

bool resetNVS = true;                            // Set this true if a new parameter needs to be set. (Boot, then chanage back to false)

#define BLACK 0x0000
#define WHITE 0xFFFF
#define GREEN 0x07E0
#define RED 0xF800
#define YELLOW 0xFFE0
#define BLUE 0x001F
#define PURPLE 0x780F
#define ORANGE 0xFDA0

// Volatile variables are required for ISR data sharing
volatile bool button1Pressed = false;
volatile bool button2Pressed = false;
static bool lastBtn1State = HIGH;
static bool lastBtn2State = HIGH;

// Timing footprints for millisecond software debouncing inside the ISRs
unsigned long lastInterruptTimeBtn1 = 0;
unsigned long lastInterruptTimeBtn2 = 0;
const unsigned long DEBOUNCE_DELAY_MS = 200; // Ignore electrical bounce noise for 200ms

volatile bool displayTapped = false;
volatile unsigned long lastDebounceTime = 0; 
const unsigned long DEBOUNCE_DELAY = 120;  // milliseconds
const unsigned long DEBOUNCE_UNDELAY = 220;  // milliseconds
volatile bool wasPressed = false;
volatile bool wasUnPressed = false;

void IRAM_ATTR handleButton1Press() {
    unsigned long currentTime = millis();
    if (currentTime - lastInterruptTimeBtn1 > DEBOUNCE_DELAY_MS) {
        button1Pressed = true;
        lastInterruptTimeBtn1 = currentTime;
    }
}

volatile unsigned long lastShowButtonTime = 0; 
volatile unsigned long showButtonTap = false;
const unsigned long SHOW_BUTTON_DELAY = 300;  // milliseconds

static SemaphoreHandle_t logMutex = NULL;           // Create a mutex lock handle for logging stability
static QueueHandle_t espRequestQueue = NULL;
unsigned int nextQueueSlot = 0;                     // Queue Slot Counter (Resets every day)
static SemaphoreHandle_t devicesMutex = NULL;       // Used for CORE Locking DeviceRegister
static SemaphoreHandle_t sessionsMutex = NULL;
static QueueHandle_t mDNSRecoveryQueue = NULL;
SemaphoreHandle_t spi_bus_mutex = NULL;   // Handle the Touch Interface

const size_t MAX_MDNS_QUEUE_SIZE = 20;  // Maximum number of pending device recoveries allowed

// Function prototype for the single background worker
void permanentmDNSRecoveryWorker(void* pvParameters);
void backgroundQueueWorker(void* parameter);
void printMemoryStats(void);

CustomTouchPoint getTouchPoint(void);

unsigned short grays[13];

int angle = 0;
int maxRate = 2;
int rate = 2;
int xt = 0, yt = 0;
int xs = 120;
int ys = 12;
int deb = 0;

// Temp and Humidity
volatile float gateHumid = 0;
volatile float gateTemp = 0;
volatile float gatePress = 0;

bool forceDisplay = true;
volatile int prevDisplayMenu = 99;
volatile int displayMenu = 0;
const int maxMenus = 4;

bool refreshingDisplay = false;  // true is a full , false is a partial refresh of the display
unsigned long previousREFRESH = 0;
unsigned long cpuREFRESH = 2000;
unsigned long timeREFRESH = 1000;
uint32_t displayRESET = 20000;  // The time in ms the display will switch back to show the gatestatus.
uint32_t displayTimer = 0;

unsigned long previousBarsREFRESH = 0;
unsigned long barsREFRESH = 4000;

String authToken = "";

char deviceHostName[33] = "GateDisplay1";   // The name of this sensor

char ssid[32] = "WifiSSID";                 //  The Default Wifi SSDI
char ssidpsk[64] = "XXXXXXXXXX";            //  the Default Wifi PSK

#define WEB_PORT 80  // Port 80 Default for http (https is currently not supported)

char NTPServer[32] = "au.pool.ntp.org";     // NTP Server closest to your country
long gmtOffset_sec = 36000;                 // Timezone: GMT 00 = London (0), EST New York (-18000), IST India (+19800), AEST Sydney (36000)
int daylightOffset_sec = 0;                 // Set to 3600 60mins if DST is active, else 0
char myTimeZone[] = "GMT+10";

IPAddress gate_IP(0, 0, 0, 0);              // IP Address of the Gate Sensor. (Manually set if you dont want it to be Searched using MDNS)
IPAddress camera_IP(0, 0, 0, 0);            // IP Address of the Camera Sensor. (Manually set if you dont want it to be search using MDNS)
IPAddress local_IPv6;
char gatesensor[27] = "gatesensor2";
char camerasensor[27] = "gatecamera";

char defaultUserName[32] = "admin";         // Username for the admin access (Set to blank after NVS has been updated)
char defaultUserPassword[32] = "admin";     // Password for the admin access (Set to blank after NVS has been updated)

const bool useDHCP = true;                  // Best option to use DHCP to give this device the ip addresses
const bool enableIPv6 = false;              // Get IPv6 IPaddresses instead if IPv4

const bool useFirewall = true;              // Ensure Connections come from the gatesensor or camerasesnor only.

// If useDHCP is set to false then you must set and use these network details.
// Manually set Static IP address - otherwise DHCP will handle this.
IPAddress local_IP(192, 168, 10, 10);  //Required if not using DHCP

// Manually set Gateway IP address - otherwise DHCP will handle this.
IPAddress gateway_IP(192, 168, 10, 1);  //Required if not using DHCP

// Manually set Subnet Mask - otherwise DHCP will handle this.
IPAddress subnet_IP(255, 255, 255, 0);  //Required if not using DHCP

// Manually set the Primary and Secondard DNS servers
IPAddress primaryDNS(1, 1, 1, 1);    //Required if not using DHCP
IPAddress secondaryDNS(8, 8, 8, 8);  //Required if not using DHCP

static int numPings = 1;     // Default - Raise a little more for reliable response. (more may be slower)
bool monitorGateway = true;  // Set to true to ping the gateway every so often.

// 2.4 GHz Operational thresholds
const int32_t RSSI_BEST    = -30;
const int32_t RSSI_MID     = -75;      // Small packets hold stability longer than large ones
const int32_t RSSI_WORST   = -90;

// Ping ranges tailored for small payloads (low airtime requirement)
const int32_t PING_BEST    = 10;       // Fast, lightweight local frame transmission
const int32_t PING_MID     = 30;       // Moderate contention/retransmissions
const int32_t PING_WORST   = 200;      // Maximum delay under heavy packet loss before drop (System will reboot if this is detected in ping)

// ********************************************************************************************************************************************************************************************
// * Dont modify anything from here onwards
// ********************************************************************************************************************************************************************************************

char lastAction[25];

WiFiUDP ntpUDP;

bool updategate = true;
bool updatebox = true;
bool updatehelp = true;

bool showdatetime = false;  // A flag to show if the datetime has been logged
bool showcputemp = false;   // If Enabled gets the CPU temp details

// Used for Outputing Extra debugging messages
bool debugRequests = true;        // Adds extra info into the log messages about the request
bool debugSentRequests = true;    // Adds extra info about sent Messages
bool debugRequestParams = false;  // Used to Show the parameters and values parsed in received requests
bool debugTokens = true;          // Adds extra info about Tokens
bool debugMDNS = true;            // Adds extra info about MDNS Searches
bool debugQueue = true;           // Adds extra info about Queue Processing
bool debugGateway = true;         // Adds extra info about Queue Processing
bool debugMemory = true; 

bool debugPing = true;  // Adds extra info about Pings and Pongs
bool enablePing = true;

const bool enableNetworkLimiter = false;  // Set False if you dont want to have the device block fast messages from ip Addresses
const bool enableThrash = false;          // Set False if you dont want to have the device block fast messages from ip Addresses

const unsigned long timeoutMDNS = 3000;  // Define timeout time in milliseconds (example: 2000ms = 2s)
const size_t resultsMDNS = 20;

char charDeviceToken[TOKEN_SIZE] = { 0 };
volatile bool badToken = false;

volatile bool tokenReady = false;  // 'volatile' tells the ESP32 this changes in the background
volatile bool requestNewToken = true;
volatile bool requestingToken = false;
unsigned long previousTokenMillis = 0;                // previous checktime.
unsigned long requestTokenInterval = 0;               // 1st time check straight away.
unsigned long requestDefaultTokenInterval = 3600000;  // every hour get a new device token.

const unsigned long timeoutFLASH = 400;  // Define Screen Notification timeout time in milliseconds (example: 2000ms = 2s)
unsigned long previousFLASH = 0;
bool flashState = false;

// Used for Wifi Reconnections
unsigned long previousMillis = 0;
unsigned long interval = 30000;

const char versionID[10] = "1.3.1";

#define XDUID "rr71e233-87d8-4dbf-b1de-3a36d12bb833"  // This identifier is used to ensure that all devices are allowed to communicate
char base64Buffer[128] = {0};
char resultBuffer[128] = {0};

char wifiHostName[33] = {0};
char hostNameLower[33] = {0};
char hostNameUpper[33] = {0};
char deviceregistrar[33] = {0};

char gatesensorlocal[33] = {0};

char charhostcamera[27] = {0};
char charhostcameralocal[33] = {0};

//char* hostcamera = charhostcamera;
//char* hostcameralocal = charhostcameralocal;

//char* WIFIssid = ssid;
//char* WIFIpassword = ssidpsk;

//char* http_username = defaultUserName;
//char* http_password = defaultUserPassword;

//char* myHostName = deviceHostName;
//char* wifihostname = wifiHostName;

unsigned long lastGlobalPing = 0;
const unsigned long PING_INTERVAL = 600000;  // 10 minutes (10 * 60 * 1000)

volatile bool isAsyncClientBusy = false;        // Ensures that Queued tasks dont hammer the Async send Requests

struct LatencyPacket {
  char header[4];
  uint32_t timestamp;
};

bool hasRegistered = false;
bool hasReceivedStatus = false;
bool hasRequestedStatus = false;

// Store the Log Messages
#define LOG_MSG_MAX_LEN 256  // Max characters per log message
#define LOG_BUFFER_SIZE 100  // Number of log messages to store   (256 * 100 Bytes)

// Structure to manage the circular buffer
typedef struct {
  char logs[LOG_BUFFER_SIZE][LOG_MSG_MAX_LEN];
  int head;       // Index for writing the next log
  int tail;       // Index for reading the oldest log (not used for simple overwrite)
  int sentpos;    // Index for the last send logs
  int sentcount;  // Index for the last logs unsent.
  int count;      // Current number of logs in the buffer
} CircularLogBuffer;

CircularLogBuffer logBuffer;

//Used for Log Messages
#define SMALL_BUFFER 64
#define MEDIUM_BUFFER 128
#define LARGE_BUFFER 256
#define LARGEST_BUFFER 512

TaskHandle_t UITaskHandle = NULL;

// Declare Extra Space for other Messages.
static char logMSG[LARGE_BUFFER] = { 0 };
static char messageBuffer[LARGE_BUFFER] = { 0 };
static char asyncBuffer[LARGEST_BUFFER] = { 0 };
static char asyncLogBuffer[LARGE_BUFFER] = { 0 };
static char requestParamsBuffer[LARGE_BUFFER] = { 0 };
static char msgbuffer[LARGEST_BUFFER] = { 0 };
static char pingbuffer[LARGEST_BUFFER] = { 0 };
static char msgbufferMed[32] = { 0 };
static char msgbufferSmall[16] = { 0 };
static char ipBuffer[IP_CHAR] = { 0 };  // IPv4 or IPv6 Addresses Returned into this variable


int move_flag = 0;

char status[11] = "?";
char prevStatus[11] = { 0 };

char gateStatus[11] = "?";
char prevGateStatus[11] = { 0 };

char lidStatus[11] = "?";
char prevLidStatus[11] = { 0 };
volatile bool BoxAlarm = false;
volatile bool GateAlarm = false;
volatile bool SwitchDisplay = false;
volatile bool HelpAlarm = false;

uint16_t gateBackColor = TFT_GREEN;  // CLOSED is Green
uint16_t gateBackColorOpposite = TFT_RED;
uint16_t gateDarkColor = 1664;

char actionFunction[20] = { 0 };

char alarmAction[12] = { 0 };

volatile int8_t dBm = 0;
volatile int8_t prevdBm = 0;

unsigned long lastTimeToClose = 0;
unsigned long lastTimeToOpen = 0;
unsigned long maxTimeToClose = 0;
unsigned long maxTimeToOpen = 0;
unsigned long minTimeToClose = 0;
unsigned long minTimeToOpen = 0;
unsigned long maxBoxOpenTime = 0;

static char lastTimeClosed[10];
static char lastTimeOpened[10];

static char lastBoxClosed[10];
static char lastBoxOpened[10];

// Macro check: Returns true ONLY if we are running on a Native USB-CDC instance
#if defined(ARDUINO_USB_CDC_ON_BOOT) && (ARDUINO_USB_CDC_ON_BOOT == 1)
#define IS_NATIVE_USB 1
#else
#define IS_NATIVE_USB 0
#endif

// Define Character Sanatize Values. To ensure that no illegal chars can be used.
const char* allowedCharsNumber = "0123456789.";
const char* allowedCharsIP = "0123456789.:abcdefABCDEF/%";
const char* allowedCharsText = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_ .-";
const char* allowedCharsDomain = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.-";
const char* allowedChars_Password = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789@#$%^&*()-_+={}[]|\;:,./?\"";

struct RequestContext {
  //IPAddress* ip;
  char* ip;
  char* host;
  char* local;
  char* url;
  char token[32];         // Safe local snapshot storage for charDeviceToken
  char encryptedB64[64];  // Safe local buffer for the base64 encrypted string
};

struct Session {
  char token[TOKEN_SIZE];
  uint16_t port;
  char ip[IP_CHAR];
  char clientID[16];
  unsigned long createdAt;
  bool adminSession;
};
std::vector<Session> active_sessions;

static unsigned long lastSessionCleanup = 0;

// Network Limiter Settings
const unsigned long REQUEST_COOLDOWN_MS = 50;  // Min time between requests
const unsigned long PING_COOLDOWN_MS = 7;      // Min time between Ping / Pong requests
const size_t MAX_TRACKED_IPS = 48;             // Prevent RAM overflow

class ThreadSafeRateLimiter {
private:
  std::unordered_map<std::string, unsigned long> clients;
  std::list<std::string> lru_list;
  std::unordered_map<std::string, std::list<std::string>::iterator> lru_map;

  // Tracks whole 32-bit representations of whitelisted IPs
  std::vector<uint32_t> whitelistedIPs;

  SemaphoreHandle_t mutex;

  // Internal lookup check (assumes mutex is already acquired)
  bool isWhitelistedInternal(IPAddress ip) {
    uint32_t targetIP = (uint32_t)ip;
    for (uint32_t trusted : whitelistedIPs) {
      if (trusted == targetIP) {
        return true;
      }
    }
    return false;
  }

  bool checkAndStore(const std::string& ipKey, unsigned long now, unsigned long cooldown) {
    auto it = clients.find(ipKey);

    if (it != clients.end()) {
      if (now - it->second < cooldown) {
        return true;
      }
      it->second = now;
      refreshLRU(ipKey);
    } else {
      if (clients.size() >= MAX_TRACKED_IPS) {
        evictOldest();
      }
      clients[ipKey] = now;
      insertLRU(ipKey);
    }
    return false;
  }

  


  void refreshLRU(const std::string& ipKey) {
    lru_list.erase(lru_map[ipKey]);
    lru_list.push_front(ipKey);
    lru_map[ipKey] = lru_list.begin();
  }

  void insertLRU(const std::string& ipKey) {
    lru_list.push_front(ipKey);
    lru_map[ipKey] = lru_list.begin();
  }

  void evictOldest() {
    if (lru_list.empty()) return;
    std::string oldest = lru_list.back();
    lru_list.pop_back();
    clients.erase(oldest);
    lru_map.erase(oldest);
  }

public:
  ThreadSafeRateLimiter() {
    mutex = xSemaphoreCreateMutex();
  }

  // --- Dynamic Whitelist Management (Thread-Safe) ---

  // Adds an IP to the whitelist dynamically
  bool addTrustedIP(IPAddress ip) {
    if (xSemaphoreTake(mutex, pdMS_TO_TICKS(20)) == pdTRUE) {
      if (!isWhitelistedInternal(ip)) {
        whitelistedIPs.push_back((uint32_t)ip);
      }
      xSemaphoreGive(mutex);
      return true;
    }
    return false;  // Mutex timeout
  }

  // Removes an IP from the whitelist dynamically
  bool removeTrustedIP(IPAddress ip) {
    uint32_t targetIP = (uint32_t)ip;
    bool removed = false;

    if (xSemaphoreTake(mutex, pdMS_TO_TICKS(20)) == pdTRUE) {
      for (auto it = whitelistedIPs.begin(); it != whitelistedIPs.end(); ++it) {
        if (*it == targetIP) {
          whitelistedIPs.erase(it);
          removed = true;
          break;
        }
      }
      xSemaphoreGive(mutex);
    }
    return removed;
  }

  // Clear all whitelisted entries
  void clearWhitelist() {
    if (xSemaphoreTake(mutex, pdMS_TO_TICKS(20)) == pdTRUE) {
      whitelistedIPs.clear();
      xSemaphoreGive(mutex);
    }
  }

  // --- Primary Access Point ---
  bool shouldBlock(IPAddress ip, unsigned long cooldown) {
    bool block = false;

    if (xSemaphoreTake(mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
      // Check dynamic whitelist vector first
      if (isWhitelistedInternal(ip)) {
        xSemaphoreGive(mutex);
        return false;
      }

      // Fallback to rate limiting calculations
      std::string ipKey = ip.toString().c_str();
      block = checkAndStore(ipKey, millis(), cooldown);

      xSemaphoreGive(mutex);
    } else {
      return true;  // Fail secure under heavy thread contention
    }
    return block;
  }
};
ThreadSafeRateLimiter limiter;


class RateLimitFilter : public AsyncWebHandler {
  private:
    // --- Flash-Optimised URL Whitelist (Zero-Allocation) ---
    const char* whitelistedURLs[7] = {
      "/register",
      "/events",
      "/logs",
      "/logmessages",
      "/ssidlist",
      "/sync",
      "/admin"
    };
    const size_t whitelistSize = 3;

    // Helper to check for static browser assets using raw character pointers
    bool isStaticAsset(const char* url) const {
      if (url == nullptr) return false;
      size_t len = strlen(url);
      if (len < 4) return false;  // Minimum length check (e.g., ".js")

      // Point directly to the last 3 and 4 characters of the path string
      const char* ext3 = &url[len - 3];
      const char* ext4 = &url[len - 4];

      return (strcmp(ext4, ".ico") == 0 || strcmp(ext4, ".png") == 0 || strcmp(ext4, ".jpg") == 0 || strcmp(ext4, ".css") == 0 || strcmp(ext3, ".js") == 0);
    }

    // Helper to check if the path matches whitelisted URLs
    bool isWhitelistedURL(const char* url) const {
      if (url == nullptr) return false;
      for (size_t i = 0; i < whitelistSize; i++) {
        if (strcmp(url, whitelistedURLs[i]) == 0) {
          return true;
        }
      }
      return false;
    }

  public:
    bool canHandle(AsyncWebServerRequest* request) const override {

      if (enableThrash == false) return false;

      // Grab the raw C-string pointer directly from the library object
      const char* currentURL = request->url().c_str();

      // Skip rate limiter for icons and styles
      if (isStaticAsset(currentURL)) {
        return false;
      }

      // Skip rate limiter for critical whitelisted paths
      if (isWhitelistedURL(currentURL)) {
        return false;
      }

      // Evaluate tracking maps for standard endpoints
      IPAddress clientIP = request->client()->remoteIP();
      if (limiter.shouldBlock(clientIP, REQUEST_COOLDOWN_MS)) {
        return true;  // Block this client and handle with a 429 response
      }

      return false;  // Safe client, pass down to normal endpoints
    }

  void handleRequest(AsyncWebServerRequest* request) override {
    request->send(429, "text/plain", "Too Many Requests (Rate Limited)");
  }

  // Boilerplate configurations to satisfy the parent handler class template
  void handleUpload(AsyncWebServerRequest* request, const String& filename, size_t index, uint8_t* data, size_t len, bool final) override {}
  void handleBody(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) override {}
};

typedef void (*DelegateFunction)(); //used to create callbacks or event handlers

// Define the Event Structure
struct Event {
  int hour;
  int minute;
  DelegateFunction action;  // The placeholder for your function
  const char* name;         // Label for debugging
};

// The Schedule Array
// Map times to the specific functions defined above
Event schedule[] = {
  { 0, 01, resetQueue, "Reset Queue" },
  { 0, 0, printMemoryStats, "Memory" },
  { 8, 0, printMemoryStats, "Memory" },
  { 10, 0, printMemoryStats, "Memory" },
  { 12, 0, printMemoryStats, "Memory" },
  { 14, 0, printMemoryStats, "Memory" },
  { 16, 0, printMemoryStats, "Memory" },
  { 18, 0, printMemoryStats, "Memory" },
  { 20, 0, printMemoryStats, "Memory" },
  { 22, 0, printMemoryStats, "Memory" }
};

// Calculate array size automatically
const int scheduleCount = sizeof(schedule) / sizeof(schedule[0]);

const char* getAsyncErrorDescription(int8_t error) {
  switch (error) {
    case 0: return "OK: No error";
    case -1: return "ERR_MEM: Out of memory";
    case -2: return "ERR_BUF: Buffer error";
    case -3: return "ERR_TIMEOUT: Timeout";
    case -4: return "ERR_RTE: Routing problem";
    case -5: return "ERR_INPROGRESS: Operation in progress";
    case -11: return "ERR_CONN: Not connected";
    case -13: return "ERR_ABRT: Connection aborted";
    case -14: return "ERR_RST: Connection reset";
    case -15: return "ERR_CLSD: Connection closed";
    case -55: return "ERR_DNS: Host not found / DNS failure";
    default: return "Unknown Error";
  }
};

void safeCleanupRequest(AsyncClient* client, RequestContext* ctx);

String readAllLogs() {
  String loglines = "";

  int i;
  int index;

  for (i = 0; i < logBuffer.count; i++) {
    index = (logBuffer.head - logBuffer.count + i + LOG_BUFFER_SIZE) % LOG_BUFFER_SIZE;
    loglines += "<p>" + String(logBuffer.logs[index]) + "</p>";
  }

  return loglines;
};

String readSendLogs() {
  String loglines = "";
  if (logBuffer.sentcount > 0) {
    int i;
    int index;

    for (i = 0; i < logBuffer.sentcount; i++) {
      index = (logBuffer.sentpos + i + LOG_BUFFER_SIZE) % LOG_BUFFER_SIZE;
      loglines += "<p>" + String(logBuffer.logs[index]) + "</p>";
    }
  }
  return loglines;
};

// Function to initialize the log buffer and set the data to blank
void initLogBuffer() {
  char blanklog[LOG_MSG_MAX_LEN] = { 0 };
  int i;

  for (i = 0; i < logBuffer.count; i++) {
    strncpy(logBuffer.logs[i], blanklog, LOG_MSG_MAX_LEN - 1);
  }

  logBuffer.head = 0;
  logBuffer.tail = 0;  // Tail can stay at 0 if we just want to read all
  logBuffer.count = 0;
  logBuffer.sentpos = 0;
  logBuffer.sentcount = 0;
}

/// Function to store a new log message into memory array
void storeLog(const char* message) {

  snprintf(logBuffer.logs[logBuffer.head], LOG_MSG_MAX_LEN, "%s", message);

  if (logBuffer.sentcount == 0) {
    logBuffer.sentpos = logBuffer.head;
  }
  logBuffer.sentcount++;

  // Move the head to the next position (wrap around if necessary)
  logBuffer.head = (logBuffer.head + 1) % LOG_BUFFER_SIZE;

  // If the buffer is not full, increment the count. If it is full, the head
  // will wrap around and overwrite the oldest log, which is the desired rotating behavior.
  if (logBuffer.count < LOG_BUFFER_SIZE) {
    logBuffer.count++;
  }
}

String processor(const String& var) {
  if (var == "WIFIHOSTNAME") {
    return String(wifiHostName);
  }
  if (var == "HOSTNAME") {
    return String(deviceHostName);
  }
  if (var == "VERSION") {
    return String(versionID);
  }

  return String();
};

// Forsecurity purposes uses this function
String adminprocessor(const String& var) {

  if (var == "VERSION") {
    return String(versionID);
  }
  if (var == "WIFIHOSTNAME") {
    return String(wifiHostName);
  }
  if (var == "HOSTNAME") {
    return String(deviceHostName);
  }
  if (var == "ADMINUSERNAME") {
    return String(defaultUserName);
  }
  if (var == "ADMINPASSWORD") {
    return String(defaultUserPassword);
  }
  if (var == "IP") {
    return String(local_IP.toString().c_str());
  }
  if (var == "GATEWAY") {
    return String(gateway_IP.toString().c_str());
  }
  if (var == "SUBNET") {
    return String(subnet_IP.toString().c_str());
  }
  if (var == "DNS1") {
    return String(primaryDNS.toString().c_str());
  }
  if (var == "DNS2") {
    return String(secondaryDNS.toString().c_str());
  }
  if (var == "SSID") {
    return String(ssid);
  }
  if (var == "WIFIPASSWORD") {
    return String(ssidpsk);
  }
  if (var == "GATESENSOR") {
    return String(gatesensor);
  }

  if (var == "GATECAMERA") {
    return String(charhostcamera);
  }
  if (var == "NUMPINGS") {
    return String(numPings);
  }
  if (var == "NTPSERVER") {
    return String(NTPServer);
  }
  if (var == "GMTOFFSET") {
    return String(gmtOffset_sec);
  }
  if (var == "DAYLIGHTOFFSET") {
    return String(daylightOffset_sec);
  }

  if (var == "DEBUGREQUESTS") {
    if (debugRequests == true) {
      return String("checked");
    }
    return String("");
  }

  return String();
};

// FNV-1a 32-bit hash function calculated at compile-time
constexpr unsigned int hashString(const char* str, unsigned int h = 2166136261U) {
  return !*str ? h : hashString(str + 1, (h ^ *str) * 16777619U);
}


bool WIFIfailed = false;
volatile unsigned long currentMillis = 0;

AsyncWebServer server(WEB_PORT);

AsyncEventSource events("/events");     // Send(Push) the status of the values back to the clients browser
AsyncEventSource logevents("/logs");    // Send(Push) the logs back to the clients browser

void sanitizeText(char* str, const char* allowed) {
  if (str == NULL || allowed == NULL) return;

  int writeIndex = 0;
  for (int readIndex = 0; str[readIndex] != '\0'; readIndex++) {
    // strchr returns a pointer if the char is found in 'allowed'
    if (strchr(allowed, str[readIndex]) != NULL) {
      str[writeIndex++] = str[readIndex];
    }
  }
  str[writeIndex] = '\0';  // Add new null terminator
}

int getHrSystemUptime() {
  // millis() returns milliseconds; divide by 10 to get 1/100ths of a second
  // This will roll over after ~497 days, which is standard for 32-bit SNMP TimeTicks
  return (int)(millis() / 10);
};

// char original[] = "ESP32 DATA";
// char lowerCopy[sizeof(original)]; // Create a new variable of the same size
// toLowerCaseNew(original, lowerCopy);
// 'original' is still "ESP32 DATA"
// 'lowerCopy' is now "esp32 data"
/*void toLowerCaseNew(const char* src, char* dest) {
  while (*src) {
    *dest = tolower((unsigned char)*src);
    src++;
    dest++;
  }
  *dest = '\0';  // Always add the null terminator at the end
};
*/

void toLowerCaseNew(const char* __restrict src, char* __restrict dest, size_t dest_size) {
  // Fail-safe exit conditions
  if (src == NULL || dest == NULL || dest_size == 0) {
    if (dest != NULL && dest_size > 0) {
      dest[0] = '\0';  // Properly initialize empty string on error
    }
    return;
  }

  size_t i = 0;

  // Copy safely up to dest_size - 1
  while (src[i] != '\0' && i < (dest_size - 1)) {
    dest[i] = (char)tolower((unsigned char)src[i]);
    i++;
  }

  // Hard-coded null-termination guard
  dest[i] = '\0';
}

void toUpperCaseNew(const char* __restrict src, char* __restrict dest, size_t dest_size) {
  // Fail-safe exit conditions
  if (src == NULL || dest == NULL || dest_size == 0) {
    if (dest != NULL && dest_size > 0) {
      dest[0] = '\0';  // Properly initialize empty string on error
    }
    return;
  }

  size_t i = 0;

  // Copy safely up to dest_size - 1
  while (src[i] != '\0' && i < (dest_size - 1)) {
    dest[i] = (char)toupper((unsigned char)src[i]);
    i++;
  }

  // Hard-coded null-termination guard
  dest[i] = '\0';
}

void setup(void) {

  pinMode(BUTTON_1_PIN, INPUT_PULLUP);

  // FALLING fires the exact microsecond the pin transitions to ground
  attachInterrupt(digitalPinToInterrupt(BUTTON_1_PIN), handleButton1Press, FALLING);

  spi_bus_mutex = xSemaphoreCreateMutex();

  //canvas.setColorDepth(16); // Set to 16-bit RGB565 to match the display matrix  https://rgbcolorpicker.com/565
  //canvas.setAttribute(PSRAM_ENABLE, true); // Force allocation to PSRAM (Saves SRAM)

  //  Setup the Gray color array
  int co = 220;
  for (int i = 0; i < 13; i++) {
    grays[i] = tft.color565(co, co, co);
    co = co - 20;
  }

  canvas.createSprite(SCREEN_WIDTH, SCREEN_HEIGHT);

  gfx->begin();
  gfx->setBrightness(180);
  gfx->fillScreen(0);

  canvas.fillSprite(0);
  
  canvas.loadFont(pt64Font);
  canvas.setTextColor(TFT_WHITE, TFT_BLACK);
  canvas.setTextDatum(TC_DATUM);  //Top Centre
  canvas.drawString("GATE", SCREEN_CENTER, 38);
  canvas.drawString("Display", SCREEN_CENTER, 105);
  canvas.unloadFont();

  canvas.loadFont(pt44Font);
  canvas.setTextColor(grays[3], TFT_BLACK);
  canvas.drawString("By C.Diphoorn", SCREEN_CENTER, 322);
  canvas.unloadFont();

  gfx->startWrite();
  gfx->draw16bitBeRGBBitmap(0,0,(uint16_t*)canvas.getPointer(), SCREEN_WIDTH, SCREEN_HEIGHT);
  gfx->endWrite(); 

  // Create a structure to hold chip information
  esp_chip_info_t ESP32info;
  esp_chip_info(&ESP32info);

  esp_reset_reason_t reason = esp_reset_reason();

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type, Accept");  // Without this you get net::ERR_CONNECTION_REFUSED

  initDevices();
  initSessions();
  initLogBuffer();  // Set log buffer defaults
  initQueue();      // Initalize Threadsafe Queueing using xSemaphores Run on Core 1
  initmDNSQueue();
  initLogger();     // Initalize Threadsafe Logging usaing xSemaphores

  Serial.begin(115200);
  // Only runs if the compiler is actively mapping Serial to Native USB-CDC
  #if defined(ARDUINO_USB_CDC_ON_BOOT) && (ARDUINO_USB_CDC_ON_BOOT == 1)
    Serial.setTxTimeoutMs(0);  // This stops Serial.flush() from locking up if the PC isn't listening.
  #endif

  #if IS_NATIVE_USB
    // Native USB allows flushing the RX hardware queue directly
    Serial.clearWriteError();  // Clear internal stream error flags if any

    // Timeout loop prevents locking up if USB stack gets corrupted mid-read
    uint32_t startTime = millis();
    while (Serial.available() > 0 && (millis() - startTime < 50)) {
      Serial.read();
    }
  #else
    // Standard ESP32-WROOM UART clear (Fast and memory-safe)
    while (Serial.available() > 0) {
      Serial.read();
    }
  #endif

  toLowerCaseNew(deviceHostName, wifiHostName, 33);
  toLowerCaseNew(deviceHostName, hostNameLower, 33);
  toLowerCaseNew(gatesensor, gatesensorlocal, 33);

  char local[] = ".local";
  strcat(wifiHostName, local);
  strcat(gatesensorlocal, local);
  toUpperCaseNew(wifiHostName, hostNameUpper, 33);

  canvas.loadFont(pt32FontN);
  canvas.setTextColor(TFT_GREEN, TFT_BLACK);
  canvas.drawString(String(ssid), SCREEN_CENTER, 220);
  
  gfx->startWrite();
  gfx->draw16bitBeRGBBitmap(0,0,(uint16_t*)canvas.getPointer(), SCREEN_WIDTH, SCREEN_HEIGHT);
  gfx->endWrite(); 

  // Clear the input buffer before reading new data
  while (Serial.available()) {
    Serial.read();  // Read and discard available bytes
  }

  if (reason == ESP_RST_SW) {
    Serial.println("**** This device was manualy reset.");
  }

  uint32_t flash_size;
  esp_flash_get_size(NULL, &flash_size);

  Serial.println("");
  Serial.printf(" CPU: %dx @ %dMhz FLASH: %u MB", ESP32info.cores, getCpuFrequencyMhz(), flash_size / (1024 * 1024));
  Serial.println("");

  if (deviceHostName) {
    Serial.println("");
    Serial.print("HOST: ");
    Serial.println(deviceHostName);
  }

  Serial.print("WIFI: ");
  Serial.print(ssid);
  Serial.print(" ");

  // We start by connecting to a WiFi network
  WiFi.hostname(wifiHostName);
  WiFi.setHostname(wifiHostName);  // This must be set before wifi mode otherwise it wont work!
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);

  // Configures static IP address
  if (useDHCP == false) {
    if (!WiFi.config(local_IP, gateway_IP, subnet_IP, primaryDNS, secondaryDNS)) {
      Serial.println("");
      Serial.println("ERROR: No LocalIP, Gateway, Subnet, or DNS IPs configured.");
    }
  }

  if (enableIPv6 == true) WiFi.enableIPv6();

  WiFi.setSleep(WIFI_PS_NONE);  // Disable Power Savings

  // Connect Wifi to the network
  WiFi.begin(ssid, ssidpsk);

  // Wait until connection or timeout
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 30000) {
    delay(500);
    if (Serial) Serial.print(".");
  }

  if (Serial) Serial.println();

  if (WiFi.status() != WL_CONNECTED) rebootDevice("No Wifi");

  // Update the Devices Date And Time via Wifi using a UDP NTP Server
  configTime(gmtOffset_sec, daylightOffset_sec, NTPServer);

  // RSSI (Received Signal Strength Indicator) is a measurement of the power of a received radio signal
  // typically expressed in dBm
  dBm = WiFi.RSSI();
  prevdBm = 0;

  String rating = getSignalRating(dBm);

  Serial.print("RSSI: ");
  Serial.print(dBm);
  Serial.print(" ");
  Serial.println(rating);

  Serial.print("CHAN: ");
  Serial.println(WiFi.channel());

  getWifiIP();

  Serial.print("  IP: ");
  Serial.println(local_IP);

  if (local_IPv6) {
    Serial.print("IPV6: ");
    Serial.println(local_IPv6);
  }

  if (wifiHostName) {
    Serial.print("HOST: ");
    Serial.println(wifiHostName);
  }

  Serial.print(" MAC: ");
  Serial.println(WiFi.macAddress());

  Serial.print("  GW: ");
  Serial.println(gateway_IP);

  Serial.print(" DNS: ");
  Serial.print(primaryDNS);
  Serial.print(" ");
  Serial.println(secondaryDNS);

  Serial.print(" NTP: ");
  Serial.println(NTPServer);

 // Enable mDNS
  reconnectMDNS();

  if (gate_IP == IPAddress(0, 0, 0, 0)) {
    gate_IP = findHost(gatesensor);
    if (gate_IP != IPAddress(0, 0, 0, 0)) {
      Serial.print("GATE: ");
      Serial.println(String(gatesensorlocal) + " (" + gate_IP.toString().c_str() + ")");
    }
    addNewDevice(gatesensor, gatesensorlocal, gate_IP.toString().c_str(), "", "gate", 0, true);
  }

  if (monitorGateway == true) {
    if (gateway_IP != IPAddress(0, 0, 0, 0)) {
      addNewDevice("gateway", "gateway", gateway_IP.toString().c_str(), "", "gateway", 0, true);
    }
  }

  limiter.addTrustedIP(local_IP);  // Ensure its own IP address is OK

  server.onNotFound([](AsyncWebServerRequest* request) {
    if (request->method() == HTTP_OPTIONS) {
      request->send(200);
    } else {
      request->send(404);
    }
  });

  server.on("/", HTTP_GET, [gatesensor](AsyncWebServerRequest* request) {
    request->send(200, "text/html", index_html, processor);
  });

  // Send the Favicon
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->client()->setNoDelay(true);
    request->send_P(200, "image/x-icon", Favicon, Favicon_Len);
  });

  server.on("/logmessages", HTTP_GET, [](AsyncWebServerRequest* request) {
    IPAddress remoteIp = request->client()->remoteIP();
    //char localIpBuffer[IP_CHAR] = {0}; 
    const char* localIpBuffer = getIP(&remoteIp);
    //getIP(&remoteIp, localIpBuffer, sizeof(localIpBuffer)); // Fills buffer safely

    if (isBadSessionToken(request)) {
      sendUnAuthorisedRequest(request);

      snprintf(asyncLogBuffer, sizeof(asyncLogBuffer), "<-- | BLOCKED %s Session Web Request /logmessages", localIpBuffer);
      logMessage(asyncLogBuffer);

      return;
    }


    sprintf(asyncLogBuffer, "<-- | /LOGMESSAGES Received from %s", localIpBuffer);
    logMessage(asyncLogBuffer);

    String payload = readAllLogs();
    logBuffer.sentcount = 0;             // reset the amount of logs sent back to 0
    logBuffer.sentpos = logBuffer.head;  // reposition the sent position.

    xSemaphoreGive(logMutex);

    request->send(200, "text/html", payload);

    if (debugRequests == true) {
      snprintf(asyncLogBuffer, sizeof(asyncLogBuffer), "--> | %s | Sent log messages %d bytes", localIpBuffer, payload.length());
      logMessage(asyncLogBuffer);
    }
  });

  server.on("/action", HTTP_GET, [gatesensor](AsyncWebServerRequest* request) {
    IPAddress remoteIp = request->client()->remoteIP();
    //char localIpBuffer[IP_CHAR] = {0}; 
    const char* localIpBuffer = getIP(&remoteIp);
    //getIP(&remoteIp, localIpBuffer, sizeof(localIpBuffer)); // Fills buffer safely
    uint16_t clientPort = request->client()->remotePort();

    char localMsgBuffer[128] = {0}; 

    if (isValidXDUIDToken(request) == false) {
      sendUnAuthorisedRequest(request);
      snprintf(logMSG, sizeof(logMSG), "<-- | BLOCKED %s:%hu Device Web Request /action", localMsgBuffer, clientPort);
      logMessage(logMSG);
      return;
    }

    if (debugRequestParams == true) {
      listRequestParams(request);
    }

    char actionFunction[20] = {0};

    if (debugRequests == true) {
      snprintf(localMsgBuffer, sizeof(localMsgBuffer), "<-- | /ACTION %s Received From %s:%hu", actionFunction, localIpBuffer,  clientPort );
      logMessage(localMsgBuffer);
    }

    unsigned int actionHash = hashString(actionFunction);
    switch (actionHash) {
      case hashString("reregister"): break;  //functionReRegister(AsyncWebServerRequest* request); break;
      case hashString("alarm"):
        checkParameters(request);
        break;  //functionAlarm(AsyncWebServerRequest* request); break;
      case hashString("update"):
        strlcpy(prevStatus, status, sizeof(prevStatus));
        strlcpy(prevGateStatus, gateStatus, sizeof(prevGateStatus));
        strlcpy(prevLidStatus, lidStatus, sizeof(prevLidStatus));
        checkParameters(request);
        break;  //functionUpdate(AsyncWebServerRequest* request); break;
    }

    request->send(200, "text/html", "ok");
  });

  server.on("/update", HTTP_GET, [gatesensor](AsyncWebServerRequest* request) {
    //request->client()->setNoDelay(true);
    IPAddress remoteIp = request->client()->remoteIP();
    //char localIpBuffer[IP_CHAR] = {0}; 
    const char* localIpBuffer = getIP(&remoteIp);
    //getIP(&remoteIp, localIpBuffer, sizeof(localIpBuffer)); // Fills buffer safely
  
    uint16_t clientPort = request->client()->remotePort();

    char localMsgBuffer[128] = {0}; 

    if (isValidXDUIDToken(request) == false) {
      sendUnAuthorisedRequest(request);
      snprintf(localMsgBuffer, sizeof(localMsgBuffer),"<-- | BLOCKED %s:%hu Device Request /update", localIpBuffer, clientPort);
      logMessage(localMsgBuffer);
      return;
    }

    if (debugRequests == true) {
      snprintf(localMsgBuffer, sizeof(localMsgBuffer), "<-- | %s:%hu Received Request /update", localIpBuffer, clientPort);
      logMessage(localMsgBuffer);
    }

    if (debugRequestParams == true) {
      listRequestParams(request);
    }

    strlcpy(prevStatus, status, sizeof(prevStatus));
    strlcpy(prevGateStatus, gateStatus, sizeof(prevGateStatus));
    strlcpy(prevLidStatus, lidStatus, sizeof(prevLidStatus));

    checkParameters(request);

    if (strncmp(status, "CLOSED", 6) == 0 || strncmp(status, "OPEN", 4) == 0) {
      GateAlarm = false;
    }
    if (strncmp(lidStatus, "CLOSED", 6) == 0) {
      BoxAlarm = false;
    }

    if (strncmp(prevStatus, status, 6) != 0 || (strncmp(prevGateStatus, gateStatus, 7) != 0)) {
      prevDisplayMenu = 99;
      displayMenu = 0;
      updategate = true;
      //displayGateStatus();
    } else if (strncmp(prevLidStatus, lidStatus, 6) != 0) {
      updatebox = true;
      prevDisplayMenu = 99;
    }

    hasReceivedStatus = true;

    request->send(200, "text/html", "OK");
  });

  server.on("/alarm", HTTP_GET, [gatesensor](AsyncWebServerRequest* request) {
    //request->client()->setNoDelay(true);
    IPAddress remoteIp = request->client()->remoteIP();
    //char localIpBuffer[IP_CHAR] = {0}; 
    const char* localIpBuffer = getIP(&remoteIp);
    //getIP(&remoteIp, localIpBuffer, sizeof(localIpBuffer)); // Fills buffer safely
    uint16_t clientPort = request->client()->remotePort();
    
    char localMsgBuffer[128] = {0}; 

    if (isValidXDUIDToken(request) == false) {
      sendUnAuthorisedRequest(request);
      snprintf(localMsgBuffer, sizeof(localMsgBuffer), "<-- | %s:%hu | Blocked Device Request /alarm", localIpBuffer, clientPort);
      logMessage(localMsgBuffer);
      return;
    }

    if (debugRequestParams == true) {
      listRequestParams(request);
    }

    BoxAlarm = false;
    GateAlarm = false;

    checkParameters(request);

    if (debugRequests == true) {
      snprintf(localMsgBuffer, sizeof(localMsgBuffer), "<-- | %s:%hu | Received Request /alarm?alarm=%s", localIpBuffer, clientPort, alarmAction);
      logMessage(localMsgBuffer);
    }

    prevDisplayMenu = 99;

    if (strncmp(alarmAction, "boxopen", 7) == 0) {
      strlcpy(lidStatus, "OPEN", sizeof(lidStatus));
      BoxAlarm = true;
      SwitchDisplay = true;
      updatebox = true;
    }

    if (strncmp(alarmAction, "gateopen", 8) == 0) {
      strlcpy(gateStatus, "OPEN", sizeof(gateStatus));
      GateAlarm = true;
      SwitchDisplay = true;
      updategate = true;
      //refreshingDisplay = true;
    }
    if (strncmp(alarmAction, "gateclosed", 10) == 0) {
      strlcpy(gateStatus, "CLOSED", sizeof(gateStatus));
      GateAlarm = true;
      SwitchDisplay = true;
      updategate = true;
    }
    
    if (strncmp(alarmAction, "help", 10) == 0) {
      HelpAlarm = true;
      SwitchDisplay = true;
      updatehelp = true;
    }


    request->send(200, "text/html", "OK");
  });

  // Receieve a Request for this device to Re-Register again.
  server.on("/reregister", HTTP_GET, [gatesensor](AsyncWebServerRequest* request) {
    //request->client()->setNoDelay(true);
    IPAddress remoteIp = request->client()->remoteIP();
    //char localIpBuffer[IP_CHAR] = {0}; 
    const char* localIpBuffer = getIP(&remoteIp);
    //getIP(&remoteIp, localIpBuffer, sizeof(localIpBuffer)); // Fills buffer safely
    
    uint16_t clientPort = request->client()->remotePort();
    char localMsgBuffer[128] = {0}; 

    if (isValidXDUIDToken(request) == false) {
      sendUnAuthorisedRequest(request);
      snprintf(localMsgBuffer, sizeof(localMsgBuffer), "<-- | %s:%hu | Blocked Web Request /reregister", localIpBuffer, clientPort);
      logMessage(localMsgBuffer);
      return;
    }

    if (request->hasParam("hostname")) {
      const AsyncWebParameter* p = request->getParam("hostname");
      if (p != nullptr) {  // Extra safety check
        char tmpname[33];
        strncpy(tmpname, p->value().c_str(), sizeof(tmpname) - 6);
        strlcpy(deviceregistrar, tmpname, sizeof(deviceregistrar));
      }
    }

    snprintf(localMsgBuffer, sizeof(localMsgBuffer), "<-- | %s:%hu %s | Requested this Device to Re-Register", localIpBuffer, clientPort, deviceregistrar);
    logMessage(localMsgBuffer);

    // Force regsitration to happen.
    hasRegistered = false;
    hasRequestedStatus = false;
    hasReceivedStatus = false;
    tokenReady = false;
    requestNewToken = true;
    requestingToken = false;
    previousTokenMillis = 0;
    requestTokenInterval = 0;

    request->send(200, "text/html", "OK");
  });

  // Start WebServer
  server.begin();

  Serial.println(" WEB: http://" + String(wifiHostName) + " - OK");

  Serial.println("");
  Serial.print(" CPU: ");
  Serial.print(returnCPUMhz());
  Serial.println(" Mhz");

  /*
  Serial.print(" TFT: ");
  Serial.print("TFT_MISO ");
  Serial.print(TFT_MISO);
  Serial.print(" ");
  Serial.print("TFT_MOSI ");
  Serial.print(TFT_MOSI);
  Serial.print(" ");
  Serial.print("TFT_SCLK ");
  Serial.print(TFT_SCLK);
  Serial.println("");
  Serial.println("");
  */

  Serial.print("TEMP: ");
  showCPUTemp();
  Serial.println();
  currentRSSI = (int8_t)WiFi.RSSI();

    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, 400000U);

    // Initialize the Touch engine configuration
    touch.setPins(TOUCH_RST_PIN, TOUCH_INT_PIN);
    
    // Begin communication with CST9217 address (0x3A)
    if (!touch.begin(Wire, CST92XX_SLAVE_ADDRESS, I2C_SDA_PIN, I2C_SCL_PIN)) {
        Serial.println("❌ ERROR: Touch screen chip CST9217 not detected on I2C bus!");
        while (1) delay(100);
    }
    
    Serial.println("✅ Touch screen hardware found and listening successfully.");

  canvas.setTextColor(TFT_BLACK, TFT_BLACK);
  canvas.drawString(String(ssid), SCREEN_CENTER, 220);
  
  canvas.setTextColor(TFT_GREEN, TFT_BLACK);
  canvas.drawString("http://" + String(wifiHostName), SCREEN_CENTER, 220);
  canvas.unloadFont();

  gfx->startWrite();
  gfx->draw16bitBeRGBBitmap(0,0,(uint16_t*)canvas.getPointer(), SCREEN_WIDTH, SCREEN_HEIGHT);
  gfx->endWrite(); 

  delay(5000);

  canvas.fillSprite(0);
  canvas.setSwapBytes(1);

  gfx->startWrite();
  gfx->draw16bitBeRGBBitmap(0,0,(uint16_t*)canvas.getPointer(), SCREEN_WIDTH, SCREEN_HEIGHT);
  gfx->endWrite(); 

  display.createSprite(BOX_WIDTH, BOX_HEIGHT);
  wifibars.createSprite(276, 28);
  menuPosition.createSprite(SCREEN_WIDTH-190, MENUPOS_HEIGHT);


  displayWifiBars(0);
  delay(1000);
  
  //Ensure all code that writes to the physical screen happens inside this new task.
  xTaskCreatePinnedToCore(
    uiTaskCode,     // Function that implements the task
    "UITask",       // Name of the task
    4096,           // Stack size in words (4KB is safe for display/Wi-Fi tasks)
    NULL,           // Parameter input
    1,              // Priority of the task
    &UITaskHandle,  // Task handle
    0               // Core ID: 0 (Core 1 is default for Arduino loop)
  );

  triggerStartupCheck();
}

// Getting the request query parameters
void listRequestParams(AsyncWebServerRequest* request) {

  requestParamsBuffer[0] = '\0';

  int params = request->params();
  size_t bufferSize = sizeof(requestParamsBuffer);

  for (int i = 0; i < params; i++) {
    const AsyncWebParameter* p = request->getParam(i);
    // Use strncat to prevent buffer overflow (crashing)
    strncat(requestParamsBuffer, p->name().c_str(), bufferSize - strlen(requestParamsBuffer) - 1);
    strncat(requestParamsBuffer, " = ", bufferSize - strlen(requestParamsBuffer) - 1);
    strncat(requestParamsBuffer, p->value().c_str(), bufferSize - strlen(requestParamsBuffer) - 1);

    snprintf(logMSG, sizeof(logMSG), "--- | %s", requestParamsBuffer);
    logMessage(logMSG);
    memset(requestParamsBuffer, 0, sizeof(requestParamsBuffer));
  }
}

void checkSchedule() {

  if (WiFi.status() == WL_CONNECTED) {
  
    struct tm t;
    if (!getLocalTime(&t)) return;

    // needs to be static so each action is not run multiple times during the same minute.
    static int lastMinute = -1;     
  
    // Only check once per minute to prevent repeated triggering
    if (t.tm_min != lastMinute) {

      for (int i = 0; i < scheduleCount; i++) {
        // Check Hour and Minute match
        if (t.tm_hour == schedule[i].hour && t.tm_min == schedule[i].minute) {
          snprintf(msgbuffer, sizeof(msgbuffer), "TASK Initiating Scheduled Task | %s", schedule[i].name);
          logMessage(msgbuffer);
          schedule[i].action();
        }
      }
      lastMinute = t.tm_min;  // Update logic lock
    }
  
  } // WIFI Connected

}

void reconnectMDNS() {
 
 if (WiFi.status() == WL_CONNECTED) {

    MDNS.end();
    if (!MDNS.begin(deviceHostName)) {
      Serial.println("MDNS: ERROR setting up mDNS on this device.");
    } else {
      MDNS.addService("_http", "_tcp", WEB_PORT);
      MDNS.addServiceTxt("_http", "_tcp", "type", "Display");
      Serial.println("MDNS: Enabled on UDP 5353");
    }
 }

}

void checkParameters(AsyncWebServerRequest* request) {
  
  //  STATUS | GET /update?is=OPEN&Status=Open&lastclose=0&maxclose=0&minclose=0&lastclosed=&lastopen=0&maxopen=0&minopen=0&lastopened=&lastboxopened=&lastboxclosed=&temp=0.00&humid=0.00&press=0.00

  if (request->hasParam("function")) {
    strlcpy(actionFunction, request->getParam("function")->value().c_str(), sizeof(actionFunction));
  }
  // Check if query parameters exist
  if (request->hasParam("Status")) {
    strlcpy(gateStatus, request->getParam("Status")->value().c_str(), sizeof(gateStatus));
  }
  if (request->hasParam("is")) {
    strlcpy(status, request->getParam("is")->value().c_str(), sizeof(status));
  }
  if (request->hasParam("lid")) {
    strlcpy(lidStatus, request->getParam("lid")->value().c_str(), sizeof(lidStatus));
  }
  if (request->hasParam("alarm")) {
    strlcpy(alarmAction, request->getParam("alarm")->value().c_str(), sizeof(alarmAction));
  }
  if (request->hasParam("lastclose")) {
    lastTimeToClose = strtol(request->getParam("lastclose")->value().c_str(), NULL, 10);
  }
  if (request->hasParam("lastopen")) {
    lastTimeToOpen = strtol(request->getParam("lastopen")->value().c_str(), NULL, 10);
  }
  if (request->hasParam("maxclose")) {
    maxTimeToClose = strtol(request->getParam("maxclose")->value().c_str(), NULL, 10);
  }
  if (request->hasParam("maxopen")) {
    maxTimeToOpen = strtol(request->getParam("maxopen")->value().c_str(), NULL, 10);
  }
  if (request->hasParam("minclose")) {
    minTimeToClose = strtol(request->getParam("minclose")->value().c_str(), NULL, 10);
  }
  if (request->hasParam("minopen")) {
    minTimeToOpen = strtol(request->getParam("minopen")->value().c_str(), NULL, 10);
  }
  if (request->hasParam("boxopen")) {
    maxBoxOpenTime = strtol(request->getParam("boxopen")->value().c_str(), NULL, 10);
  }
  if (request->hasParam("lastopened")) {
    strlcpy(lastTimeOpened, request->getParam("lastopened")->value().c_str(), sizeof(lastTimeOpened));
  }
  if (request->hasParam("lastclosed")) {
    strlcpy(lastTimeClosed, request->getParam("lastclosed")->value().c_str(), sizeof(lastTimeClosed));
  }
  if (request->hasParam("lastboxopened")) {
    strlcpy(lastBoxOpened, request->getParam("lastboxopened")->value().c_str(), sizeof(lastBoxOpened));
  }
  if (request->hasParam("lastboxclosed")) {
    strlcpy(lastBoxClosed, request->getParam("lastboxclosed")->value().c_str(), sizeof(lastBoxClosed));
  }
  if (request->hasParam("humid")) {
    String tmpst = String(request->getParam("humid")->value().c_str());
    gateHumid = tmpst.toFloat();
  }
  if (request->hasParam("press")) {
    String tmpst = String(request->getParam("press")->value().c_str());
    gatePress = tmpst.toFloat();
  }
  if (request->hasParam("temp")) {
    String tmpst = String(request->getParam("temp")->value().c_str());
    gateTemp = tmpst.toFloat();
  }
  
  // action is the ctx-url value used in the web request from the gatesensor
  if (request->hasParam("action")) {
    strlcpy(lastAction, request->getParam("action")->value().c_str(), sizeof(lastAction));
  }
}


// Helper function to map indoor Wi-Fi RSSI to an optimal local timeout window
uint32_t getLocalTimeoutByRSSI(int32_t rssi) {
  
  // Cap extreme values
    if (rssi >= RSSI_BEST) {
        return PING_BEST;
    }
    if (rssi <= RSSI_WORST) {
        return PING_WORST;
    }

    // Healthy Range (-30 dBm to -75 dBm)
    if (rssi > RSSI_MID) {
        int32_t rssi_range = RSSI_BEST - RSSI_MID;
        int32_t ping_range = PING_MID - PING_BEST;
        return PING_BEST + ((RSSI_BEST - rssi) * ping_range) / rssi_range;
    }

    // Degraded Range (-75 dBm to -90 dBm)
    int32_t rssi_range = RSSI_MID - RSSI_WORST;
    int32_t ping_range = PING_WORST - PING_MID;
    return PING_MID + ((RSSI_MID - rssi) * ping_range) / rssi_range;

  //if (rssi >= -65) return 80;   // Excellent indoor signal: fast-fail target
  //if (rssi >= -75) return 200;  // Standard indoor signal
  //if (rssi >= -85) return 500;  // Weak signal: give room for hardware retries
  //return 1000;                  // Edge of coverage boundary range

}


void handleHourlyNetworkCheck() {
  // 60 minutes converted to milliseconds (60 min  60 sec * 1000 ms)
  const unsigned long CHECK_INTERVAL_MS = 3600000;
  static unsigned long lastCheckTime = 0;

  // Check if 60 minutes have passed since the last execution window
  // Unsigned subtraction naturally handles the 50-day millis() overflow safely
  if (millis() - lastCheckTime < CHECK_INTERVAL_MS) {
    return;  // Exit instantly. Yield processing back to background SNMP/NTP/Web requests
  }

  // Update the execution timestamp tracker immediately
  lastCheckTime = millis();

  // Verify network hardware interface state before attempting packet transmission
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
}

// https://rgbcolorpicker.com/565
// RGB565 (also known as 16-bit RGB) is a color format that uses 16 bits to represent a color, with 5 bits for the red channel, 6 bits for the green channel, and 5 bits for the blue channel.

void displayGateStatus() {

  displayMenu = 0;

  if (strncmp(prevStatus, status, 6) != 0) {
    updategate = true;
  }

  uint16_t backColor = TFT_GREEN;  // CLOSED is Green
  uint16_t backColorOPP = TFT_RED;
  uint16_t darkColor = 1888; //1664;
  uint16_t darkColorOPP = 59392; //53248;

  int x = BOX_CENTER;
  int y = BOX_HEIGHT/2; 
  display.setTextPadding(0);
  
  if (updategate == true) {
    
    canvas.fillSprite(0);
    canvas.loadFont(pt64Font);
    canvas.setTextDatum(TC_DATUM);  //Top Centre
    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.drawString("GATE", SCREEN_WIDTH/2, 28);
    canvas.unloadFont();
    gfx->startWrite();
    gfx->draw16bitBeRGBBitmap(0,0,(uint16_t*)canvas.getPointer(), SCREEN_WIDTH, HEADING_HEIGHT);
    gfx->endWrite(); 
  }
  
  if (strncmp(status, "OPEN", 4) == 0) {
    backColor = TFT_RED;
    backColorOPP = TFT_GREEN;
    darkColor = 59392;
    darkColorOPP = 1888;
  }
  display.fillSmoothRoundRect(0, 0, BOX_WIDTH, BOX_HEIGHT, 8, backColor, TFT_BLACK);
  //display.drawSmoothRoundRect(0, 0, BOX_WIDTH, BOX_HEIGHT, 8, darkColor, TFT_BLACK);

  display.setTextDatum(4);  //Middle Center
  
  if(showButtonTap == false) {
    display.loadFont(pt130Font);
  } else {
    display.loadFont(pt110Font);
  }
  display.setTextColor(darkColor, backColor);  // Use only 1 Color = looks better if not antialaised
  String showThis = String(status);
  display.drawString(showThis, x-1, y-1);
  display.drawString(showThis, x, y-1);
  display.drawString(showThis, x+1, y-1);
  display.drawString(showThis, x-1, y);
  display.drawString(showThis, x+1, y);
  display.drawString(showThis, x-1, y+1);
  display.drawString(showThis, x, y+1);
  display.drawString(showThis, x+1, y+1);
  if(showButtonTap == false) {
  display.setTextColor(TFT_WHITE, darkColor);
  } else {
    display.setTextColor(TFT_WHITE, darkColor);
  }
  display.drawString(showThis, x, y);
  display.unloadFont();
  
  if (GateAlarm == true) {
    if (flashState == true) {
      display.fillRoundRect(0, 0, BOX_WIDTH, BOX_HEIGHT, 8, backColor);
      display.setTextDatum(4);  //Middle Center
      display.loadFont(pt130Font);
      display.setTextColor(darkColor, backColor);  // looks better if not antialaised
      String showThis = String("LEFT");
      display.drawString(showThis, x-1, y-1);
      display.drawString(showThis, x, y-1);
      display.drawString(showThis, x+1, y-1);
      display.drawString(showThis, x-1, y);
      display.drawString(showThis, x+1, y);
      display.drawString(showThis, x-1, y+1);
      display.drawString(showThis, x, y+1);
      display.drawString(showThis, x+1, y+1);

      display.setTextColor(TFT_WHITE, darkColor);  // looks better if not antialaised
      display.drawString(showThis, x, y);
      display.unloadFont();
    }
  }

  char tempGateStatus[10] = {0};
  toLowerCaseNew(gateStatus, tempGateStatus, 10);

  if (strncmp("Opening", gateStatus, 7) == 0 || strncmp("Closing", gateStatus, 7) == 0 || strncmp("Unknown", gateStatus, 7) == 0 || strncmp("Stopped", gateStatus, 7) == 0) {
    display.loadFont(pt44Font);
    display.setTextDatum(TC_DATUM);              //Top Centre
    display.setTextColor(TFT_WHITE, backColor);  //Antialised with Foreground and background colors
    display.drawString(String(tempGateStatus), BOX_CENTER, BOX_HEIGHT - 65);
    display.unloadFont();
  }

  //if (showButtonTap == true) {
  //  int radius = BOX_HEIGHT/4;
  //  display.fillCircle(BOX_CENTER, BOX_HEIGHT/2, radius, backColorOPP);
  //  display.drawCircle(BOX_CENTER, BOX_HEIGHT/2, radius, darkColorOPP);
  //}

  gfx->startWrite();
  gfx->draw16bitBeRGBBitmap(BOX_X,BOX_Y,(uint16_t*)display.getPointer(),BOX_WIDTH,BOX_HEIGHT);
  gfx->endWrite(); 
  gfx->flush(); 
  updategate = false;

}

void displayBoxStatus() {

  displayMenu = 1;

  if (strncmp(prevLidStatus, lidStatus, 6) != 0) {
    updatebox = true;
  }

  display.setTextPadding(0);

  uint16_t backColor = TFT_GREEN;  // CLOSED is Green
  uint16_t darkColor = 1664;
  
  int x = BOX_CENTER;
  int y = BOX_HEIGHT/2; 

  if (strncmp(lidStatus, "OPEN", 4) == 0) {
    backColor = TFT_BLUE;
    darkColor = 26;
  }
  if (strncmp(lidStatus, "?", 1) == 0) {
    backColor = TFT_BLUE;
    darkColor = 26;
  }

  if (updatebox == true) {
    canvas.fillSprite(0);
    canvas.loadFont(pt64Font);
    canvas.setTextDatum(1);  //Top Centre
    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.drawString("BOX", SCREEN_WIDTH/2, 28);
    canvas.unloadFont();
    gfx->startWrite();
    gfx->draw16bitBeRGBBitmap(0,0,(uint16_t*)canvas.getPointer(), SCREEN_WIDTH, HEADING_HEIGHT);
    gfx->endWrite(); 
    
    display.fillSmoothRoundRect(0, 0, BOX_WIDTH, BOX_HEIGHT, 8, backColor, TFT_BLACK);
    display.setTextDatum(4);  //Middle Center
    display.loadFont(pt130Font);
    display.setTextColor(darkColor, backColor);  // looks better if not antialaised

    String showThis = String(lidStatus);
      display.drawString(showThis, x-1, y-1);
      display.drawString(showThis, x, y-1);
      display.drawString(showThis, x+1, y-1);
      display.drawString(showThis, x-1, y);
      display.drawString(showThis, x+1, y);
      display.drawString(showThis, x-1, y+1);
      display.drawString(showThis, x, y+1);
      display.drawString(showThis, x+1, y+1);

    display.setTextColor(TFT_WHITE, backColor);  // looks better if not antialaised
    display.drawString(showThis, x, y);
    display.unloadFont();
  }

  if (BoxAlarm == true) {
    if (flashState == true) {
      display.fillRoundRect(0, 0, BOX_WIDTH, BOX_HEIGHT, 8, backColor);
      display.setTextDatum(4);  //Middle Center
      display.loadFont(pt130Font);
      String showThis = "LEFT";
      display.setTextColor(darkColor, backColor);  // looks better if not antialaised
      display.drawString(showThis, x-1, y-1);
      display.drawString(showThis, x, y-1);
      display.drawString(showThis, x+1, y-1);
      display.drawString(showThis, x-1, y);
      display.drawString(showThis, x+1, y);
      display.drawString(showThis, x-1, y+1);
      display.drawString(showThis, x, y+1);
      display.drawString(showThis, x+1, y+1);

      display.setTextColor(TFT_WHITE, backColor);  // looks better if not antialaised
      display.drawString(showThis, x, y);
      display.unloadFont();
    }
  }

  gfx->startWrite();
  gfx->draw16bitBeRGBBitmap(BOX_X,BOX_Y,(uint16_t*)display.getPointer(),BOX_WIDTH,BOX_HEIGHT);
  gfx->endWrite(); 
  gfx->flush(); 
  updatebox = false;
}

void displayHelp() {

  displayMenu = 10;
  
  uint16_t backColor = TFT_YELLOW; 
  uint16_t darkColor = 1664;
  int x = BOX_CENTER;
  int y = BOX_HEIGHT/2; 
  display.setTextPadding(0);

  if (updatehelp == true) {
    //display.deleteSprite();
    //display.createSprite(SCREEN_WIDTH, SCREEN_HEIGHT -100);
    //display.setSwapBytes(1);
    canvas.fillSprite(0);
    canvas.loadFont(pt54Font);
    canvas.setTextDatum(1);  //Top Centre
    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.drawString("ALERT",SCREEN_CENTER, 28);
    canvas.unloadFont();

    display.fillRoundRect(0, 0, BOX_WIDTH, BOX_HEIGHT, 8, backColor);

    display.setTextDatum(4);  //Middle Center
    display.loadFont(pt64Font);
    display.setTextColor(darkColor, backColor);  // looks better if not antialaised

    String showThis = String("HELP");
      display.drawString(showThis, x-1, y-1);
      display.drawString(showThis, x, y-1);
      display.drawString(showThis, x+1, y-1);
      display.drawString(showThis, x-1, y);
      display.drawString(showThis, x+1, y);
      display.drawString(showThis, x-1, y+1);
      display.drawString(showThis, x, y+1);
      display.drawString(showThis, x+1, y+1);

    display.setTextColor(TFT_WHITE, backColor);  // looks better if not antialaised
    display.drawString(showThis, x, y);
    display.unloadFont();
  }

  if (HelpAlarm == true) {
    if (flashState == true) {
      display.fillRoundRect(6, 75, 225, 90, 8, backColor);
      display.setTextDatum(4);  //Middle Center
      display.loadFont(pt64Font);
      
      String showThis = "GATE";
      display.setTextColor(darkColor, backColor);  // looks better if not antialaised
      display.drawString(showThis, x-1, y-1);
      display.drawString(showThis, x, y-1);
      display.drawString(showThis, x+1, y-1);
      display.drawString(showThis, x-1, y);
      display.drawString(showThis, x+1, y);
      display.drawString(showThis, x-1, y+1);
      display.drawString(showThis, x, y+1);
      display.drawString(showThis, x+1, y+1);

      display.setTextColor(TFT_WHITE, backColor);  // looks better if not antialaised
      display.drawString(showThis, x, y);
      display.unloadFont();
    }
  }

  gfx->startWrite();
  gfx->draw16bitBeRGBBitmap(0,0,(uint16_t*)canvas.getPointer(), SCREEN_WIDTH, HEADING_HEIGHT);
  gfx->draw16bitBeRGBBitmap(BOX_X, BOX_Y,(uint16_t*)display.getPointer(), BOX_WIDTH, BOX_HEIGHT);
  gfx->endWrite(); 
  gfx->flush(); 
  updatehelp = false;
}

void getWifiIP() {

  local_IP = WiFi.localIP();
  //IPv6 254.128.0.0 = FE80::

  gateway_IP = WiFi.gatewayIP();
  subnet_IP = WiFi.subnetMask();

  if (WiFi.dnsIP(0)) {
    primaryDNS = WiFi.dnsIP(0);
  }
  if (WiFi.dnsIP(1)) {
    secondaryDNS = WiFi.dnsIP(1);
  }
  if (WiFi.linkLocalIPv6()) {
    local_IPv6 = WiFi.linkLocalIPv6();
  }
}

void displayInfo1() {

  displayMenu = 2;
  display.setTextPadding(0);

  if (refreshingDisplay == false) {
    
    canvas.fillSprite(0);
    canvas.loadFont(pt64Font);
    canvas.setTextDatum(TC_DATUM);  //Top Centre
    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.drawString("INFO", SCREEN_CENTER, 28);
    canvas.unloadFont();
    gfx->startWrite();
    gfx->draw16bitBeRGBBitmap(0,0,(uint16_t*)canvas.getPointer(),SCREEN_WIDTH,HEADING_HEIGHT);
    gfx->endWrite(); 

    display.setTextColor(TFT_WHITE, TFT_BLACK);   
    display.loadFont(pt32FontN);
    display.fillSprite(0);
    display.setTextDatum(0);  //Top Left
    display.setTextPadding(0);
    display.drawString("Last Open:", 75, 70);
    display.drawString("Last Closed:", 55, 110);
  }

  display.setTextDatum(4);  // Top Center
  display.loadFont(pt32Font);
  display.setTextColor(TFT_GREEN, TFT_BLACK);
  display.drawString(wifiHostName, BOX_CENTER, 20);
  display.setTextColor(TFT_WHITE, TFT_BLACK);
  display.loadFont(pt32FontN);
  display.setTextPadding(20);
  display.drawString(returnMinDateTime(), BOX_CENTER, 185);
  display.setTextPadding(80);
  display.setTextDatum(0);  //Top Left
  display.drawString(returnLastOpened(), 190, 70);
  display.drawString(returnLastClosed(), 190, 110);
  display.setTextPadding(0);
  display.unloadFont();
  gfx->startWrite();
  gfx->draw16bitBeRGBBitmap(BOX_X, BOX_Y, (uint16_t*)display.getPointer(), BOX_WIDTH, BOX_HEIGHT);
  gfx->endWrite(); 
  gfx->flush(); 
  refreshingDisplay = false;
}

void displayInfo2() {

  displayMenu = 3;
  display.setTextPadding(0);

  if (refreshingDisplay == false) {
    canvas.fillSprite(0);
    canvas.loadFont(pt64Font);
    canvas.setTextDatum(1);  //Top Centre
    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.drawString("TEMP",  SCREEN_CENTER, 28);
    canvas.unloadFont();

    gfx->startWrite();
    gfx->draw16bitBeRGBBitmap(0,0,(uint16_t*)canvas.getPointer(), SCREEN_WIDTH, HEADING_HEIGHT);
    gfx->endWrite(); 

    display.fillSprite(0);
    display.setTextDatum(0);  //Top Left
    display.loadFont(pt32FontN);
    display.setTextPadding(0);
    display.setTextColor(TFT_WHITE, TFT_BLACK);
    display.drawString("Temperature:", 45, 20);
    display.drawString("Humidity:", 83, 70);
    display.drawString("Pressure:", 77, 120);
  }

  display.setTextDatum(0);  //Top Left
  display.loadFont(pt32Font);
  display.setTextColor(TFT_WHITE, TFT_BLACK);
  display.setTextPadding(80);
  display.drawString(returnTemp(), 190, 20);
  display.drawString(returnHumid(), 190, 70);
  display.drawString(returnPress(), 190, 120);
  display.setTextPadding(0);
  display.unloadFont();
  
  gfx->startWrite();
  gfx->draw16bitBeRGBBitmap(BOX_X,BOX_Y,(uint16_t*)display.getPointer(),BOX_WIDTH,BOX_HEIGHT);
  gfx->endWrite(); 
  gfx->flush(); 

  refreshingDisplay = false;

}

void displayInfo3() {

  displayMenu = 4;
  display.setTextPadding(0);

  if (refreshingDisplay == false) {
    
    canvas.fillSprite(0);
    canvas.loadFont(pt64Font);
    canvas.setTextDatum(1);  //Top Centre
    canvas.setTextColor(TFT_WHITE, TFT_BLACK);
    canvas.drawString("CPU", SCREEN_CENTER, 28);
    canvas.unloadFont();
 
    gfx->startWrite();
    gfx->draw16bitBeRGBBitmap(0,0,(uint16_t*)canvas.getPointer(), SCREEN_WIDTH, HEADING_HEIGHT);
    gfx->endWrite(); 

    display.fillSprite(0);
    display.setTextDatum(0);  //Top Left
    display.setTextPadding(0);
    display.setTextColor(TFT_WHITE, TFT_BLACK);
    display.loadFont(pt32FontN);
    display.drawString("Free Blocks:", 55, 20);
    display.drawString("Free Heap:", 69, 70);
    display.drawString("CPU Mhz:", 80, 120);
    display.drawString("CPU Temp:", 70, 170);
  }

  display.setTextDatum(0);  //Top Left
  display.loadFont(pt32Font);
  display.setTextColor(TFT_WHITE, TFT_BLACK);
  display.setTextPadding(80);
  display.drawString(getMemoryBlock(), 190, 20);
  display.drawString(getMemoryHeap(), 190, 70);
  display.drawString(getCPUMhz(), 190, 120);
  display.drawString(getCPUTemp(), 190, 170);
  display.unloadFont();
  
  gfx->startWrite();
  gfx->draw16bitBeRGBBitmap(BOX_X,BOX_Y,(uint16_t*)display.getPointer(),BOX_WIDTH,BOX_HEIGHT);
  gfx->endWrite(); 
  gfx->flush(); 
  refreshingDisplay = false;

}

void displayMenuPosition(int menu) {
 
 uint16_t menuColor = grays[1];

  int radius = (MENUPOS_HEIGHT -3)/2;
  int buttonWidth = radius *2;
  int startY = MENUPOS_Y;
  int gap = 10;
  int width = SCREEN_WIDTH - 190;
  int height = buttonWidth+2;
  int MaxMenus = 5;
  int startX = 155;
  //int maxwidth = (MaxMenus * (buttonWidth + gap))+ radius+3;

  for (int i = 0; i < MaxMenus; i++) {
    int x = i +1;
    if (i != menu) {
      menuColor = grays[8];
    } else {
      menuColor = grays[3];
    }
    menuPosition.fillSmoothCircle((x * (buttonWidth + gap)), radius, radius, menuColor, TFT_BLACK);
  }
  
  gfx->startWrite();
  // Update the Display
  gfx->draw16bitBeRGBBitmap(
      startX, startY,               // Physical location on the round display face
      (uint16_t*)menuPosition.getPointer(), // Source pointer aligned exactly to Arduino_GFX specs
      width, MENUPOS_HEIGHT                 // Bounding box dimensions
  );
  gfx->endWrite(); 
  gfx->flush(); 

}

void displayWifiBars(int bars) {

  uint16_t barColor = TFT_GREEN;

  if (WiFi.status() != WL_CONNECTED) {
    barColor = TFT_RED;
    bars = 0;
  }

  int barWidth = 16;
  int barHeight = 28;
  int startX = WIFIPOS_X;
  int startY = WIFIPOS_Y;
  int gap = 4;
  int width = SCREEN_WIDTH - startX;
  int height = barHeight;
  int y = 0;
  int x = 0;

  for (int i = 0; i < 5; i++) {
    y= ((5 - i - 1) * 3);
    x= (i * (barWidth + gap));
    if (i < bars) {
      // Draw filled bar
      wifibars.fillRoundRect(x, y, barWidth, barHeight - y, 2, barColor);
    } else {
      // Draw outline for empty bar
      wifibars.fillRect(x+1, y+1, barWidth-2, barHeight - y-2, TFT_BLACK);
      wifibars.drawRoundRect(x, y, barWidth, barHeight - y, 2, barColor);
    }
  }
  
  gfx->startWrite();
  // Update the Display
  gfx->draw16bitBeRGBBitmap(
      startX, startY,               // Physical location on the round display face
      (uint16_t*)wifibars.getPointer(), // Source pointer aligned exactly to Arduino_GFX specs
      width, height                 // Bounding box dimensions
  );
  gfx->endWrite(); 
  gfx->flush(); 
}

void clearMenu() {
  //gfx->fillRect(5,170, 230, 65, BLACK);
}

void convertToBase64(char* mesg, char* buffer, size_t bufferSize) {

  const int sl = strlen(mesg);
  // Ensure the buffer is large enough for the encoded string and null terminator
  const int el = Base64.encodedLength(sl);

  if (el + 1 <= bufferSize) {
    Base64.encode(buffer, mesg, sl);
    // buffer is already null-terminated by Base64.encode if implemented correctly
  } else {
    // Handle error: buffer too small
    buffer[0] = '\0';  // Set to empty string
  }
}

void convertFromBase64(char* mesg, char* buffer, size_t bufferSize) {

  const int sl = strlen(mesg);
  // Ensure the buffer is large enough for the encoded string and null terminator
  const int el = Base64.decodedLength(mesg, sl);

  if (el + 1 <= bufferSize) {
    Base64.decode(buffer, mesg, sl);
  } else {
    // Handle error: buffer too small
    buffer[0] = '\0';  // Set to empty string
  }
}

// Send a unauthrorised reponse back to the client browser as there was no session token.
void sendUnAuthorisedRequest(AsyncWebServerRequest* request) {
  AsyncWebServerResponse* response = request->beginResponse(401, "text/plain; charset=utf-8", "Unauthorized");
  response->addHeader("WWW-Authenticate", "Bearer realm=\"Secure Area\"");
  request->send(response);
}


  IPAddress findHost(const char* hostname) {
    if (hostname == nullptr || hostname[0] == '\0') return IPAddress(0, 0, 0, 0);
    if (WiFi.status() != WL_CONNECTED) return IPAddress(0, 0, 0, 0);

    // Yield control minimally to ensure system tasks run smoothly
    vTaskDelay(pdMS_TO_TICKS(10)); 

    // 1. FIRST PASS: Fast Native Cache/DNS lookup
    IPAddress resolvedIP;
    if (WiFi.hostByName(hostname, resolvedIP)) {
        if (resolvedIP != IPAddress(0, 0, 0, 0)) {
            return resolvedIP;
        }
    }

    mdns_result_t* results = nullptr;

    // 2. SECOND PASS: Query exact hostname using standard mDNS system queries
    if (enableIPv6) {
        // Query explicitly for IPv6 (AAAA Record) for this exact hostname
        esp_err_t err = mdns_query(hostname, NULL, NULL, MDNS_TYPE_AAAA, timeoutMDNS, 1, &results);
        if (err == ESP_OK && results && results->addr) {
            IPAddress finalIP(
                results->addr->addr.u_addr.ip6.addr[0],
                results->addr->addr.u_addr.ip6.addr[1],
                results->addr->addr.u_addr.ip6.addr[2],
                results->addr->addr.u_addr.ip6.addr[3]
            );
            mdns_query_results_free(results);
            return finalIP;
        }
        if (results) mdns_query_results_free(results);
    }

    // 3. FALLBACK / DEFAULT PASS: Query explicitly for IPv4 (A Record)
    esp_err_t err = mdns_query(hostname, NULL, NULL, MDNS_TYPE_A, timeoutMDNS, 1, &results);
    if (err == ESP_OK && results && results->addr) {
        IPAddress finalIP(results->addr->addr.u_addr.ip4.addr);
        mdns_query_results_free(results);
        return finalIP;
    }

    if (results) mdns_query_results_free(results);
    return IPAddress(0, 0, 0, 0);
  }

/*
IPAddress findHost(const char* hostname) {
  if (hostname == nullptr || hostname[0] == '\0') return IPAddress(0, 0, 0, 0);
  if (WiFi.status() != WL_CONNECTED) return IPAddress(0, 0, 0, 0);

  mdns_result_t* results = nullptr;
  vTaskDelay(pdMS_TO_TICKS(100));  // Yields control to ESP32 system tasks for 100ms
  esp_err_t err = mdns_query_ptr("_http", "_tcp", timeoutMDNS, resultsMDNS, &results);

  if (err != ESP_OK || !results) return IPAddress(0, 0, 0, 0);

  IPAddress finalIP(0, 0, 0, 0);
  bool foundV4 = false;
  bool foundV6 = false;

  // Scan results safely
  for (mdns_result_t* r = results; r != nullptr; r = r->next) {
    if (!r->hostname || strcasecmp(r->hostname, hostname) != 0) {
      continue;
    }

    // Scan addresses for this host
    for (mdns_ip_addr_t* a = r->addr; a != nullptr; a = a->next) {

      // Found IPv4
      if (a->addr.type == IPADDR_TYPE_V4) {
        finalIP = IPAddress(a->addr.u_addr.ip4.addr);
        foundV4 = true;
        break;
      }

      // Found IPv6 (Save it, but keep looking for a V4)
      if (!foundV6 && a->addr.type == IPADDR_TYPE_V6) {
        // Fixed constructor using 4x32-bit words
        finalIP = IPAddress(
          a->addr.u_addr.ip6.addr[0],
          a->addr.u_addr.ip6.addr[1],
          a->addr.u_addr.ip6.addr[2],
          a->addr.u_addr.ip6.addr[3]);
        foundV6 = true;
      }
    }

    if (foundV4) break;
  }

  // This is now guaranteed to run, preventing heaps leaks
  mdns_query_results_free(results);
  return finalIP;
}
*/

String getSignalRating(int8_t dbm) {
  if (dbm >= -50) {
    return " Excellent Signal (Full Strength)";
  } else if (dbm >= -60) {
    return " Good Signal (Strong)";
  } else if (dbm >= -70) {
    return " Fair Signal (Average)";
  } else if (dbm >= -80) {
    return " Weak Signal (Poor)";
  } else {
    return " Very Poor or no signal";
  }
}

void initSessions() {

  if (sessionsMutex == NULL) {
    sessionsMutex = xSemaphoreCreateMutex();
  }
}

void initDevices() {

  if (devicesMutex == NULL) {
    devicesMutex = xSemaphoreCreateCounting(1, 1);
  }
}

void initLogger() {
  // Initialize the logging mutex
  if (logMutex == NULL) {
    logMutex = xSemaphoreCreateMutex();
  }
}

void initmDNSQueue() {

  // Set the size allocation to match our 'mDNSJob' structure type, NOT a pointer type!
  mDNSRecoveryQueue = xQueueCreate(MAX_MDNS_QUEUE_SIZE, sizeof(mDNSJob));
  
  if (mDNSRecoveryQueue != NULL) {
    
    // Allocation increased to 8192 to prevent mDNS stack overflows.
    // Pinned explicitly to Core 1 to leave Core 0 clear for radio traffic.
    xTaskCreatePinnedToCore(
        permanentmDNSRecoveryWorker,  // Function name
        "mDNSWorkerTask",             // Debugging name
        8192,                         // Stack size 8192 protects networking calls
        NULL,                         // No parameters needed
        1,                            // Normal priority
        NULL,                         // No handle tracking needed
        1                             // Core 1 handles user application tasks smoothly
    );
    
    Serial.println("SYSTEM: mDNS WorkerQueue Successfully Initialized.");
  } else {
     Serial.println("ERROR: mDNS Critical Error allocating WorkerQueue!");
  }
}

void resetQueue() {
  nextQueueSlot = 0;
}

void initQueue() {
  
  // Allocates all memory contiguously upfront (30 * 124 bytes)
  espRequestQueue = xQueueCreate(MAX_QUEUE_SIZE, sizeof(RequestQueue));
    
  if (espRequestQueue != NULL) {

    // Start the background worker thread
    xTaskCreatePinnedToCore(
        backgroundQueueWorker,    // Function name
        "QueueWorker",            // Name of the task (for debugging)
        8192,                     // 4096 - Stack size in bytes (allocate more if doing networking)
        NULL,                     // Task input parameter
        1,                        // Priority (1 is standard, higher means more urgent) - 0 to share with IDLE task
        NULL,                     // Task handle
        1                         // Core ID (ESP32 has Core 0 and Core 1. Arduino runs on 1)
    );
    Serial.println("SYSTEM: QUEUE WorkerQueue Successfully Initialized.");
  } else {
    Serial.println("ERROR: QUEUE Critical Error allocating WorkerQueue!");
  }
}


void handleSendLogsUpdate() {
  String payload = "";
  bool updatesAvailable = false;

  // Acquire the lock to read the logs and manipulate pointers safely
  if (logMutex != NULL && xSemaphoreTake(logMutex, pdMS_TO_TICKS(50)) == pdTRUE) {

    if (logBuffer.sentcount > 0) {
      payload = readSendLogs();
      updatesAvailable = true;

      // Safely update your tracking markers while holding the lock
      logBuffer.sentpos = (logBuffer.sentpos + logBuffer.sentcount) % LOG_BUFFER_SIZE;
      logBuffer.sentcount = 0;
    }

    // Release the lock immediately
    xSemaphoreGive(logMutex);
  }

  // Send the payload over the network completely OUTSIDE the locked zone
  if (updatesAvailable && payload.length() > 0) {
    logevents.send(payload.c_str(), "logs");
  }
}

void logMessage(const char* mesg) {
  // Thread-safe local buffers on the stack (unique to each core)
  char dateBuffer[32];
  char microBuffer[8];
  char fullLogBuffer[512];  // Expanded to handle large messages safely

  // Fetch timestamps safely using the thread-safe function variants
  returnDateTimeX(dateBuffer, sizeof(dateBuffer));
  returnDateTimeMicro(microBuffer, sizeof(microBuffer));

  // Assemble the final log string safely in isolated memory
  snprintf(fullLogBuffer, sizeof(fullLogBuffer), "%s.%s | %s", dateBuffer, microBuffer, mesg);

  // Protect shared hardware resources (Serial and Storage) using a Mutex
  if (logMutex != NULL && xSemaphoreTake(logMutex, pdMS_TO_TICKS(10)) == pdTRUE) {

    // Store log safely without other tasks interrupting mid-write
    storeLog(fullLogBuffer);

    // Print to Serial safely
    Serial.println(fullLogBuffer);
    
    // Release the lock immediately for the next core to use
    xSemaphoreGive(logMutex);
  }
}

void rebootDevice(const char* reason) {
  //Save the Last Boot Details
  //preferences.begin("wifi", false);
  //preferences.putString("lastboot", reason);  // Save the reason fror rebooting
  //preferences.end();
  esp_restart();
}


bool isWifiNetworkHealthy() {
  // Hard check: Is the Wi-Fi physically disconnected?
  if (WiFi.status() != WL_CONNECTED) return false;

  // Soft check: Is the master's own signal in the drop-out zone?
  currentRSSI = (int8_t)WiFi.RSSI();
  if (currentRSSI <= -90) {
    // -90 dBm or worse means the master itself is about to disconnect.
    // It will drop packets locally, so we cannot trust network metrics right now.
    return false;
  }

  return true;  // Local connection is stable and trustworthy
}

// a Task Process running in the Core x
// Simply queueing any new requests each to to be kicked off
// Simply queueing any new requests each to to be kicked off
void backgroundQueueWorker(void* parameter) {
  char localMsgBuffer[128] = {0}; 

  while (1) {
    // If Wi-Fi is not connected, yield and wait.
    if (WiFi.status() != WL_CONNECTED) {
      vTaskDelay(pdMS_TO_TICKS(1000)); // Wait 1 second before checking Wi-Fi status again
      continue;  // Jumps back to the top of the while(1) loop
    }

    // BACKOFF GUARD: If the async network client is actively transmitting a payload,
    // Do not pull a new job yet. Yield CPU control and let the client finish.
    if (isAsyncClientBusy) {
      vTaskDelay(pdMS_TO_TICKS(50));
      continue;
    }

    RequestQueue currentJob;

    // EVENT-DRIVEN BLOCKING: Task blocks here automatically with portMAX_DELAY.
    // When the queue is empty, this task goes completely to sleep, allowing the 
    // system IDLE task to run and feed the Watchdog Timer natively with 0% CPU waste.
    if (xQueueReceive(espRequestQueue, &currentJob, portMAX_DELAY) == pdTRUE) {
    
      // Ensure that null termination is guaranteed
      currentJob.request[sizeof(currentJob.request) - 1] = '\0';
      currentJob.ip[sizeof(currentJob.ip) - 1] = '\0';
      currentJob.hostname[sizeof(currentJob.hostname) - 1] = '\0';

      IPAddress addr;
      if (addr.fromString(currentJob.ip)) {  // Ensure the IP string parses successfully

        if (currentJob.request[0] != '\0') {

          if (debugQueue == true) {
              snprintf(localMsgBuffer, sizeof(localMsgBuffer), "--- | QUEUE %u Now Running %s", currentJob.position, currentJob.request);
              logMessage(localMsgBuffer);
          }
          
          // ACTION PROCESSING TREE
          if (strcasecmp(currentJob.request, "ping") == 0) {
            // Runs icmpDevicePing loop outside the mutex boundaries
            triggerPingByIP(currentJob.ip);
            
          } else if (strcasecmp(currentJob.request, "reboot") == 0) {
            rebootDevice("User Requested");
            
          } else if (strcasecmp(currentJob.request, "mdns") == 0) {
            
            // Safely schedules an mDNS discovery job onto the secondary worker thread
            triggerRediscoveryByIP(currentJob.ip);
            
          } else {
            
            if (currentJob.hostname[0] != '\0') {
              sendHTTPrequest(currentJob.request, addr, currentJob.hostname);
            } 
          }

        }  // Request not blank
      }
    
      // Prevent radio buffer flooding
      vTaskDelay(pdMS_TO_TICKS(100));  
    } 

    // Extra heartbeat breathing room slice
    vTaskDelay(pdMS_TO_TICKS(10));

  } // While
}


bool pullFromQueue(RequestQueue& outputItem) {
  if (espRequestQueue == NULL) return false;

    if (xQueueReceive(espRequestQueue, &outputItem, portMAX_DELAY) == pdTRUE) {
      return true;
    }
    
  return false;

}

bool pushToQueue(const RequestQueue& newItem) {
  if (espRequestQueue == NULL) return false;
  
  // Checks space and copies item into the queue.
  // Set wait time to 0 so it returns instantly instead of blocking the async task.
  if (xQueueSend(espRequestQueue, &newItem, 0) == pdTRUE) {
      return true;
  }
  
  return false;

}

bool alreadyInQueue(const char* new_request, const char* new_ip) {
    if (espRequestQueue == NULL) return true;

    RequestQueue item{};
    bool exists = false;
    UBaseType_t queueLength = uxQueueMessagesWaiting(espRequestQueue);

    // Use a temporary buffer to avoid modifying the queue
    RequestQueue buffer[queueLength];

    for (UBaseType_t i = 0; i < queueLength; ++i) {
        if (xQueuePeek(espRequestQueue, &item, 0) == pdTRUE) {
            if (strcmp(item.ip, new_ip) == 0 && strcmp(item.request, new_request) == 0) {
                exists = true;
                break;
            }
            // Store the item in the buffer
            buffer[i] = item;
        }
        // Move to the next item
        xQueueReceive(espRequestQueue, &item, 0);
    }

    // Restore the original queue items
    for (UBaseType_t i = 0; i < queueLength; ++i) {
        xQueueSend(espRequestQueue, &buffer[i], 0);
    }

    return exists;
}

void addToQueue(const char* new_ip, const char* new_hostname, const char* new_request) {
    if (espRequestQueue == NULL) return;

    // Duplicate checks should be handled via the DeviceRegister object flags 
    // before calling addToQueue().

    RequestQueue item{};

    // Wipe the entire struct memory clean with zeros first
    memset(&item, 0, sizeof(RequestQueue));

    // Safely copy string data and force null termination
    strncpy(item.ip, new_ip ? new_ip : "", sizeof(item.ip) - 1);
    item.ip[sizeof(item.ip) - 1] = '\0';

    strncpy(item.hostname, new_hostname ? new_hostname : "", sizeof(item.hostname) - 1);
    item.hostname[sizeof(item.hostname) - 1] = '\0';

    strncpy(item.request, new_request ? new_request : "", sizeof(item.request) - 1);
    item.request[sizeof(item.request) - 1] = '\0';
    
    // Use an atomic block or a brief portMUX lock 
    // if nextQueueSlot is shared across cores, or just utilize FreeRTOS tickers
    portMUX_TYPE myMutex = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&myMutex);
    nextQueueSlot++;
    item.position = nextQueueSlot;
    portEXIT_CRITICAL(&myMutex);
    
    // Thread-Safe Atomic Queue Write
    if (xQueueSend(espRequestQueue, &item, 0) == pdTRUE) {
        if (debugQueue) {
            char localMsgBuffer[128] = {0}; 
            unsigned int currentItems = uxQueueMessagesWaiting(espRequestQueue);

            if (strcmp(item.ip, "0.0.0.0") == 0) {
                snprintf(localMsgBuffer, sizeof(localMsgBuffer), "--- | QUEUE %u.%u Added %s", item.position, currentItems, item.request);
            } else {
                snprintf(localMsgBuffer, sizeof(localMsgBuffer), "--- | QUEUE %u.%u Added %s for %s", item.position, currentItems, item.request, item.ip);
            }
            logMessage(localMsgBuffer);
        }
    }  
    // Handle queue saturation cleanly
    else {
        RequestQueue discardedItem;

        // Eject the single oldest item instantly from the front
        if (xQueueReceive(espRequestQueue, &discardedItem, 0) == pdTRUE) {
            if (debugQueue) {
                char localMsgBADBuffer[128]; 
                snprintf(localMsgBADBuffer, sizeof(localMsgBADBuffer), "--- | QUEUE Hit Max (%u Items) - Dropping Oldest", (unsigned int)MAX_QUEUE_SIZE);
                logMessage(localMsgBADBuffer);
            }
        }

        // Retry the push and check the result to ensure it wasn't sniped by another core
        if (xQueueSend(espRequestQueue, &item, 0) != pdTRUE) {
            if (debugQueue) {
                logMessage("--- | QUEUE ERROR: Failed to insert item after drop purge.");
            }
        }
    }
}


void returnDateTimeX(char* output, size_t size) {
  time_t epochTime = time(NULL);
  struct tm timeinfo;
  localtime_r(&epochTime, &timeinfo);
  strftime(output, size, "%a %d %b %Y %H:%M:%S", &timeinfo);
}

void returnDateTime(char* output, size_t size) {
  time_t epochTime = time(NULL);
  struct tm timeinfo;
  localtime_r(&epochTime, &timeinfo);
  strftime(output, size, "%a %d %b %Y %H:%M:%S", &timeinfo);
}

void returnDateTimeMicro(char* output, size_t size) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  snprintf(output, size, "%04ld", (long)(tv.tv_usec / 1000));
}


// Function to return the Date and Time as a String EG: Sun 22 Feb 2026 08:37:00
char* returnMinDate() {
  time_t epochTime = time(NULL);
  struct tm timeinfo;
  localtime_r(&epochTime, &timeinfo);

  // 'static' ensures the memory isn't cleared when the function ends
  static char timeValue[26];
  strftime(timeValue, sizeof(timeValue), "%a %d %b %Y", &timeinfo);

  return timeValue;
}

// Function to return the Date and Time as a String EG: 22-02-2026 08:37:05
char* returnMinDateTime() {
  time_t epochTime = time(NULL);
  struct tm timeinfo;
  localtime_r(&epochTime, &timeinfo);
  // 'static' ensures the memory isn't cleared when the function ends
  static char theTime[26];
  strftime(theTime, sizeof(theTime), "%a %d %b %Y %H:%M", &timeinfo);

  return theTime;
}

// Function to return the Date and Time as a String EG: Sunday 22 February 2026 09:46:02 GMT
char* returnFullDateTime() {
  time_t epochTime = time(NULL);

  struct tm timeinfo;
  localtime_r(&epochTime, &timeinfo);
  // 'static' ensures the memory isn't cleared when the function ends
  static char theTime[64];
  strftime(theTime, sizeof(theTime), "%A %d %B %Y %H:%M:%S %Z", &timeinfo);

  return theTime;
}



// Function to return the Time as a String EG: 08:37:05
char* returnTime() {
  time_t epochTime = time(NULL);

  struct tm timeinfo;
  localtime_r(&epochTime, &timeinfo);

  // 'static' ensures the memory isn't cleared when the function ends
  static char timeValue[10];
  strftime(timeValue, sizeof(timeValue), "%H:%M:%S", &timeinfo);

  return timeValue;
}

// Function to return the Date as a String EG: 22-02-2026
char* returnDate() {
  time_t epochTime = time(NULL);
  struct tm timeinfo;
  localtime_r(&epochTime, &timeinfo);

  // 'static' ensures the memory isn't cleared when the function ends
  static char timeValue[16];
  strftime(timeValue, sizeof(timeValue), "%d-%m-%Y", &timeinfo);

  return timeValue;
}

// Function to return the Time as a String EG: 08:37:05
char* returnMinTime() {
  time_t epochTime = time(NULL);
  struct tm timeinfo;
  localtime_r(&epochTime, &timeinfo);

  // 'static' ensures the memory isn't cleared when the function ends
  static char timeValue[8];
  strftime(timeValue, sizeof(timeValue), "%H:%M", &timeinfo);

  return timeValue;
}

// Function to return the Date Time (secs set to 00) as a String EG: 22-02-2026 08:37:00
char* returnDateTimeZeroSecs() {
  time_t epochTime = time(NULL);
  struct tm timeinfo;
  localtime_r(&epochTime, &timeinfo);

  // 'static' ensures the memory isn't cleared when the function ends
  static char timeValue[26];
  strftime(timeValue, sizeof(timeValue), "%d-%m-%Y %H:%M:00", &timeinfo);

  return timeValue;
}

char* returnLastOpened() {

  static char someText[26];
  snprintf(someText, sizeof(someText), "%s", &lastTimeOpened);

  return someText;
}

char* returnLastClosed() {

  static char someText[26];
  snprintf(someText, sizeof(someText), "%s", lastTimeClosed);

  return someText;
}

char* returnTemp() {

  static char someText[26];
  snprintf(someText, sizeof(someText), "%.2f °C", gateTemp);

  return someText;
}

char* returnHumid() {

  static char someText[26];
  snprintf(someText, sizeof(someText), "%.2f %%", gateHumid);

  return someText;
}

char* returnPress() {

  static char someText[26];
  snprintf(someText, sizeof(someText), "%.1f hPa", &gatePress);

  return someText;
}

// Update Date and Time with NTP Server
void updateNTP() { 
  struct tm timeinfo; 
  
  // Update the Devices Date And Time via Wifi using a UDP NTP Server 
  if (WiFi.status() == WL_CONNECTED) { 
    configTime(gmtOffset_sec, daylightOffset_sec, NTPServer); 
    
    bool syncSuccess = false;
    
    // Attempt to sync up to 3 times (initial attempt + 2 retries)
    for (int attempt = 1; attempt <= 3; attempt++) {
      if (getLocalTime(&timeinfo, 5000)) { // 5sec timeout
        syncSuccess = true;
        break; // Exit loop early if successful
      }
      
      snprintf(logMSG, sizeof(logMSG), "--- | NTP Sync Failed. Attempt %d Retrying...", attempt);
      logMessage(logMSG);
      vTaskDelay(pdMS_TO_TICKS(500));  //delay(500); // Short delay between retries
    }
    
    if (syncSuccess) { 
      sprintf(logMSG, "The system clock has now Synchronised with %s", NTPServer); 
      logMessage(logMSG); 
    } else { 
      sprintf(logMSG, "The system clock has failed to Synchronised after 3 attempts with %s", NTPServer); 
      logMessage(logMSG); 
    } 
  } 
} //UpdateNTP

// Function verify update has occured with the NTP Server
void checkNTP() {
  struct tm timeinfo;

  if (getLocalTime(&timeinfo, 100)) {
    sprintf(logMSG, "--- | The system clock has syncronised with %s", NTPServer);
    logMessage(logMSG);
    storeLog("Ready >_");
  }
}

void showDateTime(void) {

  Serial.print(returnMinDate());
}

String returnCPUMhz() {
  uint32_t freq = getCpuFrequencyMhz();
  if (!isnan(freq)) {
    return String(freq);
  }
  return String();
}

String returnCPUTemp() {
  float temp = temperatureRead();
  if (!isnan(temp)) {
    return String(temp);
  }
  return String();
}

void showCPUTemp(void) {

  float temp = temperatureRead();
  if (!isnan(temp)) {
    Serial.print("CPU ");
    Serial.print(returnCPUTemp());
    Serial.println(" °C");
  }
}

char* formatAsBytes(size_t bytes) {
    static char buf[40];  
    double dBytes = (double)bytes;

    char numBuf[32];
    insert_commas(numBuf, dBytes, 0);
    snprintf(buf, sizeof(buf), "%s Bytes", numBuf);

    return buf;
}

char* formatAsSize(size_t bytes) {
    static char buf[40];  
    const char* units[] = { "B", "KB", "MB", "GB", "TB" };
    int i = 0;
    double dBytes = (double)bytes;

    while (dBytes >= 1024 && i < 4) {
        dBytes /= 1024;
        i++;
    }

    char numBuf[32];
    insert_commas(numBuf, dBytes, (i == 0 ? 0 : 2));
    snprintf(buf, sizeof(buf), "%s %s", numBuf, units[i]);

    return buf;
}

void insert_commas(char* dest, double value, int precision) {
  char raw[32];
  // Format the number to a string first
  if (precision == 0) snprintf(raw, sizeof(raw), "%.0f", value);
  else snprintf(raw, sizeof(raw), "%.2f", value);

  char* dot = strchr(raw, '.');
  int len = dot ? (int)(dot - raw) : (int)strlen(raw);
  int commas = (len - 1) / 3;
  int out_idx = 0;

  for (int i = 0; i < (int)strlen(raw); i++) {
    dest[out_idx++] = raw[i];
    // Insert comma if we are in the integer part and at the right position
    if (i < len - 1 && (len - 1 - i) % 3 == 0) {
      dest[out_idx++] = ',';
    }
  }
  dest[out_idx] = '\0';
}

char* formatBytes(size_t bytes) {
  static char buf[40];  // Increased buffer size for commas
  const char* units[] = { "B", "KB", "MB", "GB", "TB" };
  int i = 0;
  double dBytes = (double)bytes;

  while (dBytes >= 1024 && i < 4) {
    dBytes /= 1024;
    i++;
  }

  char numBuf[32];
  insert_commas(numBuf, dBytes, (i == 0 ? 0 : 2));
  snprintf(buf, sizeof(buf), "%s %s", numBuf, units[i]);

  return buf;
}

void showCPUMhz(void) {
  Serial.print(" CPU: ");
  Serial.print(returnCPUMhz());
  Serial.println(" Mhz");
}

const char* getMemoryBlock() {
  static char buf[30] = { 0 };
  sprintf(buf, "%s", formatBytes(heap_caps_get_largest_free_block(MALLOC_CAP_8BIT)));
  return buf;
}
const char* getMemoryHeap() {
  static char buf[30] = { 0 };
  sprintf(buf, "%s", formatBytes(heap_caps_get_free_size(MALLOC_CAP_8BIT)));
  return buf;
}

const char* getCPUMhz() {
  static char buf[30] = { 0 };
  sprintf(buf, "%d Mhz", getCpuFrequencyMhz());
  return buf;
}

const char* getCPUTemp() {
  static char buf[30] = { 0 };
  sprintf(buf, "%.1f °C", temperatureRead());
  return buf;
}

void triggerPingByIP(const char* searchIP) {
    if (searchIP == nullptr || searchIP[0] == '\0') return;
    if (strcmp(searchIP, "0.0.0.0") == 0) return;
    
    bool pingWorked = false;
    bool deviceFound = false;
    char targetHostname[27] = {0}; 
    char localType[20] = {0};

    int32_t rssi = WiFi.RSSI();     //Get the latest Wifi Connection strength
    uint32_t adaptiveTimeout = 0;

    // Secure a brief lock to get the hostname identifier
    if (devicesMutex != NULL && xSemaphoreTake(devicesMutex, portMAX_DELAY) == pdTRUE) {
        for (auto* device : active_devices) { 
            if (device != nullptr && strcmp(device->ip, searchIP) == 0) {
                snprintf(targetHostname, sizeof(targetHostname), "%s", device->hostname);
                snprintf(localType, sizeof(localType), "%s", device->type);
                deviceFound = true;
                break;
            }
        }
        xSemaphoreGive(devicesMutex);
    }

    // Run the ping OUTSIDE the mutex lock window
    if (deviceFound) {
        
        // Set packetCount to 1 so it sends exactly one request.
        // Set adaptiveTimeout to max 1000ms. If a device is dead, the task returns in 1.0s, well within the 5-second Watchdog limit.
        byte packetCount = 1;
        uint16_t adaptiveTimeout = getLocalTimeoutByRSSI(rssi); 

        // Execute the ping using the Arduino library
        pingWorked = Ping.pingMs(searchIP, packetCount, adaptiveTimeout);
        float currentLatency = 0.0f;

        if (pingWorked) {
            currentLatency = (float)Ping.averageTime(); // Retrieve the response latency
        } else {
            currentLatency = -1.0f; // Standard marker for a dropped packet
        }

        // Re-lock briefly to safely update the circular history matrix
        if (devicesMutex != NULL && xSemaphoreTake(devicesMutex, portMAX_DELAY) == pdTRUE) {
            
            // Fast O(1) map index lookup
            auto it = device_lookup_map.find(targetHostname);
            if (it != device_lookup_map.end() && it->second != nullptr) {
                DeviceRegister* device = it->second;

                // Update basic status tracking fields
                device->failed = !pingWorked;
                device->lastReplyTime = millis();
                
                if (!pingWorked) {
                    device->failCount++;
                } else {
                    device->failCount = 0;
                }
              
                // Store the newest latency measurement into the current index slot
                device->latencyHistory[device->historyIdx] = currentLatency;
                
                // Advance the index marker and wrap it around using modulo 5
                device->historyIdx = (device->historyIdx + 1) % 5;

                // Compute the running average dynamically based on valid packets
                float latencySum = 0.0f;
                int validPacketsCount = 0;
                int totalDroppedPackets = 0;

                for (int i = 0; i < 5; i++) {
                    if (device->latencyHistory[i] >= 0.0f) { 
                        latencySum += device->latencyHistory[i];
                        validPacketsCount++;
                    } else if (device->latencyHistory[i] == -1.0f) { 
                        totalDroppedPackets++;
                    }
                }

                // Update the structural packet loss calculation
                device->packetLossPercent = (totalDroppedPackets / 5.0f) * 100.0f;

                // Update the average latency value safely (prevent division by zero)
                if (validPacketsCount > 0) {
                    device->avgLatency = latencySum / (float)validPacketsCount;
                    device->lastLatency = currentLatency; 
                } else {
                    device->avgLatency = 0.0f; 
                    device->lastLatency = 0.0f;
                    device->unstable = true;   // Flag network connection state as completely dropped
                }
            }
            
            // RELEASE the lock immediately. Total runtime inside this block is under 2 microseconds!
            xSemaphoreGive(devicesMutex);
        }

        // Handle thread-isolated logging and gateway emergency reboots
        char localLogBuf[96] = { 0 };

        if (pingWorked) {
          if (debugPing == true) {
              snprintf(localLogBuf, sizeof(localLogBuf), "--> | PING %s %s using %d dBm max %.0u ms Response %.1f ms", targetHostname, searchIP, rssi, adaptiveTimeout, currentLatency);
              logMessage(localLogBuf);
          }
          return;
        } else {
          if (debugPing == true) {
              snprintf(localLogBuf, sizeof(localLogBuf), "--> | PING %s %s using %d dBm TIMEOUT max %.0u ms", targetHostname, searchIP, rssi, adaptiveTimeout);
              logMessage(localLogBuf);
          }

          if (strcasecmp(localType, "gateway") == 0) {
              // Degraded Network Connection. Disconnect and Reconnect to hopefully get a better connection
              if (adaptiveTimeout >= PING_WORST) {
                  disconnectWifi();
                  reconnectWifi();
                  
                  if (WiFi.status() != WL_CONNECTED) {
                    snprintf(localLogBuf, sizeof(localLogBuf), "--- | WIFI Gateway %s | %d dBm %u ms | REBOOTING Due to a Degraded Network Connection", searchIP, rssi, adaptiveTimeout);
                    logMessage(localLogBuf);
                    rebootDevice("Bad Gateway");  
                  }
                  reconnectMDNS();

              }//AdaptiveTimeout
          }//Gateway
        }//PingWorked
    }//found device
}

void triggerRediscoveryByIP(const char* failedIP) {
    if (failedIP == nullptr || failedIP[0] == '\0') return;

    DeviceRegister* targetDevice = nullptr;
    unsigned long now = millis();

    // Safely lock the vector, verify criteria, and capture the pointer address
    if (devicesMutex != NULL) {
        if (xSemaphoreTake(devicesMutex, portMAX_DELAY) == pdTRUE) {
            
            // Core 0 web traffic is safely locked out while we search the array
            for (auto &device : active_devices) {
                if (strcmp(device->ip, failedIP) == 0) {
                    
                    // Re-enforce the 30-second cooldown window to prevent queue saturation
                    if (now - device->lastRecoveryAttempt > 30000) {
                        if (strcasecmp(device->type, "gateway") != 0) {
                            
                            // Flag failure state under structural protection
                            device->failed = true;
                            device->lastRecoveryAttempt = now;
                            
                            // Capture the memory location pointer for execution outside the lock
                            targetDevice = device; 
                        }
                    }
                    break; // Exit search early since a match was discovered
                }
            }
            
            // ALWAYS release the lock instantly when done with vector memory
            xSemaphoreGive(devicesMutex);
        }
    }

    // Execute logging and task queueing safely OUTSIDE the critical zone
    if (targetDevice != nullptr) {
        if (debugMDNS == true) {
            char localMdnsLog[128] = { 0 };
            snprintf(localMdnsLog, sizeof(localMdnsLog), "--> | MDNS %s %s | Initiating Network Scan", targetDevice->ip, targetDevice->hostname);
            logMessage(localMdnsLog);
        }

        // Push to the native queue securely without locking up core-level mutex frames
        triggerNonBlockingRecovery(targetDevice);
    }
}


void update_device(const char* hostname, const char* ip, const char* token, const char* type, int8_t dBm, bool monitored) {
  // Guard against null pointers for all string parameters before proceeding
  if (hostname == nullptr || ip == nullptr || token == nullptr || type == nullptr) return;
  if (hostname[0] == '\0' || ip[0] == '\0') return;
  
  if (devicesMutex != NULL && xSemaphoreTake(devicesMutex, portMAX_DELAY) == pdTRUE) {
    
    // Use hash-map lookup
    // Note: Normalize your lookup keys to lowercase if case-insensitivity is mandatory.
    auto it = device_lookup_map.find(hostname);

    if (it != device_lookup_map.end()) {

        DeviceRegister* device = it->second;

        if (device != nullptr) {
            // memcpy or direct array assignments are significantly faster than strncpy padding
            snprintf(device->ip, sizeof(device->ip), "%s", ip ? ip : "");
            snprintf(device->token, sizeof(device->token), "%s", token ? token : "");
            snprintf(device->type, sizeof(device->type), "%s", type ? type : "");

            device->monitored = monitored;
            device->failed = false;
            device->dBm = dBm;
            device->isRegistering = false; 
        }

    }

    xSemaphoreGive(devicesMutex);
  }

}

void addNewDevice(const char* hostname, const char* hostnamelocal, const char* ip, const char* token, const char* type, int8_t dBm, bool monitored) {
  
  // Guard against null pointers for all incoming parameters to prevent string manipulation panics
  if (hostname == nullptr || hostnamelocal == nullptr || ip == nullptr || token == nullptr || type == nullptr) return;
  if (hostname[0] == '\0' || hostnamelocal[0] == '\0' || ip[0] == '\0') return;
    
  // Set up a temporary device object entirely on the stack before locking the mutex
  DeviceRegister* newDevice = new DeviceRegister();

  newDevice->historyIdx = 0;
  for (int i = 0; i < 5; i++) {
    newDevice->latencyHistory[i] = 0.0f;
  }
  newDevice->lastLatency = 0;

  strlcpy(newDevice->token, token, sizeof(newDevice->token));
  strlcpy(newDevice->ip, ip, sizeof(newDevice->ip));
  strlcpy(newDevice->hostname, hostname, sizeof(newDevice->hostname));
  strlcpy(newDevice->hostnamelocal, hostnamelocal, sizeof(newDevice->hostnamelocal));
  strlcpy(newDevice->type, type, sizeof(newDevice->type));
          
  newDevice->monitored = monitored;
  newDevice->unstable = false;
  newDevice->failed = false;
  newDevice->dead = false;
  newDevice->dBm = dBm;

  newDevice->lastSent = 0;
  newDevice->lastReplyTime = 0;
  newDevice->isRegistering = false;
  newDevice->mDNSTaskRunning = false;
  newDevice->nextRecoveryAllowed = 0; 
  
  if (strcasecmp(ip, "0.0.0.0") == 0 || strcasecmp(ip, "::") == 0) {
    newDevice->failed = true;
  }

  if (devicesMutex != NULL) {
    if (xSemaphoreTake(devicesMutex, portMAX_DELAY) == pdTRUE) {
      
      // Safely check container dimensions and perform the memory modification
      if (active_devices.size() < MAX_DEVICES) {
        active_devices.push_back(newDevice);
        device_lookup_map[newDevice->hostname] = newDevice;  // Store the precise vector index location for this hostname
      }
      
      xSemaphoreGive(devicesMutex);
    }
  }
}

void permanentmDNSRecoveryWorker(void *pvParameters) {
    char msgMDNSbuffer[MEDIUM_BUFFER] = {0};

    while (1) {
        if (WiFi.status() != WL_CONNECTED) {
            vTaskDelay(pdMS_TO_TICKS(1000));  
            continue; 
        }
        
        mDNSJob job;
        
        // Task blocks here automatically until a hostname arrives in the queue.
        if (xQueueReceive(mDNSRecoveryQueue, &job, portMAX_DELAY) == pdTRUE) {
            
            // Run the slow mDNS discovery completely UNPROTECTED.
            // This can take up to 2 seconds. The server thread runs free!
            IPAddress newIP = findHost(job.hostname);
            unsigned long currentMillis = millis();

            // Briefly lock the system map index to commit the network changes
            if (devicesMutex != NULL && xSemaphoreTake(devicesMutex, portMAX_DELAY) == pdTRUE) {
                
                auto it = device_lookup_map.find(job.hostname);
                if (it != device_lookup_map.end() && it->second != nullptr) {
                    DeviceRegister* device = it->second;

                    // FIX: Safe parsing fallback. If it fails, oldIP becomes 0.0.0.0, which is fine!
                    IPAddress oldIP;
                    if (!oldIP.fromString(device->ip)) {
                        oldIP = IPAddress(0, 0, 0, 0);
                    }

                    if (newIP != oldIP && newIP != IPAddress(0,0,0,0)) {
                        // Update IP address safely without padding overflows
                        snprintf(device->ip, sizeof(device->ip), "%s", newIP.toString().c_str());

                        // Reset device records under mutex boundary protection
                        device->dead = false;
                        device->failed = false;
                        device->failCount = 0;
                        device->lastSent = 0; 
                        device->lastReplyTime = currentMillis;     
                
                        device->lastRecoveryAttempt = 0;   
                        device->nextRecoveryAllowed = 0;   
                        device->pingsSent = 0;
                        device->pingsFailures = 0;

                        if (debugMDNS) {
                            snprintf(msgMDNSbuffer, sizeof(msgMDNSbuffer), "--- | MDNS %s Device %s now has a new IP Address %s", device->type, device->hostname, device->ip);
                            logMessage(msgMDNSbuffer);
                        }
                    
                        device->isRegistering = true; 
                        addToQueue(device->ip, device->hostname, "reregister");

                    } else if (newIP == IPAddress(0,0,0,0)) {
                        device->failed = true;
                        
                        // Set a 5-minute throttling backoff directly on the device 
                        // so checkDeadAndMissedPings() doesn't hammer the queue instantly again
                        device->nextRecoveryAllowed = currentMillis + PING_INTERVAL; 

                        if (debugMDNS) {
                            snprintf(msgMDNSbuffer, sizeof(msgMDNSbuffer), "--- | MDNS %s Device %s %s is OFFLINE ", device->type, device->ip, device->hostname);
                            logMessage(msgMDNSbuffer);
                        }
                    } else {
                        // Means newIP == oldIP (Device IP hasn't shifted, but it's responding to mDNS again)
                        device->dead = false;
                        device->failed = false;
                        device->failCount = 0;
                        device->lastSent = 0;
                        device->lastReplyTime = currentMillis;        

                        device->lastRecoveryAttempt = 0;  
                        device->nextRecoveryAllowed = 0;  
                        
                        if (debugMDNS) {
                            snprintf(msgMDNSbuffer, sizeof(msgMDNSbuffer), "--- | MDNS %s Device %s %s is now ONLINE", device->type, device->ip, device->hostname);
                            logMessage(msgMDNSbuffer);
                        }
                    }

                    // Always release the tracking execution flags before unlocking
                    device->lastRecoveryAttempt = currentMillis;
                    device->mDNSTaskRunning = false;  
                }
                
                xSemaphoreGive(devicesMutex);
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/**
 * Validates the "auth" header for token using C-style strings (char*).
 * 
 * @param request The AsyncWebServerRequest object.
 * @return true if the token is NOT valid, false if the token is good.
 */
bool isBadSessionToken(AsyncWebServerRequest* request) {

       // Get a pointer to the raw header object
      const AsyncWebHeader* authHeader = request->getHeader("auth");
      if (authHeader == nullptr) return false;

      if (request->client() == nullptr) return false;

      char tokenValue[24] = {0};

      strlcpy(tokenValue, authHeader->value().c_str(), sizeof(tokenValue));

      size_t tokenLen = strlen(tokenValue);
      if (tokenLen == 0 || tokenLen > 20) {
          return false;
      }

      bool sessionFound = false;
      int currentsessions = 0;

      if (sessionsMutex != NULL) {
        if (xSemaphoreTake(sessionsMutex, portMAX_DELAY) == pdTRUE) {
            currentsessions = active_sessions.size();
            // Capturing via reference allows accurate, stable stack verification
            auto it = std::find_if(active_sessions.begin(), active_sessions.end(),
                                 [&](const Session& s) {
                                   return strcasecmp(s.token, tokenValue) == 0;
                                 });

            if (it != active_sessions.end()) {
                sessionFound = true;
            }
            xSemaphoreGive(sessionsMutex);
        }
      }

      if (sessionFound) {
        if (debugTokens == true) {
          char localSessionLog[128] = { 0 };
          snprintf(localSessionLog, sizeof(localSessionLog), "<-- | TOKEN %s Found in %d active session%s", tokenValue, currentsessions, (currentsessions == 1) ? "" : "s");
          logMessage(localSessionLog);
        }
        return false; // Token is valid!
      }
  return true; 
}

/**
 * Validates the "Authorization" header for a XDUID token using C-style strings (char*).
 * 
 * @param request The AsyncWebServerRequest object.
 * @return true if the token is valid, false otherwise.
 */
bool isValidXDUIDToken(AsyncWebServerRequest* request) {
  
  const AsyncWebHeader* authHeader = request->getHeader("Authorization");
  if (authHeader == nullptr) return false;
  if (request->client() == nullptr) return false;

  const char* bearerPrefix = "Bearer ";
  size_t bearerLen = strlen(bearerPrefix);

  char token[64] = {0};

  if (authHeader->value().startsWith(bearerPrefix)) {
      strlcpy(token, authHeader->value().c_str() + bearerLen, sizeof(token));
  } else {
    return false;
  }

  if (strcmp(token, XDUID) == 0) {
    if (debugTokens) {
      snprintf(logMSG, sizeof(logMSG), "<-- | XDUID Recieved %s Authorized Device Token", token);
      logMessage(logMSG);
    }
    return true;  // Token matches perfectly
  }

  return false;  
}

/*
bool isValidXDUIDToken(AsyncWebServerRequest* request) {
  
  // check if the header even exists
  if (!request->hasHeader("Authorization")) {
    return false;
  }

  // Fetch the header. 
  String authString = request->header("Authorization");
  
  // Use safe, read-only C-pointer
  const char* authHeader = authString.c_str();  

  const char* bearerPrefix = "Bearer ";
  size_t bearerLen = strlen(bearerPrefix);

  // check for the "Bearer " prefix
  if (strncmp(authHeader, bearerPrefix, bearerLen) == 0) {
    // Move the pointer forward past "Bearer " to isolate the token
    const char* token = authHeader + bearerLen;

    // Compare the isolated token string directly with your XDUID variable
    if (strcmp(token, XDUID) == 0) {
      if (debugTokens) {
        snprintf(logMSG, sizeof(logMSG), "<-- | XDUID Received %s Authorized Device Token", token);
        logMessage(logMSG);
      }
      return true;  // Token matches perfectly
    }
  }

  return false;  
}
*/

float getPacketLossPercentage(const DeviceRegister *device) {
  if (device->pingsSent == 0) return 0.0f;
  if (device->pingsSent == 0) return 0.0f;

  uint32_t lost = device->pingsFailures;
  return ((float)lost / (float)device->pingsSent) * 100.0f;
}

float getSmartAverageLatency(const DeviceRegister *device) {
  float sum = 0.0f;
  float maxLatency = 0.0f;
  int validCount = 0;

  for (int i = 0; i < 5; i++) {
    float val = device->latencyHistory[i];

    // Ensure we only count positive, real millisecond measurements
    if (val > 0.0f) {
      sum += val;
      validCount++;

      if (val > maxLatency) {
        maxLatency = val;
      }

    }// Val
  }// For

  // If the array hasn't filled up yet, return the single most recent latency value
  // instead of returning an invalid 0.0f average!
  if (validCount == 0) {
    return device->lastLatency;
  }

  // Trim the worst spike if we have enough samples
  if (validCount > 2) {
    sum -= maxLatency;
    validCount--;
  }

  return sum / validCount;

}  //getSmartAverageLatency


void triggerNonBlockingRecovery(DeviceRegister *device) {
    // Guard against null pointers early to prevent memory faults
    if (device == nullptr || devicesMutex == NULL) return;
    
    unsigned long now = millis();
    bool addedTask = false;

    // Enforce lock BEFORE reading or updating any variables on the device struct
    if (xSemaphoreTake(devicesMutex, pdMS_TO_TICKS(10)) == pdTRUE) {

        // Core-safety check (Protected inside Mutex)
        if (device->mDNSTaskRunning) {
            xSemaphoreGive(devicesMutex);
            return; 
        }
        
        // Throttling window check (Protected inside Mutex)
        if (device->lastRecoveryAttempt > 0 && (now - device->lastRecoveryAttempt < PING_INTERVAL)) {
            xSemaphoreGive(devicesMutex);
            return; // Exit early. It hasn't been 5 minutes yet!
        }

        // Update the state flags safely under lock protection
        device->mDNSTaskRunning = true;
        device->nextRecoveryAllowed = now;

        // Create a standalone string copy package for the queue
        // This structural shift matches our updated 'mDNSJob' definition
        mDNSJob job;
        strlcpy(job.hostname, device->hostname, sizeof(job.hostname));

        // Send data copy by value to the background worker (Non-blocking)
        if (xQueueSend(mDNSRecoveryQueue, &job, 0) != pdTRUE) {
            // Roll back the state flag if the queue is saturated so it can retry later
            device->mDNSTaskRunning = false;
            addedTask = true;
        }

        // ALWAYS release the lock instantly when done
        xSemaphoreGive(devicesMutex);
        
        if (debugMDNS == true && addedTask == true) {
              char localMsgBuffer[128] = {0};
              snprintf(localMsgBuffer, sizeof(localMsgBuffer), "--- | MDNS Added %s", job.hostname);
              logMessage(localMsgBuffer);
        }

    }
    
}

// Safe, unified cleanup routine to prevent double-frees and heap memory leaks
void safeCleanupRequest(AsyncClient* client, RequestContext* ctx) {
    // 1. Decouple the client arguments instantly to stop racing callbacks
    if (client != nullptr) {
        client->onError(nullptr, nullptr);
        client->onDisconnect(nullptr, nullptr);
        client->onData(nullptr, nullptr);
        
        // Schedule the client object to be safely deleted from the heap 
        // once the current stack frame exits. DO NOT use "delete client;" here.
        client->free(); 
    }

    // 2. Free the context structure exactly once
    if (ctx != nullptr) {
        if (ctx->ip != nullptr)   { free(ctx->ip);   ctx->ip = nullptr; }
        if (ctx->host != nullptr) { free(ctx->host); ctx->host = nullptr; }
        if (ctx->url != nullptr)  { free(ctx->url);  ctx->url = nullptr; }
        
        free(ctx);
    }

    // 3. Always unblock your background worker loop state
    isAsyncClientBusy = false;
}


// Need to ensure that each request has its own values as these are Async Requests
// Using a new struct variable to contain all the device details as we can only pass one variable using the AsyncClient Request.
// Freeing up the memory allocated data when the Async Requests now running or complete
//
void sendHTTPrequest(const char* aurl, IPAddress& adeviceIP, char* ahost) {

  if (WiFi.status() != WL_CONNECTED) return;
  if (adeviceIP == IPAddress(0, 0, 0, 0)) return;
  if (String(ahost).length() == 0) return;
  if (ahost[0] == '\0') return;

  const char* localIPBuffer = getIP(&adeviceIP);

  //Allocate the container struct
  RequestContext* ctx = (RequestContext*)malloc(sizeof(RequestContext));
  if (!ctx) {
      //logMessage("CRITICAL: Out of memory allocating RequestContext container!");
      isAsyncClientBusy = false; // Release worker task lock if allocation fails
      return;
  }

  //Allocate and copy the data
  ctx->ip = (char*)malloc(strlen(localIPBuffer) +1);
  strcpy(ctx->ip, localIPBuffer);
  
  ctx->host = (char*)malloc(strlen(ahost) + 1);
  strcpy(ctx->host, ahost);

  ctx->url = (char*)malloc(strlen(aurl) + 1);
  strcpy(ctx->url, aurl);

  upperCase(ctx->url);
  lowerCase(ctx->host);

  char localRegBuffer[64] = {0};
  snprintf(localRegBuffer, sizeof(localRegBuffer), "--> | Sending %s to %s %s", ctx->url, ctx->ip,  ctx->host);
  logMessage(localRegBuffer);
 
  // Mark client as busy before launching the async request
  // isAsyncClientBusy = true; 

  AsyncClient* client = new AsyncClient();

   // FIX 3: Safe, bounded copying into the token and base64 arrays using explicit numeric sizing constraints
  if (strcasecmp(ctx->url, "PRESS") == 0 || strcasecmp(ctx->url, "STATUS") == 0) {
      if (charDeviceToken != nullptr && charDeviceToken[0] != '\0') {
          // Bounded transfer to prevent overflows
          strncpy(ctx->token, charDeviceToken, sizeof(ctx->token) - 1);
          ctx->token[sizeof(ctx->token) - 1] = '\0'; // Seal the string
          
          encryptAndBase64(ctx->token, ctx->encryptedB64); 
          ctx->encryptedB64[sizeof(ctx->encryptedB64) - 1] = '\0'; // Seal the payload
      }
  }

  // Mark client as busy before launching the async request
  //isAsyncClientBusy = true;

  if (!client) {
      if(ctx->ip) free(ctx->ip); if(ctx->host) free(ctx->host); 
      if(ctx->local) free(ctx->local); if(ctx->url) free(ctx->url);
      delete ctx;
      delete client;
      isAsyncClientBusy = false;
      return;
  }

  client->onError([](void* arg, AsyncClient* c, int8_t error) {
    // Extract the pointer context safely
    RequestContext* context = (RequestContext*)arg;

      if (context != nullptr) {
          char localMsgBuffer[128] = {0};
          snprintf(localMsgBuffer, sizeof(localMsgBuffer), "--> | Send Request FAILED %s %s #%d %s", context->ip, context->host, error, getAsyncErrorDescription(error));
          logMessage(localMsgBuffer);
          
          // Re-queue diagnostic metrics safely
          addToQueue(context->ip, context->host, "ping");
      }

      // Pass 'context' out. Inside safeCleanupRequest, it will wipe out the memory footprint.
      safeCleanupRequest(c, context);
  }, ctx);

  // Ensure Heap memory is returned as the Request is now fully deleted.
  client->onDisconnect([](void* arg, AsyncClient* c) {
      RequestContext* context = (RequestContext*)arg;

      safeCleanupRequest(c, context);
  }, ctx);
  

  if (strcasecmp(aurl, "registerdevice") == 0) {
   
    //Wait and get the response as a device token back from the gatesensor.
    client->onData([](void* arg, AsyncClient* client, void* data, size_t len) {
      RequestContext* context = (RequestContext*)arg;
      if (data == nullptr || len == 0 || context == nullptr) return;

      // Create a safe, null-terminated copy of the network packet 
      std::vector<char> safePacket(len + 1);
      memcpy(safePacket.data(), data, len);
      safePacket[len] = '\0';
      char* response = safePacket.data();

      if (debugRequests) {
        // Avoid using shared global messageBuffer directly inside interrupt contexts if possible
        char localLog[34];
        snprintf(localLog, sizeof(localLog), "<-- | RESPONSE Received %zu Bytes", len);
        logMessage(localLog);
      }

      int responseCode = -1;

      // Safe Response Code Check 
      if (len > 12 && strncmp(response, "HTTP/", 5) == 0) {
        char codeStr[4] = { response[9], response[10], response[11], '\0' };
    
        // Check if they are actually numbers before using atoi
        if (isdigit(codeStr[0]) && isdigit(codeStr[1]) && isdigit(codeStr[2])) {
          responseCode = atoi(codeStr); // Completely exception-safe
        }
      }

      if (responseCode != 200 && responseCode != 0) {
        badToken = true;
      }

      char* bodyStart = strstr(response, "\r\n\r\n");
      char* src = response;
      size_t dataLen = len;

      if (bodyStart != nullptr) {
        src = bodyStart + 4;
        dataLen = len - (src - response);
      }

        // Isolated local buffer for extraction to prevent global data racing
        if (dataLen > 0 && dataLen < 64) {
          char localBase64[64] = {0};
          memcpy(localBase64, src, dataLen);

          // Call your decryption routine using the isolated packet fragment
          int decryptedLength = base64AndDecrypt(localBase64, resultBuffer);

          if (decryptedLength > 0 && charDeviceToken[0] == '\0') {
            size_t copyLen = decryptedLength;
            if (copyLen > TOKEN_SIZE) {
              copyLen = TOKEN_SIZE;
            }
            memcpy(charDeviceToken, resultBuffer, copyLen);
            charDeviceToken[copyLen] = '\0'; 
            tokenReady = true;

            if (debugRequests == true) {
              char localLog[128] ={0};
              snprintf(localLog, sizeof(localLog), "<-- | RESPONSE %d Processed %zu Bytes | Body %zu Bytes (%s)", responseCode, len, dataLen, localBase64);
              logMessage(localLog);
            }
          }
        } else {
          if (debugRequests == true) {
            char localLog[64] = {0};
            if (responseCode == 503) {
              snprintf(localLog, sizeof(localLog), "<-- | RESPONSE %d Server Busy", responseCode);
            } else {
              snprintf(localLog, sizeof(localLog), "<-- | RESPONSE %d Processed %zu Bytes | Body %zu Bytes", responseCode, len, dataLen);
            }
            logMessage(localLog);
          }

          if (responseCode != 200 && responseCode != 0) {
            memcpy(charDeviceToken, "##########", 10);
            charDeviceToken[10] = '\0';
          }
        }
    }, ctx);
    
    // register a Async web callback
    client->onConnect([](void* arg, AsyncClient* client) {
      RequestContext* context = (RequestContext*)arg;
      if (context == nullptr || client == nullptr) return;

      char threadBuffer[256] = {0};
      snprintf(threadBuffer, sizeof(threadBuffer), "GET /registerdevice?hostname=%s&type=display&dbm=%d HTTP/1.1\r\nHost: %s\r\nAuthorization: Bearer %s\r\nUser-Agent: ESP32\r\nConnection: close\r\n\r\n", hostNameLower, dBm, context->ip, XDUID);
      client->write(threadBuffer);
    }, ctx);

  }  //registerdevice

  if (strcasecmp(aurl, "press") == 0) {

    client->onConnect([](void* arg, AsyncClient* client) {
      RequestContext* context = (RequestContext*)arg;
      if (context == nullptr || client == nullptr) return;

      char localWriteBuffer[256] = {0};
      snprintf(localWriteBuffer, sizeof(localWriteBuffer), "GET /press HTTP/1.1\r\nHost: %s\r\nauth:%s\r\nUser-Agent: ESP32\r\nConnection: close\r\n\r\n", context->ip, context->encryptedB64);
      client->write(localWriteBuffer);
    }, ctx);
  }  //press

  if (strcasecmp(aurl, "status") == 0) {
    client->onConnect([](void* arg, AsyncClient* client) {
      RequestContext* context = (RequestContext*)arg;
      if (context == nullptr || client == nullptr) return;

      char localWriteBuffer[256] = {0};
      snprintf(localWriteBuffer, sizeof(localWriteBuffer), "GET /status?hostname=%s HTTP/1.1\r\nHost: %s\r\nauth:%s\r\nUser-Agent: ESP32\r\nConnection: close\r\n\r\n", hostNameLower, context->ip, context->encryptedB64);
      client->write(localWriteBuffer);

      memset(prevLidStatus, 0, sizeof(prevLidStatus));
      memset(prevGateStatus, 0, sizeof(prevGateStatus));
      memset(prevStatus, 0, sizeof(prevStatus));
    }, ctx);
  }  //status

  if (!client->connect(ctx->ip, WEB_PORT)) {
   
    if (debugSentRequests == true) {
      snprintf(asyncLogBuffer, sizeof(asyncLogBuffer), "--> | FAILED Send Request connecting to %s %s on port %d", ctx->ip, ctx->host, WEB_PORT);
      logMessage(asyncLogBuffer);
    }

    if (ctx != nullptr) {
        if (ctx->ip != nullptr)   { free(ctx->ip);   ctx->ip = nullptr; }
        if (ctx->host != nullptr) { free(ctx->host); ctx->host = nullptr; }
        if (ctx->url != nullptr)  { free(ctx->url);  ctx->url = nullptr; }
        free(ctx);
    }

    isAsyncClientBusy = false;
  }


}  //sendhttp

// Add to the Queue a Ping to check the device is still alive
void checkDevicesPing() {
  if (!isWifiNetworkHealthy()) return;
  if (enablePing == false) return;

  unsigned long now = millis();

  if (now - lastGlobalPing >= PING_INTERVAL) {
      lastGlobalPing = now;

      // 1. Temporary local stack structure to hold text snapshot copies
      struct PingTarget {
          char ip[sizeof(DeviceRegister::ip)];
          char hostname[sizeof(DeviceRegister::hostname)];
      };
      std::vector<PingTarget> targetsToPing;

      // 2. Safely lock the vector, extract data, and release instantly
      if (devicesMutex != NULL && xSemaphoreTake(devicesMutex, portMAX_DELAY) == pdTRUE) {
          
          for (const auto* device : active_devices) {
              if (device == nullptr) continue; // Safety guard against unallocated memory slots

              // ONLY snap if monitored AND IP is valid
              if (device->monitored && strcmp(device->ip, "0.0.0.0") != 0) {
                  PingTarget target;
                  strlcpy(target.ip, device->ip, sizeof(target.ip));
                  strlcpy(target.hostname, device->hostname, sizeof(target.hostname));
                  
                  targetsToPing.push_back(target);
              }
          }
          
          // ALWAYS release the lock instantly (Total lock time is under 1 millisecond!)
          xSemaphoreGive(devicesMutex);
      }

      // 3. Loop through isolated snapshot data to pace queue additions safely OUTSIDE the mutex
      // The rest of your system (web routes, background workers) runs free while this loop spaces things out.
      for (const auto& target : targetsToPing) {
          addToQueue(target.ip, target.hostname, "ping");
          
          // Safe to delay here! This cleanly yields CPU control and feeds the watchdog.
          vTaskDelay(pdMS_TO_TICKS(100)); 
      }
  }
  
}

void loop() {

  
  CustomTouchPoint pt = { false, 0, 0 };
    
  if (xSemaphoreTake(spi_bus_mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
      pt = getTouchPoint();
     xSemaphoreGive(spi_bus_mutex); // Free the bus up for the display task
  }

    if (pt.isPressed) {
      if(pt.x > PRESS_X && pt.x < PRESS_W && pt.y > PRESS_Y && pt.y < PRESS_H) {
        if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY && displayTapped == false) {
        
          wasPressed = true;
          showButtonTap = true;
      
          if (displayMenu == 0 ) {
            displayTapped = true;
            lastDebounceTime = millis(); 
          }
        }
        
      }
      //Serial.print(pt.x);
      //Serial.print("x");
      //Serial.println(pt.y);
    }
    
    
  // Output to log file once things have completed.
  if (showdatetime == false) {
    checkNTP();
    showdatetime = true;
  }

  currentMillis = millis();
  unsigned long now = millis();

  reconnectWifiIfnotConnected();  // if WiFi is down, try reconnecting

  if (WiFi.status() == WL_CONNECTED) {

    currentRSSI = (int8_t)WiFi.RSSI();

    //  The Async Response has now completed by setting the token result.
    if (tokenReady == true) {

      if (charDeviceToken[0] != '\0') {

        
        if (debugTokens == true) {
          snprintf(messageBuffer, sizeof(messageBuffer), "<-- | TOKEN Received AES128 %s", charDeviceToken);
          logMessage(messageBuffer);
        }
          // if registrar is blank then this device has request to reregister, and not ben asked to reregister, so its going to be blank
          if (deviceregistrar[0] == '\0') {
            strlcpy(deviceregistrar, gatesensorlocal, sizeof(deviceregistrar));
          }
        if (isValidTokenChars(charDeviceToken) && badToken == false) {
          hasRegistered = true;
          requestNewToken = false;
          requestingToken = false;
          requestTokenInterval = requestDefaultTokenInterval;
          tokenReady = false;
          hasRequestedStatus = false;
          hasReceivedStatus = false;
          badToken = false;
          snprintf(messageBuffer, sizeof(messageBuffer), "--- | This Device is now succesfully Registered to %s", deviceregistrar); // hostNameLower,
          logMessage(messageBuffer);

        } else {
          //Bad Token Received
          snprintf(messageBuffer, sizeof(messageBuffer), "--- | BAD TOKEN Received. Requesting %s to Re-Register.", deviceregistrar); // hostNameLower,
          logMessage(messageBuffer);
          badToken = false;
          tokenReady = false;
          hasRegistered = false;
          memset(charDeviceToken, 0, sizeof(charDeviceToken));
          requestTokenInterval = 30000;  // every 3 secs retry just incase the Async request reply fails.
          requestingToken = true;
          previousTokenMillis = millis();
          addToQueue(gate_IP.toString().c_str(), gatesensor, "registerdevice");

        }
      }  // blankChar

    }  // Token Ready

    if (requestNewToken == true) {

      if (millis() - previousTokenMillis >= requestTokenInterval) {

        if (requestingToken == false && gate_IP != IPAddress(0, 0, 0, 0)) {
          tokenReady = false;
          hasRegistered = false;
          //charDeviceToken[0] = '\0';
          memset(charDeviceToken, 0, sizeof(charDeviceToken));
          requestTokenInterval = 30000;  // every 3 secs retry just incase the Async request reply fails.
          requestingToken = true;
          previousTokenMillis = millis();
          addToQueue(gate_IP.toString().c_str(), gatesensor, "registerdevice");
          
        } else {

          bool gateAlive = icmpPing(gate_IP);
          if (gateAlive == false) {
            IPAddress newIP = findHost(gatesensor);
            if (newIP != IPAddress(0, 0, 0, 0)) {
              gate_IP = newIP;
              requestingToken = false;
              requestTokenInterval = 2000;
              previousTokenMillis = millis();
            }
          } else {
            requestingToken = false;
            requestTokenInterval = 10000;
            previousTokenMillis = millis();
          }
          // Do Nothing now that the device cant be registered
        }

      }  //Interval
    }    //RequestNewToken

    if(showButtonTap == true && lastShowButtonTime > 0 ) {
      if(millis() - lastShowButtonTime >SHOW_BUTTON_DELAY) {
        showButtonTap = false;
        lastShowButtonTime = 0;
        updategate = false;
        displayGateStatus();
      }
    }        

    if (displayTapped == true) {

      if(HelpAlarm == true ) {
        // Turn Off Alarm
        HelpAlarm =  false;
        updatebox = true;
        prevDisplayMenu = 99;
      } else {
        updategate = false;
        displayGateStatus();
        lastShowButtonTime = millis();
        const char state[] = "press";
        sendHTTPrequest(state, gate_IP, gatesensor);  // Send the Request the Press the button on the Gate Sensor
        vTaskDelay(pdMS_TO_TICKS(200));  //delay(200);
      }
      displayTapped = false;
    }

    if (hasRegistered == true && hasRequestedStatus == false) {
      hasRequestedStatus = true;
      addToQueue(gate_IP.toString().c_str(), gatesensor, "status");
    }

  }  //Wifi Is Connected

  //lastPressedState = pt.isPressed;

  checkDevicesPing();
  
  handleHourlyNetworkCheck();
  
  checkSchedule();


}  //.. END LOOP

// Internet Control Message Protocol - Echo Request and Reply (PING)
// Use an adpative mechanism to help ensure that we wait a reasonal amount of time for the process to work or eventually timeout.
// Minimising on Timeouts ensures that this process runs as quickly as possible.
bool icmpDevicePing(DeviceRegister *device) {
  
  char localIp[IP_CHAR] = { 0 };
  char localHostname[27] = { 0 };
  char localType[17] = { 0 };
  int localHistoryIdx = 0;
  
  // Briefly lock the mutex just to read the device metadata securely
  if (devicesMutex != NULL) {
    if (xSemaphoreTake(devicesMutex, portMAX_DELAY) == pdTRUE) {
        strlcpy(localIp, device->ip, sizeof(localIp));
        strlcpy(localHostname, device->hostname, sizeof(localHostname));
        strlcpy(localType, device->type, sizeof(localType));
        localHistoryIdx = device->historyIdx;
        xSemaphoreGive(devicesMutex);
    }
  }

  // Guard against invalid target addresses early
  if (localIp[0] == '\0' || strcmp(localIp, "0.0.0.0") == 0) return false;

  IPAddress targetIP;
  if (!targetIP.fromString(localIp)) return false;

  bool pingWorked = false;
  int32_t rssi = WiFi.RSSI();
  uint32_t adaptiveTimeout = 0;

  int totalSent = 0;
  int totalFailures = 0;
  
  // Execute the slow network ping operation completely outside the mutex block
  // This keeps Core 0 web traffic 100% unblocked while waiting for network latency!
  for (int attempt = 0; attempt <= numPings; attempt++) {
    adaptiveTimeout = getLocalTimeoutByRSSI(rssi);
    const byte packetCount = 1; 
    totalSent++; 
    
    pingWorked = Ping.pingMs(targetIP, packetCount, adaptiveTimeout);
    yield();
    
    if (!pingWorked) {
      totalFailures++; 
    } else {
      break;  
    }
  }

  float packetLossPercent = 0.0f;
  if (totalFailures > 0 && totalSent > 0) {
    packetLossPercent = ((float)totalFailures / (float)totalSent) * 100.0f;
  }
  
  float localLatency = -1.0f;
  if (pingWorked) {
    localLatency = Ping.averageTime();
  }

  // Local structures for metrics gathering
  float calculatedAvgLatency = 0.0f;
  int updatedFailCount = 0;

  // Lock the mutex again to write the results back to the original struct safely
  if (devicesMutex != NULL) {
    if (xSemaphoreTake(devicesMutex, portMAX_DELAY) == pdTRUE) {
        
        device->dBm = rssi;
        device->packetLossPercent = packetLossPercent;
        device->pingsFailures = totalFailures;
        device->lastSent = millis();
        
        if (pingWorked) {
            device->lastLatency = localLatency; 
            device->latencyHistory[localHistoryIdx] = localLatency;
            device->historyIdx = (localHistoryIdx + 1) % 5;
            
            device->dead = false;        
            device->failed = false;      
            device->unstable = false;  

            if (device->pingsFailures > 0) {
                device->unstable = true;  
            } else {
                device->failCount = 0;
            }
        } else {
            device->lastLatency = -1.0f; 
            device->failed = true;
            device->failCount++;
        }
        
        // Calculate the safe average while inside the lock zone
        device->avgLatency = getSmartAverageLatency(device);
        updatedFailCount = device->failCount;
        
        xSemaphoreGive(devicesMutex);
    }
  }

  // Handle thread-isolated logging and gateway emergency reboots
  char localLogBuf[96] = { 0 };

  if (pingWorked) {
      if (debugPing == true) {
          snprintf(localLogBuf, sizeof(localLogBuf), "--> | PING %s %s (using %d dBm max %.0u ms) Response #%d %.1f ms", localHostname, localIp, rssi, adaptiveTimeout, totalSent, localLatency);
          logMessage(localLogBuf);
      }
      return true;
  } else {
      if (debugPing == true) {
          snprintf(localLogBuf, sizeof(localLogBuf), "--> | PING %s %s (using %d dBm) TIMEOUT #%d %.0u ms", localHostname, localIp, rssi, totalSent, adaptiveTimeout);
          logMessage(localLogBuf);
      }

      if (strcasecmp(localType, "gateway") == 0) {
        if (adaptiveTimeout >= 500) {
          disconnectWifi();
          reconnectWifi();
          if (WiFi.status() != WL_CONNECTED) {
            snprintf(localLogBuf, sizeof(localLogBuf), "--- | WIFI Gateway %s | %d dBm %u ms | REBOOTING Due to a Degraded Network Connection", localIp, rssi, adaptiveTimeout);
            logMessage(localLogBuf);
            rebootDevice("Poor WIFI");  
          }
          reconnectMDNS();
        }//AdaptiveTimeout
      }//Gateway

  }//PingWorked?
    
  return false;
}


bool icmpPing(IPAddress targetIP) {

  if (targetIP == IPAddress(0, 0, 0, 0)) return false;

  bool pingWorked = false;
  int32_t rssi = WiFi.RSSI();
  uint32_t adaptiveTimeout = 0;
  int maxpings = 0;

  for (int attempt = 0; attempt <= numPings; attempt++) {
    adaptiveTimeout = getLocalTimeoutByRSSI(rssi);
    const byte packetCount = 1;

    pingWorked = Ping.pingMs(targetIP, packetCount, adaptiveTimeout);
    maxpings++;

    if (pingWorked) {
      break;
    }
  }
  if (debugPing == true) {

    float Latency = Ping.averageTime();
    snprintf(msgbuffer, sizeof(msgbuffer), "--> | PING %s | %d dBm %.0u ms| Response #d %.1f ms", targetIP.toString().c_str(), rssi, adaptiveTimeout, maxpings, Latency);
    logMessage(msgbuffer);
  }
  return pingWorked;
}

void printMemoryStats() {
  if(debugMemory == false) return;
  printMemoryTable();

}

void uiTaskCode(void* pvParameters) {
    
  // FreeRTOS tasks must run in an infinite loop
  for (;;) {

    if (button1Pressed ) {

      if(button1Pressed) {
        displayMenu++;
      }
      button1Pressed= false;
      
    } 

    if (prevDisplayMenu != displayMenu && SwitchDisplay == false) {
      prevDisplayMenu = displayMenu;
      updategate = true;
      updatebox = true;

      displayTimer = millis();
      displayMenuPosition(displayMenu);

      if(prevDisplayMenu == 10) displayMenu = 0;
      if (displayMenu == maxMenus + 1) displayMenu = 0;
      if (displayMenu == -1) displayMenu = maxMenus;
      if (displayMenu == 4) displayInfo3();
      if (displayMenu == 3) displayInfo2();
      if (displayMenu == 2) displayInfo1();
      if (displayMenu == 1) displayBoxStatus();
      if (displayMenu == 0) displayGateStatus();
    }

    // Switch Display to show the alarm but only once
    if (SwitchDisplay == true) {
      updategate = true;
      updatebox = true;
      if (BoxAlarm == true) displayBoxStatus();
      if (GateAlarm == true) displayGateStatus();
      if (HelpAlarm == true) displayHelp();
      SwitchDisplay = false;
    }

    // Wi-Fi Bars Refresh
    if (millis() - previousBarsREFRESH >= barsREFRESH) {
      if (WiFi.status() == WL_CONNECTED) {
        currentRSSI = (int8_t)WiFi.RSSI();
        if (currentRSSI != prevdBm) {
          int signalPercent = constrain(map(currentRSSI, -100, -30, 0, 100), 0, 100);
          int bars = map(signalPercent, 0, 100, 0, 5);
          displayWifiBars(bars);
          prevdBm = currentRSSI;
        }
      } else {
        displayWifiBars(0);
        prevdBm = 0;
      }
      previousBarsREFRESH = millis();
    }

    // Go Back to GateStatus if left for 1 min
    if (millis() - displayTimer >= displayRESET && displayMenu != 0) {
      displayTimer = millis();
      updategate = true;
      displayMenu = 0;
      displayGateStatus();
    }

    // Refresh "INFO" Every second
    if (displayMenu == 2) {
      if (millis() - previousREFRESH >= timeREFRESH) {
        previousREFRESH = millis();
        refreshingDisplay = true;
        displayInfo1();
      }
    }

    // Refresh "CPU" Every second
    if (displayMenu == 4) {
      if (millis() - previousREFRESH >= cpuREFRESH) {
        previousREFRESH = millis();
        refreshingDisplay = true;
        displayInfo3();
      }
    }

    // Make the Box Display Flash
    if (displayMenu == 1 && BoxAlarm == true) {
      if (millis() - previousFLASH >= timeoutFLASH) {
        previousFLASH = millis();
        flashState = !flashState;
        updatebox = true;
        displayBoxStatus();
      }
    }

    // Make the Gate Display Flash
    if (displayMenu == 0 && GateAlarm == true) {
      if (millis() - previousFLASH >= timeoutFLASH) {
        previousFLASH = millis();
        flashState = !flashState;
        updategate = true;
        displayGateStatus();
      }
    }

    // Make the Gate Display Flash
    if (displayMenu == 10 && HelpAlarm == true) {
      if (millis() - previousFLASH >= timeoutFLASH) {
        previousFLASH = millis();
        flashState = !flashState;
        updatehelp = true;
        displayHelp();
      }
    }

    // CRITICAL: Feed the watchdog timer and allow background tasks to run
    vTaskDelay(pdMS_TO_TICKS(20));
  }
}


bool isBlank(char* str) {
  while (*str) {
    if (!isspace((unsigned char)*str)) return false;
    str++;
  }
  return true;
}

char* getIP(IPAddress* addr) {
  // Safety check: Ensure the pointer is not null before accessing
  if (addr == nullptr) {
    return nullptr; // Return nullptr if the address pointer is null
  }

  // Calculate the buffer size
  size_t bufferSize = 0;
  if (addr->type() == IPv4) {
    bufferSize = 16; // 4 * 3 digits + 3 dots + 1 null terminator = 16
  } else {
    bufferSize = 40; // 8 * 4 hex digits + 7 colons + 1 null terminator = 40
  }

  // Allocate memory for the resulting string
  char* updateBuffer = new char[bufferSize];

  // Use -> to access the type() method through the pointer
  if (addr->type() == IPv4) {
    // Access individual bytes using the [] operator on the dereferenced pointer
    snprintf(updateBuffer, bufferSize, "%d.%d.%d.%d",
             addr->operator[](0), addr->operator[](1), addr->operator[](2), addr->operator[](3));
  } else {
    // IPv6: Combine bytes into 16-bit words
    snprintf(updateBuffer, bufferSize, "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x",
             (addr->operator[](0) << 8 | addr->operator[](1)), (addr->operator[](2) << 8 | addr->operator[](3)),
             (addr->operator[](4) << 8 | addr->operator[](5)), (addr->operator[](6) << 8 | addr->operator[](7)),
             (addr->operator[](8) << 8 | addr->operator[](9)), (addr->operator[](10) << 8 | addr->operator[](11)),
             (addr->operator[](12) << 8 | addr->operator[](13)), (addr->operator[](14) << 8 | addr->operator[](15)));
  }

  return updateBuffer; // Return the pointer to the allocated string
} 

/*
void getIP(const IPAddress* addr, char* outBuffer, size_t maxLen) {
  // Safety guard against bad pointers or empty buffers
  if (addr == nullptr || outBuffer == nullptr || maxLen == 0) {
    if (outBuffer && maxLen > 0) outBuffer[0] = '\0';
    return;
  }

  if (addr->type() == IPv4) {
    snprintf(outBuffer, maxLen, "%d.%d.%d.%d",
             addr->operator[](0), addr->operator[](1), addr->operator[](2), addr->operator[](3));
  } else {
    // IPv6: Combine bytes into 16-bit words safely
    snprintf(outBuffer, maxLen, "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x",
             (addr->operator[](0) << 8 | addr->operator[](1)), (addr->operator[](2) << 8 | addr->operator[](3)),
             (addr->operator[](4) << 8 | addr->operator[](5)), (addr->operator[](6) << 8 | addr->operator[](7)),
             (addr->operator[](8) << 8 | addr->operator[](9)), (addr->operator[](10) << 8 | addr->operator[](11)),
             (addr->operator[](12) << 8 | addr->operator[](13)), (addr->operator[](14) << 8 | addr->operator[](15)));
  }
}
*/

void toUpperCase(char* str) {
  if (str == NULL) return;  // Always check for NULL

  for (int i = 0; str[i] != '\0'; i++) {
    str[i] = toupper((unsigned char)str[i]);
  }
}

void toLowerCase(char* str) {
  if (str == NULL) {
    return;
  }
  for (size_t i = 0; str[i] != '\0'; i++) {
    str[i] = (char)tolower((unsigned char)str[i]);
  }
}

void lowerCase(char* str) {
    if (str == NULL) return; // Always check for NULL
    
  for (int i = 0; str[i] != '\0'; i++) {
      str[i] = tolower((unsigned char)str[i]);
  }
}

// Turn all chars into uppercase characters
void upperCase(char* str) {
  if (str == NULL) return;  // Always check for NULL

  for (int i = 0; str[i] != '\0'; i++) {
    str[i] = toupper((unsigned char)str[i]);
  }
}


void triggerStartupCheck() {
  if(enablePing == false) return;

  if (devicesMutex != NULL) {

    if (xSemaphoreTake(devicesMutex, 0) == pdTRUE) {
      for (auto *device : active_devices) {
        if (device->monitored) {

            IPAddress target;
            if (target.fromString(device->ip)) {
              
              if (strcasecmp(device->ip, "0.0.0.0") != 0 && strcasecmp(device->ip, "::") != 0) {
                 addToQueue(device->ip, device->hostname, "ping" );
              }

            } 

        } // Monitored
      } //For

    }
    xSemaphoreGive(devicesMutex);
  }//DevicesMutex

}

void disconnectWifi() {

  snprintf(msgbuffer, sizeof(msgbuffer), "--- | WIFI | Disconnecting from %s", ssid);
  logMessage(msgbuffer);

  MDNS.end();
  WiFi.disconnect();
  delay(1000);
 
}

void reconnectWifi() {

  snprintf(msgbuffer, sizeof(msgbuffer), "--- | WIFI | Reconnecting to %s", ssid);
  logMessage(msgbuffer);

  unsigned long startAttemptTime = millis();
  WiFi.reconnect();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
  }
}

void reconnectWifiIfnotConnected() {

  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval)) {

    disconnectWifi();
    reconnectWifi();

    if (WiFi.status() == WL_CONNECTED) {
      
      reconnectMDNS();
      currentRSSI = (int8_t)WiFi.RSSI();
      dBm = WiFi.RSSI();
      requestTokenInterval = 500;
      previousTokenMillis = 0;
      requestNewToken = true;
      prevdBm = 0;

    }

    previousMillis = currentMillis;

  }  // Wifi Interval
}

bool is_valid_char(char c) {
    // Check if the character is a digit or an uppercase letter
    if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z')) {
        return true;
    }
    return false;
}

// Simple check if the Token contains valid characters. 
// Helps to confim that the token doesnt contain bad memory values becuase of a corrupted TCP conversation.
bool isValidTokenChars(const char* buffer) {
    // Iterate through the buffer
    size_t length = strlen(buffer);
    for (size_t i = 0; i < length; ++i) {
        if (!is_valid_char(buffer[i])) {
            return false; // Return false if an invalid character is found
        }
    }
    return true; // Return true if all characters are valid
}

void printMemoryTable() {

  if( totalHeapBytes == 0 || maxBlockBytes == 0) {
    totalHeapBytes = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    maxBlockBytes  = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
  }
  float fragmentation = 100.0f * (1.0f - ((float)maxBlockBytes / totalHeapBytes));

  char heapStr[21];
  char blockStr[21];
  char fragStr[14];

  int val_heap = snprintf(heapStr, sizeof(heapStr), "%s", formatAsBytes(totalHeapBytes));
  int val_block =  snprintf(blockStr, sizeof(blockStr), "%s", formatAsBytes(maxBlockBytes));
  int val_frag = snprintf(fragStr, sizeof(fragStr), "%.1f%%", fragmentation);

  int pad_heapL = (20 - val_heap) / 2;
  if (pad_heapL < 0) pad_heapL = 0;
  int pad_heapR = 20 - val_heap - pad_heapL;

  int pad_blockL = (20 - val_block) / 2;
  if (pad_blockL < 0) pad_blockL = 0;
  int pad_blockR = 20 - val_block - pad_blockL;

  int pad_fragL = (13 - val_frag) / 2;
  if (pad_fragL < 0) pad_fragL = 0;
  int pad_fragR = 13 - val_frag - pad_fragL;

  logMessage("--------------------------------------------------------------");
  logMessage("  Free Memory Heap   |  Free Memory Blocks  |  Fragmentation |");
  snprintf(msgbuffer, sizeof(msgbuffer), "%*s%s%*s | %*s%s%*s | %*s%s%*s  |",  pad_heapL, "", heapStr, pad_heapR, "",  pad_blockL, "", blockStr, pad_blockR, "",  pad_fragL, "", fragStr, pad_fragR, "");
  logMessage(msgbuffer);
  logMessage("--------------------------------------------------------------");

}

CustomTouchPoint getTouchPoint() {
    CustomTouchPoint pt = { false, 0, 0 };

    if (alreadyReading) return pt;
    alreadyReading = true;

    // Use SensorLib's native read algorithm to bypass raw register shift glitches
    if (touch.isPressed()) {
        int16_t raw_x[1];
        int16_t raw_y[1];
        
        // Retrieve point indices from the hardware stack
        uint8_t totalPoints = touch.getPoint(raw_x, raw_y, 1);
        
        if (totalPoints > 0) {
            pt.isPressed = true;
            pt.x = raw_x[0];
            pt.y = raw_y[0];

            // ⚠️ WAVESHARE ORIENTATION FIX ⚠️
            // If the finger tracking matches but acts upside-down or sideways,
            // un-comment the lines below that align with your graphics rotation:
            // 
             pt.x = (SCREEN_WIDTH - 1) - pt.x; // Invert X axis
             pt.y = (SCREEN_HEIGHT - 1) - pt.y; // Invert Y axis
            // uint16_t temp = pt.x; pt.x = pt.y; pt.y = temp; // Swap axes
        }
    }

    // Force constraints safely 
    if (pt.isPressed) {
        pt.x = constrain(pt.x, 0, SCREEN_WIDTH - 1);
        pt.y = constrain(pt.y, 0, SCREEN_HEIGHT - 1);
    }

    alreadyReading = false;
    return pt;
}
