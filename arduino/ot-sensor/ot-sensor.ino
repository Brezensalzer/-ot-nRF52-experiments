//---------------------------------------------------------------------------
// ot-sensor.ino
//---------------------------------------------------------------------------
#include <UdpSocket.h>
#include <IPAddress.h>
#include <OpenThread.h>

// save power without serial interface...
const boolean DEBUG = false;

const uint8_t CHANNEL = 15;
const char PSK[] = "J01NME";
const uint8_t EXTPANID[] = {0x11, 0x11, 0x11, 0x11, 0x22, 0x22, 0x22, 0x22};

// Mesh-local multicast address ff03::1
// we make sure the message reaches the thread border router
IPAddress server(0xff03, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0001); // multicast address

UDPSocket Udp;

void dump_info();

const size_t MAX_PAYLOAD_LEN = 40;
const unsigned short DEST_PORT  = 4711;
const int INTERVAL = 5;

String message;
char recvBuffer[MAX_PAYLOAD_LEN];
long lastsend = INTERVAL * -1000;
uint16_t seq_id = 0;

//--- MS8607 temperature, humidity and pressure sensor ---
#include <Wire.h>
#include <Adafruit_MS8607.h>
#include <Adafruit_Sensor.h>

Adafruit_MS8607 ms8607;

//---- Button for JOIN Mode ------------------------------
int buttonPin = 4;  // on board switch

//---------------------------------------------------------------------------
void signalLED() {
//---------------------------------------------------------------------------
  digitalWrite(3, HIGH);
  delay(2000);
  digitalWrite(3, LOW);
}

//---------------------------------------------------------------------------
void setup() {
//---------------------------------------------------------------------------
  delay(1000);
  if (DEBUG) {
    Serial.begin(115200);
    while(!Serial) delay(10);
  }
  
  // Initialize sensor --------------------------
  if (!ms8607.begin()) {
    if (DEBUG) Serial.println("Failed to find MS8607 chip");
    while (1) { delay(10); }
  }
  if (DEBUG) Serial.println("MS8607 Found!\n");

  // Initialize switch for joining
  pinMode(buttonPin, INPUT_PULLUP);
  signalLED();
  boolean JOIN_MODE = !digitalRead(buttonPin);
  if (DEBUG) Serial.print("join mode active: ");
  if (DEBUG) Serial.println(JOIN_MODE);
  
  // Initialize OpenThread ----------------------
  if (DEBUG) Serial.println("Start OpenThread Sensor Node");
  OpenThread.init();
  OpenThread.begin();
  if (JOIN_MODE) {
    OpenThread.panid(0xFFFF);
    OpenThread.channel(CHANNEL);
    OpenThread.extpanid(EXTPANID);
  }
  OpenThread.routereligible.disable();
  OpenThread.txpower(8);
  OpenThread.ifconfig.up();

  if (DEBUG) {
    Serial.print("channel  = ");
    Serial.println(OpenThread.channel() );
    Serial.print("extpanid = 0x");
    Serial.println(OpenThread.extpanid() );
    Serial.print("eui64 = ");
    Serial.println(OpenThread.eui64());
  }
  delay(100);

  OTErr err;

  if (JOIN_MODE) {
    do {
      err = OpenThread.joiner.start(PSK);
  
      if (err) {
        if (DEBUG) Serial.print("...");
        if (DEBUG) Serial.print(err);
        delay(100);
        continue;
      }
  
      err = OpenThread.thread.start();
  
      if(err) {
        if (DEBUG) Serial.print("Thread process can't start: ");
        if (DEBUG) Serial.println(err);
        delay(100);
        continue;
      }
    } while(err != 0);
  }
  else {
    err = OpenThread.thread.start();    
  }

  if (DEBUG) {
    Serial.println("-----------------------");
    Serial.println("Success to join network");
    Serial.println("-----------------------");
  
    dump_info();
  
    Serial.println("--------------");
    Serial.println("  Start echo  ");
    Serial.println("--------------");
  }

} // end setup()

//---------------------------------------------------------------------------
void loop() {
//---------------------------------------------------------------------------
  // read sensor data
  sensors_event_t temp, pressure, humidity;
  Adafruit_Sensor *pressure_sensor = ms8607.getPressureSensor();
  Adafruit_Sensor *temp_sensor = ms8607.getTemperatureSensor();
  Adafruit_Sensor *humidity_sensor = ms8607.getHumiditySensor();

  temp_sensor->getEvent(&temp);
  pressure_sensor->getEvent(&pressure);
  humidity_sensor->getEvent(&humidity);

  // format JSONmessage
  message = "{ CurrentTemperature: ";
  message.concat(temp.temperature);
  message.concat(", CurrentRelativeHumidity:");
  message.concat(humidity.relative_humidity);
  message.concat(", CurrentPressure: ");
  message.concat(pressure.pressure);
  message.concat(", Alive: ");
  message.concat(++seq_id);
  message.concat(" }");

  if (DEBUG) {
    Serial.print("Send to [");
    Serial.print(server);
    Serial.printf("]:%d -> '", DEST_PORT);
    Serial.print(message);
    Serial.println("'");
  }
  message.concat("\n");
  
  // send packet
  Udp.beginPacket(server, DEST_PORT);
  Udp.write(message.c_str(), message.length() );
  Udp.endPacket();

  delay(10000);
  waitForEvent();
}

#define dump_param(p)\
{ \
  Serial.print(#p " = "); \
  Serial.println(OpenThread.p()); \
}

#define dump_hex(p)\
{ \
  Serial.print(#p " = "); \
  Serial.println(OpenThread.p(), HEX); \
}


//---------------------------------------------------------------------------
void dump_info()
//---------------------------------------------------------------------------
{
  dump_param(channel);
  dump_param(channel.supported);
  dump_param(channel.preferred);

#if OPENTHREAD_FTD
  for(auto ite = OpenThread.child(); !ite.end(); ite.next() ) {
    Serial.print("child: ");
    Serial.println( ite.get() );
  }

  for(auto ite = OpenThread.childip(0); !ite.end(); ite.next() ) {
    ite.get();
  }

  dump_param(childip.max);
  dump_param(childmax);
  dump_param(childtimeout);
  dump_param(contextreusedelay);
#endif

  dump_param(counters.mac);
  dump_param(counters.mle);

#if OPENTHREAD_FTD
  dump_param(delaytimermin);
  for(auto ite = OpenThread.eidcache(); !ite.end(); ite.next() ) {
    Serial.print("eidcache: ");
    Serial.println( ite.get() );
  }
#endif

  dump_param(eui64);
  dump_param(extaddr);
  dump_param(extpanid);

  for(auto ite = OpenThread.ipaddr(); !ite.end(); ite.next() ) {
    Serial.print("ipaddr: ");
    Serial.println( ite.get() );
  }

  dump_param(ipaddr.linklocal);
  dump_param(ipaddr.mleid);
  dump_param(ipaddr.rloc);

  for(auto ite = OpenThread.ipmaddr(); !ite.end(); ite.next() ) {
    Serial.print("ipmaddr: ");
    Serial.println( ite.get() );
  }

  dump_param(ipmaddr.promiscuous);
  dump_param(joiner.id);

#if OPENTHREAD_FTD
  dump_param(joinerport);
#endif

  dump_param(keysequence.counter);
  dump_param(keysequence.guardtime);

  //otLeaderData leaderd;
  //OpenThread.leaderdata(&leaderd);

#if OPENTHREAD_FTD
  dump_param(leaderpartitionid);
  dump_param(leaderweight);
#endif

  dump_param(mac.retries.direct);
#if OPENTHREAD_FTD
  dump_param(mac.retries.indirect);
#endif

  for(auto ite = OpenThread.macfilter.addr(); !ite.end(); ite.next() ) {
    Serial.print("macfilter.addr: ");
    Serial.println( ite.get() );
  }

  for(auto ite = OpenThread.macfilter.rss(); !ite.end(); ite.next() ) {
    Serial.print("macfilter.rss: ");
    Serial.println( ite.get() );
  }

  dump_param(masterkey);
//  dump_param(mode);

#if OPENTHREAD_FTD
  for(auto ite = OpenThread.neighbor(); !ite.end(); ite.next() ) {
    Serial.print("neighbor: ");
    Serial.println( ite.get() );
  }
#endif

  dump_param(netdataregister);

#if OPENTHREAD_FTD
  //OpenThread.networkdiagnostic.get(nullptr, nullptr, 0);
  //OpenThread.networkdiagnostic.reset(nullptr, nullptr, 0);
  dump_param(networkidtimeout);
#endif

  dump_param(networkname);
  dump_param(panid);
#if OPENTHREAD_FTD
  dump_param(parentpriority);
#endif
  dump_param(pollperiod);

  for(auto ite = OpenThread.prefix(); !ite.end(); ite.next() ) {
    Serial.print("prefix: ");
    Serial.println( ite.get() );
  }

  dump_param(promiscuous);

#if OPENTHREAD_FTD
//  dump_param(preferrouterid);
  dump_param(pskc);
//  dump_param(pskc.passphrase);
//  dump_param(releaserouterid);
#endif

  dump_param(rloc16);

  for(auto ite = OpenThread.route(); !ite.end(); ite.next() ) {
    Serial.print("route: ");
    Serial.println( ite.get() );
  }

#if OPENTHREAD_FTD
  for(auto ite = OpenThread.router(); !ite.end(); ite.next() ) {
    Serial.print("router: ");
    Serial.println( ite.get() );
  }

  dump_param(routerdowngradethreshold);
  dump_param(routereligible);
  dump_param(routerselectionjitter);
  dump_param(routerupgradethreshold);
#endif
  dump_param(state);
  dump_param(state.child);
  dump_param(state.detached);
#if OPENTHREAD_FTD
  dump_param(state.router);
  dump_param(state.leader);
#endif

  dump_param(thread.version);
  dump_param(txpower);
  dump_param(version);
}
