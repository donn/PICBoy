# Development Logbook

## Graphics Subsystem
### 2017

#### November 30
    - Model: `ST7735B1` ([ref](http://www.displayfuture.com/Display/datasheet/controller/ST7735.pdf))
        - It might also be `S6D02A1`
    - Interface library somewhere?
        - [Adafruit ST7735 lib](https://github.com/adafruit/Adafruit-ST7735-Library)
        - [Samsung S6D02A1 lib](https://github.com/sumotoy/TFT_S6D02A1)
            - Seems confusing though. "No SPI transaction"??

#### December 18
Adapting code from [this guide](http://www.instructables.com/id/Arduino-TFT-display-and-font-library/).

| PIN | \# |
| --- | -- |
| CS  | 9  |
| DC  | 8  |
| RST | 7  |

