import pytest
import socket
import time
from subprocess import Popen


SERVER_PORT = 31337


@pytest.fixture(scope="function")
def server():
    print("Starting server...")
    proc = Popen(["bin/server", "-p", str(SERVER_PORT)])
    time.sleep(0.5)
    yield proc
    print("Shutdown server...")
    proc.kill()


def test_acceptance_transceiver(server):
    sock = socket.socket(
        socket.AF_INET,
        socket.SOCK_DGRAM)
    sock.settimeout(2)
    sock.connect(("localhost", 31337))

    # RX
    workload = bytearray(b'\x00\x01\x00\x01\x00')
    sock.send(workload)
    response = sock.recv(64)
    assert response == workload

    # TX
    workload = bytearray(b'\x00\x01\x00\x01\x01')
    sock.send(workload)
    response = sock.recv(64)
    assert response == workload

    # RXTX
    workload = bytearray(b'\x00\x01\x00\x01\x02')
    sock.send(workload)
    response = sock.recv(64)
    assert response == workload

    # RX->RXTX->TX->TX->RX
    workload = bytearray(b'\x00\x01\x00\x05\x00\x02\x01\x01\x00')
    sock.send(workload)
    response = sock.recv(64)
    assert response == workload


    # TODO: Validate output


def test_acceptance_amplifier(server):
    sock = socket.socket(
        socket.AF_INET,
        socket.SOCK_DGRAM)
    sock.settimeout(2)
    sock.connect(("localhost", 31337))

    # ON
    workload = bytearray(b'\x00\x00\x00\x01\x00')
    sock.send(workload)
    response = sock.recv(64)
    assert response == workload

    # OFF
    workload = bytearray(b'\x00\x00\x00\x01\x01')
    sock.send(workload)
    response = sock.recv(64)
    assert response == workload

    # ON->OFF->OFF->ON->ON
    workload = bytearray(b'\x00\x00\x00\x05\x01\x00\x00\x01\x01')
    sock.send(workload)
    response = sock.recv(64)
    assert response == workload

    # TODO: Validate output


def test_acceptance_incorrect_tag(server):
    sock = socket.socket(
        socket.AF_INET,
        socket.SOCK_DGRAM)
    sock.settimeout(2)
    sock.connect(("localhost", 31337))

    workload = bytearray(b'\x01\x00\x00\x01\x00')
    sock.send(workload)
    with pytest.raises(TimeoutError):
        response = sock.recv(64)

    # TODO: Validate output


def test_acceptance_incorrect_length(server):
    sock = socket.socket(
        socket.AF_INET,
        socket.SOCK_DGRAM)
    sock.settimeout(2)
    sock.connect(("localhost", 31337))

    workload = bytearray(b'\x00\x00\x00\x05\x00')
    sock.send(workload)
    with pytest.raises(TimeoutError):
        response = sock.recv(64)

    # TODO: Validate output
