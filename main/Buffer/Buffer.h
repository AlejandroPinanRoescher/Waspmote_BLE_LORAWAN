/*! \def _BUFFER_h
    \brief The Buffer flag
 */
#ifndef _BUFFER_h
#define _BUFFER_h

/******************************************************************************
 * Includes
 ******************************************************************************/

#include <inttypes.h>
/******************************************************************************
 * Class
 ******************************************************************************/
#define dataToSend_Size 60
#define ReceivedData_Size 30
//! Buffer Class
/*!
  defines all the variables and functions used 
 */
class Buffer{

  /// private methods //////////////////////////
private:

   
  /// public methods and attributes ////////////
public:


    Buffer();
    
    ~Buffer();
    
/******************************************************************************
              To Send uplink data to the LoRaWAN network                      *
******************************************************************************/
    
    uint8_t dataToSend_Index;
    
    uint8_t dataToSend[dataToSend_Size];
    
    void putDataToSend(uint8_t *value, uint8_t type);
    
    uint8_t* getDataToSend();
    
    uint8_t getDataToSendSize();
    
    void clearDataToSend();

/******************************************************************************
          To receive downlink data from the LoRaWAN network                   *
******************************************************************************/
    
    uint8_t networkReceivedData[ReceivedData_Size];
    
    void putNetworkReceivedData(char *value);

    uint8_t getNetworkReceivedData(uint8_t index);
    
    void clearNetworkReceivedData();
    
#endif 

};
