/*! \file BLECentral.h
    \brief Library for managing the Bluetooth low energy module, BLE112, as a Central device
    \date 05/11/2018
    \author Alejandro Piñan Roescher
*/

/*! \def BLECentral_h
    \brief The library flag
 */   
#ifndef _BLECENTRAL_H
#define _BLECENTRAL_H

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "defines.h"
#include <inttypes.h>

/******************************************************************************
 * Definitions & Declarations
 ******************************************************************************/
 
/*! \struct gatt_client_service_t
    \brief  Structure to identificate a service
 */ 
typedef struct {
  uint16_t start_group_handle;/**< Service start group handle */
  uint16_t end_group_handle;/**< Service end group handle */
  uint16_t uuid16;/**< Service SIG BLE 16 bits uuid  */
  uint8_t  uuid128[16];/**< Service 128 bits uuid  */
}gatt_client_service_t;

/*! \struct gatt_client_characteristic_t
    \brief  Structure to identificate a characteristic
 */     
typedef struct {
  uint16_t start_handle;/**< Characteristic start handle */
  uint16_t value_handle;/**< Characteristic value handle */
  uint8_t  properties;/**< Characteristic properties */
  uint16_t uuid16;/**< Characteristic SIG BLE 16 bits uuid  */
  uint8_t  uuid128[16];/**< Characteristic 128 bits uuid  */
}gatt_client_characteristic_t;

/*! \struct gatt_client_characteristic_descriptor_t
    \brief  Structure to identificate a descriptor
*/     
typedef struct {
  uint16_t handle;/**< Descriptor handle */
  uint16_t uuid16;/**< Descriptor SIG BLE 16 bits uuid */
  uint8_t  uuid128[16];/**< Descriptor 128 bits uuid */
}gatt_client_characteristic_descriptor_t;

/*! \struct descriptor_t
    \brief  Struct to store descriptors
*/     
typedef struct {
  gatt_client_characteristic_descriptor_t descriptor;/**< Struct gatt_client_characteristic_descriptor_t*/
}descriptor_t;

/*! \struct characteristic_t
    \brief  Struct to store a characteristic and its descriptors
    
*/ 
typedef struct {
  gatt_client_characteristic_t charac;/**< Struct gatt_client_characteristic_t */
  uint8_t numberOfDescriptors;/**< Number of descriptors in a characteristic */
  descriptor_t *descriptor;/**< Pointer to descriptor_t struct */
}characteristic_t;

/*! \struct service_t
    \brief  Struct to store a service and its characteristics
*/ 
typedef struct {
  gatt_client_service_t service;/**< Struct gatt_client_service_t */
  uint8_t numberOfCharacteristics;/**< Number of characteristic in a service */
  characteristic_t *characteristic;/**<Pointer to characteristic_t struct */
}service_t;

/*! \struct Device_t
    \brief  Struct to save a BLE device and its related data
*/ 
typedef struct {
  uint8_t  connected_handle;/**< The connection handle */
  uint8_t  numberOfServices;/**< Number of service in BLE profile*/
  char  mac[13];/**< The device MAC address */
  service_t *service;/**< Pointer to servicio_t struct*/
}Device_t;


/*
                                BGAPI packet structure 
    Byte 0:
          [7] - 1 bit, Message Type (MT)         0 = Command/Response, 1 = Event
         [:3] - 4 bits, Technology Type (TT)     0000 = Bluetooth 4.0 single mode, 0001 = Wi-Fi
         [:0] - 3 bits, Length High (LH)         Payload length (high bits)
    Byte 1:     8 bits, Length Low (LL)          Payload length (low bits)
    Byte 2:     8 bits, Class ID (CID)           Command class ID
    Byte 3:     8 bits, Command ID (CMD)         Command ID
    Bytes 4-n:  0 - 2048 Bytes, Payload (PL)     Up to 2048 bytes of payload
*/

/*! \struct trama_grupo_t
    \brief  Struct to make command to discover services and characteristics
*/ 
typedef struct {
	uint8_t t_length;/**< The total lenght of the command*/
	uint8_t messageType;/**< The type of command*/ 
	uint8_t payloadLenght;/**< The payloadLenght of the command*/
	uint8_t classID;/**< Command class ID*/ 
	uint8_t commandID;/**< Command ID*/
	uint8_t Connectionhandle;/**< Connectionhandle*/
	uint16_t startFirstAttributeHandle;/**< startFirstAttributeHandle*/
  uint16_t endLastAttributeHandle;/**< endLastAttributeHandle*/
  uint8_t uuidLenght;/**< uuidLenght*/
  uint16_t uuid;/**< uuid*/
} readByGroupCommand_t;

/*! \struct trama_descriptor_t
    \brief  Struct to make command to discover descriptors
*/
typedef struct {
	uint8_t t_length;/**< The total lenght of the command*/
	uint8_t messageType;/**< The type of command*/ 
	uint8_t payloadLenght;/**< The payloadLenght of the command*/
	uint8_t classID;/**< Command class ID*/
	uint8_t commandID;/**< Command ID*/
	uint8_t Connectionhandle;/**< Connectionhandle*/
	uint16_t startFirstAttributeHandle;/**< startFirstAttributeHandle*/
  uint16_t endLastAttributeHandle;/**< endLastAttributeHandle*/
} findInformationCommand_t;

/******************************************************************************
 * Class                                                                      *
 ******************************************************************************/

//! BLECentral Class
/*!
  defines all the variables and functions used 
 */
class BLECentral{
  
/// public methods and attributes ////////////	
public:

    BLECentral();
   
    ~BLECentral();

    int8_t turnOnModule(uint8_t socket);
    
    void turnOffModule();
    
    uint8_t bleAdvdataDecode(uint8_t type, uint8_t advdata_len, uint8_t *p_advdata, uint8_t *len, uint8_t *p_field_data);

    uint8_t scanReport(char *nameToSearch);
    
    void configureScanner(uint8_t txPower, uint8_t discoverMode, uint16_t scanInterval, uint16_t scanWindow, uint8_t scanFilter );
    
    uint16_t startScanningDevice(char mac[]);
  
    uint16_t startScanning(uint8_t time);

    uint16_t connect(char mac[]);
    
    uint16_t connectWithSelectedParameters(char mac[], uint16_t conn_interval_min, uint16_t conn_interval_max, uint16_t timeout, uint16_t latency);
   
    uint16_t disconnect(uint8_t connectionHandle);

    uint8_t discoverServices();

    uint8_t discoverCharacteristics();
   
    uint8_t discoverDescriptors();
   
    uint8_t discoverBLEProfile();

    void printBLEProfile();
 
    uint8_t* readAttribute( uint8_t *uuid128);
    
    uint16_t writeAttribute(uint8_t connection, uint8_t *uuid128, uint8_t *data, uint8_t length);
    
    uint8_t enableNotification(uint8_t *uuid128);

    uint8_t* receiveNotifications();

    uint8_t getConnectionHandler();
    
    uint8_t getConnectionStatus();
    
/// private methods //////////////////////////    
private:

    readByGroupCommand_t getDiscoverServiceGroupCommand();

    readByGroupCommand_t getDiscoverCharacteristicsCommand();//Read By Type
  
    findInformationCommand_t getDiscoverDescriptorsCommand();

    //! Variable : Struct to save a BLE device and its data
    /*! For the management of the device by the master
    */
    Device_t *device;
      
    void newDevice();
   
    void newService( uint8_t discoveredService[]);
    
    void newCharacteristic(service_t *service, uint8_t discoveredCharacteristic[]);
    
    void newDescriptor(characteristic_t *characteristic, uint8_t discoveredDescriptor[]);
 
    void freeDevice();
  
    uint16_t uuid16ToHandle(uint16_t uuid16);
    
    uint16_t uuid128ToHandle(uint8_t *uuid128);
    
    void service_uuid16_to_uuid128(service_t *service);
    
    void characteristic_uuid16_to_uuid128(characteristic_t *characteristic);
    
};

#endif  
