# diddy simulator

why would you want to play this 😭🙏

## demo

[![demo vid](https://cloud-jbldiszlu-hack-club-bot.vercel.app/0img_20241216_213336836_hdr_2.jpg)](https://youtube.com/watch?v=19FiCwYzgO4)

## wiring

### display (st7735)

- CS -> GPIO5
- DC -> GPIO17
- RES -> GPIO16
- SDA -> GPIO23
- SCL -> GPIO18
- VCC -> 5V
- GND -> GND

### left push button

1 pin to GND and the other to GPIO25

### right push button

1 pin to GND and the other to GPIO26

### piezo buzzer

negative pin to GND and positive to GPIO33
