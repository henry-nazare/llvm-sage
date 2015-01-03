from expr import Expr
from sage.all import MixedIntegerLinearProgram
from sympy import Add, Mul, Integer, Symbol, StrictLessThan, LessThan, \
                  StrictGreaterThan, GreaterThan, Unequality, Equality
from operator import add, mul, lt, le, gt, ge, ne, eq

methods = {
  StrictLessThan:    lt,
  LessThan:          le,
  StrictGreaterThan: gt,
  GreaterThan:       ge,
  Unequality:        ne,
  Equality:          eq
}

def convert(milp, expr):
  if isinstance(expr, Integer):
    return int(expr)
  elif isinstance(expr, Symbol):
    return milp.var[str(expr)]
  elif isinstance(expr, Add):
    return reduce(add, map(lambda e: convert(milp, e), expr.args), 0)
  elif isinstance(expr, Mul):
    return reduce(mul, map(lambda e: convert(milp, e), expr.args), 1)
  print expr, type(expr)
  assert type(expr) in methods
  return methods[type(expr)](convert(milp, expr.lhs), convert(milp, expr.rhs))

class MILP:
  def __init__(self):
    self.milp = MixedIntegerLinearProgram(maximization=False)
    self.var = self.milp.new_variable(integer=True)

  def add_constraint(self, expr):
    self.milp.add_constraint(convert(self, expr.sympy()))

  def set_objective(self, expr):
    self.milp.set_objective(convert(self, expr.sympy()))

  def solve(self):
    self.milp.solve()
    return {k: int(v) for k, v in self.milp.get_values(self.var).iteritems()}

