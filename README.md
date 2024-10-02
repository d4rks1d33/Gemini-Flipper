# Gemini-Flipper

Currently in development stage, here I'm going to upload the .js code that I currently use so that anyone who wants to join and help improve it can make their PRs and also for those C developers who want to help create the native Flipper app and be accessible by everyone from https://lab.flipper.net/apps

Important: The javascript app now doesn't work with the new firmware, I need to rewrite it to adapt the new changes 

## How it is work

Currently the code takes the api key in plain text stored in:

"/ext/apps_data/gemini_ia/key.txt" 

(You can get the Gemini Api Key using this link: https://ai.google.dev/gemini-api/docs/api-key?hl=es-419 You only need a Gmail account)

it reads the .txt file and sends it via serial command to the esp32 as soon as the app is initialized (it would be nice to implement some kind of encryption to the api key but it is important that it is sent in plain text and unencrypted to the esp32 because this is how the firmware can read and implement it)

~~About saved APs: when the user connects to a new AP the information needed to connect again is stored in "/ext/apps_data/gemini_ia/SavedAPs.txt" I need to improve the way it saves the information because it overwrites the other AP you have saved~~ Fixed (Sidenote: To send the SSID and password must be send in 1 single command in this format "SSID//password" <-- this is how the ESP32 can handle the conection (I tried a lot to manage this in 2 steps but the ESP32 doesn't want to help me lol)

~~I added a menu to show stored APs so the user just need to select to one that wants to connect to~~ Now if it detects a known network it connects automatically, otherwise it will ask the user to enter the SSID and password of the network they want to connect to

I added the source code of the new firmware for the esp32 and also the pre-compile binary for ESP-Flasher app (You can find the binary in the relases section)

## How to flash the firmware using ESP-Flasher

Go to "Manual flash"

Place the Bootloader binary into bootloader (0x1000)

Place the partitions binary into Part Table (0x8000)

Place the firmware into firmwareA (0x10000)

Flash it (you need to reboot the board for start using the firmware)

![Captura de pantalla 2024-09-28 213108](https://github.com/user-attachments/assets/c2e8a0a4-3865-452a-831f-d7200ae45084)

![Captura de pantalla 2024-09-28 213123](https://github.com/user-attachments/assets/031063aa-c4bf-4fbe-baa6-745573cc8411)


## TO DO

- [ ] Rewrite javascript app for work with the new firmware

- [ ] Javascript: Change the menu to only leave the option to "connect to a new AP" the "saved APs" option is obsolete, now the list of saved APs is sent in the background to the ESP32 after setting the name so that it detects if any of the networks is available and connects automatically, otherwise it will ask the user to enter the username and password of the AP to which they want to connect and will save the data of this new AP for the future

- [x] Rewrite the ESP32 firmware now should work with all the ESP32 models using ESP-TOOLS

- [x] improve the handling of how the APs are saved, currently it saves them but rewrites the previous one

- [x] improve the handling of how the saved APs are sent to the esp32 to avoid re-entering the SSID and password (Improving the previous point this should solve itself)

- [x] load the firmware binary for the esp32 (nothing to do here just load the binary)

## Screenshots

![image](https://github.com/user-attachments/assets/3878b4a2-223d-4d23-b395-2d25cf710fed)

![image](https://github.com/user-attachments/assets/777e2d55-f9fd-4c63-bb47-450b020b80e0)

## Native app

The native app is in development.  Currently it just shows the main menu and then each option shows "Under construction".

## Contributors

(I guess this needs to be updated on GitHub, I'll leave this comment here just in case until the list of contributors is properly updated. I want everyone who contributes to this repository to have the recognition they deserve for their time and effort helping with this project.)

<a href="https://github.com/d4rks1d33/Gemini-Flipper/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=d4rks1d33/Gemini-Flipper&max=50&columns=4&anon=1" />
</a>
