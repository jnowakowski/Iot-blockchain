#include <WiFi.h>
#include <Web3.h>
#include <Contract.h>

#define USE_SERIAL Serial
#define ENV_SSID     "Krakow"
#define ENV_WIFI_KEY "8.warszawa.8"
const string CONTRACT_ADDRESS = "0xb9bb5ad4fb18026435cfc377802f4529e47cc356";
const char* ACCOUNT_ADDRESS = "0xdce18b3f54c1ef557d48b98d3521aeda35ccc848";
const string INFURA_HOST = "ropsten.infura.io";
const string INFURA_PATH = "/YOUR_PATH";

Web3 web3(&INFURA_HOST, &INFURA_PATH);

void eth_call_example();

void setup() {
    USE_SERIAL.begin(115200);

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFi.begin(ENV_SSID, ENV_WIFI_KEY);

    // attempt to connect to Wifi network:
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        // wait 1 second for re-trying
        delay(1000);
    }

    USE_SERIAL.println("Connected");
}

void loop() {
    // put your main code here, to run repeatedly:
    delay(2000);
    eth_call();
}

void eth_call() {
    Contract contract(&web3, &CONTRACT_ADDRESS);
    strcpy(contract.options.from, ACCOUNT_ADDRESS);
    strcpy(contract.options.gasPrice,"2000000000000");
    contract.options.gas = 5000000;
    string func = "readUsersLog()";
    string param = contract.SetupContractData(&func);
    string result = contract.Call(&param);
    USE_SERIAL.println(result.c_str());
}
