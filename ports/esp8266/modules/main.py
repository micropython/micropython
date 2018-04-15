from utime import ticks_ms, ticks_diff, sleep_ms
from micropython import const
import uasyncio as asyncio
import network
import gc
import urequests
import utime
import urandom
import ntptime
from machine import RTC

from counter import Counter

DB = "counters-87809";
if not DB:
    print('not Firebase DB key specified, skip sending data');

ACCESS_TOKEN="AIzaSyBiZak8OSV1wu0ENz7fUdHdQ0S4qRFc0QU";

#####################################
# Class handles both counters and sends their status to MQTT
#####################################
class WaterMeter():

    cold_counter = Counter(0, 200);

    def __init__(self):
        self.internet_outage = True;
        self.internet_outages = 0;
        self.internet_outage_start = ticks_ms();

        loop = asyncio.get_event_loop();
        loop.create_task(self._counter_coro(self.cold_counter, "Another litre passed"));

    async def setTime():
        t = ntptime.settime();
        await asyncio.sleep(0);

    async def sendToFirebase(value1):
        timestamp = int(utime.localtime());
        data = {"timestamp":timestamp,"6123464":0,"11443721":0,"11444802":value1,"electricity":0};
        response = urequests.post('https://' + DB + '.firebaseio.com/.json', json=data);
        print(data);
        #print(response.status_code);
        await asyncio.sleep(0);

    async def _counter_coro(self, counter, topic):
        # Publish initial value
        value = counter.value();
        await self.publish(topic, str(value));

        # Publish each new value
        while True:
            value = await counter;
            await self.sendToFirebase(value);
            await self.publish_msg(topic, str(value));


    async def wifi_connection_handler(self, state):
        self.internet_outage = not state;
        if state:
            print('WiFi is up.');
            duration = ticks_diff(ticks_ms(), self.internet_outage_start) #// 1000
            await self.publish_debug_msg('ReconnectedAfter', duration);
        else:
            self.internet_outages += 1;
            self.internet_outage_start = ticks_ms();
            print('WiFi is down.');
            
        await asyncio.sleep(0);

    async def publish_debug_msg(self, subtopic, msg):
        await self.publish_msg(subtopic, str(msg));


    # Publish a message if WiFi and broker is up, else discard
    async def publish_msg(self, topic, msg):
        print("Publishing message on topic {}: {}".format(topic, msg));
        if not self.internet_outage:
            await self.publish(topic, msg);
        else:
            print("Message was not published - no internet connection");


    async def _connect_to_WiFi(self):
        print('Connecting to WiFi');
        sta_if = network.WLAN(network.STA_IF);
        sta_if.active(True);
        sta_if.connect("Smart_box-142", "natalia31081980");
        
        conn = False;
        while not conn:
            await self.connect();
            conn = True;

        print('Connected!');
        self.internet_outage = False;


    async def _run_main_loop(self):
        # Loop forever
        mins = 0;
        while True:
            gc.collect();  # For RAM stats.
            mem_free = gc.mem_free();
            mem_alloc = gc.mem_alloc();

            try:
                await self.publish_debug_msg("Uptime", mins);
                await self.publish_debug_msg("Repubs", self.REPUB_COUNT);
                await self.publish_debug_msg("Outages", self.internet_outages);
                await self.publish_debug_msg("MemFree", mem_free);
                await self.publish_debug_msg("MemAlloc", mem_alloc);
            except Exception as e:
                print("Exception occurred: ", e);
            mins += 1;

            await asyncio.sleep(60);


    async def main(self):
        while True:
            try:
                await self.setTime();
                await self._connect_to_WiFi();
                await self._run_main_loop();
                    
            except Exception as e:
                print('Global communication failure: ', e);
                await asyncio.sleep(20);


gc.collect();
client = WaterMeter();
loop = asyncio.get_event_loop();
loop.run_until_complete(client.main());
