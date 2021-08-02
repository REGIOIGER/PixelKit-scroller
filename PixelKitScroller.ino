#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "time.h"

#include <HTTPClient.h>
#include <Arduino_JSON.h>

#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#include <FastLED.h>
#include <LEDMatrix.h>
#include <LEDText.h>
#include <FontMatrise.h>
#include <LEDSprites.h>

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -21600;
const int   daylightOffset_sec = 3600;

// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "**********************************************"  // YOUR BOT TOKEN

const unsigned long BOT_MTBS = 60000; // mean time between scan messages

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime; // last time messages' scan has been done


// Change the next 6 defines to match your matrix type and size

#define LED_PIN        4        // LED Matrix on Pixel Kit
#define COLOR_ORDER    GRB
#define CHIPSET        WS2812B

#define MATRIX_WIDTH   16
#define MATRIX_HEIGHT  -8
#define MATRIX_TYPE    HORIZONTAL_MATRIX

cLEDMatrix<MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> leds;

cLEDText ScrollingMsg;

const unsigned char TxtDemo[] = { EFFECT_FRAME_RATE "\x00"
                                  EFFECT_HSV_AH "\x00\xff\xff\xff\xff\xff"
                                  //EFFECT_SCROLL_LEFT " Some text here...           mal  "
                                  //EFFECT_SCROLL_LEFT "   Hello world!  "
                                  EFFECT_SCROLL_LEFT "  This is the another way  " };

uint16_t Options;


#define SHAPE_WIDTH    74
#define SHAPE_HEIGHT   8

// pokemon banner eves

const uint8_t SpritehkData[] = {
  B8_4BIT(00000000),B8_4BIT(00000000),B8_4BIT(00000000),B8_4BIT(00000100),B8_4BIT(00000000),B8_4BIT(00000000),B8_4BIT(00000000),B8_4BIT(00000000),B8_4BIT(00000000),B8_4BIT(00000000),
  B8_4BIT(00000002),B8_4BIT(22000000),B8_4BIT(00000000),B8_4BIT(30001400),B8_4BIT(00000050),B8_4BIT(60000000),B8_4BIT(00000000),B8_4BIT(20022000),B8_4BIT(00000000),B8_4BIT(00000000),
  B8_4BIT(00000027),B8_4BIT(70508000),B8_4BIT(00000090),B8_4BIT(30900300),B8_4BIT(00000050),B8_4BIT(60060000),B8_4BIT(000000A7),B8_4BIT(2B007200),B8_4BIT(00000000),B8_4BIT(00000000),
  B8_4BIT(00000058),B8_4BIT(80558000),B8_4BIT(00000019),B8_4BIT(39100300),B8_4BIT(00000056),B8_4BIT(65050600),B8_4BIT(000000AA),B8_4BIT(BB007200),B8_4BIT(00000000),B8_4BIT(00000000),
  B8_4BIT(00000058),B8_4BIT(02508700),B8_4BIT(00000010),B8_4BIT(C0133C00),B8_4BIT(00000006),B8_4BIT(01585000),B8_4BIT(0000000A),B8_4BIT(0B207200),B8_4BIT(00000000),B8_4BIT(00000000),
  B8_4BIT(00000085),B8_4BIT(52288700),B8_4BIT(0000001C),B8_4BIT(CC1CC300),B8_4BIT(00000056),B8_4BIT(61668500),B8_4BIT(0000007A),B8_4BIT(A22BB700),B8_4BIT(00000000),B8_4BIT(00000000),
  B8_4BIT(00000005),B8_4BIT(57227000),B8_4BIT(00000001),B8_4BIT(11CC3300),B8_4BIT(00000011),B8_4BIT(15666000),B8_4BIT(00000007),B8_4BIT(227BB000),B8_4BIT(00000000),B8_4BIT(00000000),
  B8_4BIT(00000008),B8_4BIT(08008000),B8_4BIT(00000000),B8_4BIT(30CD0300),B8_4BIT(00000008),B8_4BIT(06506000),B8_4BIT(0000000A),B8_4BIT(00A0A000),B8_4BIT(00000000),B8_4BIT(00000000) };
const struct CRGB SpritehkCols[15] = { CRGB(255,255,255), CRGB(255,221,171), CRGB(4,64,112), CRGB(145,141,136), CRGB(188,120,22), CRGB(255,204,17), CRGB(184,146,60), CRGB(110,61,14), CRGB(73,163,235), CRGB(192,57,0), CRGB(247,97,0), CRGB(14,117,171), CRGB(2,36,64), CRGB(0,0,0), CRGB(0,0,0) };
cSprite Spritehk(74, 8, SpritehkData, 1, _4BIT, SpritehkCols);


/*
const uint8_t SpritehkData[] = {
  B8_2BIT(11121112),B8_2BIT(11121111),
  B8_2BIT(11112111),B8_2BIT(11211111),
  B8_2BIT(11111122),B8_2BIT(21111111),
  B8_2BIT(11111222),B8_2BIT(22111111),
  B8_2BIT(11221222),B8_2BIT(22122111),
  B8_2BIT(11111222),B8_2BIT(22111111),
  B8_2BIT(11111122),B8_2BIT(21111111),
  B8_2BIT(11112111),B8_2BIT(11211111) };
const struct CRGB SpritehkCols[3] = { CRGB(16,66,115), CRGB(255,255,0), CRGB(0,0,0) };
cSprite Spritehk(16, 8, SpritehkData, 1, _2BIT, SpritehkCols);
*/
/*
const uint8_t SpritehkData[] = {
  B8_3BIT(11100111),B8_3BIT(21100111),
  B8_3BIT(11011002),B8_3BIT(32011011),
  B8_3BIT(11011123),B8_3BIT(33222221),
  B8_3BIT(11011122),B8_3BIT(22233321),
  B8_3BIT(10111111),B8_3BIT(11123201),
  B8_3BIT(00011111),B8_3BIT(11112000),
  B8_3BIT(10111111),B8_3BIT(11111101),
  B8_3BIT(00010111),B8_3BIT(11101000),
  B8_3BIT(10110111),B8_3BIT(11101101),
  B8_3BIT(11011114),B8_3BIT(41111011),
  B8_3BIT(11100111),B8_3BIT(11100111),
  B8_3BIT(11011000),B8_3BIT(00011011),
  B8_3BIT(11000233),B8_3BIT(33200011),
  B8_3BIT(11111222),B8_3BIT(22211111),
  B8_3BIT(11111011),B8_3BIT(11011111),
  B8_3BIT(11155500),B8_3BIT(00555111) };
const struct CRGB SpritehkCols[7] = { CRGB(255,255,255), CRGB(170,0,170), CRGB(255,85,255), CRGB(170,85,0), CRGB(170,170,170), CRGB(0,0,0), CRGB(0,0,0) };
cSprite Spritehk(16, 16, SpritehkData, 1, _3BIT, SpritehkCols);
*/

cLEDSprites Sprites(&leds);

//String strLocalIp = WiFi.localIP().toString();
//const char* cstrIp = strLocalIp.c_str();

const char* ssid = "**************"; // YOUR SSID
const char* password = "*****************"; // YOUR PASSWORD

// Your Domain name with URL path or IP address with path
String openWeatherMapApiKey = "********************************";
// Example:
//String openWeatherMapApiKey = "bd939aa3d23ff33d3c8f5dd1dd435";

// Replace with your country code and city
//String city = "LA";
//String countryCode = "US";

// THE DEFAULT TIMER IS SET TO 10 SECONDS FOR TESTING PURPOSES
// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 10 seconds (10000)
unsigned long timerDelay = 300000; // two minutes

String jsonBuffer;

String strCad = "  Hello world!  ";
String teleText = "    :)     ";
String timeCad = "    ";

int ledStatus = 0;

void handleNewMessages(int numNewMessages)
{
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);
  
  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";

    teleText = teleText + text + "   ";
    bot.sendMessage(chat_id, "Message delivered", "Markdown");

    FastLED.setBrightness(64);
    FastLED.clear(true);
    delay(100);
    FastLED.showColor(CRGB::Red);
    delay(100);
    FastLED.clear(true);
    delay(100);
    FastLED.showColor(CRGB::Red);
    delay(100);
    FastLED.clear(true);
    delay(100);    
    FastLED.showColor(CRGB::Red);
    delay(100);
    FastLED.clear(true);
    delay(100);
    FastLED.showColor(CRGB::Red);
    delay(100);
    FastLED.clear(true);
    delay(100);
    FastLED.showColor(CRGB::Red);
    delay(100);
    FastLED.clear(true);
    delay(100);
    FastLED.showColor(CRGB::Red);
    delay(100);
    FastLED.clear(true);
    delay(100);    
    FastLED.showColor(CRGB::Red);
    delay(100);
    FastLED.clear(true);
    delay(100);
    FastLED.showColor(CRGB::Red);
    delay(100);
    FastLED.clear(true);
    //FastLED.show();
    
    FastLED.setBrightness(8);
    if (text == "/start")
    {
      String welcome = "Welcome to PixelKitBot, " + from_name + ".\n";
      welcome += "The message will be show\n\n";
      welcome += "in PixelKit soon.\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
    if (text == "/delete")
    { teleText = "";
      String welcome = "The message was deleted on PixelKit.\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, password);

  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org

  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds[0], leds.Size());
  FastLED.setBrightness(8);
  FastLED.clear(true);
  delay(500);
  FastLED.showColor(CRGB::Red);
  delay(1000);
  FastLED.show();


  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 minutes (timerDelay variable), it will take 5 minutes before publishing the first reading.");

  ScrollingMsg.SetFont(MatriseFontData); //RobotronFontData);
  ScrollingMsg.Init(&leds, leds.Width(), ScrollingMsg.FontHeight() + 1, 0, 0);

  //String strLocalIp = WiFi.localIP().toString();
  //const char* cstrIp = strLocalIp.c_str();
  
  //ScrollingMsg.SetText((unsigned char *)cstrIp, sizeof(cstrIp) - 1);
  
  ScrollingMsg.SetText((unsigned char *)"   ALL SYSTEMS GO...   ", 23);
  
  //ScrollingMsg.SetTextDirection(CHAR_DOWN);
  ScrollingMsg.SetTextColrOptions(COLR_RGB | COLR_SINGLE, 0xff, 0x00, 0xff);
  Options = INSTANT_OPTIONS_MODE;
  ScrollingMsg.SetOptionsChangeMode(Options);

  
  //Spritehk.SetPositionFrameMotionOptions(0/*X*/, -8/*Y*/, 0/*Frame*/, 0/*FrameRate*/, 0/*XChange*/, 1/*XRate*/, 0/*YChange*/, 1/*YRate*/, SPRITE_X_KEEPIN | SPRITE_Y_KEEPIN);
  Sprites.AddSprite(&Spritehk);

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); // get UTC time via NTP

}

int counter = 0;
int icon = 0;

void loop() {


  //*****************************************************************************
  
  // Send an HTTP GET request
  if ((millis() - lastTime) > timerDelay) {
    // Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){

      
      String serverPath = "http://api.openweathermap.org/data/2.5/onecall?lat=******************&lon=********************&exclude=minutely,daily&lang=sp&appid=*****************************";
      
      //"http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;
      
      jsonBuffer = httpGETRequest(serverPath.c_str());
      //Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);
  
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
    
      //Serial.print("JSON object = ");
      //Serial.println(myObject);
      
      //Serial.print("Temperature: ");
      //Serial.println(myObject["current"]["temp"]);
      //Serial.print("Pressure: ");
      //Serial.println(myObject["current"]["pressure"]);
      //Serial.print("Humidity: ");
      //Serial.println(myObject["current"]["humidity"]);
      //Serial.print("Wind Speed: ");
      //Serial.println(myObject["current"]["speed"]);

      //JSONVar value = myObject["main"]["temp"];

      icon = (int)myObject["current"]["weather"][0]["id"];
      
      strCad =  " Current weather: " + String((const char*)myObject["current"]["weather"][0]["description"]) + "  T: " + String(int(myObject["current"]["temp"])-273) + "\`C  H: " + String(int(myObject["current"]["humidity"])) + "%  "; 
      strCad = strCad + " Weather next hour: " + String((const char*)myObject["hourly"][0]["weather"][0]["description"]) + "   "; 
      Serial.println(strCad);
      
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }

  // ***************************************************************************

   
  if (ScrollingMsg.UpdateText() == -1)
  {
    counter++;
    if(counter%10 == 1)
    {
      ScrollingMsg.SetText((unsigned char *)TxtDemo, sizeof(TxtDemo) - 1);
    }
    
    else if(counter%10 == 2)
    {

      if (millis() - bot_lasttime > BOT_MTBS)
      {
        int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

        while (numNewMessages)
          {
            Serial.println("got response");
            handleNewMessages(numNewMessages);
            numNewMessages = bot.getUpdates(bot.last_message_received + 1);
          }

        bot_lasttime = millis();
      }
      
      const char* cteleText = teleText.c_str();
      ScrollingMsg.SetText((unsigned char *)cteleText, teleText.length());
    }
 
    else if(counter%10 == 3)
    {
    const char* cstrCad = strCad.c_str();
    ScrollingMsg.SetText((unsigned char *)cstrCad, strCad.length());
    }

    else if(counter%10 == 4)
    {

      struct tm timeinfo;
      if(!getLocalTime(&timeinfo)){
        Serial.println("Failed to obtain time");
        return;
       }
      Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
      char timeStringBuff[50]; //50 chars should be enough
      strftime(timeStringBuff, sizeof(timeStringBuff), "%A, %B %d %H:%M", &timeinfo);
      timeCad = "    " + String(timeStringBuff) + "    ";
      //ScrollingMsg.SetText((unsigned char *)timeStringBuff, sizeof(timeStringBuff));

      const char* ctimeCad = timeCad.c_str();
      ScrollingMsg.SetText((unsigned char *)ctimeCad, timeCad.length());

    }
    
    else if(counter%10 == 5)
    {
      int y_pos = 16;
      int y_change = -1;
      Spritehk.SetPositionFrameMotionOptions(16/*X*/, 0/*Y*/, 0/*Frame*/, 0/*FrameRate*/, -1/*XChange*/, 1/*XRate*/, 0/*YChange*/, 1/*YRate*/, SPRITE_X_KEEPIN | SPRITE_Y_KEEPIN);
  
      for(int i=0;i<SHAPE_WIDTH + 8;i++){
        y_pos+=y_change;
        FastLED.clear();
        if(y_pos == 16)
        {
        y_change = -y_change;
        Spritehk.SetPositionFrameMotionOptions(16/*X*/, 0/*Y*/, 0/*Frame*/, 0/*FrameRate*/, -1/*XChange*/, 1/*XRate*/, 0/*YChange*/, 1/*YRate*/, SPRITE_X_KEEPIN | SPRITE_Y_KEEPIN);  
        }
        
        if(y_pos == -3 || y_pos == -19 || y_pos == -35 || y_pos == -51){
          delay(2000);
        }
        
        Sprites.UpdateSprites();
        Sprites.RenderSprites();
        FastLED.show();
        delay(50);
      }

      FastLED.clear();
    }
    
    Options ^= INSTANT_OPTIONS_MODE;
    ScrollingMsg.SetOptionsChangeMode(Options);
    ScrollingMsg.UpdateText();
    if(counter > 5)counter = 0;
  }
  else
  {
    FastLED.show();
  }  
  delay(30);
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();
  return payload;

}
