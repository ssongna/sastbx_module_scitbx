#include <scitbx/array_family/boost_python/flex_fwd.h>

#include <boost/python/class.hpp>
#include <boost/python/args.hpp>
#include <boost/python/def.hpp>

#include <scitbx/math/distributions.h>

#if defined(__GNUC__) && __GNUC__ == 3 && __GNUC_MINOR__ == 2
# define SCITBX_MATH_STUDENTS_T_DISABLED // to avoid compilation errors
#endif

namespace scitbx { namespace math {

/*! Wrappers for boost::math statistical distributions.
    See also:
    http://www.boost.org/libs/math/doc/sf_and_dist/html/math_toolkit/dist.html
 */
namespace {

  template <typename FloatType>
  struct normal_distribution_wrappers
  {
    typedef boost::math::normal_distribution<FloatType> wt;

    static void
    wrap()
    {
      using namespace boost::python;

      class_<wt>("normal_distribution", no_init)
        .def(init<FloatType, FloatType>((arg("mean")=0, arg("sd")=1)))
      ;
    }
  };

#ifndef SCITBX_MATH_STUDENTS_T_DISABLED
  template <typename FloatType>
  struct students_t_distribution_wrappers
  {
    typedef FloatType ft;
    typedef boost::math::students_t_distribution<FloatType> wt;

    // workaround for Intel C++ 12.0.3
    static FloatType
    find_degrees_of_freedom_wrapper(
      ft difference_from_mean, ft alpha, ft beta, ft sd, ft hint)
    {
      return wt::find_degrees_of_freedom(
        difference_from_mean, alpha, beta, sd, hint);
    }

    static void
    wrap()
    {
      using namespace boost::python;

      class_<wt>("students_t_distribution", no_init)
        .def(init<FloatType>(arg("v")))
        .def("degrees_of_freedom", &wt::degrees_of_freedom)
        .def("find_degrees_of_freedom", find_degrees_of_freedom_wrapper, (
          arg("difference_from_mean"),
          arg("alpha"),
          arg("beta"),
          arg("sd"),
          arg("hint")=100))
        .staticmethod("find_degrees_of_freedom")
      ;
    }
  };
#endif

  template <typename FloatType, class Distribution>
  struct non_member_function_wrappers
  {
    typedef Distribution wt;

    #define NEW_MEMBER(name)                \
    static FloatType name(wt const &self) { \
      return boost::math::name(self);                    \
    }

    NEW_MEMBER(mean);
    NEW_MEMBER(median);
    NEW_MEMBER(mode);
    NEW_MEMBER(variance);
    NEW_MEMBER(standard_deviation);
    NEW_MEMBER(skewness);
    NEW_MEMBER(kurtosis);

    #undef NEW_MEMBER

    #define NEW_MEMBER(name)                               \
    static FloatType name(wt const &self, FloatType arg) { \
      return boost::math::name(self, arg);                              \
    }

    NEW_MEMBER(pdf);
    NEW_MEMBER(cdf);
    NEW_MEMBER(quantile);

    #undef NEW_MEMBER

    static scitbx::af::shared<FloatType> quantiles(wt const &self, std::size_t n) {
      return scitbx::math::quantiles<FloatType>(self, n);
    }

    static void
    wrap()
    {
      using namespace boost::python;
      def("mean"              , mean);
      def("median"            , median);
      def("mode"              , mode);
      def("variance"          , variance);
      def("standard_deviation", standard_deviation);
      def("skewness"          , skewness);
      def("kurtosis"          , kurtosis);
      def("pdf"               , pdf);
      def("cdf"               , cdf);
      def("quantile"          , quantile);
      def("quantiles"         , quantiles);
    }
  };

} // namespace <anonymous>

namespace boost_python {

  void wrap_distributions()
  {
    normal_distribution_wrappers<double>::wrap();
    non_member_function_wrappers<
      double, boost::math::normal_distribution<double> >::wrap();
#ifndef SCITBX_MATH_STUDENTS_T_DISABLED
    students_t_distribution_wrappers<double>::wrap();
    non_member_function_wrappers<
      double, boost::math::students_t_distribution<double> >::wrap();
#endif
  }

}}} // namespace scitbx::math::boost_python
