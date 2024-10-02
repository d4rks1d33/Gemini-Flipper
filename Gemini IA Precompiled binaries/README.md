# How to flash

Go to ESP-TOOLS

Enter esp32 into bootloader mode

Go to "Manual flash"

Place the Bootloader binary into bootloader (0x1000)

Place the partitions binary into Part Table (0x8000)

Place the firmware into firmwareA (0x10000)

Flash it in slow mode or fast mode both works for me (you need to reboot the board for start using the firmware)

Sidenote: Please select the .zip corresponding to your ESP32 model to ensure it is compatible

# Screenshots

![image](https://github.com/user-attachments/assets/879b8621-29d0-4c55-85ff-f44fa94e1d4a)

![image](https://github.com/user-attachments/assets/3ee4cfea-37d1-413d-9bfe-25451e251d07)
