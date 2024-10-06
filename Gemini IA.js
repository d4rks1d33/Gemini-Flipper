let submenu = require("submenu");
let serial = require("serial");
let keyboard = require("keyboard");
let textbox = require("textbox");
let dialog = require("dialog");
let storage = require("storage");

serial.setup("usart", 115200);

let shouldexit = false;
let path = "/ext/apps_data/gemini_ia/SavedAPs.txt";

function sendSerialCommand(command, menutype) {
    serial.write(command);
    if (menutype !== -1) {
        receiveSerialData(menutype);
    }
}

function receiveSerialData(menutype) {
    textbox.setConfig("end", "text");
    textbox.show();

    serial.readAny(0);

    while (textbox.isOpen()) {
        let rx_data = serial.readAny(250);
        if (rx_data !== undefined) {
            textbox.addText(rx_data);
        }
    }
    serial.write("stop");

    if (menutype === 0) {
        mainMenu();
    }

    if (menutype === 1) {
        startChatting();
    }
}

function trimString(str) {
    let start = 0;
    let end = str.length - 1;

    while (start <= end && (str[start] === ' ' || str[start] === '\n' || str[start] === '\r')) {
        start++;
    }

    while (end >= start && (str[end] === ' ' || str[end] === '\n' || str[end] === '\r')) {
        end--;
    }

    return str.slice(start, end + 1);
}

function stringToInt(str) {
    let num = 0;
    let isNegative = false;
    let start = 0;

    if (str[0] === '-') {
        isNegative = true;
        start = 1;
    }

    for (let i = start; i < str.length; i++) {
        let digit = str.charCodeAt(i) - 48;
        if (digit >= 0 && digit <= 9) {
            num = num * 10 + digit;
        } else {
            break;
        }
    }

    return isNegative ? -num : num;
}

function splitInput(input, delimiter) {
    let index = input.indexOf(delimiter);
    if (index === -1) {
        return [input];
    }
    
    let part1 = input.slice(0, index);
    let part2 = input.slice(index + delimiter.length);
    
    return [part1, part2];
}

function sendAPIKey() {
    let apiKeyPath = "/ext/apps_data/gemini_ia/key.txt";
    if (storage.exists(apiKeyPath)) {
        let apiKey = storage.read(apiKeyPath);
        let apiKeyString = arraybuf_to_string(apiKey);
        sendSerialCommand(apiKeyString, -1);
    } else {
        dialog.message("Error", "API key not found.");
    }
}

function promptForText(header, defaultValue) {
    keyboard.setHeader(header);
    return keyboard.text(100, defaultValue, true);
}

function setName() {
    let name = promptForText("Enter your name", "");
    if (name !== undefined) {
        sendSerialCommand(name, -1);

        delay(5000);

        if (storage.exists(path)) {
            let savedAPData = storage.read(path);
            let apString = arraybuf_to_string(savedAPData);

            let apLines = [];
            let currentLine = "";

            for (let i = 0; i < apString.length; i++) {
                let char = apString[i];
                if (char === '\n') {
                    apLines.push(trimString(currentLine));
                    currentLine = "";
                } else {
                    currentLine += char;
                }
            }

            if (currentLine.length > 0) {
                apLines.push(trimString(currentLine));
            }

            let apCombined = "";
            if (apLines.length === 1) {
                apCombined = apLines[0];
            } else if (apLines.length > 1) {
                for (let i = 0; i < apLines.length; i++) {
                    apCombined += apLines[i];
                    if (i < apLines.length - 1) {
                        apCombined += ", ";
                    }
                }
            }

            if (apCombined.length > 0) {
                sendSerialCommand(apCombined, -1);
            }

            receiveSerialData(0);

        } else {
            dialog.message("Error", "No saved APs found. Connect manually to a new AP (see output)");
            receiveSerialData(0);
        }

    } else {
        receiveSerialData(0);
    }
}

function saveAPToFile(ssid, password) {
    let apData = ssid + "//" + password + "\n";
    let updatedData = "";

    if (storage.exists(path)) {
        let savedAPData = storage.read(path);
        let apString = arraybuf_to_string(savedAPData);

        let apLines = [];
        let currentLine = "";

        for (let i = 0; i < apString.length; i++) {
            let char = apString[i];
            if (char === '\n') {
                apLines.push(trimString(currentLine));
                currentLine = "";
            } else {
                currentLine += char;
            }
        }

        if (currentLine.length > 0) {
            apLines.push(trimString(currentLine));
        }

        let ssidExists = false;

        for (let i = 0; i < apLines.length; i++) {
            let line = trimString(apLines[i]);

            if (line.length === 0) continue;

            let existingAP = splitInput(line, "//")[0];

            if (existingAP === ssid) {
                ssidExists = true;
                updatedData += apData;
            } else {
                updatedData += line + "\n";
            }
        }

        if (!ssidExists) {
            updatedData += apData;
        }

        storage.write(path, updatedData);
    } else {
        storage.write(path, apData);
    }
}

function connectToNewAP() {
    let ssid = promptForText("Enter SSID", "");
    if (ssid === undefined || trimString(ssid).length === 0) {
        dialog.message("Error", "No SSID entered.");
        mainMenu();
        return;
    }

    let tempSSID = trimString(ssid);

    sendSerialCommand(tempSSID, -1);

    delay(500);

    let password = promptForText("Enter Password", "");
    if (password === undefined || trimString(password).length === 0) {
        dialog.message("Error", "No password entered.");
        mainMenu();
        return;
    }

    let tempPassword = trimString(password);

    sendSerialCommand(tempPassword, -1);

    delay(500);

    receiveSerialData(0);

    let formattedAP = tempSSID + "//" + tempPassword;
    saveAPToFile(tempSSID, tempPassword);
}

function startChatting() {
    let chatMessage = promptForText("Enter message", "");
    if (chatMessage === undefined) {
        mainMenu();
        return;
    }
    sendSerialCommand(chatMessage, 1);
}

function help() {
    dialog.message("Help", "App to interact with Gemini IA using the esp32");
    mainMenu();
}

function mainMenu() {
    submenu.setHeader("Gemini IA");
    submenu.addItem("Set your name", 0);
    submenu.addItem("Connect to new AP", 1);
    submenu.addItem("Start Chatting", 2);
    submenu.addItem("Help", 3);

    let result = submenu.show();

    if (result === 0) {
        setName();
    }

    if (result === 1) {
        connectToNewAP();
    }

    if (result === 2) {
        startChatting();
    }

    if (result === 3) {
        help();
    }

    if (result === undefined) {
        shouldexit = true;
    }
}

function mainLoop() {
    sendAPIKey();
    while (!shouldexit) {
        mainMenu();
        let confirm = dialog.message("Exit", "Press OK to exit, Cancel to return.");
        if (confirm === 'OK') {
            sendSerialCommand('stop', 0);
            break;
        }
    }
}

function arraybuf_to_string(arraybuf) {
    let string = "";
    let data_view = Uint8Array(arraybuf);
    for (let i = 0; i < data_view.length; i++) {
        string += chr(data_view[i]);
    }
    return string;
}

mainLoop();
