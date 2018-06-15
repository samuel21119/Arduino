import requests
import socket
import threading
import logging
import mraa
import time
import sys
import httplib, urllib
import json
sys.path.insert(0, '/usr/lib/python2.7/bridge/')
from bridgeclient import BridgeClient as bridgeclient

value = bridgeclient()
deviceId = "DQyeXFUK"
deviceKey = "Yme4nPJeIag3mjON"

def post_to_mcs(payload):
    headers = {"Content-type": "application/json", "deviceKey": deviceKey}
    not_connected = 1
    while (not_connected):
        try:
            conn = httplib.HTTPConnection("api.mediatek.com:80")
            conn.connect()
            not_connected = 0
        except (httplib.HTTPException, socket.error) as ex:
            print "Error: %s" % ex
            time.sleep(10)  # sleep 10 seconds

    conn.request("POST", "/mcs/v2/devices/" + deviceId + "/datapoints", json.dumps(payload), headers)
    response = conn.getresponse()
    print( response.status, response.reason, json.dumps(payload), time.strftime("%c"))
    data = response.read()
    conn.close()



# change this to the values from MCS web console
DEVICE_INFO = {
    'device_id' : 'DQyeXFUK',
    'device_key' : 'Yme4nPJeIag3mjON'
}

# change 'INFO' to 'WARNING' to filter info messages
logging.basicConfig(level='INFO')

heartBeatTask = None

def establishCommandChannel():
    # Query command server's IP & port
    connectionAPI = 'https://api.mediatek.com/mcs/v2/devices/%(device_id)s/connections.csv'
    r = requests.get(connectionAPI % DEVICE_INFO,
                 headers = {'deviceKey' : DEVICE_INFO['device_key'],
                            'Content-Type' : 'text/csv'})
    logging.info("Command Channel IP,port=" + r.text)
    print "Command Channel IP,port=" + r.text
    print '\n'
    (ip, port) = r.text.split(',')

    # Connect to command server
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((ip, int(port)))
    s.settimeout(None)

    # Heartbeat for command server to keep the channel alive
    def sendHeartBeat(commandChannel):
        keepAliveMessage = '%(device_id)s,%(device_key)s,0' % DEVICE_INFO
        commandChannel.sendall(keepAliveMessage)
        logging.info("beat:%s" % keepAliveMessage)
        print "beat:%s" % keepAliveMessage
        print '\n'
    def heartBeat(commandChannel):
        sendHeartBeat(commandChannel)
        # Re-start the timer periodically
        global heartBeatTask
        heartBeatTask = threading.Timer(60, heartBeat, [commandChannel]).start()

    heartBeat(s)
    return s
send = 0
count = 0
def waitAndExecuteCommand(commandChannel):
    global send
    global count
    while True:
        command = commandChannel.recv(1024)
        logging.info("recv:" + command)
        print "recv:" + command
        print '\n'
        # command can be a response of heart beat or an update of the LED_control,
        # so we split by ',' and drop device id and device key and check length
        fields = command.split(',')[2:]
        if count == 0:
            SEND()
            count = 1
        else:
            count = 0
        if len(fields) > 1:
            timeStamp, dataChannelId, commandString = fields
            if dataChannelId == 'LED_Control':
                # check the value - it's either 0 or 1
                commandValue = int(commandString)
                logging.info("led :%d" % commandValue)
                print "led :%d" % commandValue
                print '\n'
                setLED(commandValue)


pin = None
def setupLED():
    global pin
    # on LinkIt Smart 7699, pin 44 is the Wi-Fi LED.
    pin = mraa.Gpio(44)
    pin.dir(mraa.DIR_OUT)
def SEND():
    global send
    if send == 1:
        h0 = value.get("Humidity")
        t0 = value.get("Temperature")
        p0 = value.get("Pressure")
        d0 = value.get("Dust")
        payload = {"datapoints":[{"dataChnId":"Humidity","values":{"value":h0}},{"dataChnId":"Temperature","values":{"value":t0}},{"dataChnId":"Barometer","values":{"value":p0}},{"dataChnId":"Dust","values":{"value":d0}}]}
        post_to_mcs(payload)
    else:
        if send == 0:
            payload = {"datapoints":[{"dataChnId":"Humidity","values":{"value":0}},{"dataChnId":"Temperature","values":{"value":0}},{"dataChnId":"Barometer","values":{"value":0}},{"dataChnId":"Dust","values":{"value":0}},{"dataChnId":"Power","values":{"value":0}}]}
            send = 2
            post_to_mcs(payload)
def setLED(state):
    # Note the LED is "reversed" to the pin's GPIO status.
    # So we reverse it here.
    global send
    h0 = value.get("Humidity")
    t0 = value.get("Temperature")
    p0 = value.get("Pressure")
    d0 = value.get("Dust")
    if state:
        payload = {"datapoints":[{"dataChnId":"Humidity","values":{"value":h0}},{"dataChnId":"Temperature","values":{"value":t0}},{"dataChnId":"Barometer","values":{"value":p0}},{"dataChnId":"Dust","values":{"value":d0}},{"dataChnId":"Power","values":{"value":1}}]}
        post_to_mcs(payload)
        send = 1
    else:
        payload = {"datapoints":[{"dataChnId":"Humidity","values":{"value":0}},{"dataChnId":"Temperature","values":{"value":0}},{"dataChnId":"Barometer","values":{"value":0}},{"dataChnId":"Dust","values":{"value":0}},{"dataChnId":"Power","values":{"value":0}}]}
        post_to_mcs(payload)
        send = 2

setupLED()
channel = establishCommandChannel()
waitAndExecuteCommand(channel)
