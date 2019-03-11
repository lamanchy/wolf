import unittest
from os.path import isfile
from subprocess import Popen, PIPE
from threading import Lock

build_lock = Lock()


class BaseTest(object):
    @classmethod
    def setUpClass(cls):
        global build_lock
        build_lock.acquire()
        rc = Popen(
            "/snap/clion/61/bin/cmake/linux/bin/cmake --build /home/lamanchy/CLionProjects/wolf/cmake-build-debug --target all -- -j 2",
            shell=True).wait()
        assert rc == 0
        build_lock.release()

    def setUp(self):
        assert isfile("../cmake-build-debug/parser")
        self.wolf = Popen("../cmake-build-debug/parser", shell=True, stdin=PIPE, stdout=PIPE, stderr=PIPE)

    def tearDown(self):
        pass

    def for_input_expects_output(self, input, output):
        stdout, stderr = self.wolf.communicate(input)
        assert output in stdout
        # assert len(stderr) == 0
        assert self.wolf.returncode == 0


class TestStringMethods(BaseTest, unittest.TestCase):
    def test_upper(self):
        input = """{"@timestamp": "2018-06-21T15:00:00.006Z", "message": "starting stuff 1", "group": "default", "host": "localhost"}"""
        output = """{"@timestamp":"2018-06-21T15:00:00.006Z","algorithm":"elapsed","elapsedId":"stuff_elapsed","group":"default","host":"localhost","position":"start","type":"correlation_data","uniqueId":"1"}"""
        self.for_input_expects_output(input, output)


if __name__ == '__main__':
    unittest.main()
