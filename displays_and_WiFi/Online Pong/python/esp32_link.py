"""
Connection from the player client (client.py) to the ESP32 on the local network.

The ESP32 acts as the TCP server (port 5000). client.py connects to it and
sends the score, status, and game-over event as JSON lines.
Sending is handled in a separate thread so that an unreachable ESP32
does not slow down the game. If the connection is lost, it automatically
reconnects and sends the current state again.

Usage in client.py:
    import esp32_link
    esp32_link.start("192.168.x.x")      # empty string = no ESP32
    esp32_link.on_state(message)         # on "state"
    esp32_link.on_ready()                # on game_event "ready"
    esp32_link.on_waiting()              # on game_event "waiting"
    esp32_link.on_game_over(message)     # on game_event "game_over"
"""

import json
import queue
import select
import socket
import threading
import time

ESP32_PORT = 5000

_queue = queue.Queue()
_enabled = False

_status = "waiting"     # "waiting" | "active"
_scores = None          # (score1, score2); None until the score is known


# --------------------------------------------------
# START
# --------------------------------------------------

def start(ip):
    """Start the connection to the ESP32. Empty IP = no ESP32."""

    global _enabled

    if not ip:
        print("No ESP32 configured.")
        return

    _enabled = True

    threading.Thread(
        target=_worker,
        args=(ip,),
        daemon=True
    ).start()


# --------------------------------------------------
# INTERNAL
# --------------------------------------------------

def _send(message):
    if _enabled:
        _queue.put(message)


def _encode(message):
    return (json.dumps(message) + "\n").encode()


def _close(sock):
    if sock is not None:
        try:
            sock.close()
        except OSError:
            pass


def _peer_closed(sock):
    """True if the ESP32 has closed the connection (e.g. after a restart)."""

    try:
        readable, _, _ = select.select([sock], [], [], 0)

        if readable:
            return sock.recv(1) == b""

    except OSError:
        return True

    return False


def _worker(ip):
    sock = None

    while True:

        # ---------- Establish connection ----------

        if sock is None:

            try:
                sock = socket.create_connection((ip, ESP32_PORT), timeout=3)
                sock.settimeout(3)

                # Discard outdated messages from the offline period;
                # the current state will be sent again immediately
                while not _queue.empty():
                    _queue.get_nowait()

                sock.sendall(_encode({"type": "status", "status": _status}))

                if _scores is not None:
                    sock.sendall(_encode({
                        "type": "score",
                        "score1": _scores[0],
                        "score2": _scores[1],
                        "scorer": 0
                    }))

                print("Connected to ESP32.")

            except OSError:
                _close(sock)
                sock = None
                time.sleep(3)
                continue

        # ---------- Send messages ----------

        try:
            message = _queue.get(timeout=0.5)

        except queue.Empty:

            if _peer_closed(sock):
                print("ESP32 connection closed.")
                _close(sock)
                sock = None

            continue

        try:
            if _peer_closed(sock):
                raise OSError("closed")

            sock.sendall(_encode(message))

        except OSError:
            print("ESP32 connection lost, reconnecting...")
            _close(sock)
            sock = None


# --------------------------------------------------
# EVENTS FROM client.py
# --------------------------------------------------

def on_state(message):
    """Call this for every 'state' message from the server."""

    global _status, _scores

    if _status != "active":
        _status = "active"
        _send({"type": "status", "status": "active"})

    score1 = message.get("score1", 0)
    score2 = message.get("score2", 0)

    if _scores is None:
        scorer = 0
    elif (score1, score2) == _scores:
        return
    elif score1 > _scores[0]:
        scorer = 1
    elif score2 > _scores[1]:
        scorer = 2
    else:
        scorer = 0      # Reset to 0:0

    _scores = (score1, score2)

    _send({
        "type": "score",
        "score1": score1,
        "score2": score2,
        "scorer": scorer
    })


def on_ready():
    """Call this for game_event 'ready': new game, score 0:0."""

    global _status, _scores

    _status = "active"
    _scores = (0, 0)

    _send({"type": "status", "status": "active"})
    _send({"type": "score", "score1": 0, "score2": 0, "scorer": 0})


def on_waiting():
    """Call this for game_event 'waiting': a player has left the game."""

    global _status, _scores

    _status = "waiting"
    _scores = (0, 0)

    _send({"type": "status", "status": "waiting"})


def on_game_over(message):
    """Call this for game_event 'game_over': display the winner on the ESP32."""

    global _scores

    score1 = message.get("score1", 0)
    score2 = message.get("score2", 0)

    _scores = (score1, score2)

    _send({
        "type": "game_over",
        "winner": message.get("winner", 0),
        "score1": score1,
        "score2": score2
    })