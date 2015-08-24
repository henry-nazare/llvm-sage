from expr import Expr
from sage.all import oo

import operator

class Range:
  def __init__(self, lower=None, upper=None):
    self.lower = lower
    self.upper = upper

  def __add__(self, other):
    return Range(self.lower + other.lower, self.upper + other.upper)

  def __sub__(self, other):
    lower = Expr(-oo)
    if not self.lower.is_minus_inf() and not other.lower.is_minus_inf():
      lower = self.lower - other.lower

    upper = Expr(oo)
    if not self.upper.is_plus_inf() and not other.upper.is_plus_inf():
      upper = self.upper - other.upper

    return Range(lower, upper)

  def __mul__(self, other):
    lower = Expr(-oo)
    if not self.lower.is_minus_inf() and not other.lower.is_minus_inf():
      lower = self.lower * other.lower

    upper = Expr(oo)
    if not self.upper.is_plus_inf() and not other.upper.is_plus_inf():
      upper = self.upper * other.upper

    return Range(lower, upper)

  def __div__(self, other):
    lower = Expr(-oo)
    if not self.lower.is_minus_inf() and not other.lower.is_minus_inf():
      lower = self.lower/other.lower

    upper = Expr(oo)
    if not self.upper.is_plus_inf() and not other.upper.is_plus_inf():
      upper = self.upper/other.upper

    return Range(lower, upper)

  def __eq__(self, other):
    return self.lower == other.lower and self.upper == other.upper

  def __repr__(self):
    return self.__str__()

  def __str__(self):
    return "[" + str(self.lower) + ", " + str(self.upper) + "]"

