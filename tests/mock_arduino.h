#ifndef MOCK_ARDUINO_H
#define MOCK_ARDUINO_H

#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <stdint.h>

// Types
typedef uint8_t byte;
typedef bool boolean;

// Constants
#define OUTPUT 1
#define INPUT 0
#define HIGH 1
#define LOW 0

// Mock Functions
unsigned long _current_millis = 0;
unsigned long millis() {
    return _current_millis;
}

void delay(unsigned long ms) {
    _current_millis += ms;
}

void pinMode(uint8_t pin, uint8_t mode) {}
void digitalWrite(uint8_t pin, uint8_t val) {}

// Serial Mock
class SerialMock {
public:
    void begin(unsigned long baud) {}
    void print(const std::string& s) { std::cout << s; }
    void print(const char* s) { std::cout << s; }
    void print(int n) { std::cout << n; }
    void println(const std::string& s) { std::cout << s << std::endl; }
    void println(const char* s) { std::cout << s << std::endl; }
    void println() { std::cout << std::endl; }
    // For F() macro which returns a special type, we can simulate it
};
SerialMock Serial;

class __FlashStringHelper;
#define F(string_literal) (reinterpret_cast<const __FlashStringHelper *>(PSTR(string_literal)))
#define PSTR(s) (s)

// Overload for Serial.print for F() result
void print(const __FlashStringHelper *ifsh) {
    std::cout << (const char*)ifsh;
}
// We need to add member functions to SerialMock to handle __FlashStringHelper*
// But easier: define F to just return const char* and ignore the type mismatch if possible,
// or overload Serial.print in the mock.
// Let's redefine F to simple string literal, and Serial.print accepts const char*.
#undef F
#define F(s) s


// Servo Mock
class Servo {
public:
    int _pin;
    int _angle;
    void attach(int pin) { _pin = pin; }
    void write(int angle) { _angle = angle; std::cout << "Servo on pin " << _pin << " set to " << angle << std::endl; }
};

// SPI Mock
class SPIMock {
public:
    void begin() {}
};
SPIMock SPI;

// MFRC522 Mock
struct Uid {
    byte uidByte[10];
    byte size;
    byte sak;
};

class MFRC522 {
public:
    Uid uid;
    int _ss, _rst;

    // Test Control Flags
    bool _isNewCardPresent = false;
    bool _readCardSerialSuccess = false;

    MFRC522(int ss, int rst) : _ss(ss), _rst(rst) {
        memset(uid.uidByte, 0, 10);
        uid.size = 0;
    }

    void PCD_Init() {}

    bool PICC_IsNewCardPresent() {
        return _isNewCardPresent;
    }

    bool PICC_ReadCardSerial() {
        return _readCardSerialSuccess;
    }

    void PICC_HaltA() {
        // In simulation, we might auto-reset presence?
        _isNewCardPresent = false;
    }

    void PCD_StopCrypto1() {}
};

#endif
