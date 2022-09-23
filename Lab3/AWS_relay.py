import threading
import socket
import json
import AWSIoTPythonSDK.MQTTLib as AWSIoTPyMQTT

HOST = '0.0.0.0'
# [TODO] 166XX, XX is your tool box number
PORT = 16619

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind((HOST, PORT))
s.listen(5)
tSocket=[]

# [HINT] currentRing stores the ring state
currentRing = None
# [HINT] Lock maintain the indentity of resource
Lock = threading.Lock()
# [HINT] variable for socket
conn, addr = None,None


def mqttcallback(client, userdata, message):
    global currentRing,conn,addr,Lock
    try:
        # [TODO] write callback to deal with MQTT message from Lambda
        response = message.payload.decode("utf-8")
        if 'desired' in json.loads(response)['state']:
            currentRing = int(json.loads(response)['state']['desired']['ring'])
            conn.send(("ring:" + str(currentRing)).encode("utf-8"))
    except Exception as e:
        print(e)



# [TODO] Define ENDPOINT, CLIENT_ID, PATH_TO_CERT, PATH_TO_KEY, PATH_TO_ROOT
ENDPOINT = "a2bu30yj4l66iy-ats.iot.us-east-2.amazonaws.com"
CLIENT_ID = "lab3"
PATH_TO_CERT = "./2398bbc3bc-certificate.pem.crt"
PATH_TO_KEY = "./2398bbc3bc-private.pem.key"
PATH_TO_ROOT = "./root CA 1.txt"

myAWSIoTMQTTClient = AWSIoTPyMQTT.AWSIoTMQTTClient(CLIENT_ID)
myAWSIoTMQTTClient.configureEndpoint(ENDPOINT, 8883)
myAWSIoTMQTTClient.configureCredentials(PATH_TO_ROOT, PATH_TO_KEY, PATH_TO_CERT)

myAWSIoTMQTTClient.connect()
# [TODO] subscribe AWS topic(s)
myAWSIoTMQTTClient.subscribe("$aws/things/+/shadow/get", 1, mqttcallback)
myAWSIoTMQTTClient.subscribe("$aws/things/+/shadow/accepted", 1, mqttcallback)
myAWSIoTMQTTClient.subscribe("$aws/things/+/shadow/rejected", 1, mqttcallback)
myAWSIoTMQTTClient.subscribe("$aws/things/+/shadow/update", 1, mqttcallback)
myAWSIoTMQTTClient.subscribe("$aws/things/+/shadow/rejected", 1, mqttcallback)
myAWSIoTMQTTClient.subscribe("$aws/things/+/shadow/delta", 1, mqttcallback)

def on_new_client(clientsocket,addr):
    global currentRing
    while True:
        # [TODO] decode message from Arduino and send to AWS
        recv = clientsocket.recv(7).decode("utf-8")
        print("recv: " + recv)
        humidity = float(recv[0:5])
        currentRing = int(recv[6])
        send = json.dumps({
            "state":{
                "reported":{
                    "humidity":humidity,
                    "ring":currentRing
                }
            }
        })
        myAWSIoTMQTTClient.publish("$aws/things/lab3/shadow/update", send, 1)
    clientsocket.close()

print('server start at: %s:%s' % (HOST, PORT))
print('wait for connection...')

def main():
    global conn, addr
    try:
        conn, addr = s.accept()
        print('connected by ' + str(addr))
        t = threading.Thread(target=on_new_client,args=(conn,addr))
        tSocket.append(t)
        tSocket[-1].start()
    except Exception as e:
        print(e)
        s.close()
        print("socket close")

if __name__ == '__main__':
    main()