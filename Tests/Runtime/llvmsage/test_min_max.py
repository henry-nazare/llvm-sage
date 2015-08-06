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

  def verify_simple_expr(self, s, val):
    self.assertEqual(s, val)

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
    self.verify_simple_expr(a.min(b).max(a), a)
    self.verify_simple_expr(a.max(b).min(a), a)
    self.verify_simple_expr(a.min(b).min(c).max(a), a)
    self.verify_simple_expr(a.max(b).max(c).min(a), a)

  def test_simplify_mixed_min_max2(self):
    # Max(a, Min(b, c))
    # Cond      => Min(b, c) | Max(a, Min(b, c))
    # a < b < c => b         | b
    # a < c < b => c         | c
    # b < a < c => b         | a
    # b < c < a => b         | a
    # c < a < b => c         | a
    # c < b < a => c         | a
    # For b < a (same for c < a):
    # b < a < c => b         | a
    # b < c < a => b         | a
    # c < b < a => c         | a
    self.verify_expr(b.min(c).max(a), Max, [a, b.min(c)])
    self.verify_expr(b.max(c).min(a), Min, [a, b.max(c)])
    self.verify_simple_expr(b.min(a - one).max(a), a)
    self.verify_simple_expr(b.max(a + one).min(a), a)
    self.verify_simple_expr(b.min(c).max(a, assumptions=(b < a)), a)
    self.verify_simple_expr(b.max(c).min(a, assumptions=(b > a)), a)

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
    self.verify_expr( \
        a.max(b).max(a * n, assumptions=((a > zero) & (n > one))), Max,
        [b, a * n])

  @unittest.expectedFailure
  def test_simlpify_min_max_mul_expr_syms_with_assumptions_fail(self):
    # TODO: the following tests times out with QEPCAD (> 1 minute).
    self.verify_expr( \
        a.min(b).min(a/n, assumptions=((a > zero) & (n > one))), Min, [b, a/n])
    self.verify_expr( \
        a.max(b).max(a/n, assumptions=((a > zero) & (n > one))), Max, [a, b])

  def test_op_on_contained_min_max(self):
    self.verify_expr( \
        a.min(b).max(c) + one, Max, [c + one, (a + one).min(b + one)])
    self.verify_expr( \
        a.min(b).max(c) * two, Max, [c * two, (a * two).min(b * two)])
    self.verify_simple_expr( \
        a.min(b).max(c) * (-two), -min(-two * c, -(-two * a).max(-two * b)))
    self.verify_simple_expr( \
        a.max(b).min(c) * (-two), -max(-two * c, -(-two * a).min(-two * b)))

