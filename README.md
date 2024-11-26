# NXQ1TXH5 I2C driver
Platform-independent I2C driver for the NXQ1TXH5 controller and driver IC for a 5 V Qi-certified/compliant low-power wireless charger. 
This driver facilitates communication between a microcontroller and the NXQ1TXH5, allowing developers to configure and monitor the IC's functionality easily.

# Quick start
* Mention the header:
```C
#include "nxq1txh5.h"
```
* Declare the device handle:
```C
nxq1txh5_t nxq;
```
* Initialize I2C interface:
```C
I2Cx_Init();
```
* Provide platform depended implementations for functions below in the `nxq1txh5_ifc.c`:
```C
nxq1txh5_status_t NXQ1TXH5_I2Cx_Receive(uint8_t regAddress, uint16_t *data);
nxq1txh5_status_t NXQ1TXH5_I2Cx_Transmit(uint8_t regAddress, uint16_t *data);
```
* Link the functions above to a device handle:
```C
NXQ1TXH5_Link(&nxq, NXQ1TXH5_I2Cx_Receive, NXQ1TXH5_I2Cx_Transmit);
```

# Examples
* [STM32](platform/STM32F103C8T6/Core/Src/main.c)
