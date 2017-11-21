from __future__ import division
import sys
from libtbx import easy_run

ret = easy_run.call("scitbx.unicode_examples")
ret += easy_run.call("scitbx.show_sizes")
ret += easy_run.call("scitbx.show_exp_times 100")
ret += easy_run.call("echo OK")
sys.exit(ret)
