#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// WiFi credentials - CHANGE THESE
const char* ssid = "buleshthakur";      // Replace with your WiFi name
const char* password = "12345678";   // Replace with your WiFi password

// Create servo objects
Servo servo1;  // Pin 14
Servo servo2;  // Pin 27
Servo servo3;  // Pin 26
Servo servo4;  // Pin 33
Servo servo5;  // Pin 13

// Servo pin definitions
const int servoPin1 = 14;
const int servoPin2 = 27;
const int servoPin3 = 26;
const int servoPin4 = 33;
const int servoPin5 = 13;

// Current servo positions (0-180 degrees)
int pos1 = 90;
int pos2 = 90;
int pos3 = 90;
int pos4 = 90;
int pos5 = 90;

// Create web server on port 80
WebServer server(80);

// HTML webpage
const char webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Robotic Arm Control</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            margin: 0;
            padding: 20px;
            min-height: 100vh;
        }
        .container {
            max-width: 500px;
            margin: 0 auto;
            background: white;
            padding: 30px;
            border-radius: 15px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.3);
        }
        h1 {
            color: #333;
            margin-bottom: 30px;
        }
        .servo-control {
            margin: 25px 0;
            padding: 15px;
            background: #f5f5f5;
            border-radius: 10px;
        }
        .servo-label {
            font-weight: bold;
            color: #555;
            margin-bottom: 10px;
            font-size: 16px;
        }
        .slider-container {
            display: flex;
            align-items: center;
            gap: 15px;
            justify-content: center;
        }
        input[type=range] {
            width: 60%;
            height: 8px;
            border-radius: 5px;
            background: #ddd;
            outline: none;
            -webkit-appearance: none;
        }
        input[type=range]::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 25px;
            height: 25px;
            border-radius: 50%;
            background: #667eea;
            cursor: pointer;
        }
        input[type=range]::-moz-range-thumb {
            width: 25px;
            height: 25px;
            border-radius: 50%;
            background: #667eea;
            cursor: pointer;
            border: none;
        }
        .value-display {
            min-width: 40px;
            font-weight: bold;
            color: #667eea;
            font-size: 18px;
        }
        .preset-buttons {
            margin-top: 30px;
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 10px;
        }
        button {
            padding: 15px;
            font-size: 16px;
            border: none;
            border-radius: 8px;
            background: #667eea;
            color: white;
            cursor: pointer;
            font-weight: bold;
            transition: all 0.3s;
        }
        button:hover {
            background: #5568d3;
            transform: translateY(-2px);
            box-shadow: 0 5px 15px rgba(102,126,234,0.4);
        }
        button:active {
            transform: translateY(0);
        }
        .status {
            margin-top: 20px;
            padding: 10px;
            border-radius: 5px;
            background: #e8f5e9;
            color: #2e7d32;
            font-weight: bold;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1> Robotic Arm Control</h1>
        
        <div class="servo-control">
            <div class="servo-label">Servo 1 (Pin 14)</div>
            <div class="slider-container">
                <input type="range" min="0" max="180" value="90" id="servo1" oninput="updateServo(1, this.value)">
                <span class="value-display" id="value1">90°</span>
            </div>
        </div>

        <div class="servo-control">
            <div class="servo-label">Servo 2 (Pin 27)</div>
            <div class="slider-container">
                <input type="range" min="0" max="180" value="90" id="servo2" oninput="updateServo(2, this.value)">
                <span class="value-display" id="value2">90°</span>
            </div>
        </div>

        <div class="servo-control">
            <div class="servo-label">Servo 3 (Pin 26)</div>
            <div class="slider-container">
                <input type="range" min="0" max="180" value="90" id="servo3" oninput="updateServo(3, this.value)">
                <span class="value-display" id="value3">90°</span>
            </div>
        </div>

        <div class="servo-control">
            <div class="servo-label">Servo 4 (Pin 33)</div>
            <div class="slider-container">
                <input type="range" min="0" max="180" value="90" id="servo4" oninput="updateServo(4, this.value)">
                <span class="value-display" id="value4">90°</span>
            </div>
        </div>

        <div class="servo-control">
            <div class="servo-label">Servo 5 (Pin 13)</div>
            <div class="slider-container">
                <input type="range" min="0" max="180" value="90" id="servo5" oninput="updateServo(5, this.value)">
                <span class="value-display" id="value5">90°</span>
            </div>
        </div>

        <div class="preset-buttons">
            <button onclick="homePosition()">🏠 Home</button>
            <button onclick="resetAll()">🔄 Reset All</button>
            <button onclick="preset1()">📌 Preset 1</button>
            <button onclick="preset2()">📌 Preset 2</button>
        </div>

        <div class="status" id="status">Ready</div>
    </div>

    <script>
        function updateServo(servoNum, value) {
            document.getElementById('value' + servoNum).innerText = value + '°';
            
            fetch('/servo?num=' + servoNum + '&pos=' + value)
                .then(response => response.text())
                .then(data => {
                    document.getElementById('status').innerText = 'Servo ' + servoNum + ' → ' + value + '°';
                })
                .catch(error => {
                    document.getElementById('status').innerText = 'Error!';
                });
        }

        function homePosition() {
            setAllServos(90, 90, 90, 90, 90);
            document.getElementById('status').innerText = 'Home Position';
        }

        function resetAll() {
            setAllServos(90, 90, 90, 90, 90);
            document.getElementById('status').innerText = 'All Servos Reset';
        }

        function preset1() {
            setAllServos(45, 135, 60, 120, 90);
            document.getElementById('status').innerText = 'Preset 1 Applied';
        }

        function preset2() {
            setAllServos(135, 45, 120, 60, 180);
            document.getElementById('status').innerText = 'Preset 2 Applied';
        }

        function setAllServos(v1, v2, v3, v4, v5) {
            updateSlider(1, v1);
            updateSlider(2, v2);
            updateSlider(3, v3);
            updateSlider(4, v4);
            updateSlider(5, v5);
        }

        function updateSlider(num, val) {
            document.getElementById('servo' + num).value = val;
            document.getElementById('value' + num).innerText = val + '°';
            updateServo(num, val);
        }
    </script>
</body>
</html>
)=====";

// Handle root page
void handleRoot() {
    server.send(200, "text/html", webpage);
}

// Handle servo control
void handleServo() {
    if (server.hasArg("num") && server.hasArg("pos")) {
        int servoNum = server.arg("num").toInt();
        int position = server.arg("pos").toInt();
        
        // Constrain position to valid range
        position = constrain(position, 0, 180);
        
        // Move the appropriate servo
        switch(servoNum) {
            case 1:
                servo1.write(position);
                pos1 = position;
                break;
            case 2:
                servo2.write(position);
                pos2 = position;
                break;
            case 3:
                servo3.write(position);
                pos3 = position;
                break;
            case 4:
                servo4.write(position);
                pos4 = position;
                break;
            case 5:
                servo5.write(position);
                pos5 = position;
                break;
        }
        
        server.send(200, "text/plain", "OK");
    } else {
        server.send(400, "text/plain", "Bad Request");
    }
}

void setup() {
    // Start serial communication
    Serial.begin(115200);
    
    // Attach servos to pins
    servo1.attach(servoPin1);
    servo2.attach(servoPin2);
    servo3.attach(servoPin3);
    servo4.attach(servoPin4);
    servo5.attach(servoPin5);
    
    // Set servos to initial position
    servo1.write(pos1);
    servo2.write(pos2);
    servo3.write(pos3);
    servo4.write(pos4);
    servo5.write(pos5);
    
    delay(1000);
    
    // Connect to WiFi
    Serial.println();
    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);
    
    WiFi.begin(ssid, password);
    
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println();
    Serial.println("WiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    
    // Setup web server routes
    server.on("/", handleRoot);
    server.on("/servo", handleServo);
    
    // Start server
    server.begin();
    Serial.println("Web Server Started!");
    Serial.println("Open this IP in your browser:");
    Serial.println(WiFi.localIP());
}

void loop() {
    server.handleClient();
}
