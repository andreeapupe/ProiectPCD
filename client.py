import socket

TCP_IP = '127.0.0.1'
TCP_PORT = 17555

def setup_connection():
	global s
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((TCP_IP, TCP_PORT))

def await_input():
	message = raw_input('You: ')
	s.send(message)

	response = s.recv(1024)
	print(response)


setup_connection()
await_input()