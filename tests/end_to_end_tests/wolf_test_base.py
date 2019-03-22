#!/usr/bin/env python

import json
import re
import os
import signal
import socket
import unittest
from os.path import isfile
from subprocess import Popen, PIPE
from threading import Lock, Thread

import sys
from time import sleep

build_lock = Lock()
BASE_DIR = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

build_path_part = 'local'
if len(sys.argv) == 2:
    build_path_part = sys.argv[1]
    sys.argv.pop(1)


class WolfTestBase(unittest.TestCase):
    wolf = None
    build_name = None
    parameters = ""

    # @classmethod
    # def setUpClass(cls):
    # global build_lock
    # build_lock.acquire()
    # rc = Popen(
    #     "cmake --build /home/lamanchy/CLionProjects/wolf/cmake-build-debug --target all -- -j 4",
    #     shell=True).wait()
    # assert rc == 0
    # build_lock.release()

    def tcp_listen(self, port):
        host = ''  # Symbolic name meaning all available interfaces
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(1.0)

        try:
            s.bind((host, port))
            s.listen(1)
            conn, addr = s.accept()
        except socket.timeout as e:
            raise socket.timeout("wolf did not connect to tcp server in time", e)

        conn.settimeout(0.1)
        while self.tcp_listeners_running[port]:
            try:
                data = conn.recv(1024)

                if not data: break
                self.tcp_listeners_results[port] += data

            except socket.timeout:
                continue

            except socket.error:
                print "tcp listener error occured"
                break

        conn.close()

    def _kill_wolf(self):
        try:
            self.wolf.kill()
        except OSError:
            pass

    def _stop_tcp_listener(self, port):
        self.tcp_listeners_running[port] = False
        self.tcp_listeners[port].join()

    def add_tcp_listener(self, port):
        assert port not in self.tcp_listeners
        self.tcp_listeners[port] = Thread(target=self.tcp_listen, args=(port,))
        self.tcp_listeners_results[port] = ""
        self.tcp_listeners_running[port] = True
        self.tcp_listeners_expecting[port] = []
        self.tcp_listeners[port].start()
        self.addCleanup(self._stop_tcp_listener, port)

    def setUp(self):
        self.tcp_listeners = {}
        self.tcp_listeners_results = {}
        self.tcp_listeners_expecting = {}
        self.tcp_listeners_running = {}
        self.expected_output = []

        global build_path_part
        parser = os.path.join(BASE_DIR, 'build', build_path_part, 'Debug', self.build_name)
        assert isfile(parser)
        command = parser + " " + self.parameters

        self.wolf = Popen(command.split(" "), stdin=PIPE, stdout=PIPE, stderr=PIPE)
        self.addCleanup(self._kill_wolf)

    def tearDown(self):
        self.wolf.send_signal(2)
        for i in range(30):
            if self.wolf.poll() is None:
                sleep(0.1)
                continue

        if self.wolf.returncode != 0:
            self._kill_wolf()
            raise AssertionError("Wolf ended with %s rc, stdout:\n%s\nstderr:\n%s" % (
                self.wolf.returncode,
                self.wolf.stdout.read(),
                self.wolf.stderr.read()
            ))

        self.check_wolf_output()

    def json_to_input(self, input):
        if isinstance(input, basestring): return input + "\n"
        if not isinstance(input, list): input = [input]
        return "".join([json.dumps(i) + "\n" for i in input])

    def stdin_write(self, input):
        self.wolf.stdin.write(self.json_to_input(input))

    def tcp_write(self, port, input):
        host = socket.gethostname()
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        i = 0
        while True:
            i += 1
            try:
                s.connect((host, port))
                break
            except socket.error as e:
                if i > 11: raise socket.error("Cannot write to port %s, connection refused" % port)
                sleep(0.1)
                continue

        s.sendall(self.json_to_input(input))
        s.close()

    def stdout_contains(self, obj):
        if not isinstance(obj, list): obj = [obj]
        self.expected_output += obj

    def tcp_listener_contains(self, port, obj):
        if not isinstance(obj, list): obj = [obj]
        self.tcp_listeners_expecting[port] += obj

    def check_wolf_output(self):
        stdout = self.wolf.stdout.read()
        stderr = self.wolf.stderr.read()

        stdout_objects = []
        for line in stdout.split('\n'):
            # logs by wolf are ignored
            if re.match("^\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}.\d{9} .*", line):
                continue

            if line.startswith("[STXXL-MSG] "):
                continue

            if len(line) == 0:
                continue

            try:
                stdout_objects.append(json.loads(line))
            except ValueError:
                print line
                raise

        for line in stderr.split('\n'):
            if line.startswith("[STXXL-ERRMSG] "):
                continue

            if len(line) == 0:
                continue

            raise AssertionError("there is an error line in stderr: \n" + stderr)

        for o in self.expected_output:
            if o not in stdout_objects:
                raise AssertionError("Object:\n%s\nnot in received objects:\n%s" % (
                    json.dumps(o),
                    "\n".join([json.dumps(i) for i in stdout_objects])
                ))

        for port in self.tcp_listeners.keys():
            for obj in self.tcp_listeners_expecting[port]:
                try:
                    if obj not in [json.loads(line) for line in self.tcp_listeners_results[port].split("\n") if line]:
                        raise AssertionError("Object:\n%s\nnot in received objects:\n%s" % (
                            json.dumps(obj),
                            self.tcp_listeners_results[port])
                                             )
                except ValueError:
                    raise ValueError("Couldn't parse output " + self.tcp_listeners_results[port])
