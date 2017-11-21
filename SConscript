import libtbx.load_env
import os
op = os.path
Import("env_etc")

env_etc.scitbx_dist = libtbx.env.dist_path("scitbx")
env_etc.scitbx_include = os.path.dirname(env_etc.scitbx_dist)
env_etc.scitbx_common_includes = [
  env_etc.libtbx_include,
  env_etc.scitbx_include,
  env_etc.omptbx_include,
  env_etc.boost_adaptbx_include,
  env_etc.boost_include,
  op.dirname(libtbx.env.find_in_repositories(
    relative_path="tbxx", optional=False))
]

SConscript("error/SConscript")
SConscript("slatec/SConscript")
SConscript("array_family/SConscript")
SConscript("serialization/SConscript")
SConscript("fftpack/timing/SConscript")
SConscript("lbfgsb/dev/SConscript")
SConscript("matrix/tests/SConscript")
SConscript("math/tests/SConscript")
SConscript("sparse/tests/SConscript")

if (not env_etc.no_boost_python):
  Import("env_no_includes_boost_python_ext")
  env_scitbx_boost_python_ext = env_no_includes_boost_python_ext.Clone()
  env_scitbx_boost_python_ext.Prepend(LIBS=["scitbx_boost_python"])
  env_etc.include_registry.append(
    env=env_scitbx_boost_python_ext,
    paths=env_etc.scitbx_common_includes + [env_etc.python_include])
  Export("env_scitbx_boost_python_ext")

  SConscript("boost_python/SConscript")
  SConscript("stl/SConscript")
  SConscript("array_family/boost_python/SConscript")
  SConscript("random/boost_python/SConscript")
  SConscript("math/boost_python/SConscript")
  SConscript("linalg/boost_python/SConscript")
  SConscript("fftpack/boost_python/SConscript")
  SConscript("lbfgsb/boost_python/SConscript")
  SConscript("sparse/boost_python/SConscript")
  SConscript("iso_surface/SConscript")
  SConscript("rigid_body/SConscript")
  SConscript("wigner/SConscript")
  SConscript("graphics_utils/SConscript")
  SConscript("examples/bevington/SConscript")

  env = env_scitbx_boost_python_ext.Clone()
  env_etc.enable_more_warnings(env=env)
  env.SharedLibrary(
    target="#lib/scitbx_r3_utils_ext",
    source=["r3_utils_ext.cpp"])
  env.SharedLibrary(
    target="#lib/scitbx_cubicle_neighbors_ext",
    source=["cubicle_neighbors_ext.cpp"])

SConscript("lbfgs/SConscript")
SConscript("minpack/SConscript")
SConscript("lstbx/SConscript")
SConscript("glmtbx/SConscript")
SConscript("suffixtree/SConscript")
