import os
import unittest

from tests.end_to_end_tests.wolf_test_base import WolfTestBase


class ParserTest(WolfTestBase):
    build_name = 'parser'
    input_port = 1111
    output_port = 1112
    parameters = "--input_port %s --output_port %s --config_dir %s" % (
        input_port,
        output_port,
        os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))),
                     "configs") + os.path.sep
    )

    def setUp(self):
        super(ParserTest, self).setUp()
        self.add_tcp_listener(self.output_port)

    def test_first(self):
        self.tcp_write(self.input_port, {
            "@timestamp": "2018-06-21T15:00:00.006Z",
            "message": "starting stuff 1",
            "group": "default",
            "host": "localhost"
        })

        self.tcp_listener_contains(self.output_port, {
            "@timestamp": "2018-06-21T15:00:00.006Z",
            "algorithm": "elapsed",
            "elapsedId": "stuff_elapsed",
            "group": "default",
            "host": "localhost",
            "position": "start",
            "type": "correlation_data",
            "uniqueId": "1"
        })

    def test_second(self):
        self.tcp_write(self.input_port, {
            "@timestamp": "2018-06-21T15:00:00.006Z",
            "message": "2 end stuff",
            "group": "default2",
            "host": "localhost2"
        })

        self.tcp_listener_contains(self.output_port, {
            "@timestamp": "2018-06-21T15:00:00.006Z",
            "algorithm": "elapsed",
            "elapsedId": "stuff_elapsed",
            "group": "default2",
            "host": "localhost2",
            "position": "end",
            "type": "correlation_data",
            "uniqueId": "2"
        })

    def test_unknown_parser(self):
        self.tcp_write(self.input_port, {
            "@timestamp": "2018-06-21T15:00:00.006Z",
            "message": "1 ending stuff",
            "group": "default",
            "host": "localhost2"
        })

        self.tcp_listener_contains(self.output_port, {
            "@timestamp": "2018-06-21T15:00:00.006Z",
            "group": "default",
            "host": "localhost2",
            "logId": "default",
            "message": "1 ending stuff"
        })

    def test_count(self):
        self.tcp_write(self.input_port, [{
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
        ])

        self.tcp_listener_contains(self.output_port, [
            {"count": 1, "logId": "default", "@timestamp": "2018-06-21T15:00:00.006Z", "group": "default",
             "host": "localhost2"},
            {"count": 2, "logId": "start_stuff", "@timestamp": "2018-06-21T15:00:00.006Z", "group": "default",
             "host": "localhost"},
            {"count": 1, "logId": "stop_stuff", "@timestamp": "2018-06-21T15:00:00.006Z", "group": "default2",
             "host": "localhost2"}
        ])


if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(ParserTest)
    unittest.TextTestRunner().run(suite)
