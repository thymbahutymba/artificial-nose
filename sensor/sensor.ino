#include <SparkFunCCS811.h>

#define CCS811_ADDR 0x5B

#define RANGE (256)
#define BOTTOM_LIMIT (0 + RANGE)
#define UPPER_LIMIT (65536 - RANGE)

CCS811 sCCS811(CCS811_ADDR);
uint16_t rif_co2;
uint16_t rif_tVOC;

void setup() {
    Serial.begin(115200);
//    rif_co2 = (uint16_t)random(BOTTOM_LIMIT, UPPER_LIMIT);
//    rif_tVOC = (uint16_t)random(BOTTOM_LIMIT, UPPER_LIMIT);

    rif_co2 = 10000;
    rif_tVOC = 15000;

    /*
    CCS811Core::status returnCode = sCCS811.begin();

    float humidityVariable = (float)random(0, 10000) / 100;     // 0 to 100%
    float temperatureVariable = (float)random(500, 7000) / 100; // 5C to 70C
    sCCS811.setEnvironmentalData(humidityVariable, temperatureVariable);

    Serial.print("Begin status code: ");
    printDriverError(returnCode);
    Serial.println();
    */
}

void loop() {
    uint16_t co2 = rif_co2 + (uint16_t)random(0, RANGE);
    uint16_t tVOC = rif_tVOC + (uint16_t)random(0, RANGE);
    uint8_t s[] = {
        (uint8_t)co2, (uint8_t)(co2 >> 8),
        (uint8_t)tVOC, (uint8_t)(tVOC >> 8),
      };

    // arduino as random number generator
    Serial.write(s, 4);
    //Serial.print(10000);
    /*
    if (sCCS811.dataAvailable()) {
        sCCS811.readAlgorithmResults();
        co2 = sCCS811.getCO2();
        tvoc = sCCS811.getTVOC();
        printDebug(co2, tvoc);
    } else if (sCCS811.checkForStatusError()) {
        printSensorError();
    }
    */
    delay(1000);
}

void printDebug(uint16_t co2, uint16_t tvoc) {
    Serial.print("[");
    Serial.print(millis());
    Serial.print("] ");
    Serial.print("CO2: [");
    Serial.print(co2);
    Serial.print("] tVOC: [");
    Serial.print(tvoc);
    Serial.print("] ");
    Serial.println();
}

void printDriverError(CCS811Core::status errorCode) {
    switch (errorCode) {
    case CCS811Core::SENSOR_SUCCESS:
        Serial.println("SUCCESS");
        break;
    case CCS811Core::SENSOR_ID_ERROR:
        Serial.println("ID_ERROR");
        break;
    case CCS811Core::SENSOR_I2C_ERROR:
        Serial.println("I2C_ERROR");
        break;
    case CCS811Core::SENSOR_INTERNAL_ERROR:
        Serial.println("INTERNAL_ERROR");
        break;
    case CCS811Core::SENSOR_GENERIC_ERROR:
        Serial.println("GENERIC_ERROR");
        break;
    default:
        Serial.println("Unspecified error.");
    }
}

void printSensorError() {
    uint8_t error = sCCS811.getErrorRegister();

    if (error == 0xFF) {
        Serial.println("Failed to get ERROR_ID register.");
    } else {
        Serial.print("Error: ");
        if (error & 1 << 5)
            Serial.print("HeaterSupply");
        if (error & 1 << 4)
            Serial.print("HeaterFault");
        if (error & 1 << 3)
            Serial.print("MaxResistance");
        if (error & 1 << 2)
            Serial.print("MeasModeInvalid");
        if (error & 1 << 1)
            Serial.print("ReadRegInvalid");
        if (error & 1 << 0)
            Serial.print("MsgInvalid");
        Serial.println();
    }
}
