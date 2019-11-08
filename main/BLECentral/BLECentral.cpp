/*! \file BLECentral.cpp
    \brief Library for managing the Bluetooth low energy module BLE112 as a Central device
    \date 05/11/2018
    \author Alejandro Piñan Roescher
 */
 
 /******************************************************************************
 * Includes                                                                    *
 ******************************************************************************/
#ifndef __WPROGRAM_H__
    #include <WaspClasses.h>
#endif

#include <WaspBLE.h>
#include "defines.h"
#include "BLECentral.h"

/******************************************************************************
 * PUBLIC FUNCTIONS                                                           *
 ******************************************************************************/
 
/*! class constructor
It does nothing
\param void
\return void
*/
BLECentral::BLECentral(){
  
}

/*! class Destructor
  It does nothing
  \param void
  \return void
*/
BLECentral::~BLECentral(){
}

/*! \fn void turnOnModule(uint8_t socket)
    \brief It opens the UART and powers the module 
    \param[in] int8_t socket  Waspmote socket to connect the BLE module, can be Socket0-->0 or Socket1-->1.    
    \retval response The module response:
                                        '0' Is success
                                        '1' error reset
                                        '-1' error init

    This function opens the UART and powers the module in the corresponding socket 
*/
int8_t BLECentral::turnOnModule(uint8_t socket){
  int8_t response;
  response = BLE.ON(socket);
  if(response == 0){
    USB.println(F("BLE module switch on Ok "));
  }else{
     USB.print(F("BLE module switch, ERROR = "));
     USB.println(response, DEC);
  }
  return response;
}

/*! \fn void turnOffModule()
    \brief Turn off the BLE module  
    \param  
    \retval void
    
    This function closes the UART and switches off the module
*/
void BLECentral::turnOffModule(){
  BLE.OFF(); 
}

/*! \fn uint8_t bleAdvdataDecode(uint8_t type, uint8_t advdata_len, uint8_t *p_advdata, uint8_t *len, uint8_t *p_field_data)
    \brief Find the data given the type in advertising data.
    
    \param[in]  type          The type of field data.
    \param[in]  advdata_len   Length of advertising data.
    \param[in]  *p_advdata    The pointer of advertising data.
    \param[out] *len          The length of found data.
    \param[out] *p_field_data The pointer of buffer to store field data.
    
    \retval 0: Find the data
            1: Not find.

*/
uint8_t BLECentral::bleAdvdataDecode(uint8_t type, uint8_t advdata_len, uint8_t *p_advdata, uint8_t *len, uint8_t *p_field_data) {
    uint8_t index = 1;
    uint8_t field_length, field_type;

    while (index < advdata_len ) {
        field_length = p_advdata[index];
        field_type = p_advdata[index + 1];
        USB.print(F("      - AVD/SR data decoding -> ad_type: "));
        USB.print(field_type, HEX);
        USB.print(F(", length: "));
        USB.println(field_length, HEX);    
        if (field_type == type) {
            memcpy(p_field_data, &p_advdata[index + 2], (field_length - 1));
            *len = field_length - 1;
            return 0;
        }
        index += field_length + 1;
    }
    return 1;
}

/*! \fn uint8_t scanReport()
 *  \brief Report the Scan response.
    \param char *nameToSearch    Device adv_name to search
    \retval 0:Advertise name  Thunder Sense #02735 found
            1:Advertise name  Thunder Sense #02735 not found
   
    This function handle the report scan response and search for the Device adv_name.
*/
uint8_t BLECentral::scanReport(char *nameToSearch) {
    USB.println(F(""));
    USB.println(F("* BLE scan report: "));
    USB.print(F("   - Peer device address: "));
    Utils.hex2str(BLE.BLEDev.mac, device->mac, 6);
    USB.println(device->mac);
    USB.print(F("   - RSSI: "));
    USB.print(BLE.BLEDev.rssi, DEC);
    USB.println(F(" dBm "));
    USB.print(F("   - Advertising data packet("));
    USB.print(BLE.BLEDev.advData[0], DEC);
    USB.print(F(" Bytes): "));
    for (int index = 1; index < BLE.BLEDev.advData[0]; index++) {
        USB.print(BLE.BLEDev.advData[index], HEX);
        USB.print(F(" "));
    }
    USB.println(F(" "));
    
    uint8_t len;
    uint8_t adv_name[31];

    if (0x00 == bleAdvdataDecode(0x09, BLE.BLEDev.advData[0], BLE.BLEDev.advData, &len, adv_name)) {
        USB.print(F("  The length of Complete Local Name : "));
        USB.println(len, HEX);
        USB.print(F("  The Complete Local Name is        : "));
        adv_name[len]= 0;
        USB.println((const char *)adv_name); 
        USB.println(F(""));
        if (0x00 == memcmp(adv_name, nameToSearch, len)) {
            USB.println(F("* Thunder Sense #02735 found"));
            return 1;	
        }
    } 
    return 0;  
}

/*! \fn void configure_scanner(uint8_t tx_power, uint8_t discover, uint16_t scan_interval, uint16_t scan_window, uint8_t scanning )
    \brief BLE Scanner configuration
    \param   tx_power        Value of the power for the configuration of the module: 
                             value between 0 and 15 which give the real TX power from -23 to +3 dBm.
    \param   discover        Indicates the discovery mode:
                              •Limited discoverable: Discover only slaves which have the limited discoverable mode enabled. BLE_GAP_DISCOVER_LIMITED = 0.
                              •Generic discoverable: Discover slaves which have limited discoverable mode or generic discoverable mode enabled. BLE_GAP_DISCOVER_GENERIC = 1.
                              •Observation: Discover all devices (default). BLE_GAP_DISCOVER_OBSERVATION = 2.
    \param   scan_interval   Defines at what intervals scanner is started:
                              This variable is defined in units of 625us and has a range between 4 and 16384.
                              Ej 75, which is equivalent to 75 * 625 us = 46.875 ms. 
    \param   scan_window     Defines how long to scan at each interval:
                              This variable is defined in units of 625us and and has a range between 4 and 16384. 
                              Ej 50, which is equivalent to 50 * 625 us = 31.250 ms
    \param   scanning        Indicates the discovery mode:
                              This parameter selects between two types of scanning: passive and active.
    \retval void
    
    This function performs the BLE scanner configuration
*/
void BLECentral::configureScanner(uint8_t txPower, uint8_t discoverMode, uint16_t scanInterval, uint16_t scanWindow, uint8_t scanningFilter){	
  BLE.setDiscoverMode(discoverMode);
	BLE.setTXPower(txPower);
	BLE.setScanningParameters(scanInterval, scanWindow, scanningFilter );
  newDevice();//create Device_t struct to store device relate data.
  #if DEBUG >= 1
      BLE.getScanningParameters();
      USB.println(F("____________Central is configure and redy to start____________"));
  #endif
   	
}

/*! \fn uint8_t startScanningDevice(char mac[])
    \brief Makes an inquiry to discover specific device by its Mac
    \param   mac    The mac of the device to search
    \retval uint16_t response Scanner response status:
                                                      1:Device found
                                                      0:Device not found
                                                      error code otherwise
  
    This function starts the BLE scan process to discover specific device by its Mac 
*/
uint16_t BLECentral::startScanningDevice(char mac[]){
	uint16_t response = 0;
	
	response = BLE.scanDevice(mac);//devuelve 1
	if(response == 1){
		USB.println(F("__________Device found"));
	}else if (response == 0){
		USB.println(F("__________Device NOT found"));	
	}else{
    USB.print(F("Scanner, ERROR = "));
    USB.println(response, DEC);
  }
  return response;
}

/*! \fn uint8_t startScanning(uint8_t time)
    \brief Scan looking for BLE devices.
    \param   time   The time to scan in search of devices.
    \retval uint8_t response  Scanner status:
                                            1:Scanner ok
                                            error code otherwise     
  
    This function starts the BLE scan process for the specified time.
*/ 
uint16_t BLECentral::startScanning(uint8_t time){
    uint16_t response = 0;
    response = BLE.scanNetwork(time);
    if(response == 0){
        USB.println(F("__________Device found"));
    }else{
        USB.print(F("Scanner, ERROR = "));
        USB.println(response, DEC);
    }
    return response;
}

/*! \fn uint8_t connect(char mac[])
    \brief Start direct connection establishment procedure to a dedicated BLE device.
    \param   mac[]  The BLE MAC address of BLE device to connect
    \retval uint16_t response  connection status: 
                                                0: if invalid parameters
                                                1: if connection is successful
                                                error code otherwise.
                   
    This function will start direct connection establishment procedure to a dedicated BLE device. Central<--->Peripheral(MAC). 
*/
uint16_t BLECentral::connect(char mac[]){
	
	uint8_t response;
	USB.println(F("_________Connecting... "));
	response = BLE.connectDirect(mac);
	if (response == 1){
        USB.println(F("_________Connected"));
        USB.print(F("             -connection_handle: "));
        USB.println(BLE.connection_handle, DEC);
        USB.print(F(""));
    }else if (response == 0){
        USB.println(F("Invalid parameters"));  
    }else{
        USB.print(F("Connection ERROR = "));
        USB.println(response, DEC);
    }
    return response; 
}

/*! \fn uint8_t connectWithSelectedParameters(char mac[], uint16_t conn_interval_min, uint16_t conn_interval_max, uint16_t timeout, uint16_t latency)
    \brief Start direct connection establishment procedure to a dedicated BLE device.
    \param   mac[]  The BLE MAC address of BLE device to connect
    \retval uint16_t response  connection status: 
                                              0 if invalid parameters
                                              1 if connection is successful
                                              error code if error sending conenctDirect command
                                              event identification number otherwise
                   
    This function will start direct connection establishment procedure to a dedicated BLE device. Central<--->Peripheral(MAC). 
*/
uint16_t BLECentral::connectWithSelectedParameters(char mac[], uint16_t conn_interval_min, uint16_t conn_interval_max, 
uint16_t timeout, uint16_t latency){
	
	uint16_t response = 0;
	
	USB.println(F("_________Connecting... "));
	response = BLE.connectDirect(mac, conn_interval_min, conn_interval_max, timeout, latency);//default connectDirect(BLEAddress, 60, 76, 100, 0);
	if (response == 1){
        USB.println(F("_________Connected"));
        USB.print(F("             -connection_handle: "));
        USB.println(BLE.connection_handle, DEC);
        USB.print(F(""));
        return 1;
    }else{
        USB.println(F("NOT Connected"));  
        return 0;
    }
    
}

/*! \fn uint8_t disconnect()
    \brief Disconnects an active connection.
    \param connectionHandle The connection handle  
    \retval response  Disconnect status:
                                        0: if ok
                                        1: if connection handle is not right
                                        error code if error sending command
                                        event identifier if disconnection event not found
    
    This function disconnects an active connection.
*/ 
uint16_t BLECentral::disconnect(uint8_t connectionHandle){
	uint16_t response = 0;
	response =  BLE.disconnect(connectionHandle);
	if (response == 0){
        USB.println(F("_________Disconnected"));
    }else if (response == 1){
        USB.print(F("Connection handle is not right")); 
    }else{
        USB.print(F("Disconnect, Error = "));  
        USB.println(response, HEX);  
    }
    return response;
}

/*! \fn uint8_t discoverServices();
    \brief Discover BLE primary services
    \param  None
    \retval 1: discoverServices OK
            0: discoverServices error(Device disconnected)
    
    This function discovers all the BLE services associated with the peripheral to which it is connected.
    The result is save in Device_t->servicio[].service 
*/
uint8_t BLECentral::discoverServices(){
    
    uint16_t event = 0;
    readByGroupCommand_t command;
    command = getDiscoverServiceGroupCommand();
    USB.println(F("_________Discovering Services... "));
    BLE.sendCommand((uint8_t *)&command, command.t_length+1);
    BLE.readCommandAnswer();
    while(event != BLE_EVENT_ATTCLIENT_PROCEDURE_COMPLETED){
        event = BLE.waitEvent(1000);
        if(event == BLE_EVENT_ATTCLIENT_GROUP_FOUND){ 
            newService( BLE.event);
        }else if(event == 0){//No hay evento
            USB.println(F("The connection to the peripheral device has been disconnected"));
            return 0;  
        }
    }
    USB.print(F("_________Discovering Services completed ("));
    USB.print(device->numberOfServices, DEC);
    USB.println(F(")"));
    return 1;
}

/*! \fn uint8_t discoverCharacteristics();
    \brief Discover BLE Characteristics
    \param  None
    \retval 1: discoverServices OK
            0: discoverServices error//No event-->device disconnected
    This function discovers the Characteristics asociated to a service. 
    To use this function you must first have called the function discoverServices(),
    because it use the discovery service to search the characteristics.
*/
uint8_t BLECentral::discoverCharacteristics(){
    
    uint16_t event;
    readByGroupCommand_t command;
    command = getDiscoverCharacteristicsCommand();
    USB.println(F("_________Discovering Characteristics... "));
    for(uint8_t numSer = 0; numSer < device->numberOfServices; numSer++){
        command.startFirstAttributeHandle = ((device->service[numSer].service.start_group_handle)+1);
        command.endLastAttributeHandle = ((device->service[numSer].service.end_group_handle)-1);
        BLE.sendCommand((uint8_t *)&command, command.t_length+1);
        BLE.readCommandAnswer();
        event = BLE_EVENT_ATTCLIENT_ATTRIBUTE_VALUE;
        while(event == BLE_EVENT_ATTCLIENT_ATTRIBUTE_VALUE ){
            event = BLE.waitEvent(1000);
            if(event == BLE_EVENT_ATTCLIENT_ATTRIBUTE_VALUE){ 
                newCharacteristic(&device->service[numSer], BLE.event);
            }else if(event == 0){
                USB.println(F("The connection to the peripheral device has been disconnected"));
                return 0; 
            }
        }
    }
    USB.println(F("_________Discovering Characteristics completed"));
    USB.println(F(""));
    return 1;
}

/*! \fn uint8_t discoverDescriptors();
    \brief Discover BLE Descriptors
    \param  None
    \retval 0 discoverServices error
            1 discoverServices OK
    
    This function discovers the BLE Descriptors
    To use this function you must first have called the function discoverCharacteristics(),
    because it use the Characteristics parameters to search the descriptors.
*/ 
uint8_t BLECentral::discoverDescriptors(){
  uint8_t numSer, numCar;
  findInformationCommand_t command;
  command = getDiscoverDescriptorsCommand();
  uint16_t servEndHandle, carcValueHandle, nexCarcStarHandle;
  USB.println(F("_________Discovering Descriptors... "));
  for(numSer = 0; numSer < device->numberOfServices; numSer++){
    servEndHandle = device->service[numSer].service.end_group_handle;
    for(numCar = 0; numCar < device->service[numSer].numberOfCharacteristics; numCar++){
      carcValueHandle = device->service[numSer].characteristic[numCar].charac.value_handle;
      if( (numCar+1) < device->service[numSer].numberOfCharacteristics){
        nexCarcStarHandle = device->service[numSer].characteristic[numCar+1].charac.start_handle;
        carcValueHandle = carcValueHandle+1;
        while((carcValueHandle)!= nexCarcStarHandle){
          command.startFirstAttributeHandle = carcValueHandle ;
          command.endLastAttributeHandle = carcValueHandle;
          BLE.sendCommand((uint8_t *)&command, command.t_length+1);
          BLE.readCommandAnswer();
          BLE.waitEvent(1000);
          newDescriptor(&device->service[numSer].characteristic[numCar],BLE.event);
          carcValueHandle = carcValueHandle+1; 
        }
      }else if(servEndHandle != 0xFFFF){
        while((carcValueHandle)!= servEndHandle){
          carcValueHandle = carcValueHandle+1;
          command.startFirstAttributeHandle = carcValueHandle ;
          command.endLastAttributeHandle = carcValueHandle;
          BLE.sendCommand((uint8_t *)&command, command.t_length+1);
          BLE.readCommandAnswer();
          BLE.waitEvent(1000);
          newDescriptor(&device->service[numSer].characteristic[numCar],BLE.event);
        }
      }else{
          uint8_t evento;
          evento = BLE_EVENT_ATTCLIENT_FIND_INFORMATION_FOUND;
          while(evento == BLE_EVENT_ATTCLIENT_FIND_INFORMATION_FOUND){
              carcValueHandle = carcValueHandle+1;
              command.startFirstAttributeHandle = carcValueHandle ;
              command.endLastAttributeHandle = carcValueHandle;
              BLE.sendCommand((uint8_t *)&command, command.t_length+1);
              BLE.readCommandAnswer();
              evento = BLE.waitEvent(1000);
              if(evento == BLE_EVENT_ATTCLIENT_FIND_INFORMATION_FOUND){
                newDescriptor(&device->service[numSer].characteristic[numCar],BLE.event);
              }else if(evento == BLE_EVENT_ATTCLIENT_PROCEDURE_COMPLETED){ 
                device->service[numSer].service.end_group_handle = carcValueHandle -1;  
              }
          }
      }
    }
  }       
  USB.println(F("_________Discovering Descriptors completed"));
  USB.println(F(""));
  return 1;
} 

/*! \fn uint8_t discoverBLEProfile();
    \briefc Discover the BLE profile
    \param  None
    \retval 1: OK
            0: Error
            
    This function discovers the BLE profile.
    * Idea para no tener el problema de primero descubrir servicios, despues caract y luego descrip 
*/
uint8_t BLECentral::discoverBLEProfile(){
    
    if(discoverServices()){
        if(discoverCharacteristics()){
            if(discoverDescriptors()){
                printBLEProfile();
                return 1;
            }
        }
    }
    freeDevice();
    return 0;
    
}

/*! \fn void printBLEProfile()
    \brief Print the BLE profile: Services, Characteristics and Descriptors
    \param   None
    \retval None
   
*/
void BLECentral::printBLEProfile(){
    uint8_t numSer, numCar, numDesc;
    USB.println(F("___________________BLE Profile___________________"));
    USB.println(F(""));
    for(numSer = 0; numSer < device->numberOfServices; numSer++){
        USB.print(F("* Service number "));
        USB.println( numSer, HEX );
        USB.print(F("   - Service start handle: "));
        USB.println(device->service[numSer].service.start_group_handle, HEX);
        USB.print(F("   - Service end handle: "));
        USB.println(device->service[numSer].service.end_group_handle, HEX);
        USB.print(F("   - Service uuid16: "));
        USB.println(device->service[numSer].service.uuid16, HEX);
        USB.print(F("   - Service uuid128: "));
        for(int p = 0; p < 16; p++){
            USB.print(device->service[numSer].service.uuid128[p],HEX);
            USB.print(" ");
        }
        USB.println("");
        USB.print(F("   * Service number of characteristics: "));
        USB.print(device->service[numSer].numberOfCharacteristics, DEC);
        USB.println(F(""));
        for(numCar = 0; numCar < device->service[numSer].numberOfCharacteristics; numCar++){
            USB.print(F("* Service "));
            USB.print( numSer,HEX );
            USB.print(F("   - Characteristic: "));
            USB.println( numCar,HEX );
            USB.print(F("   - Characteristic start handle: "));
            USB.print(device->service[numSer].characteristic[numCar].charac.start_handle,HEX);
            USB.println("");
            USB.print(F("   - Characteristic value handle: "));
            USB.print(device->service[numSer].characteristic[numCar].charac.value_handle ,HEX);
            USB.println("");
            USB.print(F("   - Characteristic properties: "));
            USB.print(device->service[numSer].characteristic[numCar].charac.properties,HEX);
            USB.println("");
            USB.print(F("   - Characteristic uuid16: "));
            USB.print( device->service[numSer].characteristic[numCar].charac.uuid16,HEX);
            USB.println(F(""));
            USB.print(F("   - Characteristic uuid128: "));
            for(int p = 0; p < 16 ; p++){
            USB.print(device->service[numSer].characteristic[numCar].charac.uuid128[p],HEX);
            USB.print(" ");
            }
            USB.println(" ");
            for(numDesc = 0; numDesc < device->service[numSer].characteristic[numCar].numberOfDescriptors; numDesc++){
                USB.print(F("   - Characteristic: "));
                USB.print( numCar,HEX );
                USB.print(F("   - Descriptor: "));
                USB.println( numDesc, HEX );
                USB.print(F("   - Descriptor handle: "));
                USB.println( device->service[numSer].characteristic[numCar].descriptor[numDesc].descriptor.handle,HEX );
                USB.print(F("   - Descriptor uuid16: "));
                USB.println( device->service[numSer].characteristic[numCar].descriptor[numDesc].descriptor.uuid16, HEX );  
            }
        }
    }
    #if DEBUG >= 1
        USB.print(F("Free Memory(After creating the BLE profile):"));
        USB.println(freeMemory());
        USB.println(F(" "));
    #endif
}

/*! \fn uint8_t* readAttribute( uint8_t *uuid128)
    \brief Read Attribute by the given uuid128
    \param   *uuid128 the uuid from 128 bits to be read
    \retval pointer to BLE.attributeValue 
                    
    This function read Attribute by the given uuid128
*/
uint8_t* BLECentral::readAttribute( uint8_t *uuid128){
    USB.println(F("BLE Central read Attribute "));
    USB.print(F("  -UUID128: ")); 
    for(uint8_t i=0; i<16; i++){
      USB.print(uuid128[i], HEX); 
      USB.print(" "); 
    }
    USB.println(F(""));   
    USB.print(F("  -Handle: "));
    USB.println(uuid128ToHandle(uuid128), HEX);
    USB.println(F(""));//probado
    BLE.attributeRead(BLE.connection_handle, uuid128ToHandle(uuid128));
    return BLE.attributeValue;
	
}

/*! \fn uint8_t* writeAttribute(uint8_t connection, uint16_t atthandle, uint8_t *data, uint8_t length)
    \brief Write attribute by the given uuid128
    \param   connection The connection handle
    \param   *uuid128 The uuid128 to write the attribute
    \param   data The data to write
    \param   length The lenght of the data to write
    \retval  uint16_t response The module response:
                                                  errorCode in any case
                                                  0 means success
                                                  event identification number if write procedure completed event not received 
                    
    This function write Attribute by the given uuid128
*/
uint16_t BLECentral::writeAttribute(uint8_t connection,  uint8_t *uuid128, uint8_t *data, uint8_t length){
    uint16_t response;
    USB.println(F("Writing attribute.. "));
    response = BLE.attributeWrite(connection, uuid128ToHandle(uuid128), data, length);
    if (response == 0){
        USB.println(F("Write attribute OK"));
    }else{
       USB.println(F("Write ERROR = "));
    }
    return response;
}

/*! \fn uint8_t enableNotification(uint8_t *uuid128)
    \brief Enable notification for the given uuid128.
    \param   *uuid128 the uuid from 128 bits to enable notifications
    \retval 1: if notifications were enabled ok
            0: if failed subscribing
            
                    
    This function enable notifications of the Characteristic with the uuid128
*/
uint8_t BLECentral::enableNotification(uint8_t *uuid128){
  
    uint16_t response;
    char notify[2] = "1";
    response = BLE.attributeWrite(BLE.connection_handle, (uuid128ToHandle(uuid128) + 1), notify);
    if (response == 0){
      USB.println(F("____________Notification enable"));
      USB.print(F("  -For UUID128: "));
      for(uint8_t i=0; i<16; i++){
        USB.print(uuid128[i], HEX);
        USB.print(" ");  
      }
    USB.println(F(""));
		return 1;
    }else{
        USB.println(F("____________Failed subscribing"));
        USB.println(F(""));
		return 0;
    }
}

/*! \fn void receiveNotifications()
    \brief Receives notifications if they have been enabled. 
    \param   
    \retval  uint8_t* Pointer to the BLE.event The buffer that store the notifications events
                    
    This function recive notifications if they have been enabled before.
*/
uint8_t* BLECentral::receiveNotifications(){
    uint16_t event;
    uint16_t handler;
    USB.println(F("Waiting events..."));
    event = BLE.waitEvent(1000);
    if (event == BLE_EVENT_ATTCLIENT_ATTRIBUTE_VALUE){
		USB.println(F("Notification received"));
            /* attribute value event structure:
             Field:   | Message type | Payload| Msg Class | Method |  Connection | att handle | att type | value |
             Length:  |       1      |    1   |     1     |    1   |      1      |     2      |     8    |   n   |
             Example: |      80      |   05   |     04    |   05   |     00      |   2c 00    |     x    |   n   |*/
        handler = ((uint16_t)BLE.event[6] << 8) | BLE.event[5]; 
        USB.print(F("  -Attribute with handler "));
        USB.print(handler, DEC);
        USB.println(F(" has changed "));
        USB.print(F("  -Attribute value: "));
        BLE.event[0] = BLE.event[8];
        for(uint8_t i = 0; i < BLE.event[8]; i++){ 
        USB.printHex(BLE.event[i+9]);
            BLE.event[i+1] = BLE.event[i+9];      
        }
        USB.println(F("")); 
        return BLE.event;
    }
	
}

/*! \fn uint8_t getConnectionHandler()
    \brief Get the connection handle
    \param  None
    \retval uint8_t The connection handle
   
    This function return the connection handle
*/
uint8_t BLECentral::getConnectionHandler(){
    return BLE.connection_handle;
}

/*! \fn uint8_t getConnectionStatus()
    \brief   return the connection status
    \param   None
    \retval  1 if connected 
             others not connected 
    
    This function 
*/
uint8_t BLECentral::getConnectionStatus(){//idea limpiar device aqui usuario no deve tocarlo
    uint8_t status;

    status =  BLE.getStatus(BLE.connection_handle);
    if( status == 0 ) { //Se ha desconectado
       // central.limpiarPerfil();
       USB.print(F("The connection has been disconnected, satus: "));
       USB.println(status, DEC);
    }else if (status == 1){
       USB.print(F("The device is connected, satus: "));
       USB.println(status, DEC);
    }else{
       USB.print(F("The module does not response, satus: "));
       USB.println(status, DEC);
    }
    return status;
}


/******************************************************************************
 * PRIVATE FUNCTIONS                                                          *
 ******************************************************************************/

/*! \fn readByGroupCommand_t getDiscoverServiceGroupCommand()
    \brief Get the group service command acording to the BLE112 MODULE BGAPI
    \param   None
    \retval readByGroupCommand_t command
  
    This function get the group service command acording to the BLE112 MODULE BGAPI 
*/
readByGroupCommand_t BLECentral::getDiscoverServiceGroupCommand(){
    /* Byte Type Name Description
    //~ 0 0x00 hilen Message type: command
    //~ 1 0x06 lolen Minimum payload length
    //~ 2 0x04 class Message class: Attribute Client
    //~ 3 0x01 method Message ID
    //~ 4 uint8 connection Connection Handle
    //~ 5 - 6 uint16 start First requested handle number
    //~ 7 - 8 uint16 end Last requested handle number
    //~ 9 uint8array uuid Group UUID to find
    */    
    readByGroupCommand_t command;
    command.t_length = 12;
    command.messageType = 0;
    command.payloadLenght = 8;
    command.classID = 4;
    command.commandID = 1;
    command.Connectionhandle = BLE.connection_handle;
    command.startFirstAttributeHandle = 0x0001;
    command.endLastAttributeHandle = 0xFFFF;
    command.uuidLenght = 2;
    command.uuid = 0x2800;
    return command;
}

/*! \fn readByGroupCommand_t getDiscoverCharacteristicsCommand()
    \brief Get the characteristic command
    \param   None
    \retval readByGroupCommand_t command 
    
    This function get the characteristic command acording to the BLE112 MODULE BGAPI
*/
readByGroupCommand_t BLECentral::getDiscoverCharacteristicsCommand(){
    
    readByGroupCommand_t command;
    command.t_length = 12;
    command.messageType = 0;
    command.payloadLenght = 8;
    command.classID = 4;
    command.commandID = 2;
    command.Connectionhandle = BLE.connection_handle;
    command.startFirstAttributeHandle = 0;
    command.endLastAttributeHandle = 0;
    command.uuidLenght = 2;
    command.uuid = 0x2803;
    return command;
}

/*! \fn findInformationCommand_t getDiscoverDescriptorsCommand()
    \brief get the command acording to the BLE112 MODULE BGAPI 
    \param   None
    \retval findInformationCommand_t command
    
    This function get the command acording to the BLE112 MODULE BGAPI 
*/
findInformationCommand_t BLECentral::getDiscoverDescriptorsCommand(){
        /*//~ Byte Type Name Description
        //~ 0 0x00 hilen Message type: command
        //~ 1 0x05 lolen Minimum payload length
        //~ 2 0x04 class Message class: Attribute Client
        //~ 3 0x03 method Message ID
        //~ 4 uint8 connection Connection handle
        //~ 5 - 6 uint16 start First attribute handle
        //~ 7 - 8 uint16 end Last attribute handle 
        */
        findInformationCommand_t command;
        command.t_length = 9;
        command.messageType = 0;
        command.payloadLenght = 5;
        command.classID = 4;
        command.commandID = 3;
        command.Connectionhandle = BLE.connection_handle;
        return command;
}

/*! \fn void newDevice()
    \brief  Initialize the struct Device_t.
    \param 
    \retval

    This function initialize the storage to save a BLE device and its related data.
*/
void BLECentral::newDevice(){
    #if DEBUG >= 1
        USB.print(F("Free Memory(Before New Device):"));
        USB.println(freeMemory());
    #endif
    
    device = (Device_t*)malloc(sizeof(Device_t));
    device->numberOfServices = 0;
    device->service = NULL;
    USB.println(F("____________Storage to manage new device started____________"));
    
    #if DEBUG >= 1
        USB.print(F("Free Memory(After New Device):"));
        USB.println(freeMemory());
    #endif
}

/*! \fn void newService(uint8_t discoveredService[])
    \param  datosServicio
    \retval None
    \brief 
    
    This function save the discored Service 
*/ 
void BLECentral::newService(uint8_t discoveredService[]){
    /*  Group Found--attclient
    //~ Byte Type Name Description
    //~ 0 0x80 hilen Message type: event
    //~ 1 0x06 lolen Minimum payload length
    //~ 2 0x04 class Message class: Attribute Client
    //~ 3 0x02 method Message ID
    //~ 4 uint8 connection Connection handle
    //~ 5 - 6 uint16 start Starting handle
    //~ 7 - 8 uint16 end Ending handle
    //~ Note: "end" is a reserved word and in BGScript so "end" cannot be used as
    //~ such.
    //~ 9 uint8array uuid UUID of a service
    //~ Length is 0 if no services are found. 
    */  
    int contador = 0;
    device->numberOfServices++;
    service_t *tmp = (service_t*)malloc(sizeof(service_t)*device->numberOfServices);    
    service_t *basura;
    for(contador = 0; contador <(device->numberOfServices-1); contador++){
        tmp[contador] = device->service[contador];
    }
    tmp[contador].numberOfCharacteristics = 0;
    tmp[contador].characteristic = NULL;
    tmp[contador].service.start_group_handle = ((uint16_t)discoveredService[6] << 8) | discoveredService[5];
    tmp[contador].service.end_group_handle = ((uint16_t)discoveredService[8] << 8) | discoveredService[7];   
    tmp[contador].service.uuid16 = 0;
    memset(&tmp[contador].service.uuid128, 0x00, 16 );
    if(discoveredService[9]== 2){
        tmp[contador].service.uuid16 = ((uint16_t)discoveredService[11] << 8) | discoveredService[10];
        service_uuid16_to_uuid128(&tmp[contador]);
    }else{
        for(uint8_t indice = 0; indice < 16 ; indice++){
            tmp[contador].service.uuid128[indice] = ((uint16_t)discoveredService[25-indice]);
        }
    }
    basura = device->service;
    device->service = tmp;
    if(basura!= NULL){
        free(basura);
        tmp = NULL;
    }
}

/*! \fn void newCharacteristic(servicio_t* servicio, uint8_t discoveredCharacteristic[])
    \param  servicio
    \param  paquete
    \retval None
    \brief 
    \note Modos de descubrimiento:Obserbable, etc.........(Es una nota aclaratoria)
    
    This 
*/
void BLECentral::newCharacteristic(service_t* service, uint8_t discoveredCharacteristic[]){
    /* Attribute Value--attclient
    //~ 0 0x80 hilen Message type: event
    //~ 1 0x05 lolen Minimum payload length
    //~ 2 0x04 class Message class: Attribute Client
    //~ 3 0x05 method Message ID
    //~ 4 uint8 connection Connection handle
    //~ 5 - 6 uint16 atthandle Attribute handle
    //~ 7 uint8 type Attribute type
    //~ 8 uint8array value Attribute value (data)//yo de aqui
    */
    uint8_t contador = 0;
    uint8_t posicion = 0;   
    service->numberOfCharacteristics++;
    characteristic_t* tmp = (characteristic_t*)malloc(sizeof(characteristic_t)*service->numberOfCharacteristics);    
    characteristic_t* basura;
    for(contador = 0; contador <(service->numberOfCharacteristics-1); contador++){
        tmp[contador] = service->characteristic[contador];
    }
    tmp[contador].numberOfDescriptors = 0;
    tmp[contador].descriptor = NULL;
    tmp[contador].charac.start_handle = ((uint16_t)discoveredCharacteristic[6] << 8) | discoveredCharacteristic[5];
    tmp[contador].charac.value_handle = ((uint16_t)discoveredCharacteristic[11] << 8) | discoveredCharacteristic[10];
    tmp[contador].charac.properties = ((uint8_t)discoveredCharacteristic[9]);
    tmp[contador].charac.uuid16 = 0;
    memset(&tmp[contador].charac.uuid128, 0x00, 16 );
    posicion = ((discoveredCharacteristic[8]) - 3);
    if(posicion == 2){
        tmp[contador].charac.uuid16 = ((uint16_t)discoveredCharacteristic[13] << 8) | discoveredCharacteristic[12];
        characteristic_uuid16_to_uuid128(&tmp[contador]);
    }else{
        for(int s = 0; s < 16 ; s++){
            tmp[contador].charac.uuid128[s]= discoveredCharacteristic[27-s];
        }
    }
    basura = service->characteristic;
    service->characteristic = tmp;
    if(basura!= NULL){
        free(basura);
        tmp = NULL;
    }
}

/*! \fn void newDescriptor(caracteristica_t* caracteristica, uint8_t discoveredDescriptor[])
    \param  caracteristica
    \param  paquete
    \retval None
    \brief 
    
    This
*/ 
void BLECentral::newDescriptor(characteristic_t *characteristic , uint8_t discoveredDescriptor[]){
    /*  Find Information Found--attclient
    //~ Byte Type Name Description
    //~ 0 0x80 hilen Message type: event
    //~ 1 0x04 lolen Minimum payload length
    //~ 2 0x04 class Message class: Attribute Client
    //~ 3 0x04 method Message ID
    //~ 4 uint8 connection Connection handle
    //~ 5 - 6 uint16 chrhandle Characteristics handle
    //~ 7 uint8array uuid Characteristics type (UUID)
    */ 
    uint16_t flag1;
    uint8_t contador = 0;
    characteristic->numberOfDescriptors++;
    descriptor_t* tmp = (descriptor_t*)malloc(sizeof(descriptor_t)*characteristic->numberOfDescriptors);    
    descriptor_t* basura;
    for(contador = 0; contador <(characteristic->numberOfDescriptors-1); contador++){
        tmp[contador] = characteristic->descriptor[contador];
    }
    tmp[contador].descriptor.handle = ((uint16_t)discoveredDescriptor[6] << 8) | discoveredDescriptor[5];
    tmp[contador].descriptor.uuid16 = ((uint16_t)discoveredDescriptor[9] << 8) | discoveredDescriptor[8];
    
    basura = characteristic->descriptor;
    characteristic->descriptor = tmp;
    if(basura!= NULL){
        free(basura);
        tmp = NULL;
    }
}

/*! \fn  void freeDevice()
    \brief 
    \param   None
    \retval  None
    
*/
void BLECentral::freeDevice(){
    #if DEBUG >= 1
        USB.print(F("Free Memory(Before freeDevice):"));
        USB.println(freeMemory());
    #endif
    uint8_t numSer;
    uint8_t numCar;
    for(numSer = 0; numSer < device->numberOfServices; numSer++){
        for(numCar = 0; numCar < device->service[numSer].numberOfCharacteristics; numCar++){
            if(device->service[numSer].characteristic[numCar].descriptor != NULL){
                free(device->service[numSer].characteristic[numCar].descriptor);
            }
        }
    }
    for(uint8_t numSer = 0; numSer < device->numberOfServices; numSer++){
        if(device->service[numSer].characteristic != NULL){
                free(device->service[numSer].characteristic);
        }
    }
    if(device->service != NULL){
        free(device->service);
         device->numberOfServices = 0;
         device->service = NULL;
    }
    //~ free(device);
    #if DEBUG >= 1
        USB.print(F("Free Memory(After freeDevice):"));
        USB.println(freeMemory());
    #endif
}

/*! \fn  uint16_t uuid16ToHandle(uint16_t uuid16);
    \brief 
    \param   uuid16       
    \retval uint16_t
   
    
*/ 
uint16_t BLECentral::uuid16ToHandle(uint16_t uuid16){
    uint8_t numSer;
    uint8_t numCar;
    uint8_t numDesc;
    for(numSer = 0; numSer < device->numberOfServices; numSer++){
        if(device->service[numSer].service.uuid16 == uuid16){
           return device->service[numSer].service.start_group_handle; 
        }
        for(numCar = 0; numCar < device->service[numSer].numberOfCharacteristics; numCar++){
            
            if(device->service[numSer].characteristic[numCar].charac.uuid16 == uuid16){
                return device->service[numSer].characteristic[numCar].charac.value_handle; 
            }
            for(numDesc = 0; numDesc < device->service[numSer].characteristic[numCar].numberOfDescriptors; numDesc++){
                if(device->service[numSer].characteristic[numCar].descriptor[numDesc].descriptor.uuid16 == uuid16){
                    return device->service[numSer].characteristic[numCar].descriptor[numDesc].descriptor.handle; 
                }
            }
        }
    } 
}

/*! \fn  uint16_t uuid128ToHandle(uint8_t *uuid128)
    \brief 
    \param   uuid16       
    \retval *uuid128
    
*/ 
uint16_t BLECentral::uuid128ToHandle(uint8_t *uuid128){
  
  uint8_t numSer;
  uint8_t numCar;
  //uint16_t uuid16;
  //uuid16 =  (((uint16_t)uuid128[2] << 8) + ((uint16_t)(uuid128[3])));
   
  for(numSer = 0; numSer < device->numberOfServices; numSer++){
    if(0x00 == memcmp(device->service[numSer].service.uuid128, uuid128, 16)){//| (device->servicio[numSer].service.uuid16 == uuid16)
      return device->service[numSer].service.start_group_handle; 
    }
    for(numCar = 0; numCar < device->service[numSer].numberOfCharacteristics; numCar++){
      if(0x00 == memcmp(device->service[numSer].characteristic[numCar].charac.uuid128, uuid128, 16)){//| (device->servicio[numSer].caracteristica[numCar].charac.uuid16 == uuid16)
        return device->service[numSer].characteristic[numCar].charac.value_handle; 
      }
    }
  }
  return 0;
}

/*! \fn  uint16_t service_uuid16_to_uuid128(service_t *service);
    \brief From the service uuid16 get the service uuid128 associated
    \param   uuid16       
    \retval void
   
    This function get the service uuid16 and from defines store the service uuid128 associated
*/ 
void BLECentral::service_uuid16_to_uuid128(service_t *service){
    switch(service->service.uuid16){
        case 0x1800 :
            memcpy(service->service.uuid128, generic_access_service_uuid, 16);
            break;
            
        case 0x1801 :
            memcpy(service->service.uuid128, generic_attribute_service_uuid, 16);
            break;
            
        case 0x180A :
            memcpy(service->service.uuid128, device_information_service_uuid, 16);
            break;
            
        case 0x180F :
            memcpy(service->service.uuid128, battery_service_uuid, 16);
            break;
            
        case 0x181A :
            memcpy(service->service.uuid128, environmental_sensing_service_uuid, 16);
            break;
            
        case 0x1815 :
            memcpy(service->service.uuid128, automation_io_service_uuid, 16);
            break;
    }
}

/*! \fn  uint16_t characteristic_uuid16_to_uuid128(characteristic_t *characteristic);
    \brief From the characteristic uuid16 get the characteristic uuid128 associated
    \param   uuid16       
    \retval void
   
    This function get the characteristic uuid16 and from defines get the characteristic uuid128 associated
*/ 
void BLECentral::characteristic_uuid16_to_uuid128(characteristic_t *characteristic){
    
    switch(characteristic->charac.uuid16){
        case 0x2A00 :
            memcpy(characteristic->charac.uuid128, Service0_Characrteristic0_Device_Name_uuid, 16);
            break;
            
        case 0x2A01 :
            memcpy(characteristic->charac.uuid128, Service0_Characrteristic1_Appearance_uuid, 16);
            break;
            
        case 0x2A05 :
            memcpy(characteristic->charac.uuid128, Service1_Characrteristic0_Service_Changed_uuid, 16);
            break;
            
        case 0x2A29 :
            memcpy(characteristic->charac.uuid128, Service2_Characrteristic0_Manufacturer_Name_uuid, 16);
            break;
            
        case 0x2A24 :
            memcpy(characteristic->charac.uuid128, Service2_Characrteristic1_Model_Number_uuid, 16);
            break;
            
        case 0x2A25 :
            memcpy(characteristic->charac.uuid128, Service2_Characrteristic2_Serial_Number_uuid, 16);
            break;
            
        case 0x2A27 :
            memcpy(characteristic->charac.uuid128, Service2_Characrteristic3_Hardware_Revision_uuid, 16);
            break;
            
        case 0x2A26 :
            memcpy(characteristic->charac.uuid128, Service2_Characrteristic4_Firmware_Revision_uuid, 16);
            break;
            
        case 0x2A23 :
            memcpy(characteristic->charac.uuid128, Service2_Characrteristic5_System_ID_uuid, 16);
            break;
            
        case 0x2A19 :
            memcpy(characteristic->charac.uuid128, Service3_Characrteristic0_Battery_Level_uuid, 16);
            break;
            
        case 0x2A76 :
            memcpy(characteristic->charac.uuid128, Service4_Characrteristic0_UV_Index_uuid, 16);
            break;
            
        case 0x2A6D :
            memcpy(characteristic->charac.uuid128, Service4_Characrteristic1_Pressure_uuid, 16);
            break;
            
        case 0x2A6E :
            memcpy(characteristic->charac.uuid128, Service4_Characrteristic2_Temperature_uuid, 16);
            break;
            
        case 0x2A6F :
            memcpy(characteristic->charac.uuid128, Service4_Characrteristic3_Humidity_uuid, 16);
            break;
            
        case 0x2A56 :
            memcpy(characteristic->charac.uuid128, Service8_Characrteristic0_Digital_1_uuid, 16);
            break;
    }
}
