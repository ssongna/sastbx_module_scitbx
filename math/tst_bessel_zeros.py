from __future__ import division
from scitbx import math

def tst_sph_zeroes(l,n):
  z = math.sph_bessel_j_zeroes(l,n)
  fz = math.spherical_bessel_array(l,z)
  for ff in fz:
    assert abs(ff) < 1e-8

def tst_bessel_zeroes(l,n):
  z = math.bessel_J_zeroes(l,n)
  fz = math.bessel_J_array(l,z)
  for ff in fz:
    assert abs(ff) < 1e-8


def tst_all():
  for ii in range(30):
     tst_sph_zeroes(ii,100)
     tst_bessel_zeroes(ii,100)
  print "OK"

if __name__ == "__main__":
  tst_all()
