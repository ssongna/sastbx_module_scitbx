#ifndef SCITBX_FFTPACK_COMPLEX_TO_COMPLEX_2D_H
#define SCITBX_FFTPACK_COMPLEX_TO_COMPLEX_2D_H

#include <scitbx/fftpack/complex_to_complex.h>
#include <scitbx/error.h>
#include <omptbx/omp_or_stubs.h>

//#define SCITBX_FFTPACK_COMPLEX_TO_COMPLEX_2D_NO_PRAGMA_OMP

namespace scitbx { namespace fftpack {

  //! 2-dimensional complex-to-complex Fast Fourier Transformation.
  template <typename RealType,
            typename ComplexType = std::complex<RealType> >
  class complex_to_complex_2d
  {
    public:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
      typedef RealType real_type;
      typedef ComplexType complex_type;
#endif // DOXYGEN_SHOULD_SKIP_THIS

      //! Default constructor.
      complex_to_complex_2d() {}
      //! Initialization for transforms of lengths n.
      /*! See also: Constructor of complex_to_complex.
       */
      complex_to_complex_2d(const af::int2& n);
      //! Initialization for transforms of lengths n0, n1
      /*! See also: Constructor of complex_to_complex.
       */
      complex_to_complex_2d(std::size_t n0, std::size_t n1);
      //! Access the n (or n0, n1) that was passed to the constructor.
      af::int2 n() const
      {
        return af::int2(fft1d_[0].n(), fft1d_[1].n() );
      }
      //! In-place "forward" Fourier transformation.
      /*! See also: complex_to_complex
       */
      template <typename MapType>
      void forward(
        MapType map,
        real_type* scratch=0)
      {
        transform(select_sign<forward_tag>(), map, scratch);
      }
      //! In-place "backward" Fourier transformation.
      /*! See also: complex_to_complex
       */
      template <typename MapType>
      void backward(
        MapType map,
        real_type* scratch=0)
      {
        transform(select_sign<backward_tag>(), map, scratch);
      }

    protected:
      // This accepts complex or real maps.
      template <typename Tag, typename MapType>
      void
      transform(
        select_sign<Tag> tag,
        MapType map,
        real_type* scratch)
      {
        typedef typename MapType::value_type real_or_complex_type;
        real_or_complex_type const* select_overload = 0;
        transform(tag, map, scratch, select_overload);
      }

      // Cast map of real to map of complex.
      template <typename Tag, typename MapType>
      void
      transform(
        select_sign<Tag> tag,
        MapType map,
        real_type* scratch,
        real_type const* select_overload)
      {
        SCITBX_ASSERT(select_overload == 0);
        typedef typename MapType::accessor_type accessor_type;
        accessor_type dim_real(map.accessor());
        if (dim_real[1] % 2 != 0) {
          throw error("Number of elements in second dimension must be even.");
        }
        af::ref<complex_type, accessor_type> cmap(
          reinterpret_cast<complex_type*>(map.begin()),
          accessor_type(dim_real[0], dim_real[1] / 2));
        complex_type const* select_complex_overload = 0;
        transform(tag, cmap, scratch, select_complex_overload);
      }

      // Core routine always works on complex maps.
      template <typename Tag, typename MapType>
      void
      transform(
        select_sign<Tag> tag,
        MapType map,
        real_type* scratch,
        complex_type const* select_overload)
      {
  // FUTURE: move out of class body
  {
    if (select_overload != 0) {
      // Unreachable.
      // Trick to avoid g++ 4.4.0 warnings when compiling with -fopenmp.
      throw std::runtime_error(__FILE__);
    }
    int ny = fft1d_[0].n();
    int nz = fft1d_[1].n();
    int seq_size = 2 * std::max(ny, nz);
    scitbx::auto_array<real_type> seq_and_scratch;
    if (omp_in_parallel() == 0) omp_set_dynamic(0);
#if !defined(SCITBX_FFTPACK_COMPLEX_TO_COMPLEX_2D_NO_PRAGMA_OMP)
    #pragma omp parallel
#endif
    {
      int num_threads = omp_get_num_threads();
      int i_thread = omp_get_thread_num();
#if !defined(SCITBX_FFTPACK_COMPLEX_TO_COMPLEX_2D_NO_PRAGMA_OMP)
      #pragma omp single
#endif
      {
        seq_and_scratch = scitbx::auto_array<real_type>(
          new real_type[2 * seq_size * num_threads]);
      }
      real_type* scratch = seq_and_scratch.get() + 2 * seq_size * i_thread;
      complex_type* seq = reinterpret_cast<complex_type*>(scratch + seq_size);
#if !defined(SCITBX_FFTPACK_COMPLEX_TO_COMPLEX_2D_NO_PRAGMA_OMP)
      #pragma omp for
#endif

      for (int iz = 0; iz < nz; iz++) {
        for (int iy = 0; iy < ny; iy++) {
            seq[iy] = map(iy, iz);
          }
          fft1d_[0].transform(tag, seq, scratch);
          for (int iy = 0; iy < ny; iy++) {
            map(iy, iz) = seq[iy];
          }
      }
#if !defined(SCITBX_FFTPACK_COMPLEX_TO_COMPLEX_2D_NO_PRAGMA_OMP)
      #pragma omp for
#endif
      for (int iy = 0; iy < ny; iy++) {
        fft1d_[1].transform(tag, &map(iy, 0), scratch);
      }

    }
  }
      }
    private:
      af::tiny<complex_to_complex<real_type, complex_type>, 2> fft1d_;
  };

  template <typename RealType, typename ComplexType>
  complex_to_complex_2d<RealType, ComplexType
    >::complex_to_complex_2d(const af::int2& n)
  {
    for(std::size_t i=0;i<2;i++) {
      fft1d_[i] = complex_to_complex<real_type, complex_type>(n[i]);
    }
  }

  template <typename RealType, typename ComplexType>
  complex_to_complex_2d<RealType, ComplexType
    >::complex_to_complex_2d(std::size_t n0, std::size_t n1)
  {
    fft1d_[0] = complex_to_complex<real_type, complex_type>(n0);
    fft1d_[1] = complex_to_complex<real_type, complex_type>(n1);
  }

}} // namespace scitbx::fftpack

#endif // SCITBX_FFTPACK_COMPLEX_TO_COMPLEX_2D_H
