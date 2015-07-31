import unittest
from llvmsage.cad import CAD
from llvmsage.expr import Expr
from sympy import Min, Max

a, b = Expr("a"), Expr("b")

class TestMinMax(unittest.TestCase):
  def verify_expr(self, s, instance, args):
    self.assertIsInstance(s.expr, instance)
    self.assertItemsEqual(s.expr.args, map(lambda a: a.expr, args))

  def test_simple_min_max(self):
    self.verify_expr(a.min(b), Min, [a, b])
    self.verify_expr(a.max(b), Max, [a, b])

