#include <Arduino.h>
#include <MySQL_Generic_Ethernet.h>
#include "defines.h"

IPAddress server_ip(5, 150, 142, 221);
int server_port = 3306;
char user[] = "seeed";                        // MySQL user login username
char password[] = "support01$$";              // MySQL user login password
char default_database[] = "acap100_test_eth"; //"test_DB";
char default_table[] = "gate";                //"test_Table";

const char QUERY_TEST[] = "UPDATE acap100_test_eth.gate SET VerFW = 'TestETH' WHERE IdGate = 1";

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
    {
    }; // wait for serial port to connect
    pinMode(ResetETH, OUTPUT);
    digitalWrite(ResetETH, LOW);
    delay(100);
    digitalWrite(ResetETH, HIGH);
    delay(500);

    MYSQL_LOGERROR3(F("Board :"), BOARD_NAME, F(", setCsPin:"), USE_THIS_SS_PIN);


    Ethernet.init(USE_THIS_SS_PIN);

    IPAddress myIP;
    IPAddress myMASK;
    IPAddress myDNS;
    IPAddress myGW;

    // myIP.fromString(String("192.168.101.83"));
    myIP.fromString(String("192.168.0.83"));

    myMASK.fromString(String("255.255.255.0"));
    myDNS.fromString(String("8.8.8.8"));
    // myGW.fromString(String("192.168.101.250"));
    myGW.fromString(String("192.168.0.1"));

    Ethernet.begin(mac[0], myIP, myDNS, myGW, myMASK);

    Serial.print("Connected! IP address: ");
    Serial.println(Ethernet.localIP());

    Serial.print("Connecting to SQL Server @ ");
    Serial.print(server_ip);
    Serial.println(String(", Port = ") + server_port);
    Serial.println(String("User = ") + user + String(", PW = ") + password + String(", DB = ") + default_database);
}

void loop()
{
    if (conn.connectNonBlocking(server_ip, server_port, user, password) != RESULT_FAIL)
    {
        UpdateFW();
        conn.close();
        do
        {
            client.flush();
        } while (client.available());
        client.stop();
        EthernetClient client;
        delay(500);
        digitalWrite(ResetETH, LOW);
        Serial.println("Reset Network Card in loop");
        delay(500);
        digitalWrite(ResetETH, HIGH);
        delay(3000);
    }
    else
    {
        Serial.println("DB Connect failed");
        delay(3000);
    }
}