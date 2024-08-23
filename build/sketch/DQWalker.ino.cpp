#include <Arduino.h>
#line 1 "C:\\Users\\th3fr\\OneDrive\\Desktop\\Personal-Projects\\DQWalker\\DQWalker.ino"
#include <SparkFunLSM6DSO.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <BlockNot.h>
#include <FlashStorage.h>
#include <MemoryFree.h>
#include "Menus.h"
#include "Monster.h"
#include "Player.h"
#include <iterator>
#include <vector>

LSM6DSO imu;
uint32_t curr = HIGH;
Sharp_Color_LCD display(6);
BlockNot menuAnim(500);
BlockNot repeatTimer(2);
BlockNot gameTimer(30, SECONDS);
BlockNot afkTimer(60, SECONDS);
Player player(false);
Menus menuManager(&gameTimer);

#define BLACK 0
#define WHITE 1

#line 26 "C:\\Users\\th3fr\\OneDrive\\Desktop\\Personal-Projects\\DQWalker\\DQWalker.ino"
void setup();
#line 54 "C:\\Users\\th3fr\\OneDrive\\Desktop\\Personal-Projects\\DQWalker\\DQWalker.ino"
void loop();
#line 73 "C:\\Users\\th3fr\\OneDrive\\Desktop\\Personal-Projects\\DQWalker\\DQWalker.ino"
void leftButton();
#line 87 "C:\\Users\\th3fr\\OneDrive\\Desktop\\Personal-Projects\\DQWalker\\DQWalker.ino"
void confirmButton();
#line 102 "C:\\Users\\th3fr\\OneDrive\\Desktop\\Personal-Projects\\DQWalker\\DQWalker.ino"
void rightButton();
#line 116 "C:\\Users\\th3fr\\OneDrive\\Desktop\\Personal-Projects\\DQWalker\\DQWalker.ino"
void getStepCount();
#line 129 "C:\\Users\\th3fr\\OneDrive\\Desktop\\Personal-Projects\\DQWalker\\DQWalker.ino"
void initializeSensor();
#line 161 "C:\\Users\\th3fr\\OneDrive\\Desktop\\Personal-Projects\\DQWalker\\DQWalker.ino"
void readSensor();
#line 26 "C:\\Users\\th3fr\\OneDrive\\Desktop\\Personal-Projects\\DQWalker\\DQWalker.ino"
void setup()
{
    /*GCLK->GENDIV.reg = GCLK_GENDIV_DIV(12) |         // Divide the 48MHz clock source by divisor 12: 48MHz/12=4MHz
                     GCLK_GENDIV_ID(0);            // Select Generic Clock (GCLK) 0

    while (GCLK->STATUS.bit.SYNCBUSY);*/

    pinMode(13, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(3, INPUT_PULLUP);
    pinMode(2, INPUT_PULLUP);
    pinMode(1, INPUT_PULLUP);
    //pinMode(1, OUTPUT);

    //digitalWrite(1, HIGH);

    attachInterrupt(digitalPinToInterrupt(2), confirmButton, FALLING);
    attachInterrupt(digitalPinToInterrupt(3), leftButton, FALLING);
    attachInterrupt(digitalPinToInterrupt(1), rightButton, FALLING);

    initializeSensor();

    display.begin();
    display.setRotation(1);

    menuManager.changeMenu(front, 0, &display, &player);
}

void loop()
{
    if(menuAnim.triggered())
    {
        curr ^= HIGH;
        menuManager.updateAnim(&display, &player);
        digitalWrite(13, curr);
        readSensor();
    }

    if(menuManager.currMenu == game && gameTimer.triggered())
        menuManager.changeMenu(lb, 0, &display, &player);

    if(afkTimer.HAS_TRIGGERED)
    {
        //digitalWrite(1, LOW);
    }
}

void leftButton()
{
    if(afkTimer.triggered())
    {
        menuManager.changeMenu(front, 0, &display, &player);
        //digitalWrite(1, HIGH);
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
        //digitalWrite(1, HIGH);
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
        //digitalWrite(1, HIGH);
    }
    else
    {
        afkTimer.reset();
        menuManager.updateMenu(right, &display, &player);
    }
}

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
}

void initializeSensor()
{
    Serial.begin(115200);
    delay(2500);
    Serial.println("Testing");

    Wire.begin();
    delay(10);
    if( imu.begin() )
        Serial.println("Ready.");
    else { 
        Serial.println("Could not connect to IMU.");
        Serial.println("Freezing");
    }

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
    //Get all parameters
    Serial.print("\nAccelerometer:\n");
    Serial.print(" X = ");
    Serial.println(imu.readFloatAccelX(), 3);
    Serial.print(" Y = ");
    Serial.println(imu.readFloatAccelY(), 3);
    Serial.print(" Z = ");
    Serial.println(imu.readFloatAccelZ(), 3);

    Serial.print("\nSteps: ");
    Serial.printf("%d\n", player.lifetimeCount);
    Serial.print("\nFrom Register: ");
    imu.writeRegister(FUNC_CFG_ACCESS, 0x80);
    int16_t regOut;
    imu.readRegisterInt16(&regOut, STEP_COUNTER_L);
    imu.writeRegister(FUNC_CFG_ACCESS, 0x00);
    Serial.printf("%d\n", regOut);
    Serial.printf("%d\n", digitalRead(9));
    Serial.println(freeMemory());
}
