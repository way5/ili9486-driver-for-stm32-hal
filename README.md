# ILI9486 driver for STM32 HAL
| ![disclaimer](https://friconix.com/png/fi-ensuxs-warning-solid.png) | This is not a final release. Please refer to [issues](https://github.com/way5/ili9486-driver-for-stm32-hal/issues) :beetle: if any error found or improvement was made. |
|---|---|

## **+ Components**
    - ILI9486(L) based display
    - Touch screen controller XPT2046

## **+ Setup**
In my case STM32 uses FSMC 16bit and SPI via DMA to communicate with the display.
There are few things you need to do before the startup:

1.Since the driver uses HAL, make sure you already defined FSMC, SPI and PENIRQ pins in CubeMX.

2.There are functions that you need to define yourown. I'd recommend you doing this inside of spi.c file.

    - HAL_SPI_DC_SELECT() - SPI_NSS pin LOW
    - HAL_SPI_DC_DESELECT() - SPI_NSS pin HIGH
    - HAL_SPI_TxRx() - Transfer and receive data

3.Once PENIRQ pin interrupt happens `touchEventHandler()` should be called. You need to change pin name and group to ones you've set in CubeMX.

4.If you plan to use built in fonts check header file and uncomment `USE_DRIVER_FONTS`.

## **+ Example**

**main.cpp**
---

```C
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "spi.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ili9486_fsmc_16b.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
ILI9486_Display_Driver ddr;
/* USER CODE END PV */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void PENIRQ_Callback_IT() {
    ddr.touchEventHandler();
}
/* USER CODE END 0 */

int main(void)
{
     /* USER CODE BEGIN 2 */
    ddr.begin();
    // etc...
}
```

## **+ Credits**

1. Project uses some functions from [Adafruit-GFX](https://github.com/adafruit/Adafruit-GFX-Library) library.
2. Icons provided by **friconix. com**