#include <ArduinoLowPower.h>
#include <SparkFunLSM6DSO.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <BlockNot.h>
#include <FlashStorage.h>
#include "Menus.h"
#include "Monster.h"
#include "Player.h"

// Flash Storage for saving user memory on device (WIP, maybe swap for a tiny ROM chip later?)
FlashStorage(firstLoad, bool);
FlashStorage(playerClass, Player);

uint32_t curr = HIGH; // Onboard LED toggle
BlockNot menuAnim(500); // Animation Cycle Timer
BlockNot repeatTimer(2); // Software Debounce Timer (WIP, only if necessary)
BlockNot gameTimer(30, SECONDS); // Game Timer
BlockNot afkTimer(60, SECONDS); // Auto Sleep Timer

LSM6DSO imu;
Sharp_Color_LCD display(7);
Player player;
Menus menuManager(&gameTimer);

void setup()
{
    /*GCLK->GENDIV.reg = GCLK_GENDIV_DIV(12) |         // Divide the 48MHz clock source by divisor 12: 48MHz/12=4MHz
                     GCLK_GENDIV_ID(0);            // Select Generic Clock (GCLK) 0

    while (GCLK->STATUS.bit.SYNCBUSY);*/

    // Read saved data on device boot.
    // If previous data exists, load it in. Otherwise create a new Player class and mark that we've created one.
    if(firstLoad.read() == false)
    {
        player = Player();
        playerClass.write(player);
        firstLoad.write(true);
    }
    else
        player = playerClass.read();

    // Initialize input and output pins

    pinMode(13, OUTPUT); // Onboard LED
    pinMode(7, OUTPUT); // Display CS
    pinMode(6, OUTPUT); // Display EN (HIGH Enable, LOW Disable)
    pinMode(2, INPUT_PULLUP); // Left Button
    pinMode(1, INPUT_PULLUP); // Center/Confirm Button
    pinMode(0, INPUT_PULLUP); // Right Button

    digitalWrite(6, HIGH); 

    // Attach buttons as wakeup interrupts, allowing them to wake the MCU during deep sleep
    LowPower.attachInterruptWakeup(digitalPinToInterrupt(1), confirmButton, FALLING);
    LowPower.attachInterruptWakeup(digitalPinToInterrupt(2), leftButton, FALLING);
    LowPower.attachInterruptWakeup(digitalPinToInterrupt(0), rightButton, FALLING);

    // Initialize accelerometer ICU
    initializeSensor();

    // Initialize display and rotate it if necessary (WIP, will see if needed/the exact rotation)
    display.begin();
    display.setRotation(1);

    // Set display to main menu
    menuManager.changeMenu(front, 0, &display, &player);
}

void loop()
{
    // Menu Animation Timer tasks
    if(menuAnim.triggered())
    {
        curr ^= HIGH;
        menuManager.updateAnim(&display, &player);
        digitalWrite(13, curr);
        readSensor();
    }

    // Game End Screen Timer tasks
    if(menuManager.currMenu == game && gameTimer.triggered())
        menuManager.changeMenu(lb, 0, &display, &player);

    // AFK (Idle) Timer tasks
    if(afkTimer.HAS_TRIGGERED)
    {
        digitalWrite(6, LOW);
        LowPower.deepSleep();
    }
}

// The three button callback functions!
// They all work the exact same way, they just provide different input values to the Menu class
// If we're in deep sleep mode, wake the display and change to the main menu
// If not, send a command to the Menu class
void leftButton()
{
    if(afkTimer.triggered())
    {
        menuManager.changeMenu(front, 0, &display, &player);
        digitalWrite(6, HIGH);
    }
    else
    {
        afkTimer.reset();
        menuManager.updateMenu(left, &display, &player);
    }
}

void confirmButton()
{

    if(afkTimer.triggered())
    {
        menuManager.changeMenu(front, 0, &display, &player);
        digitalWrite(6, HIGH);
    }
    else
    {
        afkTimer.reset();
        menuManager.updateMenu(confirm, &display, &player);
    }
}

void rightButton()
{
    if(afkTimer.triggered())
    {
        menuManager.changeMenu(front, 0, &display, &player);
        digitalWrite(6, HIGH);
    }
    else
    {
        afkTimer.reset();
        menuManager.updateMenu(right, &display, &player);
    }
}

// Function to read in the step count, add it to the lifetime total of the player, reset the counter, and save to storage
void getStepCount()
{
    imu.writeRegister(FUNC_CFG_ACCESS, 0x80);
    int16_t regOut;
    imu.readRegisterInt16(&regOut, STEP_COUNTER_L);
    uint8_t read8;
    imu.readRegister(&read8, EMB_FUNC_SRC);
    imu.writeRegister(EMB_FUNC_SRC, (read8|0x80));
    imu.writeRegister(FUNC_CFG_ACCESS, 0x00);
    player.lifetimeCount += regOut;
    player.updateEXP(regOut);
    playerClass.write(player);
}

void initializeSensor()
{
    // Initialize Serial comms for data reporting, will likely be removed in final product
    Serial.begin(115200);
    delay(2500);
    Serial.println("Testing");

    // Connect to the IMU over I2C
    Wire.begin();
    delay(10);
    if( imu.begin() )
        Serial.println("Ready.");
    else { 
        Serial.println("Could not connect to IMU.");
        Serial.println("Freezing");
    }

    // Register functions to initialize the accelerometer with certain settings and reset step counter
    // I think I got these from the LSM6DSO manual since the library doesn't work properly for me
    imu.writeRegister(FUNC_CFG_ACCESS, 0x80);
    imu.writeRegister(PAGE_RW, 0x40);
    imu.writeRegister(PAGE_SEL, 0x11);
    imu.writeRegister(PAGE_ADDRESS, 0x83);
    imu.writeRegister(PAGE_VALUE, 0x05);
    imu.writeRegister(PAGE_RW, 0x00);
    imu.writeRegister(EMB_FUNC_EN_A, 0x08);
    imu.writeRegister(EMB_FUNC_EN_B, 0x10);
    imu.writeRegister(EMB_FUNC_INT1, 0x08);
    uint8_t read8;
    imu.readRegister(&read8, EMB_FUNC_SRC);
    imu.writeRegister(EMB_FUNC_SRC, (read8|0x80));
    imu.writeRegister(FUNC_CFG_ACCESS, 0x00);
    imu.writeRegister(MD1_CFG, 0x02);
    imu.writeRegister(CTRL1_XL, 0x48);
}

void readSensor()
{
    // Read in raw accelerometry data
    Serial.print("\nAccelerometer:\n");
    Serial.print(" X = ");
    Serial.println(imu.readFloatAccelX(), 3);
    Serial.print(" Y = ");
    Serial.println(imu.readFloatAccelY(), 3);
    Serial.print(" Z = ");
    Serial.println(imu.readFloatAccelZ(), 3);

    // Read in step count
    Serial.print("\nSteps: ");
    Serial.printf("%d\n", player.lifetimeCount);
    Serial.print("\nFrom Register: ");
    imu.writeRegister(FUNC_CFG_ACCESS, 0x80);
    int16_t regOut;
    imu.readRegisterInt16(&regOut, STEP_COUNTER_L);
    imu.writeRegister(FUNC_CFG_ACCESS, 0x00);
    Serial.printf("%d\n", regOut);
}