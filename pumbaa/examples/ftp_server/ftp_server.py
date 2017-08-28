import socket
import os

try:
    from os import path
except:
    # Those are not implemented in the os module.
    class Path(object):

        def join(self, *args):
            return '/'.join(args)

        def isdir(self, p):
            return p in ['/fs', '/fs/']

    path = Path()


class FtpServer(object):

    def __init__(self, ip, port ,data_port):
        self.ip = ip
        self.port = port
        self.data_port = data_port
        self.listener = socket.socket()
        self.listener.bind((self.ip, self.port))
        self.listener.listen(1)
        self.data_listener = socket.socket()
        self.data_listener.bind((self.ip, self.data_port))
        self.data_listener.listen(1)
        self.cwd = os.getcwd()
        self.client = None
        self.data_client = None

    def client_readline(self):
        line = b''
        while b'\r\n' not in line:
            byte = self.client.recv(1)
            if not byte:
                return ''
            line += byte
        return line

    def handle_list(self):
        try:
            self.client.sendall("150 Here comes the directory listing.\r\n")
            for filename in os.listdir(self.cwd):
                stat = os.stat(filename)
                file_size = stat[6]
                description = ("-rw-r--r--    1 owner group {:>13} "
                               "Jan 1  1980 {}\r\n").format(file_size, filename)
                self.data_client.sendall(description)
            self.client.sendall("226 Listed.\r\n")
        except:
            self.client.sendall('550 Failed to list directory\r\n')
        finally:
            self.data_client.close()

    def handle_retr(self, payload):
        try:
            self.client.sendall("150 Opening data connection.\r\n")
            with open(payload) as fin:
                chunk = fin.read(128)
                while chunk:
                    self.data_client.sendall(chunk)
                    chunk = fin.read(128)
            self.client.sendall("226 Transfer complete.\r\n")
        except:
            self.client.sendall('550 Failed to send file\r\n')
        finally:
            self.data_client.close()

    def handle_stor(self, payload):
        try:
            self.client.sendall("150 Ok to send data.\r\n")
            with open(payload, "wb") as fout:
                chunk = self.data_client.recv(128)
                while chunk:
                    fout.write(chunk)
                    chunk = self.data_client.recv(128)
            self.client.sendall("226 Transfer complete.\r\n")
        except:
            self.client.sendall('550 Failed to send file\r\n')
        finally:
            self.data_client.close()

    def process_command(self, command, payload):
        print("Command='{}', Payload='{}'".format(command, payload))

        if command == "USER":
            self.client.sendall("230 Logged in.\r\n")
        elif command == "SYST":
            self.client.sendall("215 Pumbaa\r\n")
        elif command == "SYST":
            self.client.sendall("502\r\n")
        elif command == "PWD":
            self.client.sendall('257 "{}"\r\n'.format(self.cwd))
        elif command == "CWD":
            if payload[0] == '/':
                new_cwd = payload
            else:
                new_cwd = path.join(self.cwd, payload)
            if path.isdir(new_cwd):
                self.client.sendall('250 Directory changed successfully\r\n')
            else:
                self.client.sendall('550 Failed to change directory\r\n')
        elif command == "EPSV":
            self.client.sendall('502\r\n')
        elif command == "TYPE":
            # probably should switch between binary and not
            self.client.sendall('200 Transfer mode set\r\n')
        elif command == "SIZE":
            try:
                size = os.stat(path.join(self.cwd, payload))[6]
                self.client.sendall('213 {}\r\n'.format(size))
            except:
                self.client.sendall('550 Could not get file size\r\n')
        elif command == "QUIT":
            self.client.sendall('221 Bye.\r\n')
        elif command == "PASV":
            self.client.sendall(
                '227 Entering Passive Mode ({},{},{}).\r\n'.format(
                    self.ip.replace('.',','),
                    self.data_port >> 8,
                    self.data_port % 256))
            self.data_client, remote_addr = self.data_listener.accept()
            print("Accepted FTP data connection from:", remote_addr)
        elif command == "LIST":
            self.handle_list()
        elif command == "RETR":
            self.handle_retr(payload)
        elif command == "STOR":
            self.handle_stor(payload)
        else:
            print("Unsupported command {} with payload {}".format(
                command, payload))
            self.client.sendall("502 Unsupported command.\r\n")

    def main(self):
        while True:
            # Wait for a client to connect.
            print("Listening for clients on {}:{}.".format(
                self.ip, self.port))
            self.client, client_addr = self.listener.accept()

            print("Accepted FTP connection from:", client_addr)

            try:
                self.client.sendall("220 Hello, this is the Pumbaa.\r\n")

                while True:
                    data = self.client_readline().strip().decode('utf-8')

                    if len(data) <= 0:
                        print("Client connection closed.")
                        break

                    command, payload =  (data.split(" ") + [""])[:2]
                    command = command.upper()

                    self.process_command(command, payload)
            finally:
                self.client.close()


if __name__ == '__main__':
    FtpServer('127.0.0.1', 10021, 13333).main()
