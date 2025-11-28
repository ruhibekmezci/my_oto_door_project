#include "mock_arduino.h"

// Forward declarations for functions in .ino
// Note: We need to match the signature in .ino
bool idKontrol(byte* okunanID);
void kapiyiAc();
void kapiyiKapat();
void ekranaYazdir();

// Include the .ino file
// We need to bypass the includes in .ino since we provide mocks via mock_arduino.h
// The .ino includes <Servo.h>, <MFRC522.h>, <SPI.h>
// We created empty files for them in tests/include, and included mock_arduino.h here.
// But we need to make sure the preprocessor finds them.
// Also, Servo, MFRC522, SPI classes are defined in mock_arduino.h
// So the empty headers should not redeclare them. They are just empty.

#include "../ruhi_ibrahim_arduino_devrelab1_project.ino"

void reset_globals() {
    // Reset global state for new test
    sonIslemZamani = 0;
    kapiAcik = false;
    _current_millis = 0;
    // rfid and motor are global objects, so we might need to reset them too if they have state.
    // motor has _angle.
    motor._angle = -1; // Unknown
    // rfid has _isNewCardPresent etc.
    rfid._isNewCardPresent = false;
    rfid._readCardSerialSuccess = false;
}

int main() {
    std::cout << "Running Tests..." << std::endl;

    // --- Test 1: Basic Authorized Access ---
    std::cout << "\n[Test 1] Authorized Access" << std::endl;
    reset_globals();
    setup();

    // Verify initial state
    if (kapiAcik) { std::cout << "FAIL: Door should be closed initially." << std::endl; return 1; }

    // Simulate Authorized Card
    // YETKILI_ID = {142, 111, 230, 63}
    rfid.uid.uidByte[0] = 142;
    rfid.uid.uidByte[1] = 111;
    rfid.uid.uidByte[2] = 230;
    rfid.uid.uidByte[3] = 63;
    rfid.uid.size = 4;
    rfid._isNewCardPresent = true;
    rfid._readCardSerialSuccess = true;

    // Run loop once
    loop();

    // Check if door opened
    if (!kapiAcik) { std::cout << "FAIL: Door did not open for authorized card." << std::endl; return 1; }
    if (motor._angle != 180) { std::cout << "FAIL: Servo angle is " << motor._angle << ", expected 180." << std::endl; return 1; }

    // Simulate time passing (2 seconds)
    _current_millis += 2000;
    rfid._isNewCardPresent = false; // Card removed or halted
    loop();

    // Door should still be open
    if (!kapiAcik) { std::cout << "FAIL: Door closed too early." << std::endl; return 1; }

    // Simulate more time (total 3001 ms)
    _current_millis += 1001;
    loop();

    // Door should be closed
    if (kapiAcik) { std::cout << "FAIL: Door did not close after timeout." << std::endl; return 1; }
    if (motor._angle != 0) { std::cout << "FAIL: Servo angle is " << motor._angle << ", expected 0." << std::endl; return 1; }

    std::cout << "PASS: Authorized Access Test" << std::endl;


    // --- Test 2: Unauthorized Access ---
    std::cout << "\n[Test 2] Unauthorized Access" << std::endl;
    reset_globals();
    setup();

    // Simulate Unauthorized Card
    rfid.uid.uidByte[0] = 0;
    rfid.uid.uidByte[1] = 0;
    rfid.uid.uidByte[2] = 0;
    rfid.uid.uidByte[3] = 0;
    rfid.uid.size = 4;
    rfid._isNewCardPresent = true;
    rfid._readCardSerialSuccess = true;

    loop();

    if (kapiAcik) { std::cout << "FAIL: Door opened for unauthorized card." << std::endl; return 1; }
    if (motor._angle != 0) { std::cout << "FAIL: Servo angle moved to " << motor._angle << "." << std::endl; return 1; }

    std::cout << "PASS: Unauthorized Access Test" << std::endl;


    // --- Test 3: 7-byte UID Security Flaw ---
    std::cout << "\n[Test 3] 7-byte UID Partial Match (Security Flaw)" << std::endl;
    reset_globals();
    setup();

    // Create a 7-byte UID where the first 4 bytes match the authorized ID
    // YETKILI_ID = {142, 111, 230, 63}
    rfid.uid.uidByte[0] = 142;
    rfid.uid.uidByte[1] = 111;
    rfid.uid.uidByte[2] = 230;
    rfid.uid.uidByte[3] = 63;
    rfid.uid.uidByte[4] = 99; // Extra byte
    rfid.uid.uidByte[5] = 88; // Extra byte
    rfid.uid.uidByte[6] = 77; // Extra byte
    rfid.uid.size = 7;        // 7-byte UID

    rfid._isNewCardPresent = true;
    rfid._readCardSerialSuccess = true;

    loop();

    // Expected behavior: Should NOT open because size is different.
    // Current behavior (Bug): Will open because it only checks first 4 bytes.

    if (kapiAcik) {
        std::cout << "FAIL (Expected): Door opened for 7-byte UID with matching prefix. This confirms the security bug." << std::endl;
        return 1; // We return 1 to indicate the bug exists (failure of the test case that expects security)
    } else {
        std::cout << "PASS: Door remained closed for 7-byte UID." << std::endl;
    }

    return 0;
}
