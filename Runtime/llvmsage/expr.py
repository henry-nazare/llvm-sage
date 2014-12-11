from cad import CAD
from sympy import And, Or, Not, Min, Max, Lt, Le, Gt, Ge, Eq, Ne, \
                  S, Integer, Symbol, Expr as SymPyExpr
from sympy.logic.boolalg import BooleanFunction, BooleanAtom, \
                                BooleanTrue, BooleanFalse
from operator import neg

class Expr(object):
  initialized = False

  @staticmethod
  def init():
    # FIXME: Are (+-)oo correctly handled?
    Expr.__mul__ = lambda s, e: Expr(s.expr *  e.expr)
    Expr.__div__ = lambda s, e: Expr(s.expr /  e.expr)
    Expr.__pow__ = lambda s, e: Expr(s.expr ** e.expr)
    Expr.__neg__ = lambda s: Expr(-s.expr)

    Expr.__eq__ = lambda s, e: Expr(Eq(s.expr, e.expr))
    Expr.__ne__ = lambda s, e: Expr(Ne(s.expr, e.expr))
    Expr.__lt__ = lambda s, e: Expr(Lt(s.expr, e.expr))
    Expr.__le__ = lambda s, e: Expr(Le(s.expr, e.expr))
    Expr.__gt__ = lambda s, e: Expr(Gt(s.expr, e.expr))
    Expr.__ge__ = lambda s, e: Expr(Ge(s.expr, e.expr))

    Expr.__and__    = lambda s, e: Expr(And(s.expr, e.expr))
    Expr.__or__     = lambda s, e: Expr(Or(s.expr,  e.expr))
    Expr.__invert__ = lambda s:    Expr(Not(s.expr))

    Expr.is_eq       = lambda s, e: s.expr == e.expr
    Expr.is_ne       = lambda s, e: s.expr != e.expr
    Expr.is_inf      = lambda s: s.expr == S.Infinity or s.expr == -S.Infinity
    Expr.is_empty    = lambda s: s.is_eq(Expr.empty)
    Expr.is_constant = lambda s: isinstance(s.expr, Integer)

    Expr.is_min = lambda s: isinstance(s.expr, Min)
    Expr.is_max = lambda s: isinstance(s.expr, Max)

    Expr.compare = lambda s, e: s.compare(e)

    # Empty. When min/max is invalid.
    Expr.empty = Expr("EMPTY")

  def __add__(self, other):
    if self.is_inf() and not other.is_inf():
      return self
    if not self.is_inf() and other.is_inf():
        return other
    return Expr(self.expr + other.expr)

  def __sub__(self, other):
    if self.is_inf() and not other.is_inf():
      return self
    if not self.is_inf() and other.is_inf():
        return -other
    return Expr(self.expr - other.expr)

  @staticmethod
  def get_nan():
    return Expr(S.NaN)

  @staticmethod
  def get_plus_inf():
    return Expr(S.Infinity)

  @staticmethod
  def get_minus_inf():
    return Expr(-S.Infinity)

  @staticmethod
  def get_true():
    return Expr(True)

  @staticmethod
  def get_false():
    return Expr(False)


  def __init__(self, val):
    if not Expr.initialized:
      Expr.initialized = True
      Expr.init()

    if isinstance(val, int) or isinstance(val, long):
      self.expr = Integer(val)
    elif isinstance(val, basestring):
      self.expr = Symbol(val)
    elif isinstance(val, bool) or isinstance(val, BooleanAtom):
      self.expr = S.One if val else S.Zero
    else:
      assert isinstance(val, SymPyExpr) or isinstance(val, BooleanFunction) \
             or (val == S.Infinity) or (val == -S.Infinity)
      self.expr = val

  def __str__(self):
    return self.expr.__str__()

  def __repr__(self):
    r = self.__str__()
    return r

  def subs(self, expr_from, expr_to):
    return Expr(self.expr.subs([(expr_from.expr, expr_to.expr)]))

  def args(self):
    return map(Expr, self.expr.args)

  @staticmethod
  def reduce_conditional(expr):
      if isinstance(expr, And):
        # And(x, y)
        x, y = expr.args
        # And(Or(a, b), y)
        if isinstance(x, Or):
          a, b = x.args
          # And(Or(a, b), Or(c, d))
          if isinstance(y, Or):
            c, d = y.args
            return [[a, c], [a, d], [b, c], [b, d]]
        # And(Or(a, b), y)
        red_args = map(reduce_contional, expr.args)
        red_args = sum(red_args, [])
        return red_args
      elif isinstance(expr.expr, Or):
        red_args = map(reduce_contional, expr.args)
        return red_args
      return expr

  @staticmethod
  def minmax_args(expr, ty, neg_ty):
    if isinstance(expr, ty):
      return list(expr.args)
    elif isinstance(expr, neg_ty):
      return map(neg, list(expr.args))
    return [expr]

  def reduce_min(self, args, assumptions):
    m = Min(*args)

    if not isinstance(m, Min):
      # Expression was simplified.
      return [m]

    del_args = [False] * len(args)
    for i in xrange(len(args)):
      if del_args[i]: continue
      for j in xrange(i + 1, len(args)):
         if del_args[j]: continue

         rest = CAD.implies(assumptions, args[i] <= args[j])
         resf = CAD.implies(assumptions, args[i] >= args[j])
         resi = CAD.implies(assumptions, args[i] >  args[j])

         if CAD.is_true(rest) and CAD.is_true(resi):
           del_args[i] = True
           del_args[j] = True
         elif CAD.is_true(rest) or CAD.is_false(resf):
           del_args[j] = True
         elif CAD.is_false(rest) or CAD.is_true(resf):
           del_args[i] = True

    res_args = [args[i] for i in xrange(len(args)) if not del_args[i]]
    return res_args

  def min(self, other, assumptions=None):
    try:
      args = Expr.minmax_args(self.expr, Min, Max) + \
             Expr.minmax_args(other.expr, Min, Max)

      if isinstance(other.expr, Max):
        if len(other.expr.args) == 2 \
           and (other.expr.args[0] == self.expr \
                or other.expr.args[1] == self.expr):
          return self

      if (self.expr == S.Infinity):
        return other
      elif (other.expr == S.Infinity):
        return self

      if (self.expr == -S.Infinity):
        return self
      elif (other.expr == -S.Infinity):
        return other

      # TODO: handle min/max parameters.
      if any(map(lambda e: e.has(Min) or e.has(Max), args)):
        return Expr(Min(self.expr, other.expr))

      res_args = \
        self.reduce_min(args, assumptions.expr if assumptions else False)
      if not res_args:
        return Expr.empty
      return Expr(Min(*res_args))
    except BaseException as b:
      print "Exception triggered: min", self, other
      print b
      raise

  def max(self, other, assumptions=None):
    res = (-self).min(-other, assumptions)
    if isinstance(res.expr, Min):
      res_args = map(neg, res.expr.args)
      return Expr(Max(*res_args))
    return res if res.is_eq(Expr.empty) else -res

