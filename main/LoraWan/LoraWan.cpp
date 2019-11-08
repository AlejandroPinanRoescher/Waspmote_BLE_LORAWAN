/*! \file LoraWan.cpp
    \brief Library for managing the LoRaWAN module 
    \date 05/11/2018
    \author Alejandro Piñan Roescher
*/

/******************************************************************************
 * Includes                                                                   *
 ******************************************************************************/

#ifndef __WPROGRAM_H__
#include <WaspClasses.h>
#endif

#include "LoraWan.h"
/******************************************************************************
 * PRIVATE FUNCTIONS                                                          *
 ******************************************************************************/


/******************************************************************************
 * PUBLIC FUNCTIONS                                                           *
 ******************************************************************************/

/*! class constructor
It does nothing
\param void
\return void
*/
LoraWan::LoraWan(){
}

/*! class Destructor
  It does nothing
  \param void
  \return void
*/
LoraWan::~LoraWan(){
}

/*! \fn uint8_t turnOnModule(uint8_t socket)
    \brief Turn on the LoRaWAN module.  
    \param[in]  socket  Waspmote socket to connect the LoRaWAN module, can be Socket0-->0 or Socket1-->1.    
    \retval response:  '0' if OK
                       '1' if error 
                       '2' if no answer 
    This function allows to switch on the LoRaWan module, it opens the MCU UART for communicating with the module 
    and it automatically enters into command mode.
*/
uint8_t LoraWan::turnOnModule(uint8_t socket){
  
    uint8_t response;
    response = LoRaWAN.ON(socket);
    if(response == 0){
        USB.println(F("LoRaWAN module switch on Ok "));  
    }else{
        USB.print(F("LoRaWAN turnOnModule(),ERROR = "));
        USB.println(response, DEC);
    }
    return response;
}

/*! \fn void turnOffModule()
    \brief Switches off the multiplexer on UART1. 
    \param   
    \retval 
    
    This function turn off the mux1(Socket1 connect to mux1), and disconnect the BLE module from the socket1.
    This is because that sometimes when we put module off, it resets. This solve the problem for me. 
*/
void LoraWan::turnOffModule(){
    Utils.muxOFF1();
    USB.println(F("LoRaWAN module switch off ")); 
}

/*! \fn void turnOffModule2()
    \brief  Switch off the LoRaWAN module  
    \param  socket the socket that it is being used,can be Socket0-->0 or Socket1-->1. 
    \retval uint8_t respuesta:
                              '0' if OK
                              others error
    
    This function switch off the LoRaWAN module and close the UART 
    * (Error:Sometimes when we put module off using this function, it resets) 
*/
uint8_t LoraWan::turnOffModule2(uint8_t socket){
    uint8_t response;
    response = LoRaWAN.OFF(socket);
    if(response == 0){
       USB.println(F("LoRaWAN module switch off ok"));  
    }else{
        USB.print(F("LoRaWAN module switch ERROR = "));  
        USB.println(response, DEC);  
    }
    return response; 
}

/*! \fn void setAdaptativeDataRate(char *onOff)
    \brief Enable or disable the adaptive data rate 
    \param  char* onOff state: "on"/"off"  enable or disable the adaptive data rate       
    \retval response:
                      '0' if OK
                      '1' if error 
                      '2' if no answer 
                      '7' if input parameter error 
    
    This function enable or disable the adaptive data rate. If is enabled, the server will optimize
    the data rate and the transmission power based on the information collected from the network.
    
*/ 
uint8_t LoraWan::setAdaptativeDataRate(char *onOff){
    uint8_t response;
    response = LoRaWAN.setADR(onOff);
    if( response == 0 ){
        USB.println(F("LoRaWAN module Adaptive Data Rate OK "));    
        USB.print(F("  -ADR:"));
        USB.println(LoRaWAN._adr, DEC);   
    }else{
        USB.print(F("LoRaWAN moduleAdaptive Data Rate, ERROR = ")); 
        USB.println(response, DEC);
    }
    return response;
}

/*! \fn void setChannelFrequency( uint8_t channel, uint32_t frequency)
    \brief Set channel frecuency for the specific channel
    \param  channel  The chanel to be set [3..15](Channels 0,1,2 are fixed in frequency)    
    \param frecuency The frequency to be set [863250000..869750000]
                                             
    \retval  response
                      '0' if OK
                      '1' if error 
                      '2' if no answer 
                      '7' if input parameter error
                      '8' if module does not support function
    
    This function set the specific frecuency at the specific channel.
*/ 
uint8_t LoraWan::setChannelFrequency( uint8_t channel, uint32_t frequency){
    uint8_t response;
    response = LoRaWAN.setChannelFreq(channel, frequency);
    if( response == 0 ) {// Check status
      USB.print(F("LoRaWAN module frequency set OK ")); 
      USB.print(F("Frequency: "));
      USB.print(LoRaWAN._freq[channel]);   
      USB.print(F(" for channel: "));
      USB.println(channel, DEC);
    }else{
      USB.print(F("LoRaWAN module frequency set, ERROR = ")); 
      USB.println(response, DEC);
    }
    return response;
}

/*! \fn void setChannelDataRateRange(uint8_t channel, uint8_t drMin, uint8_t drMax)
    \brief Set the channel data rate range.
    \param  channel  channel to be set [0..15]     
    \param  drMin    Min data rate to set --> minDR [0..5] 	               
    \param  drMax    Max data rate to set --> maxDR [0..5] 
    \retval response
                      '0' if OK
                      '1' if error 
                      '2' if no answer
                      '7' if input parameter error
                      '8' unrecognized module
    
    This function sets the data rate range on the specific channel.
*/
uint8_t LoraWan::setChannelDataRateRange(uint8_t channel, uint8_t drMin, uint8_t drMax){
    uint8_t response;
    response = LoRaWAN.setChannelDRRange(channel, drMin, drMax);
    if( response == 0 ){
        USB.println(F("LoRaWAN module Data Rate range set OK "));    
        USB.print(F("  -Data Rate min:"));
        USB.println(LoRaWAN._drrMin[channel], DEC); 
        USB.print(F("  -Data Rate max:"));
        USB.println(LoRaWAN._drrMax[channel], DEC);
    }else{
        USB.print(F("LoRaWAN module Data rate range set, ERROR = ")); 
        USB.println(response, DEC);
    }
    return response;
}

/*! \fn void setChannelDutyCycle( uint8_t channel, uint16_t dutyCycle)
    \brief Set the channel duty cycle
    \param  channel   The channel to set the duty cycle     
    \param  dutyCycle The dcycle value to set --> duty cycle X (in percentage) using the following formula: dcycle = (100/X) – 1    
    \retval  response
                      '0' if OK
                      '1' if error 
                      '2' if no answer
                      '7' if input channel parameter error
                      '8' module does not support function 
    
    This function sets the duty cycle for the specific channel
*/
uint8_t LoraWan::setChannelDutyCycle( uint8_t channel, uint16_t dutyCycle){
    uint8_t response;
    response = LoRaWAN.setChannelDutyCycle(channel, dutyCycle);
    if( response == 0 ){
        USB.println(F("LoRaWAN module Duty Cycle OK. "));    
        USB.print(F("Duty Cycle:"));
        USB.println(LoRaWAN._dCycle[channel], DEC);
    }else {
        USB.print(F("LoRaWAN module Duty cycle set, ERROR = ")); 
        USB.println(response, DEC);
    }
    return response;
}

/*! \fn void enableOrDisableChannel(uint8_t channel, char *onOff)
    \brief Enable Or disable channel
    \param  channel  The channel to enable/disable     
    \param  *onOff   The enable("on") disable("off") options    
    \retval  response
                      '0' if OK
                      '1' if error 
                      '2' if no answer
                      '7' if input parameter error
                      '8' unrecognized module
    
    This function enable or disable the specific channel 
*/
uint8_t LoraWan::enableOrDisableChannel(uint8_t channel, char *onOff){
    uint8_t response;
    response = LoRaWAN.setChannelStatus(channel, onOff);
    if( response == 0 ){
      USB.println(F("LoRaWAN module Channel status set OK"));     
    }else {
      USB.print(F("LoRaWAN module Channel status set, ERROR = ")); 
      USB.println(response, DEC);
    }
    return response;
}

/*! \fn uint8_t setTxPower(uint8_t power)
    \brief Set the transmition power fot the LoRaWAN module
    \param  power  The power level to be set [1..5] for EU:
                                                            1 -> 14 dBm
                                                            2 -> 11 dBm
                                                            3 -> 8 dBm
                                                            4 -> 5 dBm
                                                            5 -> 2 dBm     
    \retval module response: 
                            '0' if OK
                            '1' if error 
                            '2' if no answer 
                            '7' if input parameter error
                            '8' if unrecognized module 
                            
   This function is used to configure the LoRaWAN RF power level 
*/
uint8_t LoraWan::setTxPower(uint8_t power){
  uint8_t response;
  response = LoRaWAN.setPower(power);
  if( response == 0 ){
    USB.println(F("LoRaWAN module Power level set OK"));     
  }else{
    USB.print(F("LoRaWAN module Power level set, ERROR = ")); 
    USB.println(response, DEC);
  }
  return response;
}

/*! \fn uint8_t getTxPower()
    \brief Get the transmition power that is set fot the LoRaWAN module
    \param       
    \retval module response: 
                            '0' if OK
                            '1' if error 
                            '2' if no answer 
                                
    This funtion get the transmition power that is set fot the LoRaWAN module. 
    The power setting cannot be saved in the module's non-volatile memory.
    
*/
uint8_t LoraWan::getTxPower(){
  uint8_t response;
  response = LoRaWAN.getPower();
  if( response == 0 ){
    USB.println(F("LoRaWAN module Power level get OK"));    
    USB.print(F("  -Power index:"));
    USB.println(LoRaWAN._powerIndex, DEC);
  }else{
    USB.print(F("LoRaWAN module Power level get, ERROR = ")); 
    USB.println(response, DEC);
  }
  return response;  
}

/*! \fn void printChannelsStatus()
    \brief Print the module channels status
    \param         
    \retval  
    
    This function print the module channels status for debug
*/ 
void LoraWan::printChannelsStatus(){
    USB.println(F("\n----------------------------"));
    USB.println(F("LoRaWAN module channels status: "));
    for( int Channel=0; Channel<16; Channel++){
        LoRaWAN.getChannelFreq(Channel);
        LoRaWAN.getChannelDutyCycle(Channel);
        LoRaWAN.getChannelDRRange(Channel);
        LoRaWAN.getChannelStatus(Channel);
        USB.print(F("Channel: "));
        USB.println(Channel);
        USB.print(F("  -Freq: "));
        USB.println(LoRaWAN._freq[Channel]);
        USB.print(F("  -Duty cycle: "));
        USB.println(LoRaWAN._dCycle[Channel]);
        USB.print(F("  -DR min: "));
        USB.println(LoRaWAN._drrMin[Channel], DEC);
        USB.print(F("  -DR max: "));
        USB.println(LoRaWAN._drrMax[Channel], DEC);
        USB.print(F("  -Status: "));
        if (LoRaWAN._status[Channel] == 1){
            USB.println(F("on"));
        }else{
            USB.println(F("off"));
        }
        USB.println(F("----------------------------"));
    }  
}  

/*! \fn void printDeviceAddr()
    \brief Print the Device Address for debug 
    \param  
    \retval 
    
    This function print the Device Address of the LoRaWAN module
*/
uint8_t LoraWan::printDeviceAddr(){
  uint8_t response;
  response = LoRaWAN.getDeviceAddr();
  if( response == 0 ){
    USB.println(F("LoRaWAN DeviceAddr = "));
    USB.println(LoRaWAN._devAddr);
  }else{
    USB.println(F("LoRaWAN DeviceAddr, ERROR = "));
    USB.println(response, DEC);
  } 
  return response; 
}

/*! \fn void configure2OTAA(char DEVICE_EUI[], char APP_EUI[], char APP_KEY [])
    \brief   Configure the LoRaWAN module to join OTAA.
    \param   DEVICE_EUI  64-bit hexadecimal number representing the device EUI.      
    \param   APP_EUI     64-bit hexadecimal number representing the application identifier.     
    \param   APP_KEY     128-bit hexadecimal number representing the application key.    
    \retval  void
    
    This function configure the LoRaWAN module to join OTTA. This is necessary before joining OTAA.
    The OTAA join procedure requires the module to be personalized with the following information 
    before its starts the join procedure:
      •Device EUI (64-bit)
      •Application EUI (64-bit)
      •Application Key (128-bit)
*/
void LoraWan::configure2OTAA(char DEVICE_EUI[], char APP_EUI[], char APP_KEY []){
    
    LoRaWAN.setDeviceEUI(DEVICE_EUI);
    LoRaWAN.setAppEUI(APP_EUI);
    LoRaWAN.setAppKey(APP_KEY);
}

/*! \fn configure2ABP(char DEVICE_EUI[], char DEVICE_ADDR[], char NWK_SESSION_KEY [], char APP_SESSION_KEY [])
    \brief   Configure the LoRaWAN module to join ABP
    \param   DEVICE_EUI       64-bit hexadecimal number representing the device EUI.     
    \param   DEVICE_ADDR      32-bit hexadecimal number representing the device address.      
    \param   NWK_SESSION_KEY  128-bit hexadecimal number representing the network session key.       
    \param   APP_SESSION_KEY  128-bit hexadecimal number representing the application session key.       
    \retval  void
    
    This function configure the LoRaWAN module to join ABP.
    The ABP join procedure requires the module to be personalized with the following information before its
    starts the join procedure:
      •Device address (32-bit) 
      •Network Session Key (128-bit key) ensures security on network level
      •Application Session Key (128-bit key) ensures end-to-end security on application level
*/
void LoraWan::configure2ABP(char DEVICE_EUI[], char DEVICE_ADDR[], char NWK_SESSION_KEY [], char APP_SESSION_KEY []){
    
    LoRaWAN.setDeviceEUI(DEVICE_EUI);
    LoRaWAN.setDeviceAddr(DEVICE_ADDR);
    LoRaWAN.setNwkSessionKey(NWK_SESSION_KEY);
    LoRaWAN.setAppSessionKey(APP_SESSION_KEY);
}

/*! \fn uint8_t joinOTAA()
    \brief Join the LoRaWAN network by OTAA.
    \param  
    \retval uint8_t response
                    '0' if OK
                    '1' if error 
                    '2' if no answer 
                    '3' if keys were not initiated  
    
    This function Join the LoRaWAN network by OTAA. First the module must by configure to OTAA.
*/
uint8_t LoraWan::joinOTAA(){
    uint8_t response;
    response = LoRaWAN.joinOTAA();
    if(response == 0){
        USB.println(F("LoRaWAN module join the network by OTAA OK"));  
    }else{
        USB.print(F("LoRaWAN module join OTAA, ERROR = "));
        USB.println(response, DEC);    
    }
    return response;
}

/*! \fn uint8_t joinABP()
    \brief Join the LoRaWAN network by ABP
    \param  
    \retval uint8_t response 
                  '0' if OK
                  '1' if error 
                  '2' if no answer 
                  '3' if keys were not initiated   
    
    This function Join the LoRaWAN network by ABP. First the module must by configure to ABP.
*/
uint8_t LoraWan::joinABP(){
    uint8_t response;
    response = LoRaWAN.joinABP();
    if(response == 0){
        USB.println(F("LoRaWAN module join the network by ABP OK"));  
    }else{
        USB.print(F("LoRaWAN module join ABP, ERROR = "));  
        USB.println(response, DEC);  
    }
    return response;
}

/*! \fn uint8_t setRetries(uint8_t retries)
    \brief Set the number of retransmisions for an uplink confirmed packet
    \param retries The number of retransmissions, [0..255], for an uplink.     
    \retval response The module response: 
                                        '0' if OK
                                        '1' if error 
                                        '2' if no answer 
         
    This function is used to configure number of retransmisions for an uplink confirmed packet.
    
*/
uint8_t LoraWan::setRetries(uint8_t retries){
  uint8_t response;
  response = LoRaWAN.setRetries(retries);
  if( response == 0 ) {
    USB.println(F("LoRaWAN module Set Retransmissions for uplink confirmed packet OK"));     
  }else{
    USB.print(F("LoRaWAN module Set Retransmissions for uplink confirmed packet, ERROR = ")); 
    USB.println(response, DEC);
  }
  return response;
}

/*! \fn uint8_t getRetries()
    \brief Get the number of retransmision for uplink.
    \param       
    \retval response The module response: 
                                        '0' if OK
                                        '1' if error 
                                        '2' if no answer 
    This function get the number of retransmision for uplink.
*/
uint8_t LoraWan::getRetries(){
  uint8_t response;
  response = LoRaWAN.getRetries();
  if( response == 0 ) {
    USB.print(F("LoRaWAN module Get Retransmissions for uplink confirmed packet OK. ")); 
    USB.print(F("TX retries: "));
    USB.println(LoRaWAN._retries, DEC);
  }else{
    USB.print(F("LoRaWAN module Get Retransmissions for uplink confirmed packet, ERROR = ")); 
    USB.println(response, DEC);
  }
  return response;
}

/*! \fn uint8_t setAutomaticReply()
    \brief Set the automatic reply status fot the LoRaWAN module
    \param *onOff   The enable("on") disable("off") options      
    \retval module response: 
                            '0' if OK
                            '1' if error 
                            '2' if no answer 
                            '7' if input parameter error
                            
    This function is used to set the automatic reply status from module.
    This parameter cannot be stored in the module’s EEPROM using the saveConfig() function  
*/
uint8_t LoraWan::setAutomaticReply(char *onOff){
  uint8_t response;
  response = LoRaWAN.setAR(onOff);
  if( response == 0 ) {
    USB.println(F("LoRaWAN module Set automatic reply status on OK"));     
  }else {
    USB.print(F("LoRaWAN module Set automatic reply status on, ERROR = ")); 
    USB.println(response, DEC);
  }
  return response;
} 

/*! \fn uint8_t getAutomaticReply()
    \brief Get the Automatic Reply status
    \param       
    \retval module response: 
                            '0' if OK
                            '1' if error 
                            '2' if no answer 
                            '7' if input parameter error
                             
    This function is used to get the automatic reply status from module
*/
uint8_t LoraWan::getAutomaticReply(){ 
  uint8_t response;
  response = LoRaWAN.getAR();
  if( response == 0 ) {
    USB.print(F("LoRaWAN module Get automatic reply status OK. ")); 
    USB.print(F("LoRaWAN module Automatic reply status: "));
    if (LoRaWAN._ar == true){
      USB.println(F("on"));      
    }else{
      USB.println(F("off"));
    }
  }else{
    USB.print(F("LoRaWAN module Get automatic reply status, ERROR = ")); 
    USB.println(response, DEC);
  }
  return response;
}

/*! \fn void saveModuleConfig()
    \brief Save the LoRaWAN module config.
    \param       
    \retval  response:
                      '0' if OK
                      '1' if error 
                      '2' if no answer 
    
    This function save the LoRaWAN module config in the module’s non-volatile memory.
*/ 
uint8_t LoraWan::saveModuleConfig(){
    uint8_t response;
    response = LoRaWAN.saveConfig();
    if(response == 0){
       USB.println(F("LoRaWAN module saveConfig OK"));  
    }else{
        USB.print(F("LoRaWAN module saveConfig, ERROR = ")); 
        USB.println(response, DEC);
    }
    return response;
}

/*! \fn uint8_t setDataRateNextTransmision(uint8_t dataRate)
    \brief Set the data rate for the next transmision.  
    \param  uint8_t dataRate The data rate to be established.
    \retval uint8_t respuesta:
                              '0' if OK
                              '1' if error 
                              '2' if no answer 
                              '7' if input parameter error
                              '8' if unrecognized module 
    
    This function set the data rate to be used for the next transmission of the LoRaWAN module. 
    
*/
uint8_t LoraWan::setDataRateNextTransmision(uint8_t dataRate){
    uint8_t respuesta;
   respuesta = LoRaWAN.setDataRate(dataRate); 
    if(respuesta == 0){
        USB.println(F("LoRaWAN module Data Rate OK"));  
    }else{
        USB.println(F("LoRaWAN module Data Rate ERROR = "));
        USB.println(respuesta, DEC);
    }
    return respuesta;
}

/*! \fn uint8_t sendUnconfirmedData(uint8_t port, uint8_t *data, uint8_t len)
    \brief   This function sends a LoRaWAN packet without ACK
    \param   port   The port number to send data     
    \param   *data  The pointer to the data to send      
    \param   len    The len of the data to send         
    \retval  1:It Is data response from gateway
             0:Not data response from gateway
    
    This function send unconfirmed data to the LoRaWAN network, and will not expect any acknowledgement back from the server.
*/
uint8_t LoraWan::sendUnconfirmedData(uint8_t port, uint8_t *data, uint8_t len){
    uint8_t response;
    response = LoRaWAN.sendUnconfirmed( port, data, len);
    if( response == 0 ) {
        USB.println(F("LoRaWAN module Send Unconfirmed packet OK"));     
        if (LoRaWAN._dataReceived == true){ 
          return 1;
        }
    }else{
        USB.print(F("LoRaWAN module Send Unconfirmed packet ERROR = ")); 
        USB.println(response, DEC);
        //~ '0' if OK
        //~ '1' if error 
        //~ '2' if no answer 
        //~ '4' if data length error
        //~ '5' if error when sending data
        //~ '6' if module hasn't joined to a network
        //~ '7' if input port parameter error
    }
    return 0;
}

/*! \fn uint8_t sendConfirmedData(uint8_t port, uint8_t *data, uint8_t len)
    \brief   This function sends a LoRaWAN packet and waits for ACK
    \param   port   The port used to send data      
    \param   *data  The pointer to the data to send      
    \param   len    The len of the data to send     
    \retval 1:Is data response from gateway
            0:Not data response from gateway
    
    This function send confirmed data to the LoRaWAN network, and will expect acknowledgement back from the server.
*/
uint8_t LoraWan::sendConfirmedData(uint8_t port, uint8_t *data, uint8_t len){
    uint8_t response;    
    response = LoRaWAN.sendConfirmed( port, data, len);
    if( response == 0 ) {
        USB.println(F("LoRaWAN module Send Confirmed packet OK"));     
        if (LoRaWAN._dataReceived == true){ 
          return 1;
        }
    }else{
        USB.print(F("LoRaWAN module Send Confirmed packet error = ")); 
        USB.println(response, DEC);
        //~ '0' if OK
        //~ '1' if error 
        //~ '2' if no answer 
        //~ '4' if data length error
        //~ '5' if error when sending data
        //~ '6' if module hasn't joined to a network
        //~ '7' if input port parameter error
    }
    return 0;
} 

/*! \fn char* receiveDowlinkData()
    \brief After a uplink if the back-end send data this fuction recibe the dowlink data from the network.
    \param       
    \retval char* LoRaWAN._data Pointer to the recibe data buffer 
    
    This function recibe the data from the LoRaWAN network,
    but the LoRaWAN module is a class A device and to receive dowlink it can only do it after an uplink.
*/ 
char* LoraWan::receiveDowlinkData(){
    
    USB.print(F("LoRaWAN module there's data on port number "));
    USB.print(LoRaWAN._port,DEC);
    USB.print(F(".\r\n   Data: "));
    USB.println(LoRaWAN._data);
    return LoRaWAN._data;
}    

/*! \fn setBatteryLevelStatus()
 * @brief	Set the battery level for status answer frame
 * 
 * @return:  '0' if OK:	
 * 		       '1' if error  
 * 	         '2' if no answer 
 * 
 *  This function is used to set the battery level required for
 *  Device Status Answer frame in use with the LoRaWAN Class A protocol.
 */ 
uint8_t LoraWan::setBatteryLevelStatus(){
  uint8_t response;
  response = LoRaWAN.setBatteryLevel();
  if( response == 0 ){
    USB.println(F("LoRaWAN module BatteryLevelStatus set OK. "));    
  }else{
    USB.print(F("LoRaWAN module BatteryLevelStatus set, ERROR = ")); 
    USB.println(response, DEC);
  }
  return response;  
}

/*! \fn getUplinkCounter()
 * @brief	This function is used to get the value of the uplink frame counter 
 * 			that will be used for the next uplink transmission.
 * 
 * @return:  if OK:  _upCounter		
 * 		       else:'1' if error 
 * 	              '2' if no answer 
 */
uint32_t LoraWan::getUplinkCounter(){
  uint8_t response;
  response = LoRaWAN.getUpCounter();
  if(response == 0){
    return LoRaWAN._upCounter;
  }else{
    return response;
  }
}

/*! \fn getDownlinkCounter()
 * @brief	This function is used to get the value of the downlink frame counter 
 * 			  that will be used for the next downlink transmission.
 * 
 * @return:  if OK: _downCounter		
 * 		       else:'1' if error 
 * 	              '2' if no answer 
 *  
 */
uint32_t LoraWan::getDownlinkCounter(){
  uint8_t response;
  response = LoRaWAN.getDownCounter();
  if(response == 0){
    return LoRaWAN._downCounter;
  }else{
    return response;
  }
}

/*! \fn getGatewayNumber()
 * @brief	Get the numbers of the Gateway that received the uplink
 * 
 * @return:  if OK: _gwNumber		
 * 		       else:'1' if error  
 * 	              '2' if no answer 
 * 
 *  
 *  This function gets the number of gateways that successfully 
 * 	received the last Linck Check Request from the module
 */
uint8_t LoraWan::getGatewayNumber(){
  uint8_t response;
  response = LoRaWAN.getGatewayNumber();
  if(response == 0){
    return LoRaWAN._gwNumber;
  }else{
    return response;
  }
}

/*! \fn setDowlinkRX1Delay(uint16_t delay)
 * @brief	Set the specific delay for the dowlink RX1 
 * 
 * \param   delay   The delay to be set (0 to 65535) 
 * @return:  '0' if OK	
 * 		       '1' if error  
 * 	         '2' if no answer 
 * 
 *  
 * This function allows the user to set the delay between the transmission and the first reception window. 
 */
uint8_t LoraWan::setDowlinkRX1Delay(uint16_t delay){
  uint8_t response;
  response = LoRaWAN.setRX1Delay(delay);
  if(response == 0){
    return LoRaWAN._gwNumber;
  }else{
    return response;
  }
}

/*! \fn getRX1Delay() 
 * @brief	Get the first receive window delay
 * 
 * @return		
 * 	@arg	if OK:  _rx1Delay
 * 	     	 else: '1' if error 
 * 		           '2' if no answer 
 *  	           '7' if input parameter error
 * 
 * This function is used to get the first receive window delay
 */
uint8_t LoraWan::getDowlinkRX1Delay(){
  uint8_t response;
  response = LoRaWAN.getRX1Delay();
  if(response == 0){
    return LoRaWAN._rx1Delay;
  }else{
    return response;
  }
} 

/*! \fn setDowlinkRX2Parameters(uint8_t datarate, uint32_t frequency)
 * @brief	This function sets data rate and frequency used for the 
 * 			second receive window.
 * 
 * @remarks	The configuration of the receive window parameters should
 * 			be in concordance with the server configuration
 * 
 * @param	uint8_t datarate: datarate to be set [0..5]
 * 			uint32_t frequency: frequency to be set [863000000..870000000]
 * 													[433050000..434790000] 
 * 
 * @return	
 * 	@arg	'0' if OK
 * 	@arg	'1' if error 
 * 	@arg	'2' if no answer
 *	@arg	'7' if input parameter error
 *	@arg	'8' unrecognized module
 * 
 * 
 */
uint8_t LoraWan::setDowlinkRX2Parameters(uint8_t datarate, uint32_t frequency){
  uint8_t response;
  response = LoRaWAN.setRX2Parameters(datarate, frequency);
  if(response == 0){
    return LoRaWAN._gwNumber;
  }else{
    return response;
  }
}

/*! \fn getDowlinkRX2Delay()
 * @brief	This function is used to get the second receive window delay
 * 
 * @return		
 * 	@arg	'0' if OK
 * 	@arg	'1' if error 
 * 	@arg	'2' if no answer 
 *  @arg	'7' if input parameter error
 * 
 * The second receiving delay is set internally by the module calculated with the first window delay plus 1000 ms.
 */
uint8_t LoraWan::getDowlinkRX2Delay(){
  uint8_t response;
  response = LoRaWAN.getRX2Delay();
  if(response == 0){
    return LoRaWAN._rx2Delay;
  }else{
    return response;
  }
}  

/*! \fn getDowlinkRX2Parameters(char* band)
 * @brief	This function is used to get second receiving window 
 * 			parameters depending on the working band
 * 
 * @param	char* band: "868", "433" or "900"
 * 
 * @return		
 * 	@arg	'0' if OK
 * 	@arg	'1' if error 
 * 	@arg	'2' if no answer 
 *  @arg	'7' if input parameter error
 */
uint8_t LoraWan::getDowlinkRX2Parameters(char* band){
  uint8_t response;
  response = LoRaWAN.getRX2Parameters(band);
  if(response == 0){
    USB.print(F("LoRaWAN module Dowlink RX2 Parameters, Frequency = "));
    USB.print(LoRaWAN._rx2Frequency, DEC);
    USB.print(F(" ,data rate =  "));
    USB.println(LoRaWAN._rx2DataRate);
    //return LoRaWAN._rx2Frequency;
    //return LoRaWAN._rx2DataRate;
    return response;
  }else{
    return response;
  }
}
