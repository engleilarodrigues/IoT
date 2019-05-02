#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

char auth[] =  ""; //Auth Token Blynk
String apiKey = "";//Write key do ThingSpeak

//informações da rede WiFi
char ssid[] = ""; 
char pass[] = "";
char server[] = "api.thingspeak.com";

#define DHTPIN 2 //Pin D4
#define LED 4 //Pin D2

DHT dht(DHTPIN, DHT11);
BlynkTimer timer;
WiFiClient client;

//envio de informações para o blynk
void sendBlynk(){
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)){
    Serial.println("Falha na leitura do Sensor DHT11");
    return;
  }

  Blynk.virtualWrite(V5, t);
  Blynk.virtualWrite(V6, h);
}

void setup(){
  Serial.begin(115200);
  delay(10);
  dht.begin();
  
  //conexão WiFi
  Serial.println("Conectado a rede ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado!");
  
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(2000L, sendBlynk);
}

void loop(){
  //leitura do sensor DHT11
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  if (isnan(h) || isnan(t)){
    Serial.println("Falha na leitura do Sensor DHT11");
    return;
  }

  if(client.connect(server,80)){
    //envio de informações para os canais ThingSpeak
    String postStr = apiKey;
    postStr +="&field1=";
    postStr +=String(h);
    postStr +="&field2=";
    postStr +=String(t);
    postStr +="\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    //exibição dos valores coletados para verificação
    Serial.print("Temperatura: ");
    Serial.print(t);
    Serial.println(" graus Celsius");
    Serial.print("Umidade: ");
    Serial.print(h);
    Serial.println("%");
    Serial.println("Enviado ao ThingSpeak");
  }
  client.stop();
  Serial.println("Aguarde");
  Blynk.run();
  timer.run();
  delay(20000); //thingspeak necessita de no minimo 15s entre as atualizações
}
