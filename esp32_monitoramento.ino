#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4
#define VIBRATION_PIN 15
#define LED_ALERTA 2

const char* ssid = "SEU_WIFI";
const char* password = "SUA_SENHA";

const char* mqtt_server = "192.168.1.100";

WiFiClient espClient;
PubSubClient client(espClient);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

unsigned long lastMsg = 0;

void setup_wifi()
{
  delay(10);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
}

void reconnect()
{
  while (!client.connected())
  {
    if (client.connect("ESP32Monitor"))
    {
    }
    else
    {
      delay(5000);
    }
  }
}

void setup()
{
  pinMode(VIBRATION_PIN, INPUT);
  pinMode(LED_ALERTA, OUTPUT);

  Serial.begin(115200);

  sensors.begin();

  setup_wifi();

  client.setServer(mqtt_server, 1883);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }

  client.loop();

  long now = millis();

  if (now - lastMsg > 5000)
  {
    lastMsg = now;

    sensors.requestTemperatures();

    float temperatura = sensors.getTempCByIndex(0);

    int vibracao = digitalRead(VIBRATION_PIN);

    String payload =
      String("{\"temperatura\":") +
      temperatura +
      ",\"vibracao\":" +
      vibracao +
      "}";

    client.publish(
      "industria/sensores",
      payload.c_str()
    );

    if(temperatura > 35 || vibracao == 1)
    {
      digitalWrite(LED_ALERTA, HIGH);
    }
    else
    {
      digitalWrite(LED_ALERTA, LOW);
    }
  }
}
