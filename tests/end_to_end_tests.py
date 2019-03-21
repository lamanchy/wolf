#!/usr/bin/env python

import json
import re
import unittest
import os
from os.path import isfile
from subprocess import Popen, PIPE
from threading import Lock

import sys

build_lock = Lock()
BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


build_path_part = 'local'
if len(sys.argv) == 2:
    build_path_part = sys.argv[1]
    sys.argv.pop(1)


class BaseTest(object):
    # @classmethod
    # def setUpClass(cls):
    # global build_lock
    # build_lock.acquire()
    # rc = Popen(
    #     "cmake --build /home/lamanchy/CLionProjects/wolf/cmake-build-debug --target all -- -j 4",
    #     shell=True).wait()
    # assert rc == 0
    # build_lock.release()

    def setUp(self):
        global build_path_part
        parser = os.path.join(BASE_DIR, 'build', build_path_part, 'Debug', 'parser')
        assert isfile(parser)
        self.wolf = Popen(parser, shell=True, stdin=PIPE, stdout=PIPE, stderr=PIPE)

    def tearDown(self):
        pass

    def for_input_expects_output(self, input, output):
        if not isinstance(input, list): input = [input]
        if not isinstance(output, list): output = [output]
        stdout, stderr = self.wolf.communicate("\n".join([json.dumps(i) for i in input]))

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

            raise AssertionError("there is an error line in stderr: \n" + line)

        for o in output:
            if o not in stdout_objects:
                raise AssertionError("Object:\n%s\nnot in received objects:\n%s" % (
                    json.dumps(o),
                    "\n".join([json.dumps(i) for i in stdout_objects])
                ))
        # assert len(stderr) == 0
        assert self.wolf.returncode == 0


class TestStringMethods(BaseTest, unittest.TestCase):
    def test_first(self):
        self.for_input_expects_output(
            {
                "@timestamp": "2018-06-21T15:00:00.006Z",
                "message": "starting stuff 1",
                "group": "default",
                "host": "localhost"
            },
            {
                "@timestamp": "2018-06-21T15:00:00.006Z",
                "algorithm": "elapsed",
                "elapsedId": "stuff_elapsed",
                "group": "default",
                "host": "localhost",
                "position": "start",
                "type": "correlation_data",
                "uniqueId": "1"
            }
        )

    def test_second(self):
        self.for_input_expects_output(
            {
                "@timestamp": "2018-06-21T15:00:00.006Z",
                "message": "2 end stuff",
                "group": "default2",
                "host": "localhost2"
            },
            {
                "@timestamp": "2018-06-21T15:00:00.006Z",
                "algorithm": "elapsed",
                "elapsedId": "stuff_elapsed",
                "group": "default2",
                "host": "localhost2",
                "position": "end",
                "type": "correlation_data",
                "uniqueId": "2"
            }
        )

    def test_unknown_parser(self):
        self.for_input_expects_output(
            {
                "@timestamp": "2018-06-21T15:00:00.006Z",
                "message": "1 ending stuff",
                "group": "default",
                "host": "localhost2"
            },
            {
                "@timestamp": "2018-06-21T15:00:00.006Z",
                "group": "default",
                "host": "localhost2",
                "logId": "default",
                "message": "1 ending stuff"
            }
        )

    def test_count(self):
        self.for_input_expects_output(
            [
                {
                    "@timestamp": "2018-06-21T15:00:00.006Z",
                    "message": "1 ending stuff",
                    "group": "default",
                    "host": "localhost2"
                },
                {
                    "@timestamp": "2018-06-21T15:00:00.006Z",
                    "message": "2 end stuff",
                    "group": "default2",
                    "host": "localhost2"
                },
                {
                    "@timestamp": "2018-06-21T15:00:00.006Z",
                    "message": "starting stuff 1",
                    "group": "default",
                    "host": "localhost"
                },
                {
                    "@timestamp": "2018-06-21T15:00:00.006Z",
                    "message": "starting stuff 2",
                    "group": "default",
                    "host": "localhost"
                },
            ],
            [
                {"count": 1, "logId": "default", "@timestamp": "2018-06-21T15:00:00.006Z", "group": "default",
                 "host": "localhost2"},
                {"count": 2, "logId": "start_stuff", "@timestamp": "2018-06-21T15:00:00.006Z", "group": "default",
                 "host": "localhost"},
                {"count": 1, "logId": "stop_stuff", "@timestamp": "2018-06-21T15:00:00.006Z", "group": "default2",
                 "host": "localhost2"}
            ]
        )


if __name__ == '__main__':
    unittest.main()
