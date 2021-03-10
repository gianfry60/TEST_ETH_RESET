#include <Arduino.h>
#include "defines.h"

#include <MySQL_Generic_Ethernet.h>

// Select the static Local IP address according to your local network
IPAddress ip(192, 168, 101, 83);

IPAddress server_ip(5, 150, 142, 221);
int server_port = 3306;
char user[] = "seeed";                        // MySQL user login username
char password[] = "support01$$";              // MySQL user login password
char default_database[] = "acap100_test_eth"; //"test_DB";
char default_table[] = "gate";                //"test_Table";

const char QUERY_TEST[] = "UPDATE acap100_test_eth.gate SET VerFW = 'TestETH' WHERE IdGate = 1";

IPAddress myIP(192, 168, 101, 83);
IPAddress myMASK(255, 255, 255, 0);
IPAddress myDNS(8, 8, 8, 8);
IPAddress myGW(192, 168, 101, 250);

MySQL_Connection conn((Client *)&client);

#define ResetETH 4 // define reset W5500 pin

void UpdateFW()
{
    Serial.println("====================================================");
    Serial.println("              Running UPDATE FW                     ");
    Serial.println("====================================================");
    // Initiate the query class instance
    MySQL_Query query_memUP = MySQL_Query(&conn);
    Serial.print("queryVerFW=  ");
    Serial.println(QUERY_TEST);
    if (!query_memUP.execute(QUERY_TEST))
    {
        Serial.println("====================================================");
        Serial.println(("               Query UPDATE FW error "));
        Serial.println("====================================================");
        query_memUP.close();
        return;
    }
    else
    {
        Serial.println("Quering UPDATE FW OK");
        query_memUP.close();
    }
}

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ;

    pinMode(ResetETH, OUTPUT);
    digitalWrite(ResetETH, LOW);
    delay(100);
    digitalWrite(ResetETH, HIGH);
    delay(500);

    Serial.println("\nStarting Connect on " + String(BOARD_NAME) + ", with " + String(SHIELD_TYPE));

    MYSQL_LOGERROR(F("========================================="));
    MYSQL_LOGERROR(F("Default SPI pinout:"));
    MYSQL_LOGERROR1(F("MOSI:"), MOSI);
    MYSQL_LOGERROR1(F("MISO:"), MISO);
    MYSQL_LOGERROR1(F("SCK:"), SCK);
    MYSQL_LOGERROR1(F("SS:"), SS);
    MYSQL_LOGERROR(F("========================================="));

    MYSQL_LOGERROR3(F("Board :"), BOARD_NAME, F(", setCsPin:"), USE_THIS_SS_PIN);

    Ethernet.init(PIN_SPI_SS);

    // start the ethernet connection and the server:
    uint16_t index = millis() % NUMBER_OF_MAC;
    // Use Static IP
    Ethernet.begin(mac[0], myIP, myDNS, myGW, myMASK);

    Serial.print("Using mac index = ");
    Serial.println(index);

    Serial.print("Connected! IP address: ");
    Serial.println(Ethernet.localIP());

    Serial.print("Connecting to SQL Server @ ");
    Serial.print(server_ip);
    Serial.println(String(", Port = ") + server_port);
    Serial.println(String("User = ") + user + String(", PW = ") + password);
}

void loop()
{
    Serial.println("Connecting...");

    //if (conn.connect(server_ip, server_port, user, password))
    if (conn.connectNonBlocking(server_ip, server_port, user, password) != RESULT_FAIL)
    {
        Serial.println("Query run");
        UpdateFW();
        Serial.println("Closing connection...");
        conn.close(); // close the connection
        do
        {
            client.flush();
        } while (client.available());
        client.stop();
        delay(500);
        digitalWrite(ResetETH, LOW);
        delay(500);
        digitalWrite(ResetETH, HIGH);
        delay(1000);
        Ethernet.init(USE_THIS_SS_PIN);
        delay(1000);
        Ethernet.begin(mac[0], myIP, myDNS, myGW, myMASK);
        EthernetClient client;
    }
    else
    {
        Serial.println("\nConnect failed. Trying again on next iteration.");
    }

    Serial.println("\nSleeping...");
    Serial.println("================================================");

    delay(10000);
}
