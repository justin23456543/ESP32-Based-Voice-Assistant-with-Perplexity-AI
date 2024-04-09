# ESP32-Voice-Assistant-with-Speech-to-Text-Perplexity-AI-and-Text-to-Speech
A dedicated, low-cost AI voice assistant based on the ESP32 microcontroller. This project leverages Google Colab's free computing services for speech-to-text and text-to-speech processing, and integrates with the Perplexity AI API for intelligent conversation and query handling.

# Features
- Offline wake word detection using the INMP441 I2S microphone (still in development, Currently push button)
- Records user queries and sends audio to Google Colab for speech-to-text
- Processes natural language queries using Perplexity AI's API
- Converts Perplexity's response back to speech using Google Colab
- Plays back the AI-generated voice response on the ESP32 using a MAX98357A I2S amplifier and speaker
- Designed for ease of assembly, using commonly available components and Dupont connectors for testing.

# This voice assistant is still in active development. Current efforts are focused on:
- Designing mobile power supply 
- Wake word detection accuracy
- Decreasing latency
- Adding photo upload and camera hardware

# Hardware
- ESP32 development board (e.g. ESP32-WROOM-32)
- INMP441 I2S digital microphone
- MAX98357A I2S digital audio amplifier
- 4 ohm, 3W speaker
- SPI MicroSD card module for audio storage
- Power Supply (Minimum 1.25 A @ 5 V)
- Dupont cables for connections

# Getting Started
- Connect the hardware components as shown in the wiring diagram (Coming soon)
- Install the ESP32 Arduino core and required libraries
- Configure your WiFi, Google Colab, and Perplexity API credentials
- Flash the firmware to your ESP32
-Power up the device and test it by holding the button and speaking a question
