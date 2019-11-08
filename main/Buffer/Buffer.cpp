/******************************************************************************
 * Includes
 ******************************************************************************/

#ifndef __WPROGRAM_H__
  #include <WaspClasses.h>
#endif

#include "Buffer.h"

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
Buffer::Buffer(){
  
}

/*! class Destructor
  It does nothing
  \param void
  \return void
*/
Buffer::~Buffer(){
}

/******************************************************************************
              To Send uplink data to the LoRaWAN network                      *
******************************************************************************/

/*! \fn void putDataToSend(uint8_t *value, uint8_t type)
    \brief  Store the data to send to the network  
    \param  uint8_t *value The data to store, data[0] contains the size of the data 
    \param  uint8_t type The type of the data
    
    \retval void
    
    This function put the data to be send uplink to the LoRaWAN network
*/
void Buffer::putDataToSend(uint8_t *value, uint8_t type) {
  
    uint8_t nextIndex;
    nextIndex = (value[0] + 1);//value[0] contains the size of the data + 1 for the type
    if((nextIndex + dataToSend_Index) < dataToSend_Size){
      dataToSend[dataToSend_Index++]= type;
      memcpy(dataToSend + dataToSend_Index, value, (value[0] + 1));
      dataToSend_Index = dataToSend_Index + nextIndex;
      USB.print("Buffer: data stored correctly, stored data = ");
      for(uint8_t i = 0; i < dataToSend_Index; i++){
        USB.print(dataToSend[i], HEX);  
        USB.print(":");  
      }
      USB.println("");
      USB.print("  -Index ");
      USB.println(dataToSend_Index, DEC);
      USB.println("");
    }else{
      USB.print("The buffer is full, there is no more space ");
    }
}

/*! \fn void getDataToSend()
    \brief Return the data to be send to the network
    \param   void
    \retval void
  
    This function returns the data to be sent through the uplink to the LoRaWAN network
*/
uint8_t* Buffer::getDataToSend(){
  
    uint8_t currentIndex = 0;
    uint8_t elemtLenght;
    USB.println(F("________Data stored in the buffer to send:"));
    while(currentIndex < dataToSend_Index){
      USB.print(F(" Element Type: "));
      USB.print( dataToSend[currentIndex++], DEC );
      elemtLenght = dataToSend[currentIndex++];
      USB.print(F(", Element Lenght: "));
      USB.print( elemtLenght, DEC );
      USB.print(F(", Element Data: "));
      for(uint8_t i=0; i<elemtLenght; i++){
        USB.print( dataToSend[currentIndex++], HEX );
        USB.print(F(" "));
      }
      USB.println(F(""));
    } 
    return dataToSend;
}

/*! \fn void getDataToSendSize()
    \brief Returns the size of the data stored in the buffer.
    \param   void
    \retval void
  
*/
uint8_t Buffer::getDataToSendSize(){
  return dataToSend_Index;
}

/*! \fn void clearDataToSend()
    \brief  Reset the DataToSend
    \param   void
    \retval void
    
    This function resets the buffer with the uplink data that has already been sent to the network.
*/
void Buffer::clearDataToSend(){
    memset(dataToSend, 0x00, sizeof(dataToSend));
    dataToSend_Index = 0;
}


/******************************************************************************
          To receive downlink data from the LoRaWAN network                   *
******************************************************************************/

/*! \fn void putNetworkReceivedData(char *value)
    \brief  Store the data received from the downlink 
    \param   void
    \retval char *value Pointer to the received data
    
    This function Store the data received from the downlink.
    The data is received in ASCII and store as uint8_t
    Only two cases are considered:
       Case 1: Establish the time to send. Data received-->type = 1 H1 H2 M1 M2
       Case 2: Establish the sensor value to be send. Data received-->type = 2 S1 S2 S3 S4 S5 S6 S7 S8 S9 S10 S11
       *Note: Every item is received as 2 element. Example: type = 1-->31(HEX),51(ASCII)(value[0] and value[1])
       Therefore only odd positions are taken into account 
*/                                            
void Buffer::putNetworkReceivedData(char *value){
    uint8_t type;
    type = (uint8_t)(value[1]-48);
    USB.print("Message type: ");
    USB.println(type, DEC);
    
    switch(type){
    case 1://Establish the time to send 
      networkReceivedData[0] = type;
      networkReceivedData[1] = (uint8_t)((value[3]-48) * 10) + (value[5]-48);//hours
      networkReceivedData[2] = (uint8_t)((value[7]-48) * 10) + (value[9]-48);//minutes
    break;
    
    case 2://Establish the sensors value to be send
      networkReceivedData[0] = type;
      for(uint8_t i = 0; i < 11; i++){
      networkReceivedData[i+1] = (uint8_t) value[((i*2)+3)] - 48;
      }
    break;
    
    default:
      networkReceivedData[0] = 0;
    break;
  }
}

/*! \fn void getNetworkReceivedData(uint8_t index)
    \brief  Return the network received data
    \param  uint8_t index
    \retval void
    This function returns the data received from the network in the index position.
*/
uint8_t Buffer::getNetworkReceivedData(uint8_t index){
   uint8_t value;
   value = networkReceivedData[index];
   return value;
}

/*! \fn void clearNetworkReceivedData()
    \brief  Reset the receive Network Data
    \param   void
    \retval void
    
    This function reset the data to receive dowlink from the LoRaWAN network  
*/
void Buffer::clearNetworkReceivedData(){
    memset(networkReceivedData, 0x00, sizeof(networkReceivedData));
}
