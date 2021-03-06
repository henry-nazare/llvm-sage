from cad import CAD
from sympy import And, Or, Not, Min, Max, Add, Mul, Pow, Lt, Le, Gt, Ge, \
                  Eq, Ne, S, Integer, Rational, Symbol, Expr as SymPyExpr
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

  def without_invert_on_negative():
    # Min/Max op Min/Max
    if s_cons != None and e_cons != None:
      args = []
      # Min op Min or Max op Max
      if s_cons == e_cons:
        for sa in s.args:
          for ea in e.args:
            args.append(op(sa, ea))
      # Min op Max
      else:
        for sa in s.args:
          for ea in e.args:
            args.append(op(sa, -ea))
      return s_cons(*args)

    # Min/Max op Sym
    if s_cons != None and isinstance(e, Symbol):
      return s_cons(*map(lambda a: op(a, e), s.args))
    # Sym op Min/Max
    if e_cons != None and isinstance(s, Symbol):
      return e_cons(*map(lambda a: op(s, a), e.args))

    # Min/Max op Int
    if s_cons != None and isinstance(e, Integer):
      return s_cons(*map(lambda a: op(a, e), s.args))
    # Int op Min/Max
    if e_cons != None and isinstance(s, Integer):
      return e_cons(*map(lambda a: op(s, a), e.args))

    return op(s, e)

  def with_invert_on_negative():
    # Min/Max op Min/Max
    if s_cons != None and e_cons != None:
      args = []
      # Min op Min or Max op Max
      if s_cons == e_cons:
        for sa in s.args:
          for ea in e.args:
            args.append(op(sa, ea))
        for sa in s.args:
          for ea in e.args:
            args.append(op(-sa, ea))
      # Min op Max
      else:
        for sa in s.args:
          for ea in e.args:
            args.append(op(sa, -ea))
        for sa in s.args:
          for ea in e.args:
            args.append(op(-sa, -ea))
      return s_cons(*args)

    # Min/Max op Sym
    if s_cons != None and isinstance(e, Symbol):
      args = map(lambda a: op(a, e), s.args) \
          + map(lambda a: op(a, -e), s.args)
      return s_cons(*args)
    # Sym op Min/Max
    if e_cons != None and isinstance(s, Symbol):
      args = map(lambda a: op(s, a), e.args) \
          + map(lambda a: op(-s, a), e.args)
      return e_cons(*args)

    # Min/Max op Int
    if s_cons != None and isinstance(e, Integer):
      cons = s_cons_inv if e < 0 else s_cons
      return cons(*map(lambda a: op(a, e), s.args))
    # Int op Min/Max
    if e_cons != None and isinstance(s, Integer):
      cons = e_cons_inv if s < 0 else e_cons
      return cons(*map(lambda a: op(s, a), e.args))

    return op(s, e)

  if invert_on_negative:
    return with_invert_on_negative()
  return without_invert_on_negative()

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
    Expr.is_empty    = lambda s: s.is_eq(Expr.empty)

    Expr.is_inf       = lambda s: s.expr == S.Infinity or s.expr == -S.Infinity
    Expr.is_plus_inf  = lambda s: s.expr == S.Infinity
    Expr.is_minus_inf = lambda s: s.expr == -S.Infinity

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
    Expr.get_numer   = lambda s: s.expr.p
    Expr.get_denom   = lambda s: s.expr.q

    Expr.get_name = lambda s: s.expr.name

    Expr.compare = lambda s, e: s.compare(e)

    # Empty. When min/max is invalid.
    Expr.empty = Expr("EMPTY")

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
           rest, resf, resi = Expr.min_cache[key]
         else:
           rest = CAD.implies(assumptions, args[i] <= args[j])
           resf = CAD.implies(assumptions, args[i] >= args[j])
           resi = CAD.implies(assumptions, args[i] >  args[j])
           Expr.min_cache[key] = (rest, resf, resi)

         if not (CAD.is_unknown(rest) or CAD.is_unknown(resi)) \
             and CAD.is_true(rest) and CAD.is_true(resi):
           del_args[i] = True
           del_args[j] = True
         elif not (CAD.is_unknown(rest) or CAD.is_unknown(resf)) \
             and (CAD.is_true(rest) or CAD.is_false(resf)):
           del_args[j] = True
         elif not (CAD.is_unknown(rest) or CAD.is_unknown(resf)) \
             and (CAD.is_false(rest) or CAD.is_true(resf)):
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
           rest, resf, resi = Expr.max_cache[key]
         else:
           rest = CAD.implies(assumptions, args[i] >= args[j])
           resf = CAD.implies(assumptions, args[i] <= args[j])
           resi = CAD.implies(assumptions, args[i] <  args[j])
           Expr.max_cache[key] = (rest, resf, resi)

         if not (CAD.is_unknown(rest) or CAD.is_unknown(resi)) \
             and CAD.is_true(rest) and CAD.is_true(resi):
           del_args[i] = True
           del_args[j] = True
         elif not (CAD.is_unknown(rest) or CAD.is_unknown(resf)) \
             and (CAD.is_true(rest) or CAD.is_false(resf)):
           del_args[j] = True
         elif not (CAD.is_unknown(rest) or CAD.is_unknown(resf)) \
             and (CAD.is_false(rest) or CAD.is_true(resf)):
           del_args[i] = True

    res_args = [args[i] for i in xrange(len(args)) if not del_args[i]]
    return res_args

  def min_or_max(self, other, op, assumptions):
    try:
      assert op == Min or op == Max
      args = Expr.minmax_args(self.expr, op) + \
             Expr.minmax_args(other.expr, op)

      inv_op = Min if op == Max else Max
      if isinstance(other.expr, inv_op):
        if self.expr in other.expr.args:
          return self
      if isinstance(self.expr, inv_op) and other.expr in self.expr.args:
        return other

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
        return Expr(op(self.expr, other.expr))

      if op == Min:
        res_args = \
          self.reduce_min(args, assumptions.expr if assumptions else False)
      else:
        res_args = \
          self.reduce_max(args, assumptions.expr if assumptions else False)
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
    return len(self.expr.args)

