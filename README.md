# NXQ1TXX5 I2C driver
Platform-independent I2C driver for the NXQ1TXX5 controller IC series for a 5 V Qi-certified/compliant low-power wireless charger. 
This driver facilitates communication between a microcontroller and the NXQ1TXX5, allowing developers to configure and monitor the IC's functionality easily.

## Features
* Charge process ON/OFF control
* Partially possible to get the current device operation
* Digital supply voltage (Vddd) level control
* Supply voltage (Vdd) readings
* Temperature readings from the internal device sensor
* NTC pin voltage readings

## Notes
Majority of device parameters are valid only if device is in charge mode (either Qi receiver is placed to the coil or device is in digital ping mode)

## Quick start
* Mention the header:
```C
#include "nxq1txx5.h"
```
* Declare the device handle:
```C
nxq1txx5_t nxq;
```
* Initialize I2C interface:
```C
I2Cx_Init();
```
* Provide platform depended implementations for functions below in the `nxq1txx5_ifc.c`:
```C
/* Blocking */
nxq1txx5_ifc_status_t NXQ1TXX5_I2Cx_Receive(uint8_t regAddress, uint16_t *data);
nxq1txx5_ifc_status_t NXQ1TXX5_I2Cx_Transmit(uint8_t regAddress, uint16_t *data);

/* IT */
nxq1txx5_ifc_status_t NXQ1TXX5_I2Cx_Receive_IT(uint8_t regAddress, uint16_t *data);
nxq1txx5_ifc_status_t NXQ1TXX5_I2Cx_Transmit_IT(uint8_t regAddress, uint16_t *data);

/* DMA */
nxq1txx5_ifc_status_t NXQ1TXX5_I2Cx_Receive_DMA(uint8_t regAddress, uint16_t *data);
nxq1txx5_ifc_status_t NXQ1TXX5_I2Cx_Transmit_DMA(uint8_t regAddress, uint16_t *data);
```
* Use `NXQ1TXX5_Link` function to switch between data transfer modes:
```C
/* Blocking */
NXQ1TXX5_Link(&nxq, NXQ1TXX5_I2Cx_Receive, NXQ1TXX5_I2Cx_Transmit);

/* IT */
NXQ1TXX5_Link(&nxq, NXQ1TXX5_I2Cx_Receive_IT, NXQ1TXX5_I2Cx_Transmit_IT);

/* DMA */
NXQ1TXX5_Link(&nxq, NXQ1TXX5_I2Cx_Receive_DMA, NXQ1TXX5_I2Cx_Transmit_DMA);
```
* Place driver callback in the RX and TX IRQs in case of non-blocking usage:
```C
void I2C_TxCpltCallback()
{
    NXQ1TXX5_Callback(&nxq);
}

void I2C_RxCpltCallback()
{
    NXQ1TXX5_Callback(&nxq);
}
```

## TODO
According to UM11152 it is also possible to request the following parameters from device:
* `freq` - Operating frequency (Hz) of the charger.
* `dut` - Duty cycle (%) of the full bridge in; generally, it is at 50 %.
* `Icrms` - RMS current (mA) through the coil. The calculation of this current is based on the
voltage that is measured on pin VSEN.
* `ptx` - Input power (mW) of the full bridge. It excludes the dissipation in LEDs.
* `prx` - Received power (mW) that the receiver reports. This power is not the power received in
the load, but the power received in the magnetic field.
* `ppad` - Calculated loss in the charger (mW). ppad = FOD_E * Icrms2
 / 1000. When the value of FOD_E is not correct, the calculated loss is wrong.
* `pfor` - Calculated foreign power (mW). pfor = ptx ? ppad ? prx.
* `qidata` - \<headerid> \<data> \<checksum>

## Examples
* [STM32](platform/STM32F103C8T6/Core/Src/main.c)
