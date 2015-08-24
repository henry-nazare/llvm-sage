from cad import CAD
from sympy import And, Or, Not, Min, Max, Add, Mul, Pow, Lt, Le, Gt, Ge, \
                  Eq, Ne, Integer, Rational, Symbol, Expr as SymPyExpr, \
                  S as SymPyS
from sympy.logic.boolalg import BooleanFunction, BooleanAtom, \
                                BooleanTrue, BooleanFalse
import operator

def is_min_max(s):
  return isinstance(s, Min) or isinstance(s, Max)

def get_min_max(s, invert=False):
  if isinstance(s, Min):
    return (Min if not invert else Max)
  if isinstance(s, Max):
    return (Max if not invert else Min)
  return None

def apply_and_simplify(s, e, op, invert_on_negative=False):
  s_cons = get_min_max(s)
  s_cons_inv = get_min_max(s, invert=True)
  e_cons = get_min_max(e)
  e_cons_inv = get_min_max(e, invert=True)
  do_apply = \
      lambda sa, ea: \
          apply_and_simplify(sa, ea, op, invert_on_negative=invert_on_negative)

  def without_invert_on_negative():
    # Min/Max op Min/Max
    if s_cons != None and e_cons != None:
      args = []
      # Min op Min or Max op Max
      if s_cons == e_cons:
        for sa in s.args:
          for ea in e.args:
            args.append(do_apply(sa, ea))
      # Min op Max
      else:
        for sa in s.args:
          for ea in e.args:
            args.append(do_apply(sa, -ea))
      return s_cons(*args)

    # Min/Max op Sym
    if s_cons != None and isinstance(e, Symbol):
      return s_cons(*map(lambda a: do_apply(a, e), s.args))
    # Sym op Min/Max
    if e_cons != None and isinstance(s, Symbol):
      return e_cons(*map(lambda a: do_apply(s, a), e.args))

    # Min/Max op Int
    if s_cons != None and isinstance(e, Integer):
      return s_cons(*map(lambda a: do_apply(a, e), s.args))
    # Int op Min/Max
    if e_cons != None and isinstance(s, Integer):
      return e_cons(*map(lambda a: do_apply(s, a), e.args))

    return op(s, e)

  def with_invert_on_negative():
    # Min/Max op Min/Max
    if s_cons != None and e_cons != None:
      args = []
      # Min op Min or Max op Max
      if s_cons == e_cons:
        for sa in s.args:
          for ea in e.args:
            args.append(do_apply(sa, ea))
        for sa in s.args:
          for ea in e.args:
            args.append(do_apply(-sa, ea))
      # Min op Max
      else:
        for sa in s.args:
          for ea in e.args:
            args.append(do_apply(sa, -ea))
        for sa in s.args:
          for ea in e.args:
            args.append(do_apply(-sa, -ea))
      return s_cons(*args)

    # Min/Max op Sym
    if s_cons != None and isinstance(e, Symbol):
      args = map(lambda a: do_apply(a, e), s.args) \
          + map(lambda a: do_apply(a, -e), s.args)
      return s_cons(*args)
    # Sym op Min/Max
    if e_cons != None and isinstance(s, Symbol):
      args = map(lambda a: do_apply(s, a), e.args) \
          + map(lambda a: do_apply(-s, a), e.args)
      return e_cons(*args)

    # Min/Max op Int
    if s_cons != None and isinstance(e, Integer):
      sign, cons = (-1, s_cons_inv) if e < 0 else (1, s_cons)
      return sign * cons(*map(lambda a: do_apply(a, e), s.args))
    # Int op Min/Max
    if e_cons != None and isinstance(s, Integer):
      sign, cons = (-1, e_cons_inv) if s < 0 else (1, e_cons)
      return sign * cons(*map(lambda a: do_apply(s, a), e.args))

    return op(s, e)

  if invert_on_negative:
    return with_invert_on_negative()
  return without_invert_on_negative()

def is_le(s, e, assumptions):
  return Min(s, e) == s or Max(s, e) == e

def is_ge(s, e, assumptions):
  return Max(s, e) == s or Min(s, e) == e

class Expr(object):
  initialized = False

  # TODO: these caches should probably be fixed-sized LRU structures.
  min_cache = {}
  max_cache = {}

  @staticmethod
  def init():
    # FIXME: Are (+-)oo correctly handled?
    Expr.__add__ = \
        lambda s, e: Expr(apply_and_simplify(s.expr, e.expr, operator.add))
    Expr.__sub__ = \
        lambda s, e: Expr(apply_and_simplify(s.expr, e.expr, operator.sub))
    Expr.__mul__ = \
        lambda s, e: Expr(apply_and_simplify(s.expr, e.expr, operator.mul, \
            invert_on_negative=True))
    Expr.__div__ = \
        lambda s, e: Expr(apply_and_simplify(s.expr, e.expr, operator.div, \
            invert_on_negative=True))
    Expr.__pow__ = lambda s, e: Expr(s.expr ** e.expr)
    Expr.__neg__ = lambda s: Expr(-s.expr)

    Expr.eq = lambda s, e: Expr(Eq(s.expr, e.expr))
    Expr.ne = lambda s, e: Expr(Ne(s.expr, e.expr))
    Expr.__lt__ = lambda s, e: Expr(Lt(s.expr, e.expr))
    Expr.__le__ = lambda s, e: Expr(Le(s.expr, e.expr))
    Expr.__gt__ = lambda s, e: Expr(Gt(s.expr, e.expr))
    Expr.__ge__ = lambda s, e: Expr(Ge(s.expr, e.expr))

    Expr.__and__    = lambda s, e: Expr(And(s.expr, e.expr))
    Expr.__or__     = lambda s, e: Expr(Or(s.expr,  e.expr))
    Expr.__invert__ = lambda s:    Expr(Not(s.expr))

    Expr.is_empty    = lambda s: s.is_eq(Expr.empty)

    Expr.is_inf       = \
        lambda s: s.expr == SymPyS.Infinity or s.expr == -SymPyS.Infinity
    Expr.is_plus_inf  = lambda s: s.expr == SymPyS.Infinity
    Expr.is_minus_inf = lambda s: s.expr == -SymPyS.Infinity

    Expr.is_constant = lambda s: isinstance(s.expr, Integer)
    Expr.is_integer  = lambda s: isinstance(s.expr, Integer)
    Expr.is_rational = lambda s: isinstance(s.expr, Rational)
    Expr.is_symbol   = lambda s: isinstance(s.expr, Symbol)

    Expr.is_min = lambda s: isinstance(s.expr, Min)
    Expr.is_max = lambda s: isinstance(s.expr, Max)
    Expr.is_add = lambda s: isinstance(s.expr, Add)
    Expr.is_mul = lambda s: isinstance(s.expr, Mul)
    Expr.is_pow = lambda s: isinstance(s.expr, Pow)

    Expr.get_integer = lambda s: s.expr.p
    Expr.get_numer   = lambda s: Expr(s.expr.p)
    Expr.get_denom   = lambda s: Expr(s.expr.q)
    Expr.get_exp     = lambda s: Expr(s.expr.exp)
    Expr.get_base    = lambda s: Expr(s.expr.base)

    Expr.get_name = lambda s: s.expr.name

    Expr.compare = lambda s, e: s.compare(e)

    # Empty. When min/max is invalid.
    Expr.empty = Expr("EMPTY")

  @staticmethod
  def get_nan():
    return Expr(SymPyS.NaN)

  @staticmethod
  def get_plus_inf():
    return Expr(SymPyS.Infinity)

  @staticmethod
  def get_minus_inf():
    return Expr(-SymPyS.Infinity)

  @staticmethod
  def get_true():
    return Expr(True)

  @staticmethod
  def get_false():
    return Expr(False)

  def __eq__(self, other):
    if not hasattr(other.expr, "_sage_"):
      return other.expr == self.expr

    return self.expr == other.expr

  def __ne__(self, other):
    if not hasattr(other.expr, "_sage_"):
      return other.expr != self.expr

    return self.expr != other.expr

  def __init__(self, val):
    if not Expr.initialized:
      Expr.initialized = True
      Expr.init()

    if isinstance(val, int) or isinstance(val, long):
      self.expr = Integer(val)
    elif isinstance(val, basestring):
      self.expr = Symbol(val)
    elif isinstance(val, bool) or isinstance(val, BooleanAtom):
      self.expr = SymPyS.One if val else SymPyS.Zero
    else:
      assert isinstance(val, SymPyExpr) or isinstance(val, BooleanFunction) \
             or (val == SymPyS.Infinity) or (val == -SymPyS.Infinity)
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

  def sympy(self):
    return self.expr

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
  def minmax_args(expr, ty):
    if isinstance(expr, ty):
      return list(expr.args)
    return [expr]

  def reduce_min(self, args, assumptions):
    m = Min(*args)

    if not isinstance(m, Min):
      # Expression was simplified.
      return [m]

    # Use the reduced argument list.
    args = m.args

    del_args = [False] * len(args)
    for i in xrange(len(args)):
      if del_args[i]: continue
      for j in xrange(i + 1, len(args)):
         if del_args[j]: continue

         key = (args[i], args[j], assumptions)
         if Expr.min_cache.has_key(key):
           rest, resf = Expr.min_cache[key]
         else:
           rest = Min(args[i], args[j]) == args[i]
           resf = Max(args[i], args[j]) == args[i]
           Expr.min_cache[key] = (rest, resf)

         if CAD.is_true(rest):
           del_args[j] = True
         if CAD.is_true(resf):
           del_args[i] = True

    res_args = [args[i] for i in xrange(len(args)) if not del_args[i]]
    return res_args

  def reduce_max(self, args, assumptions):
    m = Max(*args)

    if not isinstance(m, Max):
      # Expression was simplified.
      return [m]

    # Use the reduced argument list.
    args = m.args

    del_args = [False] * len(args)
    for i in xrange(len(args)):
      if del_args[i]: continue
      for j in xrange(i + 1, len(args)):
         if del_args[j]: continue

         key = (args[i], args[j], assumptions)
         if Expr.max_cache.has_key(key):
           rest, resf = Expr.max_cache[key]
         else:
           rest = Max(args[i], args[j]) == args[i]
           resf = Min(args[i], args[j]) == args[i]
           Expr.max_cache[key] = (rest, resf)

         if CAD.is_true(rest):
           del_args[j] = True
         if CAD.is_true(resf):
           del_args[i] = True

    res_args = [args[i] for i in xrange(len(args)) if not del_args[i]]
    return res_args

  def min_or_max(self, other, op, assumptions):
    def contains(val, array):
      return Expr(val) in map(Expr, array)

    assumptions = assumptions.expr if assumptions else False
    try:
      assert op == Min or op == Max
      args = Expr.minmax_args(self.expr, op) + \
             Expr.minmax_args(other.expr, op)

      # Max(a, Min(b, c)) or Min(a, Max(b, c))
      inv_op = Min if op == Max else Max
      cmp_op = is_le if op == Max else is_ge
      if isinstance(other.expr, inv_op):
        if contains(self.expr, other.expr.args):
          return self
        for a in other.expr.args:
          if cmp_op(a, self.expr, assumptions):
            return self
      if isinstance(self.expr, inv_op):
        if contains(other.expr, self.expr.args):
          return other
        for a in self.expr.args:
          if cmp_op(a, other.expr, assumptions):
            return other

      if (self.expr == SymPyS.Infinity):
        return other
      elif (other.expr == SymPyS.Infinity):
        return self

      if (self.expr == -SymPyS.Infinity):
        return self
      elif (other.expr == -SymPyS.Infinity):
        return other

      # TODO: handle min/max parameters.
      if any(map(lambda e: e.has(Min) or e.has(Max), args)):
        return Expr(op(self.expr, other.expr))

      if op == Min:
        res_args = self.reduce_min(args, assumptions)
      else:
        res_args = self.reduce_max(args, assumptions)
      if not res_args:
        return Expr.empty
      return Expr(op(*res_args))
    except BaseException as b:
      print "Exception triggered: min_or_max", self, other
      print b
      raise

  def min(self, other, assumptions=None):
    return self.min_or_max(other, Min, assumptions)

  def max(self, other, assumptions=None):
    return self.min_or_max(other, Max, assumptions)

  def size(self):
    if isinstance(self.expr, Min) or isinstance(self.expr, Max):
      return len(self.expr.args)
    if not hasattr(self.expr, "args"):
      return 1

    if hasattr(self.expr.args, "__call__"):
      return sum(map(lambda a: Expr(a).size(), self.expr.args()))
    return sum(map(lambda a: Expr(a).size(), self.expr.args))

class S:
  Zero = Expr(0)
  One = Expr(1)

