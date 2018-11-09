import custom_socket
from os import system
from time import sleep


def responder(client):
    MSG = "hello world"
    RESP = "how are you?"

    print("  Client connected.")
    print("  Waiting for {} bytes...".format(len(MSG)))
    buff = client.receive(len(MSG))
    assert buff == MSG, "Client sent the wrong message, '{}'".format(buff)
    buff = client.read()
    assert buff == "", "Client sent too much, '{}'".format(buff)
    print("  Received '{}'.".format(buff))

    print("  Delaying response for 1 second")
    sleep(1)
    print("  Sending '{}'...".format(RESP))
    client.send(RESP)

    print("  Waiting for finished...")
    client.receive(1)
    print("  Closing connection.")
    client.close()


server = custom_socket.Server(25555)
server.start()
print("Testing linux...")
system("cmd.exe /c start bash.exe ./tests")
linux_client = server.wait_for_client()
responder(linux_client)
print("Finished linux.")

print("Testing windows...")
system("cmd.exe /c start tests.exe")
windows_client = server.wait_for_client()
responder(windows_client)
print("Finished windows.")
server.close()