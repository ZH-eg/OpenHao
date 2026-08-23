#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "35";
const char* password = "sz88888zlf";

#define UART_BAUD_RATE 9600
#define REQUEST_CMD "GET_DATA\n"
#define RX_BUFFER_SIZE 64

float currentTemp = 0.0;
float currentHum = 0.0;
String dataStatus = "等待数据...";

ESP8266WebServer server(80);

char rxBuffer[RX_BUFFER_SIZE];
int rxIndex = 0;

void processLine(char *line){
  int len = strlen(line);
  if(len > 0 && line[len - 1] == '\r') {
    line[len - 1] = '\0';
    len--;
  }

  char *tPtr = strstr(line, "T:");
  char *hPtr = strstr(line, ",H:");

  if(tPtr != NULL && hPtr != NULL){
    char tempStr[16] = {0};
    int tempStart = (int)(tPtr - line) + 2;
    int tempLen = (int)(hPtr - tPtr) - 2;
    if(tempLen > 0 && tempLen < 16){
      strncpy(tempStr, line + tempStart, tempLen);
      tempStr[tempLen] = '\0';
    }
    char humStr[16] = {0};
    int humStart = (int)(hPtr - line) + 3;
    int humLen = len - humStart;
    if(humLen > 0 && humLen < 16){
      strncpy(humStr, line + humStart, humLen);
      humStr[humLen] = '\0';
    }
    currentTemp = atof(tempStr);
    currentHum = atof(humStr);

    if(currentTemp >= -10.0 && currentTemp <= 80 && currentHum >= 0.0 && currentHum <= 100.0){
      dataStatus = "数据接收成功";
      Serial.printf("Parsed -> Temp :%.1f C, Hum :%.1f %%\n", currentTemp, currentHum);
    }else{
      dataStatus = "数据超出范围";
      Serial.println("Received data out of range");
    }
  }else{
    dataStatus = "数据格式错误";
    Serial.println("Invalid data format received: " + String(line));
  }
}

void handleRoot() {
  // 构建 HTML 页面
  String html = "<!DOCTYPE html><html>";
  html += "<head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<title>ESP8266 监控中心</title>";
  
  // CSS 样式部分
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; text-align: center; background-color: #f4f4f9; padding-top: 50px; }";
  html += "h1 { color: #333; }";
  html += ".card { background: white; padding: 20px; border-radius: 10px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); display: inline-block; margin: 10px; min-width: 200px; }";
  html += ".value { font-size: 2.5em; color: #007BFF; font-weight: bold; }";
  html += ".unit { font-size: 0.5em; color: #666; }";
  html += "</style>"; // 样式在这里结束
  
  html += "</head>";
  html += "<body>"; // body 在这里开始
  
  html += "<h1>ESP8266 传感器监控</h1>";
  html += "<p>状态: " + dataStatus + "</p>";
  
  // 温度卡片
  html += "<div class='card'>";
  html += "<h2>温度</h2>";
  html += "<div class='value'>" + String(currentTemp, 1) + "<span class='unit'>°C</span></div>";
  html += "</div>";
  
  // 湿度卡片
  html += "<div class='card'>";
  html += "<h2>湿度</h2>";
  html += "<div class='value'>" + String(currentHum, 1) + "<span class='unit'>%</span></div>";
  html += "</div>";
  
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(UART_BAUD_RATE);
  Serial.println("Serial Initialized");

  while(Serial.available()) {
    Serial.read(); // 清空串口缓冲区
  }
  rxIndex = 0;

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected successfully!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();

  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {
      // 收到换行符，说明一行数据完整
      if (rxIndex > 0) {
        rxBuffer[rxIndex] = '\0';
        processLine(rxBuffer);
        rxIndex = 0;
      }
    } else if (c == '\r') {
      // 忽略回车符
      continue;
    } else {
      // 正常字符，存入缓冲区
      if (rxIndex < RX_BUFFER_SIZE - 1) {
        rxBuffer[rxIndex++] = c;
      } else {
        // 缓冲区溢出保护
        rxIndex = 0;
        Serial.println("Buffer overflow");
      }
    }
  }
  delay(100);
}


