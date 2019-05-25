# NXP-KV58

## PIN config

| JTAG      | Pin       |
| :---:     | :----:    |  
| TMS       | PTA3      | 
| TCLK      | PTA0      |
| TDO       | PTA2      |
| TDI       | PTA1      |

| OSC      | Pin       |
| :---:    | :----:    |  
| EXTAL0   | PTA18     | 

| LCD       | Pin       |
| :---:     | :----:    |  
| LCD_DC    | PTC19     | 
| LCD_RST   | PTC18     |
| LCD_SDA   | PTC17     |
| LCD_SCL   | PTC16     |

| PWM           | Pin       | Description       |
| :---:         | :----:    | :---              |
| PWM0 SM0 CHA  | PTD2      | ESC signal of controller |
| PWM0 SM0 CHB  | PTD3      | Steering gear signal of controller |
| PWM0 SM1 CHA  | PTD4      | Controller signal to take over MCU |
| PWM0 SM1 CHB  | PTD5      | Reserved controller signal |
| PWM0 SM2 CHA  | PTD0      | Output for ESC |
| PWM0 SM2 CHB  | PTD1      | Output for steering gear |
| PWM1 SM0 CHA  | PTC14     | Output for WS2812 |

| ENC       | Pin   | Description   |
| :---:     | :---: | :---          |
| ENC PhA   | PTA25 | ENC Phase A | 
| ENC PhB   | PTA24 | ENC Phase B | 

| UART      | Pin   | Description   |
| :---:     | :---: | :---          |
| UART0 RX  | PTA15 | Connect to PC |
| UART0 TX  | PTA14 | Connect to PC |
| UART1 RX  | PTE1  | Debug logging |
| UART1 TX  | PTE0  | Debug logging |
| UART2 RX  | PTE17 | TOF |
| UART2 TX  | PTE16 | TOF |
| UART3 RX  | PTE5  | Reserved |
| UART3 TX  | PTE4  | Reserved |
| UART4 RX  | PTE25 | Reserved |
| UART4 TX  | PTE24 | Reserved |
| UART5 RX  | PTE9  | Reserved |
| UART5 TX  | PTE8  | Reserved |

| IIC       | Pin   | Description   |
| :---:     | :---: | :---          |
| IIC0 SCL  | PTE19 | Reserved      |
| IIC0 SDA  | PTE18 | Reserved      |

| SPI       | Pin   | Description   |
| :---:     | :---: | :---          |
| SPI0 PCS0 | PTC4  | Reserved      |
| SPI0 SCLK | PTC5  | Reserved      |
| SPI0 SDI  | PTC7  | Reserved      |
| SPI0 SDO  | PTC6  | Reserved      |

| GPIO      | Pin   | Mux           | Description   |
| :---:     | :---: | :---          | :---          |
| GPIOB0    | PTB0  | HSADC0/IIC0   | Switch0 |
| GPIOB1    | PTB1  | HSADC0/IIC0   | Switch1 |
| GPIOB2    | PTB2  | HSADC0/IIC0   | Switch2 |
| GPIOB3    | PTB3  | HSADC0/IIC0   | Switch3 |
| GPIOB4    | PTB4  | ADC0          |
| GPIOB5    | PTB5  | ADC0          |
| GPIOB6    | PTB6  | HSADC1        |
| GPIOB7    | PTB7  | HSADC1        |
| GPIOB10   | PTB10 | SPI1          |
| GPIOB11   | PTB11 | SPI1          |
| GPIOB16   | PTB16 | SPI1          |
| GPIOB17   | PTB17 | SPI1          |
| GPIOB20   | PTB20 | SPI2          |
| GPIOB21   | PTB21 | SPI2          |
| GPIOB22   | PTB22 | SPI2          |
| GPIOB23   | PTB23 | SPI2          |







