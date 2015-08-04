import unittest
from test_min_max import TestMinMax

if __name__ == '__main__':
  suite = unittest.TestLoader().loadTestsFromTestCase(TestMinMax)
  unittest.TextTestRunner(verbosity=2).run(suite)

