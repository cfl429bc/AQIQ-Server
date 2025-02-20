// Libraries
#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h>
#include <U8g2lib.h>
#include <FastLED.h>
#include <painlessMesh.h>
#include <TaskScheduler.h>
#include <map>
#include <vector>

// Files & Inclusions
#include <WebPage.h>
#include <WebPage2.h>
extern const char* webpageBegining;
extern const char* webpageMiddle;
extern const char* webpageEnd;
const char* webpage;
extern const char* webpage2;

#define PMS_RX 19
#define PMS_TX 20

HardwareSerial pmsSerial(2);

// variables for PMS7003
uint16_t pm1_0, pm2_5, pm10_0;

// Variables for averaging
const int numReadings = 15; // Number of readings to average (15 seconds worth)
int readingsPM1_0[numReadings];  // Array to store PM1.0 readings
int readingsPM2_5[numReadings];  // Array to store PM2.5 readings
int readingsPM10_0[numReadings]; // Array to store PM10 readings

int readIndex = 0;               // Current reading index
unsigned long lastReadTime = 0;   // To keep track of the time for taking readings

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


// sets variable to switch between Access Point and Sensor board
// Either "WebServer", "Mesh", or "Both"
String boardType = "Both";
// String boardType = "Mesh";


// Time taken after visiting /stop
int selfDestructTime = 5;

// Simulated sensor data for demonstration
std::map<String, std::vector<std::pair<String, String>>> sensorData;
std::map<String, uint32_t> boardMapping; // Stores mappings of "Board #" to Node ID
unsigned int globalBoardCounter = 0;

std::map<String, String> exampleBoardData = {
    {"temperature", "0"},
    {"humidity", "1"},
    {"pressure", "2"},
    {"pm1_0", "3"},
    {"pm2_5", "4"},
    {"pm10", "5"}
};

// Keys, data, and suffix arrays for sensor values
String keys[6] = {"temperature", "humidity", "pressure", "pm1_0", "pm2_5", "pm10"};    // Keys for data
float datum[6] = {1, 2, 3, 4, 5, 6};    // pm1.0, pm2.5, pm10.0, temp, hum, psi (placeholder values)
String suf[6] = {"F", "%", "hPa", "ppm", "ppm", "ppm"};    // Suffixes for readings
JsonDocument jsonReadings;
String readings;    //String to send to other nodes with sensor readings
const char* messages[5] = {" ", " ", " ", " ", " "};
const char * num[100] = {
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", 
    "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", 
    "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", 
    "31", "32", "33", "34", "35", "36", "37", "38", "39", "40", 
    "41", "42", "43", "44", "45", "46", "47", "48", "49", "50", 
    "51", "52", "53", "54", "55", "56", "57", "58", "59", "60", 
    "61", "62", "63", "64", "65", "66", "67", "68", "69", "70", 
    "71", "72", "73", "74", "75", "76", "77", "78", "79", "80", 
    "81", "82", "83", "84", "85", "86", "87", "88", "89", "90", 
    "91", "92", "93", "94", "95", "96", "97", "98", "99", "100"
    };

// Sensor data variable 
// Kinda redundent rn I am just using it for tests
float temperature = 0.0;
float humidity = 0.0;
float pressure = 0.0;
float pm1_0float = 0.0;
float pm2_5float = 0.0;
float pm10_0float = 0.0;
int counter = 0;

// Mesh network settings
#define MESH_PREFIX "esp32_mesh"
#define MESH_PASSWORD "mesh_password"
#define MESH_PORT 5555
#define connectMode = WIFI_STA   // Switch between WIFI_AP, WIFI_STA and WIFI_AP_STA (default) mode
uint32_t recent_node = 12345678;

// Wifi Information
bool AP = true;
const char* apssid = "AQIQ";
const char* appassword = "Admin12345";
String links[4] = {"/", "/test", "/test2", "/stop"};

// Creates server on the declared port
#define SERVER_PORT 80
WebServer server(SERVER_PORT);

Scheduler userScheduler;  // Task scheduler for painlessMesh
painlessMesh mesh;	// Mesh network instance

// Display the messages on the board
void displayMessages() {
    g_OLED.clearBuffer();  // Clear the screen
    for (int i = 0; i < 5; i++) {
        g_OLED.setCursor(0, g_lineHeight * (i + 1));  // Display each message on a new line
        g_OLED.printf("%s", messages[i]);
    }
    Serial.printf("M: %s \n", messages[0]);

    g_OLED.sendBuffer();  // Send the updated buffer to the OLED
}

// Updates the "messages" list
void updateMessages(const char* msg, bool display) {
    for (int i = 4; i > 0; i--) {
        messages[i] = messages[i - 1];
    }
    messages[0] = msg;
	if (display) {
        displayMessages();
    }
}

void serialDelay(int seconds) {
	Serial.println("");
	for (int i = 0; i < seconds; i++) {
        updateMessages(num[i], true);
		delay(1000);
	}
}

// Sets up the screen on the board
void initializeOLED() {
    g_OLED.begin();
    g_OLED.clear();
    g_OLED.setFont(u8g2_font_profont15_tf);    
    g_lineHeight = g_OLED.getFontAscent() - g_OLED.getFontDescent();
    updateMessages("g_OLED Start", true);
}

// Sets up the board for LED usage
void initializeFastLED() {
    FastLED.addLeds<LED_TYPE, LED_PIN, GRB>(LEDs, NUM_LEDS);
    FastLED.setBrightness(g_Brightness);
    FastLED.setMaxPowerInMilliWatts(g_PowerLimit);
    updateMessages("FastLED Start", true);
}

String getBoardName(uint32_t nodeId) {
    // Check if this node already exists in boardMapping:
    for (auto const &entry : boardMapping) {
        if (entry.second == nodeId) {
            return entry.first; // Return existing board name
        }
    }
    // Otherwise, assign a new board number:
    globalBoardCounter++;
    String boardName = "Board " + String(globalBoardCounter);
    boardMapping[boardName] = nodeId;
    Serial.printf("Assigned %s to Node ID %u\n", boardName.c_str(), nodeId);
    return boardName;
}

// String getBoardNameFromNodeId(uint32_t nodeId) {
//     // Loop through the boardMapping to find the board name corresponding to nodeId.
//     for (auto const &entry : boardMapping) {
//         if (entry.second == nodeId) {
//             return entry.first;
//         }
//     }
//     // If not found, return the node ID as a string (or you can return an empty string or error)
//     return String(nodeId);
// }

// Updates the map with the sensor data
void updateSensorData(String boardName, std::map<String, String> data) {
    std::vector<std::pair<String, String>> dataVector;
    for (auto& entry : data) {
        dataVector.emplace_back(entry.first, entry.second);
    }
    sensorData[boardName] = dataVector;
}

// Function to take HTML and replace teh data
String readHTMLFile(const char* page) {
    const char* oof = page;
    String html = String(oof);

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

    // for (auto& board : sensorData) {
    //     String boardNamePlaceholder = "{" + board.first + "}";  // e.g., "{Board 1}"
    //     html.replace(boardNamePlaceholder, board.first);        // Replace with the actual board name
    //     // int i = 0;
    //     for (auto& sensor : board.second) {
    //         String sensorPlaceholder = "{" + board.first + " " + sensor.first + "}";  // e.g., "{Board 1 temperature}"
    //         String replaceUnit = sensor.second;
    //         // i += 1;
    //         html.replace(sensorPlaceholder, replaceUnit);                           // Replace with the sensor value
    //     }
    // }
    //
    // // Replace any remaining placeholders with a default value (e.g., "N/A")
    // html.replace("{}", "N/A");
    // return html;

    // Replace {Board #} placeholders with the assigned numbers
    for (const auto& board : boardMapping) {
        String placeholder = "{" + board.first + "}";  // e.g., {Board 1}
        html.replace(placeholder, board.first);
    }

    // Replace sensor placeholders for each assigned board
    for (const auto& board : sensorData) {
        String boardName = board.first; // e.g., "Board 1"
        for (const auto& sensor : board.second) {
            String sensorPlaceholder = "{" + boardName + " " + sensor.first + "}"; 
            html.replace(sensorPlaceholder, sensor.second);
        }
    }

    // Replace any remaining placeholders with "N/A"
    html.replace("{}", "N/A");
    return html;
}

// Handles home page
void handleBasePage() {
    String htmlContent = String(R"rawliteral(<html><body><h1>ESP32 Server is Running!</h1><p><a href="/"><span style="background-color: #2b2301; color: #fff; display: inline-block; padding: 3px 10px; font-weight: bold; border-radius: 5px;">Home Page</span></a><a href="/test"><span style="background-color: #2b2301; color: #fff; display: inline-block; padding: 3px 10px; font-weight: bold; border-radius: 5px;">Test Page</span></a><a href="/test2"><span style="background-color: #2b2301; color: #fff; display: inline-block; padding: 3px 10px; font-weight: bold; border-radius: 5px;">Test Page 2</span></a><a href="/stop"><span style="background-color: #2b2301; color: #fff; display: inline-block; padding: 3px 10px; font-weight: bold; border-radius: 5px;">Stop Server</span></a></p></body></html>)rawliteral");
    server.send(200, "text/html", htmlContent);
}

//Handles page 1
void handleHTMLRoot() {
    String begining = String(webpageBegining);
    String middle = String(webpageMiddle);
    String end = String(webpageEnd);
    String combined = begining;

    // Number of times to append 'middle'
    int repeatCount = globalBoardCounter;

    // Loop to append the middle string repeatedly
    if (globalBoardCounter != 0) {
        for (int i = 0; i < repeatCount; i++) {
            String middleUpdate = middle;
            middleUpdate.replace(String("!#!"), String(i + 1));
            combined += middleUpdate;
            if (i != globalBoardCounter) {
                combined += String("<p>&nbsp;</p>");
            }
        }
    } else {
        combined += R"rawliteral(<h2 style="color: #2e6c80;">No Boards Yet:</h2>)rawliteral";
    }

    // Append the end string
    combined += end;
    

    String htmlContent = readHTMLFile(combined.c_str());
    server.send(200, "text/html", htmlContent);
}

// Handles page 2
void handleHTMLRoot2() {
    String htmlContent = readHTMLFile(webpage2);
    server.send(200, "text/html", htmlContent);
}

// Generate the links to the website
void generateLinks() {
    String ip = AP ? WiFi.softAPIP().toString() : WiFi.localIP().toString();
    for (int i = 0; i < (sizeof(links) / sizeof(links[0])); i++) {
        Serial.printf("http://%s:%d%s\n", ip.c_str(), SERVER_PORT, links[i].c_str());
    }
    updateMessages(("http://" + ip + ":" + String(SERVER_PORT) + "/").c_str(), true);
}

// Function to actually stop the web server
void stopWebServer() {
    server.stop();  // Stop the server
    Serial.println("\nWeb server stoping.");
    updateMessages("Web server stoping.", true);
    serialDelay(selfDestructTime);
    Serial.println("\nWeb server stopped.");
    updateMessages("Web server stopped.", true);
}

// Web page for stoping the server
void handleStop() {
    String html = "<html><head><script>function autoRefresh() {window.location = window.location.href;} setInterval('autoRefresh()', ";
    html += String(selfDestructTime);
    html += "000);</script></head><h1>Self Destructing...</h1></html>";
    server.send(200, "text/html", html);
    stopWebServer();  // Stop the server
}

// Creates an access point (Wifi Network) with the credentials defined at teh top
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

// Starts a server on the AP
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

std::map<String, String> jsonToMap(String jsonString) {
    std::map<String, String> dataMap;

    
    // // Your JSON string
    // String jsonString = String(R"rawliteral( {"temperature":65,"humidity":25,"pressure":73,"pm1_0":10,"pm2_5":69,"pm10":22} )rawliteral");
    
    // Create a JSON document to parse the string
    JsonDocument doc;
    
    // Deserialize the JSON string
    DeserializationError error = deserializeJson(doc, jsonString);
    
    // Check for errors in deserialization
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return dataMap;
    }
    
    // Loop through the keys and values, and store them in the map
    for (JsonPair kv : doc.as<JsonObject>()) {
        String key = kv.key().c_str();
        String value = String(kv.value().as<int>());
        dataMap[key] = value;
    }

    return dataMap;
}

String readingsToJSON(float sensTemp, float sensHum, float sensPress, float senspm1, float senspm2_5, float senspm10) {
    datum[1] = sensTemp;
    datum[2] = sensHum;
    datum[3] = sensPress;
    datum[4] = senspm1;
    datum[5] = senspm2_5;
    datum[6] = senspm10;
    for (int i = 0; i < 6; i++) {
        // int randomNum = rand() % 101;
        // Serial.println(randomNum);
        // datum[i] = randomNum;
        jsonReadings[keys[i]] = String(datum[i]);   // + " " + suf[i]
    }
    
    serializeJson(jsonReadings, readings);
    return readings;
}

void sendMessage () {
    updateMessages("sent", true);
    String msg = readingsToJSON(temperature, humidity, pressure, pm1_0float, pm2_5float, pm10_0float);
    mesh.sendBroadcast(msg);
}

// Periodic task to send a message
Task taskSendMessage(TASK_SECOND * 10 , TASK_FOREVER, &sendMessage);

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
    // Ignore messages from this node itself
    updateMessages("Recieved", true);
    // if (from == mesh.getNodeId()) {
    //     return;  // Ignore message
    // }

    Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
    
    // JSON input string.
    const char* json = msg.c_str();
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(jsonReadings, json);

    // Test if parsing succeeds.
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    Serial.print("Node: ");
    Serial.println(from);
    
    for (int i = 0; i < 6; i++) {    
        datum[i] = jsonReadings[keys[i]];
        Serial.print(keys[i]);
        Serial.print(": ");
        Serial.print(datum[i]);
        Serial.print(" ");
        Serial.println(suf[i]);
    }

    std::map<String, String> mini_map = jsonToMap(msg);
    String boardName = getBoardName(from);  // Get the proper board name
    Serial.println("From: " + boardName);
    updateSensorData(boardName, mini_map);

    if (boardType == "Both") {
        handleHTMLRoot;
        handleHTMLRoot2;
    }
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);

    // Assign a new board number if not already present
    String assignedBoard = getBoardName(nodeId);

    // Initialize sensor data for the new board
    std::map<String, String> emptyData;
    for (const auto& key : keys) {
        emptyData[key] = "N/A";  // Default values
    }
    updateSensorData(assignedBoard, emptyData);
}

void changedConnectionCallback() {
    Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void initializeMesh() {
    // Mesh network initialization
    mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
	// mesh.setDebugMsgTypes(ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE);  // all types on
    mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages
    
	// Set the mesh callbacks
	mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
	mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

    // Serial.println(mesh.getNodeId());
    // String jsonReadings = readingsToJSON();
    // receivedCallback(mesh.getNodeId(), jsonReadings);

}

void startTasks() {
    // Add the task to send messages periodically
    userScheduler.addTask(taskSendMessage);
    taskSendMessage.enable();
}

// Disconnect from mesh
void disconnectFromMesh() {
    mesh.stop();
}

void initializePMS() {
    // Initialize PMS7003 Serial on pins 21 (RX) and 22 (TX)
    pmsSerial.begin(9600, SERIAL_8N1, PMS_RX, PMS_TX);
    
    // Initialize the readings arrays to zero
    for (int i = 0; i < numReadings; i++) {
    readingsPM1_0[i] = 0;
    readingsPM2_5[i] = 0;
    readingsPM10_0[i] = 0;
  }
}

// Function to read data from PMS7003
bool readPMSData() {
  if (pmsSerial.available() >= 32) {
    uint8_t buffer[32];
    pmsSerial.readBytes(buffer, 32);

    // Check if the frame header is correct
    if (buffer[0] == 0x42 && buffer[1] == 0x4D) {
      pm1_0 = (buffer[10] << 8) | buffer[11];
      pm2_5 = (buffer[12] << 8) | buffer[13];
      pm10_0 = (buffer[14] << 8) | buffer[15];
      return true;
    }
  }
  return false;
}

// Function to calculate the average of an array
float calculateAverage(int readings[], int numReadings) {
  int sum = 0;
  for (int i = 0; i < numReadings; i++) {
    sum += readings[i];
  }
  return (float)sum / numReadings;
}

void PMSread() {
    // Check if it's time to take a new reading (every 1 second)
  if (millis() - lastReadTime >= 1000) {
    lastReadTime = millis(); // Update the last read time
    
    // Read data from the PMS7003
    if (readPMSData()) {
      // Store the readings
      readingsPM1_0[readIndex] = pm1_0;
      readingsPM2_5[readIndex] = pm2_5;
      readingsPM10_0[readIndex] = pm10_0;

      // Print current readings for debugging
      Serial.print("Reading PM1.0: "); Serial.println(pm1_0);
      Serial.print("Reading PM2.5: "); Serial.println(pm2_5);
      Serial.print("Reading PM10: "); Serial.println(pm10_0);

      // Move to the next position in the array
      readIndex++;

      // If we've reached the number of readings we want to average, calculate the average and send data to ThingSpeak
      if (readIndex >= numReadings) {
        // Calculate the averages
        float avgPM1_0 = calculateAverage(readingsPM1_0, numReadings);
        float avgPM2_5 = calculateAverage(readingsPM2_5, numReadings);
        float avgPM10_0 = calculateAverage(readingsPM10_0, numReadings);
        
        // Print the averages for debugging
        Serial.println("Averages:");
        Serial.print("Average PM1.0: "); Serial.println(avgPM1_0);
        Serial.print("Average PM2.5: "); Serial.println(avgPM2_5);
        Serial.print("Average PM10: "); Serial.println(avgPM10_0);
        
        // Send the averaged data to ThingSpeak
        // sendDataToThingSpeak(avgPM1_0, avgPM2_5, avgPM10_0);
        pm1_0float = float(pm1_0);
        pm2_5float = float(pm2_5);
        pm10_0float = float(pm10_0);


        // Reset the readIndex for the next batch of readings
        readIndex = 0;
      }
    }
  }
}

void setup() {
    delay(2500);
    Serial.begin(115200);
    Serial.println("\nStart");
    updateMessages("Start", false);

    initializeOLED();
    initializeFastLED();

    serialDelay(5);

    // updateSensorData(String("1129945228"), board1Data);
    // updateSensorData(String("1129948912"), board2Data);
    // updateSensorData(String("Board 1"), board1Data);
    // updateSensorData(String("Board 2"), board2Data);

    if (boardType == "WebServer") {
        connectAP();
        startServer();
        generateLinks();
    } else if (boardType == "Mesh") {
        initializeMesh();
        startTasks();
        initializePMS();
    } else if (boardType == "Both") {
        counter = 0;
        initializeMesh();
        // startTasks();
        
        connectAP();
        startServer();
        generateLinks();
    }
}

void loop() {
    if (boardType == "WebServer") {
        server.handleClient();
        // handleHTMLRoot();
        // handleHTMLRoot2();
        delay(5000);
    } else if (boardType == "Mesh") {
        mesh.update();
        PMSread();
        delay(100);
    } else if (boardType == "Both") {
        mesh.update();
        delay(100);
        counter += 1;
        if (counter == 25) {
            counter = 0;
            server.handleClient();
        }
        // for (int i = 0; i < 50; i++) {
        //     mesh.update();
        //     delay(100);
        //     if (i == 49) {
        //         server.handleClient();
        //     }
        // }
        
    }

}