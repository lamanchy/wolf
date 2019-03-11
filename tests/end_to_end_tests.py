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


class TestStringMethods(BaseTest, unittest.TestCase):
    def setUp(self):
        self.assertTrue(isfile("../cmake-build-debug/parser"))
        self.wolf = Popen("../cmake-build-debug/parser", shell=True, stdin=PIPE, stdout=PIPE, stderr=PIPE)

    def test_upper(self):
        stdout, stderr = self.wolf.communicate(
            """{"@timestamp": "2018-06-21T15:00:00.006Z", "message": "starting stuff 1", "group": "default", "host": "localhost"}""")

        self.assertTrue(
            """{"@timestamp":"2018-06-21T15:00:00.006Z","algorithm":"elapsed","elapsedId":"stuff_elapsed","group":"default","host":"localhost","position":"start","type":"correlation_data","uniqueId":"1"}"""
            in stdout)


if __name__ == '__main__':
    unittest.main()
