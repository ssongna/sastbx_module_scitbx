from __future__ import division
from scitbx.math import dmatrix
from stdlib import math as smath

def tst_dmatrix():
  # expected values are the d_jmn at beta=1.0 and j=2, m=-2, -2<=n<=2
  expect = [0.593133, 0.64806, 0.433605, 0.193411, 0.0528305]
  eps = 1e-4
  d2 = dmatrix( 2, 1.0)  # dmatrix( max_L, beta )
  for n in range(-2, 3):
    assert abs(expect[n+2] - d2.djmn(2, -2, n) ) < eps

  d4 = dmatrix( 4, 1.0)
  for n in range(-2, 3):
    assert abs(expect[n+2] - d4.djmn(2, -2, n) ) < eps

  d7 = dmatrix( 2, 1.0)
  for n in range(-2, 3):
    assert abs(expect[n+2] - d7.djmn(2, -2, n) ) < eps

  # check agains d(2,2,1) = -(1+cos(beta))*sin(beta)/2.0
  for ii in range(10):
    expt = -(1.0+smath.cos(ii))*smath.sin(ii)/2.0
    assert abs( dmatrix(2,ii).djmn(2,2,1) - expt ) < eps
  # check beta= multiple of pi/2

  assert abs( dmatrix( 20, smath.pi ).djmn(2,2,0) )< eps
  assert abs( dmatrix( 2, smath.pi*2 ).djmn(2,2,0) )< eps
  assert abs( dmatrix( 20, smath.pi*0.5 ).djmn(2,2,0)-smath.sqrt(6.0)/4.0 )< eps


if __name__ == "__main__":
  tst_dmatrix()
  print "OK"
