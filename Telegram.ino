  /*
   Dario Ciceri
   Codice per controllare qualsiasi cosa tramite Telegram e una scheda esp32
   Profilo Instragram: https://www.instagram.com/_dario.ciceri_/
   Pagina GitHub: https://github.com/Dario-Ciceri
   Canale YouTube: https://www.youtube.com/channel/UCuPuHsNjWX7huiztYu9ROQA
*/

#include <WiFi.h> //libreria per il wifi (meglio non modificare)
#include <WiFiClientSecure.h> //libreria per il wifi (meglio non modificare)
#include <UniversalTelegramBot.h> //libreria per telegram (meglio non modificare)
#define LED 23

/*
 * Ejemplo leer temperatura y humedad
 * Sensor DHT11 y ESP32s 
 */
#include <DHT.h>//https://github.com/adafruit/DHT-sensor-library
#define DHTPIN 22
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

#define WIFI_SSID "TP-LINK_DE16" //nome wifi (potete modificarlo)
#define PASSWORD_WIFI "67743667" //password wifi (potete modificarlo)
#define BOT_API_TOKEN "5084791438:AAGJXK1fCbH5_S8kF3mRJgdkyr6Y3BtOmY" //token chatbot telegram (potete modificarlo)

const unsigned long intervallo = 1000; //pausa per la ricezione dei messaggi (potete modificarlo)
unsigned long ultima_chiamata = 0; //gestione del millis (meglio non modificare)

WiFiClientSecure s_client; //wifi client (meglio non modificare)
UniversalTelegramBot bot(BOT_API_TOKEN, s_client); //telegram client (meglio non modificare)

const int ledPin = 23;
int ledStatus = 0;
const int Trigger = 5;//Pin digital 2 para el Trigger del sensor 
const int Echo = 18; //Pin digital 3 para el Echo del sensor
void validar_nivel ();


void gestisci_messaggi(int num_nuovi_messaggi) { //funzione per la gestione della chat telegram

  for (int i = 0; i < num_nuovi_messaggi; i++) {

    String chat_id = bot.messages[i].chat_id; //id univoco della chat
    String text = bot.messages[i].text; //testo inviato in chat dall'utente
    String from_name = bot.messages[i].from_name; //chi manda il messaggio
    
    
    if (from_name == "") {
      from_name = "Sconosciuto"; //se non possiamo sapere chi ci scrive, allora è uno sconosciuto
    }

    else if (text == "/start") { //se il testo ricevuto è /start allora invia il messaggio di benvenuto
      bot.sendMessage(chat_id, "hablame menor q hay pa' la cabeza!!");
    }

    else if (text == "Menu") {
      bot.sendMessage(chat_id, "listo ");}

    else if (text == "/ledoff")
    {
      ledStatus = 0;
      digitalWrite(ledPin, LOW); // turn the LED off (LOW is the voltage level)
      bot.sendMessage(chat_id, "Led is OFF", "");
    }

    else if (text == "/ledon")
    {
      ledStatus = 1;
      digitalWrite(ledPin, HIGH); // turn the LED on (HIGH is the voltage level)
      bot.sendMessage(chat_id, "Led is ON", "");
    }

    else if (text == "/temperatura"){
      float temp = dht.readTemperature();
      String reply = (String)"Temperatura: " + (String)temp + (String)"°C";
      bot.sendMessage(chat_id, reply);
    }


    else if (text == "/nivel"){
      long t; //timepo que demora en llegar el eco
      long d; //distancia en centimetros

      digitalWrite (Trigger, HIGH);
      delayMicroseconds (10) ;//Enviamos un pulso de 10us
      digitalWrite (Trigger, LOW);


      t = pulseIn (Echo, HIGH); //obtenemos el ancho del pulso
      d = t/59; //escalamos tiempo una distancia en cm

      if(d<=10){
        String ml = (String) d + (String) " Cm" + (String) " Todo esta Bien";
        bot.sendMessage(chat_id, ml);
      }else{
        String ml = (String) d + (String) " Cm" + (String) " URGENTE! recargar bebidas";
        bot.sendMessage(chat_id, ml);
      }
        

      
    
    }

    else if (text == "/status")
    {
      if (ledStatus)
      {
        bot.sendMessage(chat_id, "Led is ON", "");
      }
      else
      {
        bot.sendMessage(chat_id, "Led is OFF", "");
      }
    }
    else{
      bot.sendMessage(chat_id, "Comando no reconocido");
    }
    
    Serial.println(from_name); //debug su seriale
    Serial.println(chat_id); //debug su seriale
    Serial.println(text); //debug su seriale
    
  }
}

void setup() {
  Serial.begin(115200); //inizializo la seriale su 115200 baudrate
  dht.begin();

  pinMode (Trigger, OUTPUT); //pin como salida 
  pinMode (Echo, INPUT); //pin como entradal 
  digitalWrite (Trigger, LOW);//Inicializamos el pin con 0
  
  pinMode(ledPin, OUTPUT); // initialize digital ledPin as an output.
  delay(10);
  digitalWrite(ledPin, HIGH);

  Serial.println(); //debug su seriale
  Serial.print("Connetto alla rete wifi "); //debug su seriale
  Serial.println(WIFI_SSID); //debug su seriale

  WiFi.begin(WIFI_SSID, PASSWORD_WIFI); //connetto al wifi

  s_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); //creo una connessione con Telegram utilizzando un certificato di sicurezza

  while (WiFi.status() != WL_CONNECTED) { //finchè il wifi non è connesso attendi e manda dei puntini di sospensione su seriale
    Serial.print("."); //debug su seriale
    delay(500);
  }

  Serial.print("Wifi connesso. Indirizzo IP: "); //debug su seriale
  Serial.println(WiFi.localIP()); //debug su seriale
}

void loop() {

  long t; //timepo que demora en llegar el eco
  long d; //distancia en centimetros

  digitalWrite (Trigger, HIGH);
  delayMicroseconds (10) ;//Enviamos un pulso de 10us
  digitalWrite (Trigger, LOW);


  t = pulseIn (Echo, HIGH); //obtenemos el ancho del pulso
  d = t/59; //escalamos tiempo una distancia en cm

  Serial.print (" ");//todo esto es opcional
  Serial.print (d); //todo esto es opcional
  Serial.print("cm");//todo esto es opcional
  Serial.println();//todo esto es opcional
  delay(1500);

  if(d <=10){
    Serial.print("Todo está bien");// mensaje que envia el sistema cuando el nivel de bebida esta en un rango de 1 a 10 cm
  }
  else{
      Serial.print("Nivel bajo de bebida, por favor surtir");// mensaje que envia el sistema cuando el nivel de bebida es superior a 10 cm
    }

  
  if (millis() - ultima_chiamata > intervallo) { //se il valore del timer "millis" - l'ultimo valore in millisecondi di quando abbiamo eseguito le istruzioni, è maggiore del tempo di attesa "intervallo", allora esegui le istruzioni
    int num_nuovi_messaggi = bot.getUpdates(bot.last_message_received + 1); //aggiorna la lista dei messaggi in chat
    while (num_nuovi_messaggi) { //finchè ci sono nuovi messaggi, processali
      Serial.println("messaggio ricevuto"); //debug su seriale
      gestisci_messaggi(num_nuovi_messaggi);
      num_nuovi_messaggi = bot.getUpdates(bot.last_message_received + 1);
    }
    ultima_chiamata = millis(); //aggiorna l'ultimo periodo in millisecondi in cui sono state eseguite le istruzioni all'interno del ciclo while
  }

  {
  // Antes de leer Temp, hum
  //Esperar 2 segundos entre cada lectura
  delay(2000);
  // Leer temperatura ºC
  float t = dht.readTemperature();

  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.print("°C");
  Serial.print('\n');
  }
 }
