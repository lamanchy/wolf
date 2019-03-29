import socket
import unittest
from time import sleep

from wolf_test_base import WolfTestBase


class CalfParserTest(WolfTestBase):
    build_name = 'calf_parser'
    output_ip = 'localhost'
    group = 'testing'
    parameters = "--output logstash --output_ip %s --group %s --max_loglevel INFO" % (output_ip, group)
    nlog_port = 9556
    log4j_port = 9555
    serilog_port = 9559
    metrics_port = 9557
    output_port = 9070

    def setUp(self):
        super(CalfParserTest, self).setUp()
        self.add_tcp_listener(self.output_port)

    def test_nlog(self):
        self.tcp_write(self.nlog_port, {
            "time": "2018-08-19 10:21:04.951",
            "level": "INFO",
            "loggerName": "RemoteProxies.ServerLocator",
            "threadId": "5",
            "message": "Found active adapter [10.0.13.17] with id [{4C0BCDE1-C39D-4716-A2B2-B38AEADAEA68}]."
        })

        self.tcp_listener_contains(self.output_port, {
            "@timestamp": "2018-08-19T10:21:04.951Z",
            "component": "default",
            "group": self.group,
            "host": socket.gethostname(),
            "level": "INFO",
            "loggerName": "RemoteProxies.ServerLocator",
            "message": "Found active adapter [10.0.13.17] with id [{4C0BCDE1-C39D-4716-A2B2-B38AEADAEA68}].",
            "threadId": "5"})

    def test_log4j(self):
        self.tcp_write(self.log4j_port, {
            "timeMillis": 1534667206861,
            "thread": "background-preinit",
            "level": "INFO",
            "loggerName": "org.hibernate.validator.internal.util.Version",
            "message": "HV000001: Hibernate Validator 5.3.6.Final",
            "endOfBatch": False,
            "loggerFqcn": "org.hibernate.validator.internal.util.logging.Log_$logger",
            "contextMap": {},
            "threadId": 15,
            "threadPriority": 5
        })

        self.tcp_listener_contains(self.output_port, {
            "@timestamp": "2018-08-19T08:26:46.861Z",
            "component": "default",
            "contextMap": {},
            "endOfBatch": False,
            "group": self.group,
            "host": socket.gethostname(),
            "level": "INFO",
            "loggerFqcn": "org.hibernate.validator.internal.util.logging.Log_$logger",
            "loggerName": "org.hibernate.validator.internal.util.Version",
            "message": "HV000001: Hibernate Validator 5.3.6.Final",
            "thread": "background-preinit",
            "threadId": 15,
            "threadPriority": 5
        })

    def test_serilog(self):
        self.tcp_write(self.serilog_port, {
            "timestamp": "2018-02-27T15:11:12.9061030+01:00",
            "level": "Information",
            "message": "Job ticket with 880251d7-fcca-4e19-ba14-a620f4b367a3 has been successfully sent.",
            "jobGuid": "880251d7-fcca-4e19-ba14-a620f4b367a3",
            "sourceContext": "Spooler.Spooler",
            "actionId": "0febc24c-82f3-46ab-bdfd-dc4e7aae2a1c",
            "actionName": "SpoolerSimulator.Controllers.SpoolersController.SendJob (SpoolerSimulator)",
            "requestId": "0HLBTTFQC6TSJ:00000001",
            "requestPath": "/spoolers/76838cee-bed6-4c3e-9e79-a8a800387473/job",
            "activityId": "d8c73212-601f-44d5-b426-0fbb51b4ffd9",
            "logId": "SpoolerJobTicketAccepted",
            "component": "SpoolerSimulator"
        })

        self.tcp_listener_contains(self.output_port, {
            "@timestamp": "2018-02-27T14:11:12.906Z",
            "actionId": "0febc24c-82f3-46ab-bdfd-dc4e7aae2a1c",
            "actionName": "SpoolerSimulator.Controllers.SpoolersController.SendJob (SpoolerSimulator)",
            "activityId": "d8c73212-601f-44d5-b426-0fbb51b4ffd9",
            "component": "SpoolerSimulator",
            "group": self.group,
            "host": socket.gethostname(),
            "jobGuid": "880251d7-fcca-4e19-ba14-a620f4b367a3",
            "level": "INFO",
            "logId": "spoolerJobTicketAccepted",
            "message": "Job ticket with 880251d7-fcca-4e19-ba14-a620f4b367a3 has been successfully sent.",
            "requestId": "0HLBTTFQC6TSJ:00000001",
            "requestPath": "/spoolers/76838cee-bed6-4c3e-9e79-a8a800387473/job",
            "sourceContext": "Spooler.Spooler"})

    def test_metric(self):
        self.tcp_write(self.metrics_port, "something does not really matter some message")

        self.tcp_listener_contains(self.output_port, {
            "group": self.group,
            "message": "something does not really matter some message",
            "type": "metrics"})

    def test_huge_message(self):
        self.tcp_write(self.nlog_port, {
            "time": "2018-08-19 10:21:04.951",
            "level": "INFO",
            "loggerName": "RemoteProxies.ServerLocator",
            "threadId": "5",
            "message": "0" * 33000  # a 1 000 000 zeros
        })
        sleep(0.1)  # need time to process

        self.tcp_listener_contains(self.output_port, {
            "@timestamp": "2018-08-19T10:21:04.951Z",
            "component": "default",
            "group": self.group,
            "host": socket.gethostname(),
            "level": "INFO",
            "loggerName": "RemoteProxies.ServerLocator",
            "message": ("0" * 32000) + "... truncated",
            "threadId": "5"})

    def test_debug_filtering(self):
        self.tcp_write(self.nlog_port, [
            {
                "time": "2018-08-19 10:21:04.951",
                "level": "DEBUG",
                "loggerName": "RemoteProxies.ServerLocator",
                "threadId": "5",
                "message": "0"
            },
            {
                "time": "2018-08-19 10:22:04.951",
                "level": "INFO",
                "loggerName": "RemoteProxies.ServerLocator",
                "threadId": "5",
                "message": "0"
            }
        ])  # TODO fix this
        assert len(self.tcp_listeners_results[self.output_port]) == 0


if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(CalfParserTest)
    unittest.TextTestRunner().run(suite)
