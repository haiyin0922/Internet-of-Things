import threading
import socket
import time
import requests

HOST = '0.0.0.0'
PORT = 16619

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind((HOST, PORT))
s.listen(5)
tSocket=[]

# [HINT] Lock maintain the indentity of resource
Lock = threading.Lock()
# [HINT] variable for socket
conn, addr = None,None

def on_new_client(clientsocket,addr):
    while True:
        # [TODO] decode message from Arduino and send to AWS
        recv = clientsocket.recv(1).decode("utf-8")
        print("recv: " + recv)
        if int(recv) > 0:
            lineNotifyMessage(token, message)
            time.sleep(30)

    clientsocket.close()

def lineNotifyMessage(token, msg):
    headers = {
        "Authorization": "Bearer " + token, 
        "Content-Type" : "application/x-www-form-urlencoded"
    }

    payload = {'message': msg}
    r = requests.post("https://notify-api.line.me/api/notify", headers = headers, params = payload)
    
    return r.status_code

message = '你的機車被移動了!!'
token = 'dGXhN6I7C3fG1GMLBndgbNd7tL6HYZUkbFQ7CQ1H8hs'

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