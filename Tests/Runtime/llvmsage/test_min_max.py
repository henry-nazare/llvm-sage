import unittest
from llvmsage.cad import CAD
from llvmsage.expr import Expr
from sympy import Min, Max

a, b, c, d, n = Expr("a"), Expr("b"), Expr("c"), Expr("d"), Expr("n")
mone, zero, one, two = Expr(-1), Expr(0), Expr(1), Expr(2)

class TestMinMax(unittest.TestCase):
  def verify_expr(self, s, instance, args):
    self.assertIsInstance(s.expr, instance)
    self.assertItemsEqual(s.expr.args, map(lambda a: a.expr, args))

  def test_simple_min_max(self):
    self.verify_expr(a.min(b), Min, [a, b])
    self.verify_expr(a.max(b), Max, [a, b])

  def test_multiple_min_max(self):
    self.verify_expr(a.min(b).min(c), Min, [a, b, c])
    self.verify_expr(a.max(b).max(c), Max, [a, b, c])

  def test_multiple_mixed_min_max(self):
    self.verify_expr(a.min(b).max(c), Max, [c, a.min(b)])
    self.verify_expr(a.max(b).min(c), Min, [c, a.max(b)])

  def test_simplify_min_max(self):
    self.verify_expr(a.min(b).min(a), Min, [a, b])
    self.verify_expr(a.max(b).max(a), Max, [a, b])

  def test_simplify_mixed_min_max(self):
    # TODO: can be simplified to Min(a, b) instead of Max(a, Min(a, b)).
    self.verify_expr(a.min(b).max(a), Max, [a, b])
    self.verify_expr(a.max(b).min(a), Min, [a, b])

  def test_simlpify_min_max_add_expr_constants(self):
    self.verify_expr(a.min(b).min(a + one), Min, [a, b])
    self.verify_expr(a.min(b).min(a - one), Min, [a - one, b])
    self.verify_expr(a.max(b).max(a + one), Max, [a + one, b])
    self.verify_expr(a.max(b).max(a - one), Max, [a, b])

  def test_simlpify_min_max_add_expr_syms(self):
    self.verify_expr(a.min(b).min(a + n), Min, [a, b, a + n])
    self.verify_expr(a.min(b).min(a - n), Min, [a, b, a - n])
    self.verify_expr(a.max(b).max(a + n), Max, [a, b, a + n])
    self.verify_expr(a.max(b).max(a - n), Max, [a, b, a - n])

  def test_simlpify_min_max_add_expr_syms_with_assumptions(self):
    self.verify_expr( \
        a.min(b).min(a + n, assumptions=(n > zero)), Min, [a, b])
    self.verify_expr( \
        a.min(b).min(a - n, assumptions=(n > zero)), Min, [b, a - n])
    self.verify_expr( \
        a.max(b).max(a + n, assumptions=(n > zero)), Max, [b, a + n])
    self.verify_expr( \
        a.max(b).max(a - n, assumptions=(n > zero)), Max, [a, b])

  def test_simlpify_min_max_mul_expr_constants(self):
    self.verify_expr(a.min(b).min(a * two), Min, [a, b, a * two])
    self.verify_expr(a.min(b).min(a/two), Min, [a, b, a/two])
    self.verify_expr(a.max(b).max(a * two), Max, [a, b, a * two])
    self.verify_expr(a.max(b).max(a/two), Max, [a, b, a/two])

  def test_simlpify_min_max_mul_expr_constants_with_assumptions(self):
    self.verify_expr( \
        a.min(b).min(a * two, assumptions=(a > zero)), Min, [a, b])
    self.verify_expr( \
        a.min(b).min(a/two, assumptions=(a > zero)), Min, [b, a/two])
    self.verify_expr( \
        a.max(b).max(a * two, assumptions=(a > zero)), Max, [b, a * two])
    self.verify_expr( \
        a.max(b).max(a/two, assumptions=(a > zero)), Max, [a, b])

  def test_simlpify_min_max_mul_expr_syms(self):
    self.verify_expr(a.min(b).min(a * n), Min, [a, b, a * n])
    self.verify_expr(a.min(b).min(a/n), Min, [a, b, a/n])
    self.verify_expr(a.max(b).max(a * n), Max, [a, b, a * n])
    self.verify_expr(a.max(b).max(a/n), Max, [a, b, a/n])

  def test_simlpify_min_max_mul_expr_syms_with_assumptions(self):
    self.verify_expr( \
        a.min(b).min(a * n, assumptions=((a > zero) & (n > one))), Min, [a, b])
    # TODO: the following test times out with QEPCAD (> 1 minute).
    self.verify_expr( \
        a.min(b).min(a/n, assumptions=((a > zero) & (n > one))), Min, [b, a/n])
    self.verify_expr( \
        a.max(b).max(a * n, assumptions=((a > zero) & (n > one))), Max,
        [b, a * n])
    self.verify_expr( \
        a.max(b).max(a/n, assumptions=((a > zero) & (n > one))), Max, [a, b])

