// Libraries
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <U8g2lib.h>
#include <FastLED.h>
#include <map>
#include <vector>

// Files & Inclusions
#include <WebPage.h>
extern const char* webpage;
#include <WebPage2.h>
extern const char* webpage2;

// Simulated sensor data for demonstration
std::map<String, std::vector<std::pair<String, String>>> sensorData;

std::map<String, String> board1Data = {
    {"temperature", "72.5 F"},
    {"humidity", "40 %"},
    {"pressure", "1013 hPa"},
    {"pm1_0", "15 ppm"},
    {"pm2_5", "20 ppm"},
    {"pm10", "25 ppm"}
};

std::map<String, String> board2Data = {
    {"temperature", "68.0 F"},
    {"humidity", "50 %"},
    {"pressure", "1010 hPa"},
    {"pm1_0", "10 ppm"},
    {"pm2_5", "15 ppm"},
    {"pm10", "20 ppm"}
};

// Constants for OLED and LEDs
#define OLED_CLOCK      18
#define OLED_DATA       17
#define OLED_RESET      21
#define LED_PIN         7
#define NUM_LEDS        14
#define LED_TYPE        WS2812B
#define COLOR_ORDER     GRB
#define LED_BRIGHTNESS  255
#define MAX_POWER       900

// LED configuration
CRGB LEDs[NUM_LEDS] = {0};

// OLED configuration
U8G2_SSD1306_128X64_NONAME_F_HW_I2C g_OLED(U8G2_R2, OLED_RESET, OLED_CLOCK, OLED_DATA);
int g_lineHeight = 0;
int g_Brightness = 255;
int g_PowerLimit = 3000;

// Time taken after visiting /stop
int selfDestructTime = 5;

// Keys, data, and suffix arrays for sensor values
String keys[6] = {"temperature", "humidity", "pressure", "pm1_0", "pm2_5", "pm10"};    // Keys for data
int datum[6] = {1, 2, 3, 4, 5, 6};    // pm1.0, pm2.5, pm10.0, temp, hum, psi (placeholder values)
String suf[6] = {"F", "%", "hPa", "ppm", "ppm", "ppm"};    // Suffixes for readings
const char* messages[5] = {" ", " ", " ", " ", " "};

// Wifi Information
bool AP = true;
const char* apssid = "AQIQ";
const char* appassword = "Chris21bc";
String links[4] = {"/", "/t", "/test", "/stop"};
#define SERVER_PORT 80

WebServer server(SERVER_PORT);

void displayMessages() {
    g_OLED.clearBuffer();  // Clear the screen
    for (int i = 0; i < 5; i++) {
        g_OLED.setCursor(0, g_lineHeight * (i + 1));  // Display each message on a new line
        g_OLED.printf("%s", messages[i]);
    }
    Serial.printf("M: %s \n", messages[0]);

    g_OLED.sendBuffer();  // Send the updated buffer to the OLED
}

void updateMessages(const char* msg, bool display) {
    for (int i = 4; i > 0; i--) {
        messages[i] = messages[i - 1];
    }
    messages[0] = msg;
	if (display) {
        displayMessages();
    }
}

void initializeOLED() {
    g_OLED.begin();
    g_OLED.clear();
    g_OLED.setFont(u8g2_font_profont15_tf);    
    g_lineHeight = g_OLED.getFontAscent() - g_OLED.getFontDescent();
    updateMessages("g_OLED Start", true);
}

void initializeFastLED() {
    FastLED.addLeds<LED_TYPE, LED_PIN, GRB>(LEDs, NUM_LEDS);
    FastLED.setBrightness(g_Brightness);
    FastLED.setMaxPowerInMilliWatts(g_PowerLimit);
    updateMessages("FastLED Start", true);
}

void updateSensorData(String boardName, std::map<String, String> data) {
    std::vector<std::pair<String, String>> dataVector;
    for (auto& entry : data) {
        dataVector.emplace_back(entry.first, entry.second);
    }
    sensorData[boardName] = dataVector;
}

String readHTMLFile(const char* page) {
    String html = String(page);

    // Serial.println("Checking sensorData contents...");
    // if (sensorData.empty()) {
    //     Serial.println("sensorData is empty!");
    // } else {
    //     Serial.println("sensorData has data:");
    //     for (auto& board : sensorData) {
    //         Serial.print("Board: ");
    //         Serial.println(board.first);
    //         for (auto& sensor : board.second) {
    //             Serial.print("  ");
    //             Serial.print(sensor.first);
    //             Serial.print(": ");
    //             Serial.println(sensor.second);
    //         }
    //     }
    // }

    for (auto& board : sensorData) {
        String boardNamePlaceholder = "{" + board.first + "}";  // e.g., "{Board 1}"
        html.replace(boardNamePlaceholder, board.first);        // Replace with the actual board name
        int i = 0;
        for (auto& sensor : board.second) {
            i += 1;
            String sensorPlaceholder = "{" + board.first + " " + sensor.first + "}";  // e.g., "{Board 1 temperature}"
            html.replace(sensorPlaceholder, sensor.second);                           // Replace with the sensor value
        }
    }

    // Replace any remaining placeholders with a default value (e.g., "N/A")
    html.replace("{}", "N/A");
    return html;
}

void handleBasePage() {
    String htmlContent = String(R"rawliteral(<html><body><h1>ESP32 Server is Running!</h1><p><a href="/"><span style="background-color: #2b2301; color: #fff; display: inline-block; padding: 3px 10px; font-weight: bold; border-radius: 5px;">Home Page</span></a><a href="/test"><span style="background-color: #2b2301; color: #fff; display: inline-block; padding: 3px 10px; font-weight: bold; border-radius: 5px;">Test Page</span></a><a href="/test2"><span style="background-color: #2b2301; color: #fff; display: inline-block; padding: 3px 10px; font-weight: bold; border-radius: 5px;">Test Page 2</span></a><a href="/stop"><span style="background-color: #2b2301; color: #fff; display: inline-block; padding: 3px 10px; font-weight: bold; border-radius: 5px;">Stop Server</span></a></p></body></html>)rawliteral");
    server.send(200, "text/html", htmlContent);
}

void handleHTMLRoot() {
    String htmlContent = readHTMLFile(webpage);
    server.send(200, "text/html", htmlContent);
}

void handleHTMLRoot2() {
    String htmlContent = readHTMLFile(webpage2);
    server.send(200, "text/html", htmlContent);
}

//generate the links to the website
void generateLinks() {
    String ip = AP ? WiFi.softAPIP().toString() : WiFi.localIP().toString();
    for (int i = 0; i < (sizeof(links) / sizeof(links[0])); i++) {
        Serial.printf("http://%s:%d%s\n", ip.c_str(), SERVER_PORT, links[i].c_str());
    }
    updateMessages(("http://" + ip + ":" + String(SERVER_PORT) + "/").c_str(), true);
}

// Function to stop the web server
void stopWebServer() {
    server.stop();  // Stop the server
    Serial.println("\nWeb server stoping.");
    updateMessages("Web server stoping.", true);
    delay(5000);
    Serial.println("\nWeb server stopped.");
    updateMessages("Web server stopped.", true);
}

void handleStop() {
    String html = "<html><head><script>function autoRefresh() {window.location = window.location.href;} setInterval('autoRefresh()', ";
    html += String(selfDestructTime);
    html += "000);</script></head><h1>Self Destructing...</h1></html>";
    server.send(200, "text/html", html);
    stopWebServer();  // Stop the server
}

void connectAP () {
    WiFi.mode(WIFI_AP);
    if (WiFi.softAP(apssid, appassword)) {
        Serial.printf("Access Point started: %s\nPassword: %s\n", apssid, appassword);
        Serial.print("AP IP address: ");
        Serial.println(WiFi.softAPIP());
        updateMessages("Access Point started", true);
    } else {
        Serial.println("Failed to start AP.");
        updateMessages("Failed to start AP.", true);
    }
}

void startServer() {
    server.on("/", handleBasePage); // Handle test page
    server.on("/test", handleHTMLRoot); // Handle webpage
    server.on("/test2", handleHTMLRoot2); // Handle webpage2
    server.on("/stop", handleStop); // Handle stop request

    server.onNotFound([]() {
        Serial.println("Client tried to access: " + server.uri());
        server.send(404, "text/plain", "404: Not Found");
        updateMessages("404", true);
    });

    server.begin();
    Serial.println("Server started!");
}

void setup() {
    delay(2500);
    Serial.begin(115200);
    Serial.println("\nStart");
    updateMessages("Start", false);

    initializeOLED();
    initializeFastLED();


    updateSensorData("Board 1", board1Data);
    updateSensorData("Board 2", board2Data);
    updateSensorData("Board 3", board1Data);
    updateSensorData("Board 4", board2Data);

    connectAP();
    startServer();
    generateLinks();
}

void loop() {
    server.handleClient();
    datum[0] += 2.0;
    handleHTMLRoot();
    delay(5000);
}