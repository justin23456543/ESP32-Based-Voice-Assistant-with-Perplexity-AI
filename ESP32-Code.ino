#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>
#include <driver/i2s.h>
#include <SD.h>

// WiFi and WebSocket configuration
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";
const char* websocket_server = "your_colab_server_address";
const int websocket_port = 65080;
const char* websocket_path = "/";
const char* websocket_fingerprint = "your_colab_server_ssl_fingerprint";

// I2S configuration 
const i2s_port_t I2S_PORT = I2S_NUM_0;
const int SAMPLE_RATE = 16000;
const int BITS_PER_SAMPLE = 16;
const i2s_config_t i2s_config = {
  .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
  .sample_rate = SAMPLE_RATE,
  .bits_per_sample = BITS_PER_SAMPLE,
  .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
  .communication_format = I2S_COMM_FORMAT_I2S_MSB,
  .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
  .dma_buf_count = 8,
  .dma_buf_len = 1024,
  .use_apll = false,
  .tx_desc_auto_clear = false,
  .fixed_mclk = 0
};

// SD card configuration
const int SD_CS_PIN = 5;
const char* audio_filename = "/audio.wav";

// Button configuration
const int BUTTON_PIN = 4;
volatile bool is_recording = false;

// WebSocket client
WebSocketsClient webSocket;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize SD card
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Failed to initialize SD card!");
    return;
  }

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Configure I2S
  if (i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL) != ESP_OK) {
    Serial.println("Failed to install I2S driver!");
    return;
  }
  if (i2s_set_pin(I2S_PORT, NULL) != ESP_OK) {
    Serial.println("Failed to set I2S pins!");
    return;
  }

  // Set up button interrupt
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonInterrupt, CHANGE);

  // Connect to WebSocket server securely
  connectWebSocket();
}

void loop() {
  // Check WebSocket connection
  if (webSocket.isConnected()) {
    webSocket.loop();
  } else {
    Serial.println("WebSocket disconnected. Reconnecting...");
    connectWebSocket();
  }

  if (is_recording) {
    // Read audio data from I2S
    int16_t audio_buffer[1024];
    size_t bytes_read = 0;
    i2s_read(I2S_PORT, audio_buffer, sizeof(audio_buffer), &bytes_read, portMAX_DELAY);

    // Save audio data to SD card
    File audio_file = SD.open(audio_filename, FILE_APPEND);
    if (!audio_file) {
      Serial.println("Failed to open audio file!");
      return;
    }
    audio_file.write((uint8_t*)audio_buffer, bytes_read);
    audio_file.close();
  }
}

void buttonInterrupt() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    // Button pressed, start recording
    is_recording = true;
    SD.remove(audio_filename);
  } else {
    // Button released, stop recording and send audio file
    is_recording = false;
    File audio_file = SD.open(audio_filename, FILE_READ);
    if (!audio_file) {
      Serial.println("Failed to open audio file!");
      return;
    }
    size_t audio_size = audio_file.size();
    uint8_t* audio_data = new uint8_t[audio_size];
    audio_file.read(audio_data, audio_size);
    audio_file.close();
    
    if (webSocket.isConnected()) {
      webSocket.sendBIN(audio_data, audio_size);
    } else {
      Serial.println("WebSocket not connected. Skipping audio send.");
    }
    
    delete[] audio_data;
  }
}

void connectWebSocket() {
  while (!webSocket.connect(websocket_server, websocket_port, websocket_path, websocket_fingerprint)) {
    Serial.println("Failed to connect to WebSocket server. Retrying...");
    delay(5000);
  }
  Serial.println("Connected to WebSocket server");
}
