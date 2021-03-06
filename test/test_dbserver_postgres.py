#!/usr/bin/env python2
import unittest
import os, time
from socket import *

from testdc import test_dc
from common import Daemon, MDConnection
from test_dbserver import DatabaseBaseTests


CONFIG = """\
messagedirector:
    bind: 127.0.0.1:57123

general:
    dc_files:
        - %r

roles:
    - type: database
      control: 777
      generate:
        min: 1000000
        max: 1000010
      engine:
        type: postgresql
        database: astron_test
""" % test_dc

class TestDatabaseServerPostgres(unittest.TestCase, DatabaseBaseTests):
    @classmethod
    def setUpClass(cls):
        cls.daemon = Daemon(CONFIG)
        cls.daemon.start()

        sock = socket(AF_INET, SOCK_STREAM)
        sock.connect(('127.0.0.1', 57123))
        cls.conn = MDConnection(sock)

    @classmethod
    def tearDownClass(cls):
        time.sleep(0.25) # Wait for database to finish any operations
        cls.conn.close()
        cls.daemon.stop()

if __name__ == '__main__':
    unittest.main()