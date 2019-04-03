import unittest
from time import sleep

from tests.end_to_end_tests.wolf_test_base import WolfTestBase


class CorrelatorTest(WolfTestBase):
    build_name = 'correlator'
    parameters = "--test --stream_sort_seconds 1 --max_seconds_to_keep 1"

    @staticmethod
    def get_start_event(unique_id=1, elapsed_id=1):
        return {"@timestamp": "2018-06-21T14:00:00.001Z", "position": "start", "uniqueId": str(unique_id),
                "elapsedId": str(elapsed_id), "group": "default", "host": "localhost"}

    @staticmethod
    def get_end_event(unique_id=1, elapsed_id=1):
        return {"@timestamp": "2018-06-21T14:00:00.006Z", "position": "end", "uniqueId": str(unique_id),
                "elapsedId": str(elapsed_id), "group": "default", "host": "localhost"}

    @staticmethod
    def get_elapsed_event(unique_id=1, elapsed_id=1):
        return {"message": "elapsed,elapsedId=%s,status=ok,start_host=localhost,end_host=localhost,group=default "
                           "uniqueId=\"%s\",duration=5000000i 1529589600001000000" % (elapsed_id, unique_id),
                "type": "metrics"}

    @staticmethod
    def get_expired_event(unique_id=1, elapsed_id=1):
        return {"message": "elapsed,elapsedId=%s,status=expired,start_host=localhost,group=default uniqueId=\"%s\" "
                           "1529589600001000000" % (elapsed_id, unique_id),
                "type": "metrics"}

    def test_correlation(self):
        self.stdin_write([self.get_start_event(), self.get_end_event()])
        self.stdout_contains(self.get_elapsed_event())
        sleep(2)

    def test_expired(self):
        self.stdin_write([self.get_start_event()])
        self.stdout_contains(self.get_expired_event())
        sleep(4)

    def test_ignore_end(self):
        self.stdin_write([self.get_end_event()])
        sleep(2)

    def test_ignore_two_starts(self):
        self.stdin_write([self.get_start_event(), self.get_start_event(), self.get_end_event()])
        self.stdout_contains(self.get_elapsed_event())
        sleep(2)

    def test_sort_events(self):
        self.stdin_write([self.get_end_event(), self.get_start_event()])
        self.stdout_contains(self.get_elapsed_event())
        sleep(2)

    def test_fails_sort_hen_too_long_timeout(self):
        self.stdin_write(self.get_end_event())
        sleep(4)
        self.stdin_write(self.get_start_event())

        self.stdout_contains(self.get_expired_event())
        sleep(4)

    def test_multiple_of_everything(self):
        self.stdin_write([
            self.get_start_event(1, 1),
            self.get_end_event(1, 1),
            self.get_end_event(1, 2),
            self.get_start_event(1, 2),
            self.get_start_event(2, 3),
            self.get_end_event(2, 3),
            self.get_start_event(2, 3),
            self.get_end_event(3, 3),
            self.get_end_event(3, 3),
            self.get_start_event(3, 3),
            self.get_start_event(3, 3),
            self.get_start_event(3, 3),
            self.get_start_event(3, 3),
            self.get_start_event(4, 1),
            self.get_end_event(4, 2),
        ])
        sleep(3)
        self.stdout_contains([
            self.get_elapsed_event(1, 1),
            self.get_elapsed_event(1, 2),
            self.get_elapsed_event(2, 3),
            self.get_elapsed_event(3, 3),
            self.get_expired_event(4, 1)
        ])
        sleep(3)


if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(CorrelatorTest)
    unittest.TextTestRunner().run(suite)
