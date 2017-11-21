#include <iostream>
#include <vector>
#include <scitbx/fftpack/complex_to_complex_3d.h>
#include <scitbx/fftpack/real_to_complex_3d.h>
#include <scitbx/array_family/versa.h>
#include <scitbx/array_family/accessors/c_grid.h>

int main(void)
{
  std::size_t i;

  scitbx::fftpack::complex_to_complex<double> cfft(10);
  std::vector<std::complex<double> > vc(cfft.n());
  for(i=0;i<cfft.n();i++) {
    vc[i] = std::complex<double>(2.*i, 2.*i+1.);
  }
  cfft.forward(vc.begin());
  for(i=0;i<cfft.n();i++) {
    std::cout << vc[i].real() << " " << vc[i].imag() << std::endl;
  }
  cfft.backward(vc.begin());
  for(i=0;i<cfft.n();i++) {
    std::cout << vc[i].real() << " " << vc[i].imag() << std::endl;
  }

  scitbx::fftpack::real_to_complex<double> rfft(10);
  std::vector<double> vr(2 * rfft.n_complex());
  for(i=0;i<rfft.n_real();i++) {
    vr[i] = 1.*i;
  }
  rfft.forward(vr.begin());
  for(i=0;i<2*rfft.n_complex();i++) {
    std::cout << vr[i] << std::endl;
  }
  rfft.backward(vr.begin());
  for(i=0;i<rfft.n_real();i++) {
    std::cout << vr[i] << std::endl;
  }

  scitbx::fftpack::complex_to_complex_3d<double> cfft3d(2, 3, 5);
  scitbx::af::versa<std::complex<double>, scitbx::af::c_grid<3> >
  c3dmap(scitbx::af::c_grid<3>(cfft3d.n()));
  cfft3d.forward(c3dmap.ref());
  cfft3d.backward(c3dmap.ref());

  scitbx::fftpack::real_to_complex_3d<double> rfft3d(3, 4, 5);
  scitbx::af::versa<double, scitbx::af::c_grid<3> >
  r3dmap(scitbx::af::c_grid<3>(rfft3d.m_real()));
  rfft3d.forward(r3dmap.ref());
  rfft3d.backward(r3dmap.ref());
#ifdef NEVER_DEFINED
#endif

  return 0;
}
