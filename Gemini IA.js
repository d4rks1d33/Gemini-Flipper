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
        sendSerialCommand(name, 0);
    } else {
        dialog.message("Error", "No name entered.");
        mainMenu();
    }
}

function saveAPToFile(ssid, password) {
    let apData = ssid + "//" + password + "\n";
    storage.write(path, apData);
}

function connectToNewAP() {
    sendSerialCommand("no", -1);
    delay(500);
    let newAP = promptForText("Enter SSID/Password", "");
    if (newAP !== undefined) {
        sendSerialCommand(newAP, 0);
        let inputParts = splitInput(newAP, "//");
        if (inputParts.length === 2) {
            saveAPToFile(inputParts[0], inputParts[1]);

            dialog.message("Success", "AP credentials saved successfully.");
        } else {
            dialog.message("Error", "Invalid format. Please use SSID//password.");
        }
    } else {
        dialog.message("Error", "No name or credentials entered.");
        mainMenu();
    }
}

function connectToSavedAP() {
    sendSerialCommand("yes", -1);

    if (storage.exists(path)) {
        let savedAPData = storage.read(path);
        let apString = arraybuf_to_string(savedAPData);
        
        sendSerialCommand(apString, 0);
        receiveSerialData(0);
    } else {
        dialog.message("Error", "No saved AP found. Please connect to a new AP first.");
        mainMenu();
    }
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
    dialog.message("Help", "This is an app to interact with Google Gemini IA using the esp32");
    mainMenu();
}

function mainMenu() {
    submenu.setHeader("Gemini IA");
    submenu.addItem("Set your name", 0);
    submenu.addItem("Connect to new AP", 1);
    submenu.addItem("Connect to saved AP", 2);
    submenu.addItem("Start Chatting", 3);
    submenu.addItem("Help", 4);

    let result = submenu.show();

    if (result === 0) {
        setName();
    }

    if (result === 1) {
        connectToNewAP();
    }

    if (result === 2) {
        connectToSavedAP();
    }

    if (result === 3) {
        startChatting();
    }

    if (result === 4) {
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
