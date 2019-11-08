/*! \file LoraWan.h
    \brief Library for managing the LoRaWAN module
    \date 05/11/2018
    \author Alejandro Piñan Roescher
*/

/*! \def _LORAWAN_h
    \brief The library flag
 */ 
#ifndef _LORAWAN_h
#define _LORAWAN_h

/******************************************************************************
 * Includes                                                                   *
 ******************************************************************************/
#include <WaspLoRaWAN.h>
#include <inttypes.h>

/******************************************************************************
 * Class                                                                      *
 ******************************************************************************/

//! LoraWan Class
/*!
  defines all the variables and functions used 
 */
class LoraWan{

/// private methods //////////////////////////
private:

   
/// public methods ////////////
public:

    LoraWan();
    
    ~LoraWan(); 
   
    uint8_t turnOnModule(uint8_t socket);

    void turnOffModule();

    uint8_t turnOffModule2(uint8_t socket);
  
    uint8_t setAdaptativeDataRate(char* onOff);
   
    uint8_t setChannelFrequency( uint8_t channel, uint32_t frequency);

    uint8_t setChannelDataRateRange(uint8_t channel, uint8_t drMin, uint8_t drMax);
    
    uint8_t setChannelDutyCycle( uint8_t channel, uint16_t dutyCycle);

    uint8_t enableOrDisableChannel(uint8_t channel, char* onOff);
    
    uint8_t setTxPower(uint8_t power);
    
    uint8_t getTxPower();
    
    void printChannelsStatus();
    
    uint8_t printDeviceAddr();
    
    void configure2OTAA(char DEVICE_EUI[], char APP_EUI[], char APP_KEY []);
  
    void configure2ABP(char DEVICE_EUI[], char DEVICE_ADDR[], char NWK_SESSION_KEY [], char APP_SESSION_KEY []);
  
    uint8_t joinOTAA();
    
    uint8_t joinABP();
    
    uint8_t setRetries(uint8_t retries);
    
    uint8_t getRetries();
    
    uint8_t setAutomaticReply(char *onOff);
    
    uint8_t getAutomaticReply();
    
    uint8_t saveModuleConfig();
    
    uint8_t setDataRateNextTransmision(uint8_t socket);

    uint8_t sendUnconfirmedData(uint8_t port, uint8_t *data, uint8_t len);    
   
    uint8_t sendConfirmedData(uint8_t port, uint8_t *data, uint8_t len); 
   
    char* receiveDowlinkData();
    
    uint8_t setBatteryLevelStatus();
    
    uint32_t getUplinkCounter();
    
    uint32_t getDownlinkCounter();
    
    uint8_t getGatewayNumber();
    
    uint8_t setDowlinkRX1Delay(uint16_t delay);
    
    uint8_t getDowlinkRX1Delay();
    
    uint8_t setDowlinkRX2Parameters(uint8_t datarate, uint32_t frequency);
    
    uint8_t getDowlinkRX2Delay();
    
    uint8_t getDowlinkRX2Parameters(char* band);
    

};

#endif 
