from __future__ import division
import scitbx.stl.set # import dependency

import boost.python
ext = boost.python.import_ext("scitbx_stl_vector_ext")
from scitbx_stl_vector_ext import *
