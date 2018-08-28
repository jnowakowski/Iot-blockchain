#include <WiFi.h>
#include <Web3.h>
#include <Contract.h>

#define USE_SERIAL Serial
#define ENV_SSID     "Krakow"
#define ENV_WIFI_KEY "8.warszawa.8"
const string CONTRACT_ADDRESS = "0x13249c8503f3b4a9974f02cc61254df62d6e8a94";
const char* ACCOUNT_ADDRESS = "0xdce18b3f54c1ef557d48b98d3521aeda35ccc848";
const string INFURA_HOST = "ropsten.infura.io";
const string INFURA_PATH = "/YOUR_PATH";
int relayPin = 5;
int buzzerPin = 4;
Web3 web3(&INFURA_HOST, &INFURA_PATH);
WiFiServer server(80);

void eth_call();

void setup() {
    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, LOW);
    pinMode(buzzerPin, OUTPUT);
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
    server.begin();
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
    string func = "isActive()";
    string param = contract.SetupContractData(&func);
    string result = contract.Call(&param);
    char value = result[result.length()-4];
    if(value == '1')
    {
      digitalWrite(relayPin, HIGH);
      for(int i = 0; i < 1000; i++)
      {
        if ( i % 2 == 0) digitalWrite(buzzerPin, HIGH);
        else digitalWrite(buzzerPin, LOW);
        delay(1);
      }
      USE_SERIAL.println("Switch ON");
    }
    else
    {
      digitalWrite(relayPin, LOW);
      USE_SERIAL.println("Switch OFF");
    }

    WiFiClient client = server.available();
    if (client) {
      USE_SERIAL.println(" ## web client conneted.");
      client.println("HTTP/1.1 200 OK");
      client.println("Content-type:text/html");
      client.println("Connection: close");
      client.println("");
      client.println("Contract address: %s", CONTRACT_ADDRESS);
      client.println("Contract value: %s", value);
    }
    client.stop();
}
