import socket
from resp2 import *


class RedisClient:
    def __init__(self):
        self.sock = None

    def connect(self, host, port):
        self.sock = socket.create_connection((host, port))

    def send_pipeline(self, *commands):
        encoded = ""
        for command in commands:
            encoded += encode_array(command)
        self.sock.sendall(encoded.encode())

    def send_command(self, *args):
        encoded = encode_array(args)
        self.sock.sendall(encoded.encode())

    def read_response(self):
        buffer = self.sock.recv(4096).decode()
        return parse(buffer)[0]

    def close(self):
        self.sock.close()


if __name__ == "__main__":
    client = RedisClient()
    client.connect("127.0.0.1", 6969)
    client.send_pipeline(
        ["SET", "name", "Taiyo"],
        ["SET", "age", "20"],
        ["GET", "name"],
        ["INCR", "age"],
        ["GET", "name"],
    )
    print(client.read_response())
    print(client.read_response())
    print(client.read_response())
    print(client.read_response())
    print(client.read_response())
    client.close()
    #    while True:
    #        line = input("> ")
    #        if not line:
    #            break
    #        args = line.split()
    #        client.send_command(*args)
    #        print(client.read_response())
    #    client.close()
