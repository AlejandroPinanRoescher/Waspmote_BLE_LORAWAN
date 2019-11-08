/*! \file main.pde
    \brief main file
    \date 05/11/2018
    \author Alejandro Piñan Roescher
*/

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "BLECentral.h"
#include "LoraWan.h"
#include "Buffer.h"
#include "defines.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/******************************************************************************
 * Definitions & Declarations
 ******************************************************************************/
volatile stateEnum_t state;/*!< variable for the state machine*/ 
volatile uint8_t alarmFlag = 0;/*!< variable for the alarmInterruption*/ 
volatile uint8_t pcint8 = 0;/*!< variable for the PCINT8 ISR*/
//BLE MODULE 
char MAC[13] = "000b57a90aaf";/*!< MAC of the device to search */
//~ char DeviceToSearch = "Thunder Sense #02735";
//LoRAWAN MODULE                 
// Device parameters for Back-End registration
char DEVICE_EUI[]  = "00D994825A4CEA00";/*!< Device Identifier*/
//char APP_EUI[] = "70B3D57ED0015A9C";/*!< The Things Network APP identifier*/
char APP_EUI[] = "0000000000000000";/*!< Loraserver APP identifier*/
char APP_KEY[] = "170957426080C62A29819A7D656368E4";/*!< APP Key to The Things Network and loraserver*/
//To handle te alarm time, user can select the time and the minutes to receive the uplink data
uint8_t hours;
uint8_t minutes;
//Bit Map to select what sensors values will we send. In the corresponding position--> 1 value selected, 0  value not selected
uint8_t sensorsBitMap[12];//{NOT USED, UV_INDEX, PRESSURE, TEMPERATURE, AMBIENT_LIGHT, SOUND_LEVEL, HUMIDITY, BATTERY_LEVEL, ECO2, TVOC, HALL_STATE, FIELD_STRENGHT}
//frame to indicate the BLE disconnection
uint8_t BLE_Disconnected[2]= {0x01, 0x01};
//Objects to be used
BLECentral bleCentral = BLECentral();
LoraWan    lorawan    = LoraWan();
Buffer     buffer     = Buffer();

/******************************************************************************
 * Function prototyping
 ******************************************************************************/

/*! \fn void activarAlarma(uint8_t hora, uint8_t minutos)
    \brief   sets Alarm1 to the specified time
    \param   hours       Set the hours of the alarm
    \param   minutes     Set the minutes of the alarm
    \retval  void

    This function specifies the time for Alarm 1,in offset mode using RTC_ALM1_MODE4, sets alarm in RTC and enables interrupt.
 */
void activateAlarm(uint8_t hours, uint8_t minutes){
    uint8_t response;
    response = RTC.setAlarm1(0,hours,minutes,0,RTC_OFFSET,RTC_ALM1_MODE4);//RTC_OFFSET--> 'time' is added to the actual time read from RTC
    if(response == 0){
      USB.print(F("RTC Alarm 1 is set in OFFSET MODE, for hours = "));
      USB.print(hours, DEC);
      USB.print(F(" ,minutes = "));
      USB.println(minutes, DEC);
    }else{
      USB.print(F("RTC Alarm 1 error, Incorrect input parameters"));
    }
}

/*! \fn void alarmInterruption()
    \brief ISR to handle the waspmote Alarm
    \param  
    \retval 
    This function only changes the value of a flag, alarmFlag, to indicate that the alarm must be attended
*/
void alarmInterruption(){
     alarmFlag = 1;
}

/*! \fn void enableInterruptionPCINT8()
    \brief Enables the PCINT8 interrupt, which corresponds to the RX pin of socket 0 where the BLE module is connected.
    \param void
    \retval void
    
    PCMSK1 – Pin Change Mask Register 1:
    Each PCINT15:8-bit selects whether pin change interrupt is enabled on the corresponding I/O pin. 
    If PCINT15:8 isset and the PCIE1 bit in PCICR is set, pin change interrupt is enabled on the corresponding I/O pin.
    If PCINT15:8 is cleared, pin change interrupt on the corresponding I/O pin is disabled.
    
    PCIFR – Pin Change Interrupt Flag Register: 
    When a logic change on any PCINT15:8 pin triggers an interrupt request, PCIF1 becomes set (one). 
    If the I-bit inSREG and the PCIE1 bit in PCICR are set (one), the MCU will jump to the corresponding Interrupt Vector. 
    The flagis cleared when the interrupt routine is executed. Alternatively, the flag can be cleared by writing a logical one to it.
    
    PCICR – Pin Change Interrupt Control Register:
    When the PCIE1 bit is set (one) and the I-bit in the Status Register (SREG) is set (one), pin change interrupt 1 isenabled.
    Any change on any enabled PCINT15:8 pin will cause an interrupt.
    The corresponding interrupt of PinChange Interrupt Request is executed from the PCI1 Interrupt Vector. 
    PCINT15:8 pins are enabled individually bythe PCMSK1 Register.
    
*/
void enableInterruptionPCINT8(){
  USB.println(F("Enabled PCINT8 interruption to receive notifications from the BLE module"));
  PCMSK1 |= (1 << PCINT8);
  PCIFR  |= (1 << PCIF1);
  PCICR  |= (1 << PCIE1);
}

/*! \fn void disableInterruptionPCINT8()
    \brief Disables the PCINT8 interrupt, which corresponds to the RX pin of socket0 where the BLE module is connected.
    \param void
    \retval void
    
*/
void disableInterruptionPCINT8(){
        PCICR &= (0 << PCIE1);
}

/*! \fn ISR( PCINT1_vect)
    \brief ISR to handle the PCINT8(Pin PE0(RXD0/PCINT8/PDI)--> connected to Waspmote Socket0 (RXD0 BLE module))
    \param  
    \retval 
    
    Note: This ISR does not attend the interruption when the module wakes up for it, this is explained in the memory 
   
*/
ISR( PCINT1_vect){ 
    pcint8++;
}

/*! \fn void sleep()
    \brief turn waspmote in sleep mode(SLEEP_MODE_PWR_DOWN)
    \param  void
    \retval void
   
    This function turn waspmote in sleep mode(SLEEP_MODE_PWR_DOWN) 
*/
void sleep(){
    /* There are five different sleep modes in order of power saving:
      SLEEP_MODE_IDLE - the lowest power saving mode
      SLEEP_MODE_ADC
      SLEEP_MODE_PWR_SAVE
      SLEEP_MODE_STANDBY
      SLEEP_MODE_EXT_STANDBY
      SLEEP_MODE_PWR_DOWN - the highest power saving mode
    */
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_mode();//Put the device into sleep mode, taking care of setting the SE bit before, and clearing it afterwards 
}

/*! \fn void stateMachine()
    \brief different states of the BLE-LoraWAN node
    \param void 
    \retval void
    
    This function implement the sate machine of the BLE-LoraWAN node 
*/
void stateMachine(){
  
    uint8_t response = 0;
    
    switch(state){
      
        case BLE_SCANNING: 
            #if DEBUG >= 1
                USB.println(F("State: BLE_SCANNING"));
            #endif
            response = bleCentral.startScanningDevice(MAC);
            if(response){
                response = bleCentral.scanReport("Thunder Sense #02735");
                if (response){
                    state = BLE_CONNECT;	
                }
            }
            break;
            
        case BLE_CONNECT: 
            #if DEBUG >= 1
                USB.println(F("State: BLE_CONNECT"));
            #endif
            response = bleCentral.connect(MAC);
            if (response){
                state = DISCOVER_BLE_PROFILE;
                delay(1000);	
            }else{
                state = BLE_SCANNING;
            }
            break;
            
        case DISCOVER_BLE_PROFILE: 
            #if DEBUG >= 1
                USB.println(F("State: DISCOVER_BLE_PROFILE"));
            #endif
            response = bleCentral.discoverBLEProfile(); 
            if (response){
              state = ENABLE_BLE_NOTIFICATIONS;
            }else{
                state = BLE_SCANNING;
            }
            break;
            
        case ENABLE_BLE_NOTIFICATIONS: 
            #if DEBUG >= 1
                USB.println(F("State: ENABLE_BLE_NOTIFICATIONS"));
            #endif
            bleCentral.enableNotification(ServiceA_Characrteristic0_State_uuid);
            state = ENABLE_INTERRUPTIONS;
            break;
            
        case ENABLE_INTERRUPTIONS: 
            #if DEBUG >= 1
                USB.println(F("State: ENABLE_INTERRUPTIONS"));
            #endif
            activateAlarm(hours, minutes);
            enableInterruptionPCINT8();
            attachInterrupt(RTC_INT, alarmInterruption, 1);
            state = SLEEP;
            break;
 
        case SLEEP:
            #if DEBUG >= 1
                USB.println(F("State: SLEEP"));
            #endif
            alarmFlag = 0;
            sleep();
            USB.println(F("Waspmote wake up"));
            disableInterruptionPCINT8();
            state = WAKE_UP_AND_CKECK;
            break;
            
        case WAKE_UP_AND_CKECK:
            #if DEBUG >= 1
                USB.println(F("State: WAKE_UP_AND_CKECK"));
            #endif
            if(bleCentral.getConnectionStatus() != 1){//The BLE connection has been disconnected
                buffer.putDataToSend(BLE_Disconnected, BLE_DISCONNECT_TYPE);
            }else if( alarmFlag == 1){//Attend the Alarm, the established time has been met
                if(sensorsBitMap[1]==1)
                    buffer.putDataToSend(bleCentral.readAttribute(Service4_Characrteristic0_UV_Index_uuid), UV_INDEX_TYPE);
                if(sensorsBitMap[2]==1)
                    buffer.putDataToSend(bleCentral.readAttribute(Service4_Characrteristic1_Pressure_uuid), PRESSURE_TYPE);
                if(sensorsBitMap[3]==1)
                    buffer.putDataToSend(bleCentral.readAttribute(Service4_Characrteristic2_Temperature_uuid), TEMPERATURE_TYPE);
                if(sensorsBitMap[4]==1)
                    buffer.putDataToSend(bleCentral.readAttribute(Service4_Characrteristic4_Ambient_Light_uuid), AMBIENT_LIGHT_TYPE);
                if(sensorsBitMap[5]==1)
                    buffer.putDataToSend(bleCentral.readAttribute(Service4_Characrteristic5_Sound_Level_uuid), SOUND_LEVEL_TYPE);
                if(sensorsBitMap[6]==1)
                    buffer.putDataToSend(bleCentral.readAttribute(Service4_Characrteristic3_Humidity_uuid), HUMIDITY_TYPE);
                if(sensorsBitMap[7]==1)
                    buffer.putDataToSend(bleCentral.readAttribute(Service3_Characrteristic0_Battery_Level_uuid), BATTERY_LEVEL_TYPE);
                if(sensorsBitMap[8]==1)
                    buffer.putDataToSend(bleCentral.readAttribute(Service6_Characrteristic0_ECO2_uuid), ECO2_TYPE);
                if(sensorsBitMap[9]==1)
                    buffer.putDataToSend(bleCentral.readAttribute(Service6_Characrteristic1_TVOC_uuid), TVOC_TYPE);
                if(sensorsBitMap[10]==1)
                    buffer.putDataToSend(bleCentral.readAttribute(ServiceA_Characrteristic0_State_uuid), HALL_STATE_TYPE);
                if(sensorsBitMap[11]==1)
                    buffer.putDataToSend(bleCentral.readAttribute(ServiceA_Characrteristic1_Field_Strength_uuid), FIELD_STRENGHT_TYPE);
            }else{//Attend the Hall sensor notification 
                buffer.putDataToSend(bleCentral.receiveNotifications(), HALL_STATE_TYPE);
            }
            state = LORAWAN_SEND_UPLINK;
            break;

        case LORAWAN_SEND_UPLINK:
            #if DEBUG >= 1
                USB.println(F("State: LORAWAN_SEND_UPLINK"));
            #endif
            lorawan.turnOnModule(SOCKET1);
            lorawan.setAdaptativeDataRate("off");
            lorawan.setAutomaticReply("on");
            lorawan.joinABP();
            lorawan.enableOrDisableChannel(1, "off");
            lorawan.enableOrDisableChannel(2, "off");
            if(alarmFlag == 1) { 
              response = lorawan.sendUnconfirmedData(DATA_PORT, buffer.getDataToSend(), buffer.getDataToSendSize());
            }else{
              response = lorawan.sendConfirmedData(EVENT_PORT, buffer.getDataToSend(), buffer.getDataToSendSize());
            }
            if(response == 1){
              state = LORAWAN_RECEIVE_DOWNLINK;
            }else{
              state = ENABLE_INTERRUPTIONS;
            }
            buffer.clearDataToSend();
            lorawan.printChannelsStatus();
            lorawan.turnOffModule();
            USB.println(F(""));
            break;

        case LORAWAN_RECEIVE_DOWNLINK: 
            #if DEBUG >= 1
                USB.println(F("State: LORAWAN_RECEIVE_DOWNLINK"));
            #endif
            buffer.putNetworkReceivedData( lorawan.receiveDowlinkData());
            if(buffer.getNetworkReceivedData(0) == CONFIGURE_TIME_TYPE){
              hours = buffer.getNetworkReceivedData(1);
              minutes = buffer.getNetworkReceivedData(2);
              USB.print(F("Hours received: "));
              USB.println(hours,DEC);
              USB.print(F("Minutes received: "));
              USB.println(minutes,DEC);
            }else if(buffer.getNetworkReceivedData(0) == CONFIGURE_SELECTED_SENSORS_TYPE){
               USB.println(F("Selected sensors = "));
              for(uint8_t i = 0; i < 12; i++){
                sensorsBitMap[i] = buffer.getNetworkReceivedData(i);
                USB.print(sensorsBitMap[i],DEC);
                USB.print(F(":"));
              }
              USB.println(F(""));
            }
            buffer.clearNetworkReceivedData();
            state = ENABLE_INTERRUPTIONS;
            break;
    }
}
 
/*! \fn Setup()
    @brief  Initial configuration
    Global variables initialization
    LoRaWAN module: Configuration of the parameters, save Config, joins the network OTAA, and turns off the module
    BLE module: Turn On, and configure the BLE Scanner
 */
void setup(){
    USB.println(F("_______Starting setup"));
    USB.println(F(""));
    USB.println(F("_______Starting LoRaWAN module configuration"));
    state = BLE_SCANNING;//Initial state of the State Machine (BLE-LoRaWAN Node) 
    hours = 0;//Initial hour and minute, 00:02, to receive the sensors data
    minutes = 2;
    memset(sensorsBitMap, 0x01, sizeof(sensorsBitMap));//By default all sensors values to send
    lorawan.turnOnModule(SOCKET1);
    //Obliged because the nanoGateway, Lopy4, has a single channel(Waspmote has 0..15 channels, but only 0,1,2,are enabled by default) 
    lorawan.enableOrDisableChannel(1, "off");
    lorawan.enableOrDisableChannel(2, "off");
    lorawan.setChannelDataRateRange(0, 5, 5);//We use the channel 0 --->frec=868100000 and date rate=5-->sf=7(Lopy4, has a single data rate)
    lorawan.setRetries(2);//Number of retries for the send with confirmation(Used to send hall sensor events)
    //~ lorawan.setTxPower(4);
    lorawan.getTxPower();
    lorawan.setAdaptativeDataRate("off");//This parameter cannot be stored in the module’s EEPROM using the saveConfig() function
    lorawan.configure2OTAA(DEVICE_EUI, APP_EUI, APP_KEY);
    lorawan.saveModuleConfig();
    lorawan.joinOTAA();
    lorawan.turnOffModule();
    USB.println(F("_______LoRaWAN module configuration completed"));
    USB.println(F(""));
    USB.println(F("_______BLE module configuration"));
    bleCentral.turnOnModule(SOCKET0);
    bleCentral.configureScanner(BLE_GAP_DISCOVER_OBSERVATION, TX_POWER, SCAN_INTERVAL, SCAN_WINDOW, BLE_PASSIVE_SCANNING);
    USB.println(F("_______Finished setup"));
    USB.println(F(""));
}

/*! \fn loop()
 * @brief  while(true)
 */
void loop(){
  
    stateMachine();
}
