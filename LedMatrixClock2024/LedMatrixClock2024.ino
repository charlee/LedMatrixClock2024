#include <WiFi.h>
#include <HTTPClient.h>
#include "config.h"
#include "font5x7.h"


const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;


#define read_buf_3bytes(x) (buf[x*3] << 16 | buf[x*3 + 1] << 8 | buf[x*3 + 2])
#define write_buf_3bytes(x, v) (buf[x*3] = (v >> 16) & 0xff, buf[x*3 + 1] = (v >> 8) & 0xff, buf[x*3 + 2] = v & 0xff)

unsigned long lastSyncMillis = 0;  
const long interval = 24 * 60 * 60 * 1000;  // 24 hours in milliseconds

/*******************************************************************************
 * 74HC595
 */

#define DATA_PIN 22
#define LATCH_PIN 21
#define CLOCK_PIN 17

// Send 4 bytes to SN74HC595
void send_data_sn74hc595(uint32_t data) {
  digitalWrite(LATCH_PIN, LOW);
  for (int i = 0; i < 32; i++) {
    digitalWrite(CLOCK_PIN, LOW);
    digitalWrite(DATA_PIN, (data & (1 << i)) ? HIGH : LOW);
    digitalWrite(CLOCK_PIN, HIGH);
  }
  digitalWrite(LATCH_PIN, HIGH);
}


/*******************************************************************************
 * Timer
 */
hw_timer_t *timer = NULL;
volatile uint32_t counter = 0;


void ARDUINO_ISR_ATTR onTimer() {
  // Serial.printf("Timer interrupt: %d\n", counter);
}

void setup() {
  Serial.begin(115200);
  delay(10);
  
  // setup PIN mode
  pinMode(DATA_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  

  // start timer
  timer = timerBegin(1000000);
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, 1000, true, 0);
  
  // Connect to WiFi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

int day;
int hour;
int minute;
int second;
int millisec;
int lastMillisec = 0;

unsigned lastCalcMillis = 0;



uint8_t buf[21] = {0};

void show_digit(int digit, int pos) {
  uint32_t offset, v, mask, new_v, buf_v;

  // find from which bit to draw 
  offset = pos * 5;
  if (pos > 1) {

    // if high digits, then offset the colon
    offset += 2;
  }

  // draw digit
  for (int i = 0; i < 7; i++) {

    buf_v = read_buf_3bytes(i);
    v = FONT5x7[digit * 7 + i] << offset;
    mask = ~(0b11111 << offset);
    new_v = (buf_v & mask) | v;

    // write back
    write_buf_3bytes(i, new_v);
  }
}

void show_colon(bool show) {
  uint32_t v, mask, new_v, buf_v;
  for (int i = 0; i < 7; i++) {
    buf_v = read_buf_3bytes(i);

    v = COLON[i] << 10;
    mask = ~(0b11 << 10);
    if (show) {
      new_v = (buf_v & mask) | v;
    } else {
      new_v = buf_v & mask;
    }
    
    // write back
    write_buf_3bytes(i, new_v);
  }
}

// for debug purpose
void print_buf() {
  uint32_t buf_v;
  for (int i = 0; i < 7; i++) {
    buf_v = read_buf_3bytes(i);
    for (int j = 21; j >= 0; j--) {
      Serial.print((buf_v & (1 << j)) ? "x" : ".");
      if (j == 17 || j == 12 || j == 10 || j == 5) {
        Serial.print(" ");
      }
    }
    Serial.println();
  }
  Serial.println();
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastSyncMillis >= interval || lastSyncMillis == 0) {
    Serial.println("Syncing local time...");

    // show_sync(CRGB::Yellow);
  
    // sync time with server
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;
      Serial.println("WIFI connected, calling API...");

      http.begin(client, TIME_SERVER);
      int httpCode = http.GET();
      
      Serial.println(httpCode);

      if (httpCode == 200) {
        String payload = http.getString();
        Serial.print("Local time service response: ");
        Serial.println(payload);
        // Here you can add the code to sync your local clock
        
        day = payload.substring(8, 10).toInt();
        hour = payload.substring(11, 13).toInt();
        minute = payload.substring(14, 16).toInt();
        second = payload.substring(17, 19).toInt();
        millisec = payload.substring(20, 23).toInt();
        
        // confirm that the sync is successful
        lastSyncMillis = currentMillis;
        
        lastCalcMillis = millis();
      } else {
        // otherwise, wait for 10 seconds before retrying
        // show_sync(CRGB::Red);
        Serial.println("Failed to sync local time, server response:");
        Serial.println(httpCode);
        Serial.println("retrying in 10 seconds...");
        delay(10000);
      }

      http.end();
    } else {
      // if wifi is not connected, wait for 10 seconds before retrying
      // show_wifi(CRGB::Red);
      Serial.println("WIFI not connected, retrying in 10 seconds...");
      delay(10000);
    }
  }
  
  // record last millisec to reduce rendering
  lastMillisec = millisec;
    
  // update time
  currentMillis = millis();
  unsigned long diff = currentMillis - lastCalcMillis;
  millisec += diff;

  
  if (millisec >= 1000) {
    second += millisec / 1000;
    millisec %= 1000;
  }

  if (second >= 60) {
    second -= 60;
    minute++;
  }

  if (minute >= 60) {
    minute -= 60;
    hour++;
  }

  if (hour >= 24) {
    hour -= 24;
  }
  
  
  // update state counter
  lastCalcMillis = currentMillis;


  // only update display when millisec changes
  if (lastMillisec >= 500 && millisec < 500 || lastMillisec < 500 && millisec >= 500) {
    // show time
    int digit = 0;
    int i;

    digit = minute % 10;
    show_digit(digit, 0);
    
    digit = minute / 10;
    show_digit(digit, 1);

    digit = hour % 10;
    show_digit(digit, 2);

    digit = hour / 10;
    if (digit == 0) {
      show_digit(DIGIT_SPACE, 3);
    } else {
      show_digit(digit, 3);
    }
    
    if (millisec > 500) {
      show_colon(true);
    } else {
      show_colon(false);
    }
    
    Serial.printf("Time: %02d:%02d:%02d.%03d\n", hour, minute, second, millisec);
    print_buf();
  }
  
  delay(100);
}

