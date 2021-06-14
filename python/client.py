import socket

TCP_IP = '127.0.0.1'
TCP_PORT = 17555


def setup_connection():
	global clientSocket
	clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	clientSocket.connect((TCP_IP, TCP_PORT))


def await_input(data):
	clientSocket.send(data.encode())

	#dataFromServer = clientSocket.recv(1024)
	#print("Server: " + dataFromServer.decode())

def run(data):
	setup_connection()
	await_input(data)