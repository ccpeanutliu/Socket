import socket

s = socket.socket()
host = socket.gethostname()
port = 1234

s.connect((host, port))
recvm = (s.recv(1024)).decode('utf-8')
print(recvm)

login = 1

while login == 1:
	print('Input "r" or "l": ')
	inp = input()
	if inp == 'r':
		name = input('Your name: ')
		message = 'REGISTER#' + name
	elif inp == 'l':
		name = input('Your name: ')
		port = input('Your port: ')
		message = name + '#' + port
		login = 0
		print(login)
	else:
		continue
	#if recvm.find("AUTH_FAIL") == -1:
#		print('jizz')
#		login = 1
	message += '\n'
	s.send(message.encode());
	recvm = (s.recv(1024)).decode('utf-8')
	print(recvm)
	print(login)

while login != 1:
	print('Input l or e')
	inp = input()
	if inp == 'l':
		message = 'List'
	elif inp == 'e':
		message = 'Exit'
	else:
		print('Wrong syntax. \n')
		continue
	message += '\n'
	s.send(message.encode());
	recvm = (s.recv(1024)).decode('utf-8')
	print(recvm)
	if recvm == "Bye":
		break

print('close socket\n')
s.close()
