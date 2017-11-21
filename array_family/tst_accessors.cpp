#include <scitbx/array_family/flex_types.h>
#include <scitbx/array_family/accessors/c_grid.h>
#include <scitbx/array_family/accessors/c_grid_padded.h>
#include <scitbx/array_family/accessors/c_grid_periodic.h>
#include <scitbx/array_family/accessors/c_grid_padded_periodic.h>
#include <scitbx/array_family/accessors/c_interval_grid.h>
#include <scitbx/array_family/tiny_types.h>
#include <scitbx/math/utils.h> // exercise unsigned_product_leads_to_overflow
#include <iostream>

using namespace scitbx;

namespace {

# include "tst_af_helpers.cpp"

}

int main(int /*argc*/, char* /*argv*/[])
{
  {
    // many more tests via Python
    af::flex_int a;
    check_true(__LINE__, a.size() == 0);
    a = af::flex_int(af::flex_grid<>(1));
    check_true(__LINE__, a.size() == 1);
    check_true(__LINE__, a.accessor().nd() == 1);
    check_true(__LINE__, a.accessor().is_trivial_1d());
    a = af::flex_int(af::flex_grid<>(1, 2));
    check_true(__LINE__, a.size() == 2);
    check_true(__LINE__, a.accessor().nd() == 2);
    check_false(__LINE__, a.accessor().is_square_matrix());
    a = af::flex_int(af::flex_grid<>(3, 3));
    check_true(__LINE__, a.accessor().nd() == 2);
    check_true(__LINE__, a.accessor().is_0_based());
    check_false(__LINE__, a.accessor().is_padded());
    check_true(__LINE__, a.accessor().focus()[0] == 3);
    check_true(__LINE__, a.accessor().focus()[1] == 3);
    check_true(__LINE__, a.accessor().is_square_matrix());
    a = af::flex_int(af::flex_grid<>(1, 2, 3));
    check_true(__LINE__, a.size() == 6);
    check_true(__LINE__, a.accessor().nd() == 3);
    a = af::flex_int(af::flex_grid<>(1, 2, 3, 4));
    check_true(__LINE__, a.size() == 24);
    check_true(__LINE__, a.accessor().nd() == 4);
    a = af::flex_int(af::flex_grid<>(1, 2, 3, 4, 5));
    check_true(__LINE__, a.size() == 120);
    check_true(__LINE__, a.accessor().nd() == 5);
    a = af::flex_int(af::flex_grid<>(1, 2, 3, 4, 5, 6));
    check_true(__LINE__, a.size() == 720);
    check_true(__LINE__, a.accessor().nd() == 6);
    af::flex_int_const_ref cr = a.const_ref();
    check_true(__LINE__, cr.size() == 720);
    check_true(__LINE__, cr.accessor().nd() == 6);
    af::flex_int_ref r = a.ref();
    check_true(__LINE__, r.size() == 720);
    check_true(__LINE__, r.accessor().nd() == 6);
  }
  {
    af::flex_grid<> g(3, 4);
    std::size_t i = 0;
    af::flex_grid<>::index_type j(2, 0);
    for(j[0]=0;j[0]<3;j[0]++)
    for(j[1]=0;j[1]<4;j[1]++, i++) {
      check_true(__LINE__, g.is_valid_index(j));
      check_true(__LINE__, g(j) == i);
      check_true(__LINE__, g(j[0],j[1]) == i);
    }
  }
  {
    af::flex_grid<>::index_type origin(af::adapt(af::tiny<int, 2>(-2,-13)));
    af::flex_grid<>::index_type last(af::adapt(af::tiny<int, 2>(1,-8)));
    af::flex_grid<> g(origin, last, false);
    std::size_t i = 0;
    af::flex_grid<>::index_type j(2, 0);
    for(j[0]=-2;j[0]<=1;j[0]++)
    for(j[1]=-13;j[1]<=-8;j[1]++, i++) {
      check_true(__LINE__, g.is_valid_index(j));
      check_true(__LINE__, g(j) == i);
      check_true(__LINE__, g(j[0],j[1]) == i);
    }
  }
  {
    af::flex_grid<> g(3, 4, 5);
    std::size_t i = 0;
    af::flex_grid<>::index_type j(3, 0);
    for(j[0]=0;j[0]<3;j[0]++)
    for(j[1]=0;j[1]<4;j[1]++)
    for(j[2]=0;j[2]<5;j[2]++, i++) {
      check_true(__LINE__, g.is_valid_index(j));
      check_true(__LINE__, g(j) == i);
      check_true(__LINE__, g(j[0],j[1],j[2]) == i);
    }
  }
  {
    af::flex_grid<>::index_type origin(af::adapt(af::tiny<int, 3>(-2,3,-10)));
    af::flex_grid<>::index_type last(af::adapt(af::tiny<int, 3>(1,7,-6)));
    af::flex_grid<> g(origin, last, false);
    std::size_t i = 0;
    af::flex_grid<>::index_type j(3, 0);
    for(j[0]=-2;j[0]<=1;j[0]++)
    for(j[1]=3;j[1]<=7;j[1]++)
    for(j[2]=-10;j[2]<=-6;j[2]++, i++) {
      check_true(__LINE__, g.is_valid_index(j));
      check_true(__LINE__, g(j) == i);
      check_true(__LINE__, g(j[0],j[1],j[2]) == i);
    }
  }
  {
    af::c_grid<1> a;
    check_true(__LINE__, a.size() == 1);
    check_true(__LINE__, a.size_1d() == 0);
    a = af::c_grid<1>(af::tiny<std::size_t, 1>(3));
    check_true(__LINE__, a.size() == 1);
    check_true(__LINE__, a.size_1d() == 3);
    a = af::c_grid<1>(af::flex_grid<>(3));
    check_true(__LINE__, a.size() == 1);
    check_true(__LINE__, a.size_1d() == 3);
    try {
      a = af::c_grid<1>(af::flex_grid<>(3, 4));
      check_true(__LINE__, false);
    }
    catch (...) {
      check_true(__LINE__, true);
    }
    a = af::c_grid<1>(af::adapt(af::tiny<std::size_t, 1>(5)));
    verify(__LINE__, a.as_flex_grid().all(), a);
    check_true(__LINE__, a.size() == 1);
    check_true(__LINE__, a.size_1d() == 5);
    verify(__LINE__, a.index_nd(3), af::tiny<std::size_t, 1>(3));
    check_true(__LINE__, a.is_valid_index(af::tiny<std::size_t, 1>(4)));
    check_false(__LINE__, a.is_valid_index(af::tiny<std::size_t, 1>(5)));
    for(std::size_t i=0;i<5;i++) {
      check_true(__LINE__, a(af::tiny<std::size_t, 1>(i)) == i);
    }
  }
  {
    af::c_grid<4> a;
    check_true(__LINE__, a.size() == 4);
    check_true(__LINE__, a.size_1d() == 0);
    a = af::c_grid<4>(af::adapt(af::tiny<std::size_t, 4>(3,2,5,4)));
    check_true(__LINE__, a.size() == 4);
    check_true(__LINE__, a.size_1d() == 120);
    a = af::c_grid<4>(af::flex_grid<>(3,2,5,4));
    verify(__LINE__, a.as_flex_grid().all(), a);
    check_true(__LINE__, a.size() == 4);
    check_true(__LINE__, a.size_1d() == 120);
    check_false(__LINE__, a.is_valid_index(af::tiny<std::size_t, 4>(3,1,4,3)));
    check_false(__LINE__, a.is_valid_index(af::tiny<std::size_t, 4>(2,1,4,4)));
    std::size_t i = 0;
    af::tiny<std::size_t, 4> j;
    for(j[0]=0;j[0]<3;j[0]++)
    for(j[1]=0;j[1]<2;j[1]++)
    for(j[2]=0;j[2]<5;j[2]++)
    for(j[3]=0;j[3]<4;j[3]++, i++) {
      verify(__LINE__, a.index_nd(i), j);
      check_true(__LINE__, a.is_valid_index(j));
      check_true(__LINE__, a(j) == i);
    }
  }
  {
    af::c_grid<2> a;
    check_true(__LINE__, a.size() == 2);
    check_true(__LINE__, a.size_1d() == 0);
    a = af::c_grid<2>(3,2);
    check_true(__LINE__, a.size() == 2);
    check_true(__LINE__, a.size_1d() == 6);
    a = af::c_grid<2>(af::adapt(af::tiny<std::size_t, 2>(3,2)));
    check_true(__LINE__, a.size() == 2);
    check_true(__LINE__, a.size_1d() == 6);
    a = af::c_grid<2>(af::flex_grid<>(3,2));
    verify(__LINE__, a.as_flex_grid().all(), a);
    check_true(__LINE__, a.size() == 2);
    check_true(__LINE__, a.size_1d() == 6);
    check_true(__LINE__, af::c_grid<2>(2,2).is_square());
    check_false(__LINE__, af::c_grid<2>(2,3).is_square());
    check_false(__LINE__, a.is_valid_index(af::tiny<std::size_t, 2>(3,1)));
    check_false(__LINE__, a.is_valid_index(af::tiny<std::size_t, 2>(2,2)));
    check_true(__LINE__, a.is_valid_index(2,1));
    check_false(__LINE__, a.is_valid_index(2,2));
    std::size_t i = 0;
    af::tiny<std::size_t, 2> j;
    for(j[0]=0;j[0]<3;j[0]++)
    for(j[1]=0;j[1]<2;j[1]++, i++) {
      verify(__LINE__, a.index_nd(i), j);
      check_true(__LINE__, a.is_valid_index(j));
      check_true(__LINE__, a(j) == i);
      check_true(__LINE__, a(j[0],j[1]) == i);
    }
  }
  {
    af::c_grid<3> a;
    check_true(__LINE__, a.size() == 3);
    check_true(__LINE__, a.size_1d() == 0);
    a = af::c_grid<3>(3,2,5);
    check_true(__LINE__, a.size() == 3);
    check_true(__LINE__, a.size_1d() == 30);
    a = af::c_grid<3>(af::adapt(af::tiny<std::size_t, 3>(3,2,5)));
    check_true(__LINE__, a.size() == 3);
    check_true(__LINE__, a.size_1d() == 30);
    a = af::c_grid<3>(af::flex_grid<>(3,2,5));
    verify(__LINE__, a.as_flex_grid().all(), a);
    check_true(__LINE__, a.as_flex_grid().size_1d() == 30);
    check_true(__LINE__, a.size() == 3);
    check_true(__LINE__, a.size_1d() == 30);
    check_false(__LINE__, a.is_valid_index(af::tiny<std::size_t, 3>(3,1,4)));
    check_false(__LINE__, a.is_valid_index(af::tiny<std::size_t, 3>(2,2,4)));
    check_false(__LINE__, a.is_valid_index(af::tiny<std::size_t, 3>(2,1,5)));
    check_true(__LINE__, a.is_valid_index(2,1,4));
    check_false(__LINE__, a.is_valid_index(2,1,5));
    std::size_t i = 0;
    af::tiny<std::size_t, 3> j;
    for(j[0]=0;j[0]<3;j[0]++)
    for(j[1]=0;j[1]<2;j[1]++)
    for(j[2]=0;j[2]<5;j[2]++, i++) {
      verify(__LINE__, a.index_nd(i), j);
      check_true(__LINE__, a.is_valid_index(j));
      check_true(__LINE__, a(j) == i);
      check_true(__LINE__, a(j[0],j[1],j[2]) == i);
    }
  }
  {
    af::c_grid_padded<1> a;
    check_true(__LINE__, a.all().size() == 1);
    check_true(__LINE__, a.size_1d() == 0);
    check_true(__LINE__, a.focus_size_1d() == 0);
    check_false(__LINE__, a.is_padded());
    a = af::c_grid_padded<1>(af::tiny<std::size_t, 1>(3));
    check_true(__LINE__, a.size_1d() == 3);
    check_true(__LINE__, a.focus_size_1d() == 3);
    check_false(__LINE__, a.is_padded());
    verify(__LINE__, a.all(), af::tiny<std::size_t, 1>(3));
    verify(__LINE__, a.focus(), af::tiny<std::size_t, 1>(3));
    a = af::c_grid_padded<1>(af::tiny<std::size_t, 1>(4),
                             af::tiny<std::size_t, 1>(3));
    check_true(__LINE__, a.size_1d() == 4);
    check_true(__LINE__, a.focus_size_1d() == 3);
    check_true(__LINE__, a.is_padded());
    verify(__LINE__, a.all(), af::tiny<std::size_t, 1>(4));
    verify(__LINE__, a.focus(), af::tiny<std::size_t, 1>(3));
    a = af::c_grid_padded<1>(af::flex_grid<>(3));
    check_true(__LINE__, a.size_1d() == 3);
    check_false(__LINE__, a.is_padded());
    a = af::c_grid_padded<1>(af::flex_grid<>(4).set_focus(3));
    check_true(__LINE__, a.size_1d() == 4);
    check_true(__LINE__, a.is_padded());
    a = af::c_grid_padded<1>(af::adapt(af::tiny<std::size_t, 1>(5)));
    verify(__LINE__, a.as_flex_grid().all(), a.all());
    check_true(__LINE__, a.size_1d() == 5);
    for(std::size_t i=0;i<5;i++) {
      check_true(__LINE__, a(af::tiny<std::size_t, 1>(i)) == i);
    }
  }
  {
    af::c_grid_padded<4> a;
    check_true(__LINE__, a.all().size() == 4);
    check_true(__LINE__, a.size_1d() == 0);
    check_true(__LINE__, a.focus_size_1d() == 0);
    check_false(__LINE__, a.is_padded());
    a = af::c_grid_padded<4>(af::tiny<std::size_t, 4>(3,2,7,5));
    check_true(__LINE__, a.size_1d() == 3*2*7*5);
    check_true(__LINE__, a.focus_size_1d() == 3*2*7*5);
    check_false(__LINE__, a.is_padded());
    verify(__LINE__, a.all(), af::tiny<std::size_t, 4>(3,2,7,5));
    verify(__LINE__, a.focus(), af::tiny<std::size_t, 4>(3,2,7,5));
    a = af::c_grid_padded<4>(af::tiny<std::size_t, 4>(3,2,7,5),
                             af::tiny<std::size_t, 4>(3,2,6,5));
    check_true(__LINE__, a.size_1d() == 3*2*7*5);
    check_true(__LINE__, a.is_padded());
    verify(__LINE__, a.all(), af::tiny<std::size_t, 4>(3,2,7,5));
    verify(__LINE__, a.focus(), af::tiny<std::size_t, 4>(3,2,6,5));
    a = af::c_grid_padded<4>(af::flex_grid<>(3,2,7,5));
    check_true(__LINE__, a.size_1d() == 3*2*7*5);
    check_false(__LINE__, a.is_padded());
    a = af::c_grid_padded<4>(af::flex_grid<>(3,2,7,5).set_focus(3,2,6,5));
    check_true(__LINE__, a.size_1d() == 3*2*7*5);
    check_true(__LINE__, a.focus_size_1d() == 3*2*6*5);
    check_true(__LINE__, a.is_padded());
    a = af::c_grid_padded<4>(af::adapt(af::tiny<std::size_t, 4>(3,2,7,5)));
    verify(__LINE__, a.as_flex_grid().all(), a.all());
    check_true(__LINE__, a.size_1d() == 3*2*7*5);
    std::size_t i = 0;
    af::tiny<std::size_t, 4> j;
    for(j[0]=0;j[0]<3;j[0]++)
    for(j[1]=0;j[1]<2;j[1]++)
    for(j[2]=0;j[2]<7;j[2]++)
    for(j[3]=0;j[3]<5;j[3]++, i++) {
      check_true(__LINE__, a(j) == i);
    }
  }
  {
    af::c_grid_padded<2> a;
    check_true(__LINE__, a.all().size() == 2);
    check_true(__LINE__, a.size_1d() == 0);
    check_true(__LINE__, a.focus_size_1d() == 0);
    check_false(__LINE__, a.is_padded());
    a = af::c_grid_padded<2>(af::tiny<std::size_t, 2>(3,5));
    check_true(__LINE__, a.size_1d() == 3*5);
    check_true(__LINE__, a.focus_size_1d() == 3*5);
    check_false(__LINE__, a.is_padded());
    a = af::c_grid_padded<2>(3,5);
    check_true(__LINE__, a.size_1d() == 3*5);
    check_false(__LINE__, a.is_padded());
    verify(__LINE__, a.all(), af::tiny<std::size_t, 2>(3,5));
    verify(__LINE__, a.focus(), af::tiny<std::size_t, 2>(3,5));
    a = af::c_grid_padded<2>(af::tiny<std::size_t, 2>(3,5),
                             af::tiny<std::size_t, 2>(3,4));
    check_true(__LINE__, a.size_1d() == 3*5);
    check_true(__LINE__, a.focus_size_1d() == 3*4);
    check_true(__LINE__, a.is_padded());
    verify(__LINE__, a.all(), af::tiny<std::size_t, 2>(3,5));
    verify(__LINE__, a.focus(), af::tiny<std::size_t, 2>(3,4));
    a = af::c_grid_padded<2>(3,5,
                             3,4);
    check_true(__LINE__, a.size_1d() == 3*5);
    check_true(__LINE__, a.is_padded());
    verify(__LINE__, a.all(), af::tiny<std::size_t, 2>(3,5));
    verify(__LINE__, a.focus(), af::tiny<std::size_t, 2>(3,4));
    a = af::c_grid_padded<2>(af::flex_grid<>(3,5));
    check_true(__LINE__, a.size_1d() == 3*5);
    check_false(__LINE__, a.is_padded());
    a = af::c_grid_padded<2>(af::flex_grid<>(3,5).set_focus(3,4));
    check_true(__LINE__, a.size_1d() == 3*5);
    check_true(__LINE__, a.is_padded());
    a = af::c_grid_padded<2>(af::adapt(af::tiny<std::size_t, 2>(3,5)));
    verify(__LINE__, a.as_flex_grid().all(), a.all());
    check_true(__LINE__, a.size_1d() == 3*5);
    std::size_t i = 0;
    af::tiny<std::size_t, 2> j;
    for(j[0]=0;j[0]<3;j[0]++)
    for(j[1]=0;j[1]<5;j[1]++, i++) {
      check_true(__LINE__, a(j) == i);
      check_true(__LINE__, a(j[0],j[1]) == i);
    }
  }
  {
    af::c_grid_padded<3> a;
    check_true(__LINE__, a.all().size() == 3);
    check_true(__LINE__, a.size_1d() == 0);
    check_true(__LINE__, a.focus_size_1d() == 0);
    check_false(__LINE__, a.is_padded());
    a = af::c_grid_padded<3>(af::tiny<std::size_t, 3>(3,7,5));
    check_true(__LINE__, a.size_1d() == 3*7*5);
    check_true(__LINE__, a.focus_size_1d() == 3*7*5);
    check_false(__LINE__, a.is_padded());
    a = af::c_grid_padded<3>(3,7,5);
    check_true(__LINE__, a.size_1d() == 3*7*5);
    check_false(__LINE__, a.is_padded());
    verify(__LINE__, a.all(), af::tiny<std::size_t, 3>(3,7,5));
    verify(__LINE__, a.focus(), af::tiny<std::size_t, 3>(3,7,5));
    a = af::c_grid_padded<3>(af::tiny<std::size_t, 3>(3,7,5),
                             af::tiny<std::size_t, 3>(3,4,5));
    check_true(__LINE__, a.size_1d() == 3*7*5);
    check_true(__LINE__, a.focus_size_1d() == 3*4*5);
    check_true(__LINE__, a.is_padded());
    verify(__LINE__, a.all(), af::tiny<std::size_t, 3>(3,7,5));
    verify(__LINE__, a.focus(), af::tiny<std::size_t, 3>(3,4,5));
    a = af::c_grid_padded<3>(3,7,5,
                             3,4,5);
    check_true(__LINE__, a.size_1d() == 3*7*5);
    check_true(__LINE__, a.is_padded());
    verify(__LINE__, a.all(), af::tiny<std::size_t, 3>(3,7,5));
    verify(__LINE__, a.focus(), af::tiny<std::size_t, 3>(3,4,5));
    a = af::c_grid_padded<3>(af::flex_grid<>(3,7,5));
    check_true(__LINE__, a.size_1d() == 3*7*5);
    check_false(__LINE__, a.is_padded());
    a = af::c_grid_padded<3>(af::flex_grid<>(3,7,5).set_focus(3,4,5));
    check_true(__LINE__, a.size_1d() == 3*7*5);
    check_true(__LINE__, a.is_padded());
    a = af::c_grid_padded<3>(af::adapt(af::tiny<std::size_t, 3>(3,7,5)));
    verify(__LINE__, a.as_flex_grid().all(), a.all());
    check_true(__LINE__, a.size_1d() == 3*7*5);
    std::size_t i = 0;
    af::tiny<std::size_t, 3> j;
    for(j[0]=0;j[0]<3;j[0]++)
    for(j[1]=0;j[1]<7;j[1]++)
    for(j[2]=0;j[2]<5;j[2]++, i++) {
      check_true(__LINE__, a(j) == i);
      check_true(__LINE__, a(j[0],j[1],j[2]) == i);
    }
  }
  {
    af::c_grid<3, unsigned> u(1, 2, 3);
    af::c_grid<3, std::size_t> s(1, 2, 3);
    check_false(__LINE__,
      math::unsigned_product_leads_to_overflow(u.begin(), 3));
    check_false(__LINE__,
      math::unsigned_product_leads_to_overflow(s.begin(), 3));
    if (sizeof(unsigned) == 4) {
      u = af::c_grid<3, unsigned>(2101, 1358, 2653);
    }
    else {
      u = af::c_grid<3, unsigned>(2101*1358, 1358*2653, 2653*2101);
    }
    if (sizeof(std::size_t) == 4) {
      s = af::c_grid<3, std::size_t>(2101, 1358, 2653);
    }
    else {
      s = af::c_grid<3, std::size_t>(2101*1358, 1358*2653, 2653*2101);
    }
    check_true(__LINE__,
      math::unsigned_product_leads_to_overflow(u.begin(), 3));
    check_true(__LINE__,
      math::unsigned_product_leads_to_overflow(s.begin(), 3));
  }
  {
    af::c_grid_periodic<3> a(2, 5, 3);
    check_true(__LINE__, a(1, 4, 2) == 2 + 3*(4 + 5*1));
    check_true(__LINE__, a(-1, 6, -2) == a(1, 1, 1));
    af::c_grid_padded_periodic<3> b(5,7,4, // all
                                    2,5,3  // focus
                                );
    check_true(__LINE__, b(1,3,2) == 2 + 4*(3 + 7*1));
    check_true(__LINE__, b(-2, 7, 4) == b(0, 2, 1));
  }
  {
    af::int3 first(-5,-7,-2), last(12,-3,0), pos(0,-4,-1);
    af::c_interval_grid<3> a(first, last);
    check_true( __LINE__, a(pos) == ((0-(-5))*(-3-(-7))
      + (-4-(-7)))*(0-(-2)) + (-1-(-2)) );
  }

  std::cout << "Total OK: " << ok_counter << std::endl;
  if (error_counter || verbose) {
    std::cout << "Total Errors: " << error_counter << std::endl;
  }

  return 0;
}
