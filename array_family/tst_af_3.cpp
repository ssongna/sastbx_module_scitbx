#include <scitbx/array_family/simple_io.h>
#include <scitbx/array_family/misc_functions.h>
#include <scitbx/array_family/tiny.h>
#include <scitbx/array_family/tiny_reductions.h>
#include <scitbx/array_family/small.h>
#include <scitbx/array_family/small_reductions.h>
#include <scitbx/array_family/shared.h>
#include <scitbx/array_family/shared_reductions.h>
#include <scitbx/array_family/versa.h>
#include <scitbx/array_family/versa_reductions.h>

using namespace scitbx;

namespace {

# include "tst_af_helpers.cpp"

  template <typename ArrayType1,
            typename ArrayType2,
            typename ArrayType3,
            typename ArrayType4>
  void
  exercise_reductions(
    ArrayType1 const& a1,
    ArrayType2 const& a2,
    ArrayType3 const& a3,
    ArrayType4 const& a4)
  {
#if !(defined(BOOST_MSVC) && BOOST_MSVC <= 1200) // VC++ 6.0
    check_true(__LINE__, a1.all_eq(a1));
    check_false(__LINE__, a1.all_eq(a2));
    check_false(__LINE__, a1.all_eq(0));
    check_true(__LINE__, a1.all_ne(a2));
    check_false(__LINE__, a1.all_ne(a1));
    check_false(__LINE__, a1.all_ne(0));
    check_true(__LINE__, a1.all_ne(3));
    check_true(__LINE__, a1.all_lt(a2));
    check_false(__LINE__, a1.all_lt(a1));
    check_true(__LINE__, a1.all_lt(5));
    check_false(__LINE__, a1.all_lt(2));
    check_true(__LINE__, a2.all_gt(a1));
    check_false(__LINE__, a1.all_gt(a1));
    check_true(__LINE__, a1.all_gt(-1));
    check_false(__LINE__, a1.all_gt(0));
    check_true(__LINE__, a1.all_le(a1));
    check_true(__LINE__, a1.all_le(a2));
    check_true(__LINE__, a1.all_le(2));
    check_false(__LINE__, a1.all_le(0));
    check_true(__LINE__, a1.all_ge(a1));
    check_false(__LINE__, a1.all_ge(a2));
    check_true(__LINE__, a1.all_ge(0));
    check_false(__LINE__, a1.all_ge(2));
    check_true(__LINE__, a1.all_approx_equal(a1, 0));
    check_false(__LINE__, a1.all_approx_equal(a2, 0));
    check_false(__LINE__, a1.all_approx_equal(0, 0));
    check_true(__LINE__, af::order(a1, a2) == -1);
    check_true(__LINE__, af::order(a1, a1) == 0);
    check_true(__LINE__, af::order(a2, a2) == 0);
    check_true(__LINE__, af::order(a2, a1) == 1);
    check_true(__LINE__, af::max_index(a1) == 2);
    check_true(__LINE__, af::min_index(a1) == 0);
    check_true(__LINE__, af::max(a1) == a1[2]);
    check_true(__LINE__, af::min(a1) == a1[0]);
    check_true(__LINE__, af::max_absolute(a1) == a1[2]);
    check_true(__LINE__, af::sum(a1) == a1[0] + a1[1] + a1[2]);
    check_true(__LINE__, af::product(a1) == a1[0] * a1[1] * a1[2]);
    check_true(__LINE__, fn::absolute(
      af::mean(a3)
      - (a3[0] + a3[1] + a3[2]) / 3) < 1.e-6);
    check_true(__LINE__, fn::absolute(
      af::mean_sq(a3)
      - (a3[0]*a3[0] + a3[1]*a3[1] + a3[2]*a3[2]) / 3) < 1.e-6);
    check_true(__LINE__, fn::absolute(
      af::mean_weighted(a3, a4)
      - ((a3[0]*a4[0] + a3[1]*a4[1] + a3[2]*a4[2]) / af::sum(a4))) < 1.e-6);
    check_true(__LINE__, fn::absolute(
      af::mean_sq_weighted(a3, a4)
      - ((  a3[0]*a3[0]*a4[0]
          + a3[1]*a3[1]*a4[1]
          + a3[2]*a3[2]*a4[2]) / af::sum(a4))) < 1.e-6);
#endif // !(defined(BOOST_MSVC) && BOOST_MSVC <= 1200)
  }

  template <typename IntType, typename FloatType>
  struct exercise_main
  {
    static void run()
    {
      af::tiny<IntType, 3> t1(0,1,2);
      af::tiny<IntType, 3> t2(3,4,5);
      af::tiny<FloatType, 3> t3(3,4,5);
      af::tiny<FloatType, 3> t4(4,5,6);
      {
        if (verbose) std::cout << __LINE__ << std::endl;
        exercise_reductions(t1, t2, t3, t4);
      }
      {
        if (verbose) std::cout << __LINE__ << std::endl;
        af::small<IntType, 3> a1(af::adapt(t1));
        af::small<IntType, 3> a2(af::adapt(t2));
        af::small<FloatType, 3> a3(af::adapt(t3));
        af::small<FloatType, 3> a4(af::adapt(t4));
        exercise_reductions(a1, a2, a3, a4);
      }
      {
        if (verbose) std::cout << __LINE__ << std::endl;
        af::shared<IntType> a1(af::adapt(t1));
        af::shared<IntType> a2(af::adapt(t2));
        af::shared<FloatType> a3(af::adapt(t3));
        af::shared<FloatType> a4(af::adapt(t4));
        exercise_reductions(a1, a2, a3, a4);
      }
      {
        if (verbose) std::cout << __LINE__ << std::endl;
        af::versa<IntType> a1(af::adapt(t1));
        af::versa<IntType> a2(af::adapt(t2));
        af::versa<FloatType> a3(af::adapt(t3));
        af::versa<FloatType> a4(af::adapt(t4));
        exercise_reductions(a1, a2, a3, a4);
      }
    }
  };

} // namespace <anonymous>

int main(int argc, char* /*argv*/[])
{
  for(;;) {
    exercise_main<int, double>::run();
    if (argc == 1) break;
  }
  std::cout << "Total OK: " << ok_counter << std::endl;
  if (error_counter || verbose) {
    std::cout << "Total Errors: " << error_counter << std::endl;
  }
  return 0;
}
