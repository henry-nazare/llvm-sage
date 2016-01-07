from sage.all import qepcad, qepcad_formula
from operator import lt, le, gt, ge, eq, ne
from sympy import And, Or, Not, StrictLessThan, LessThan, \
                  StrictGreaterThan, GreaterThan, Unequality, Equality, Min, Max
from pexpect import ExceptionPexpect

def run_qepcad(formula):
  try:
    try:
      # Time out QEPCAD after 2 seconds.
      return qepcad(formula, timeout=2)
    except TypeError:
      return qepcad(formula)
  except ExceptionPexpect:
    return 'UNKNOWN'

class CAD:
  qf = qepcad_formula

  methods = {
    StrictLessThan:    lt,
    LessThan:          le,
    StrictGreaterThan: gt,
    GreaterThan:       ge,
    Equality:          eq,
    Unequality:        ne
  }

  complete_methods = {
    # a < b => a <= b + 1
    StrictLessThan:    lambda l, r: LessThan(l + 1, r),
    # a <= b => a < b + 1
    LessThan:          lambda l, r: StrictLessThan(l - 1, r),
    # a > b => a >= b + 1
    StrictGreaterThan: lambda l, r: GreaterThan(l + 1, r),
    # a >= b => a > b + 1
    GreaterThan:       lambda l, r: StrictGreaterThan(l + 1, r)
  }

  @staticmethod
  def complete_assumptions(formula):
    if isinstance(formula, And):
      return And(*map(CAD.complete_assumptions, formula.args))
    elif isinstance(formula, Or):
      return Or(*map(CAD.complete_assumptions, formula.args))
    elif isinstance(formula, Not):
      return Not(*map(CAD.complete_assumptions, formula.args))
    elif type(formula) in CAD.complete_methods:
      method = CAD.complete_methods.get(type(formula))
      return And(formula, method(*formula.args))
    return formula

  @staticmethod
  def to_sage(formula):
    if type(formula) in CAD.methods:
      args = map(lambda e: e._sage_(), formula.args)
      method = CAD.methods.get(type(formula))
      return method(*args)
    return formula._sage_()

  @staticmethod
  def to_qepcad(formula):
    if isinstance(formula, And):
      return CAD.qf.and_(*map(CAD.to_qepcad, formula.args))
    elif isinstance(formula, Or):
      return CAD.qf.or_(*map(CAD.to_qepcad, formula.args))
    elif isinstance(formula, Not):
      return CAD.qf.not_(CAD.to_qepcad(*formula.args))
    return CAD.qf.formula(CAD.to_sage(formula))

  @staticmethod
  def always(formula):
    # TODO: remove Min/Max from expressions.
    if formula.has(Min) or formula.has(Max):
      return 'UNKNOWN'

    f = CAD.qf.formula(CAD.to_sage(formula))
    return run_qepcad(f)

  @staticmethod
  def implies(assumptions, formula):
    if formula == True:
      return 'TRUE'
    elif formula == False:
      return 'FALSE'
    if assumptions == False:
      return CAD.always(formula)

    # TODO: remove Min/Max from expressions.
    if formula.has(Min) or formula.has(Max):
      return 'UNKNOWN'

    formula = CAD.to_sage(formula)
    assumptions = CAD.complete_assumptions(assumptions)
    assumptions_str = CAD.to_qepcad(assumptions)
    implication = CAD.qf.implies(assumptions_str, formula)
    return run_qepcad(implication)

  @staticmethod
  def is_true(result):
    return result == 'TRUE'

  @staticmethod
  def is_false(result):
    return result == 'FALSE'

  @staticmethod
  def is_unknown(result):
    return result == 'UNKNOWN'

