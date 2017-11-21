/// Tools implementing the Gauss-Newton method for non-linear least-squares.

#ifndef SCITBX_GAUSS_NEWTON_H
#define SCITBX_GAUSS_NEWTON_H

#include <scitbx/array_family/shared.h>
#include <scitbx/array_family/shared_algebra.h>
#include <scitbx/array_family/ref_algebra.h>
#include <scitbx/array_family/owning_ref.h>
#include <scitbx/array_family/accessors/row_and_column.h>
#include <scitbx/matrix/cholesky.h>
#include <scitbx/matrix/symmetric_rank_1_update.h>
#include <scitbx/sparse/matrix.h>
#include <scitbx/sparse/triangular.h>

namespace scitbx { namespace lstbx { namespace normal_equations {

#define SCITBX_LSTBX_DECLARE_ARRAY_TYPE(FloatType)                            \
    typedef FloatType scalar_t;                                               \
    typedef af::ref_owning_versa<scalar_t,                                    \
                                 af::packed_u_accessor>                       \
            symmetric_matrix_owning_ref_t;                                    \
    typedef af::ref_owning_versa<scalar_t,                                    \
                                 af::packed_u_accessor>                       \
            upper_diagonal_matrix_owning_ref_t;                               \
    typedef af::ref<scalar_t,                                                 \
                    af::packed_u_accessor>                                    \
            symmetric_matrix_ref_t;                                           \
    typedef af::versa<scalar_t,                                               \
                      af::packed_u_accessor>                                  \
            symmetric_matrix_t;                                               \
    typedef af::versa<scalar_t,                                               \
                      af::packed_u_accessor>                                  \
            upper_diagonal_matrix_t;                                          \
    typedef af::ref_owning_versa<FloatType, af::mat_grid> matrix_owning_ref_t;\
    typedef af::ref<FloatType, af::mat_grid> matrix_ref_t;                    \
    typedef af::ref_owning_shared<scalar_t> vector_owning_ref_t;              \
    typedef af::shared<scalar_t> vector_t;                                    \
    typedef af::ref<scalar_t> vector_ref_t;


  /// Normal equations for linear least-squares problem.
  /** The least-squares target reads

      \f[ L(x) = \| A x - b \|^2 \f]

      where the norm is diagonal-weighted

      \f[ \| y \|^2 = \sum_i w_i y_i^2 \f]

      Objects of this type may also be used to hold the normal equations
      from a non-linear problem after they have been built.
  */
  template <typename FloatType>
  class linear_ls
  {
  public:
    SCITBX_LSTBX_DECLARE_ARRAY_TYPE(FloatType);

    /// Construct a least-squares problem with the given number of unknowns.
    linear_ls(int n_parameters)
      : solved_(false),
        normal_matrix_(n_parameters),
        right_hand_side_(n_parameters)
    {}

    /// Number of unknown parameters
    int n_parameters() const { return right_hand_side_.size(); }

    /// Initialise the least-squares problem with the given normal matrix A
    /// and right hand side b
    linear_ls(symmetric_matrix_t const &a, vector_t const &b)
      : solved_(false),
        normal_matrix_(a),
        right_hand_side_(b)
    {
      SCITBX_ASSERT(a.accessor().n == b.size());
    }

    /// Add the equation \f$ A_{i.} x = b_i \f$ with the given weight
    void add_equation(scalar_t b_i,
                      af::const_ref<scalar_t> const &a_row,
                      scalar_t w)
    {
      scalar_t *p = normal_matrix_.begin();
      for (int i=0; i<n_parameters(); ++i)  {
        right_hand_side_[i] += w * a_row[i] * b_i;
        for (int j=i; j<n_parameters(); ++j) *p++ += w * a_row[i] * a_row[j];
      }
    }

    /// Add the equations A x = b with the given weights
    /** w[i] weights the i-th equation, i.e. the row \f$ A_{i.} \f$.
        If negate_right_hand_side, then the equation is A x + b = 0 instead
     */
    void add_equations(af::const_ref<scalar_t> const &b,
                       sparse::matrix<scalar_t> const &a,
                       af::const_ref<scalar_t> const &w,
                       bool negate_right_hand_side=false)
    {
      SCITBX_ASSERT(   a.n_rows() == b.size()
                    && b.size()   == w.size())(a.n_rows())(b.size())(w.size());
      SCITBX_ASSERT(a.n_cols() == n_parameters());
      sparse::matrix<scalar_t>
      at_w_a = a.this_transpose_times_diagonal_times_this(w);
      vector_t a_t_w_b = a.transpose_times((w * b).const_ref());
      normal_matrix_ += sparse::upper_diagonal_of(at_w_a);
      if (negate_right_hand_side) right_hand_side_ -= a_t_w_b.const_ref();
      else                        right_hand_side_ += a_t_w_b.const_ref();

    }

    /// Reset the state to construction time, i.e. no equations accumulated
    void reset() {
      solved_ = false;
      std::fill(normal_matrix_.begin(), normal_matrix_.end(), scalar_t(0));
      std::fill(right_hand_side_.begin(), right_hand_side_.end(), scalar_t(0));
    }

    /// Only available if the equations have not been solved yet
    symmetric_matrix_t normal_matrix() const {
      SCITBX_ASSERT(!solved());
      return normal_matrix_.array();
    }

    /// Only available if the equations have not been solved yet
    vector_t right_hand_side() const {
      SCITBX_ASSERT(!solved());
      return right_hand_side_.array();
    }

    /** \brief Solve the normal equations for the parameters (linear case)
         or their shift (linearised non-linear case)
     */
    void solve() {
      using scitbx::matrix::cholesky::u_transpose_u_decomposition_in_place;
      u_transpose_u_decomposition_in_place<scalar_t> cholesky(normal_matrix_);
      SCITBX_ASSERT(!cholesky.failure);
      cholesky.solve_in_place(right_hand_side_);
      solved_ = true;
    }

    bool solved() const {
      return solved_;
    }

    /// Only available after the equations have been solved
    upper_diagonal_matrix_t cholesky_factor() const {
      SCITBX_ASSERT(solved());
      return normal_matrix_.array();
    }

    /// Only available after the equations have been solved
    vector_t solution() const {
      SCITBX_ASSERT(solved());
      return right_hand_side_.array();
    }

  public:
    bool solved_;
    symmetric_matrix_owning_ref_t normal_matrix_;
    vector_owning_ref_t right_hand_side_;
  };


  /// Normal equations for non-linear least-squares
  /** The least-squares target reads

      \f[ L(x) = \frac{1}{2} \|r(x)\|^2 \f]

      where the norm is diagonal-weighted

      \f[ \| y \|^2 = \sum_i w_i y_i^2 \f]

      and \f$r(x)\f$ is a vector of residuals depending on a vector
      of unknowns \f$x\f$.
  */
  template <typename FloatType>
  class non_linear_ls
  {
  public:
    SCITBX_LSTBX_DECLARE_ARRAY_TYPE(FloatType);

    /// Construct a least-squares problem with the given number of unknowns.
    non_linear_ls(int n_parameters)
      : n_equations_(0),
        r_sq(0),
        linearised(n_parameters)
    {}

    /// Construct with an exiting L.S. problem.
    /** That is
          - this->objective()  == objective
          - this->step_equations().right_hand_side() == opposite_of_grad_objective
          - this->step_equations().normal_matrix() == normal_matrix
     */
    non_linear_ls(std::size_t n_equations,
                  scalar_t objective,
                  vector_t const &opposite_of_grad_objective,
                  symmetric_matrix_t const &normal_matrix)
    : n_equations_(n_equations),
      r_sq(2*objective),
      linearised(normal_matrix, opposite_of_grad_objective)
    {}

    /// Number of equations
    /** i.e. number of components of the residual vector \f$r(x)\f$
     */
    std::size_t n_equations() const { return n_equations_; }

    /// Number of unknown parameters
    int n_parameters() const { return linearised.n_parameters(); }

    /// Number of degrees of freedom
    std::size_t dof() const { return n_equations() - n_parameters(); }

    /// Add the given residual with the given weight
    void add_residual(scalar_t r, scalar_t w) {
      n_equations_++;
      r_sq += w*r*r;
    }

    /// Add the given residuals with the given weights
    void add_residuals(af::const_ref<scalar_t> const &r,
                       af::const_ref<scalar_t> const &w)
    {
      for (int i=0; i<r.size(); ++i) {
        add_residual(r[i], w.size() ? w[i] : 1);
      }
    }

    /// Add the linearisation of the equation \f$r_i(x) = 0\f$
    /** with the given weight */
    void add_equation(scalar_t r,
                      af::const_ref<scalar_t> const &grad_r,
                      scalar_t w)
    {
      add_residual(r, w);
      linearised.add_equation(-r, grad_r, w);
    }

    /// Add the linearisation of the equations \f$r(x) = 0\f$ all at once
    /** The Jacobian is that of \f$x \mapto r(x)\f$.
     */
    void add_equations(af::const_ref<scalar_t> const &r,
                       af::const_ref<scalar_t, af::mat_grid> const &jacobian,
                       af::const_ref<scalar_t> const &w)
    {
      SCITBX_ASSERT(   r.size() == jacobian.n_rows()
                    && (!w.size() || r.size() == w.size()))
                   (r.size())(jacobian.n_rows())(w.size());
      SCITBX_ASSERT(jacobian.n_columns() == n_parameters())
                   (jacobian.n_columns())(n_parameters());
      for (int i=0; i<r.size(); ++i) {
        add_equation(r[i], af::row(jacobian, i), w.size() ? w[i] : 1);
      }
    }

    void add_equations(af::const_ref<scalar_t> const &r,
                       sparse::matrix<scalar_t> const &jacobian,
                       af::const_ref<scalar_t> const &w)
    {
      SCITBX_ASSERT(   r.size() == jacobian.n_rows()
                    && (!w.size() || r.size() == w.size()))
                   (r.size())(jacobian.n_rows())(w.size());
      SCITBX_ASSERT(jacobian.n_cols() == n_parameters())
                   (jacobian.n_cols())(n_parameters());
      add_residuals(r, w);
      linearised.add_equations(r, jacobian, w, /*negate_right_hand_side=*/true);
    }

    /// Objective value \f$L(x)\f$ for the current value of the unknowns
    scalar_t objective() const { return r_sq/2; }

    /// The \f$chi^2\f$ of the fit
    /**
        \f [ \frac{\sum_i w_i r_i(x)^2}
                  {n_{\text{equations}} - n_{\text{parameters}}

        Strictly speaking, this is only meaningful when the residuals have
        the form used in a fit, \f$r_i(x) = \text{model} - \text{data}\f$,
        but the computation is the same in the general case.
     */
    scalar_t chi_sq() const { return r_sq/dof(); }

    /// Linearised equations to solve for a step
    linear_ls<scalar_t> &step_equations() { return linearised; }

    /// Reset the state to construction time, i.e. no equations accumulated
    void reset() {
      n_equations_ = 0;
      r_sq = 0;
      linearised.reset();
    }

  protected:
    std::size_t n_equations_;
    scalar_t r_sq;
    linear_ls<scalar_t> linearised;
  };


  /// Normal equations for least-squares fit with an overall scale.
  /** The least-squares target reads

      \f[ L(K, x) = \frac{1}{2} \frac{ \sum w ( K y_c(x) - y_o )^2 }
                                     { \sum w y_o^2 }
      \f]

      where the both of \f$ y_c(x) \f$ and \f$ y_o \f$ are vectors,
      respectively the model to fit to the data. Alternatively, the
      non-normalised

      \f[ \tilde{L}(K, x) = \frac{1}{2} \sum w ( K y_c(x) - y_o )^2 \f]

      may be used instead.

      One takes advantage of the separability of the problem:

        - step 1: \f$ K^*(x) = \argmin_K L(K, x) \f$

        - step 2: Build the Newton equations for the problem
                  \f$ \min_x L(K^*(x), x) \f$

          in the Gauss approximation of small residuals (reduced equations).

  Reference:
   Separable nonlinear least squares
   H.B. Nielsen
   Technical report IMM-REP-2000-01
   http:http://www2.imm.dtu.dk/pubdb/views/edoc_download.php/646/ps/imm646.ps

   and references therein.
  */
  template <typename FloatType,
            template<typename> class SumOfRank1Updates=matrix::sum_of_symmetric_rank_1_updates>
  class non_linear_ls_with_separable_scale_factor
  {
  public:
    SCITBX_LSTBX_DECLARE_ARRAY_TYPE(FloatType);
    typedef SumOfRank1Updates<FloatType> sum_of_rank_1_updates_t;

    /// Construct a least-squares problem with the given number of parameters.
    /** That is the length of the vector \f$ x \f$. The flag normalised
        specify whether to use the normalised objective \f$L\f$ or the
        non-normalised objective \f$\tilde{L}\f$.
     */
    non_linear_ls_with_separable_scale_factor(int n_parameters,
                                              bool normalised=true)
      : yo_dot_yc(0), yc_sq(0), yo_sq(0),
        n_params(n_parameters),
        n_data(0),
        normalised_(normalised),
        grad_yc_dot_grad_yc(n_parameters),
        yo_dot_grad_yc(n_parameters),
        yc_dot_grad_yc(n_parameters),
        grad_k_star(n_parameters),
        finalised_(false),
        reduced_ls(n_parameters)
    {}

    /// Number of unknown parameters, not including the overall scale factor
    int n_parameters() const { return n_params; }

    /// Number of equations \f$y_o = K y_c(x)\f$ plus those added to
    /// the reduced_problem().
    std::size_t n_equations() const {
      return finalised() ? reduced_ls.n_equations() : n_data;
    }

    /// Number of degrees of freedom.
    /** This does take into account the equations added to the reduced_problem().
     */
    std::size_t dof() const { return n_equations() - n_parameters(); }

    /// Whether the L.S. target is normalised by \f$ \sum w y_o^2 \f$ or not
    bool normalised() const { return normalised_; }

    void add_residual(scalar_t yc, scalar_t yo, scalar_t w) {
      n_data++;
      yo_sq += w * yo * yo;
      yo_dot_yc += w * yo * yc;
      yc_sq += w * yc * yc;
    }

    /** \brief Add the linearisation of the equation
         \f$y_{c,i} \propto y_{o,i}\f$ with weight w.
     */
    void add_equation(scalar_t yc, af::const_ref<scalar_t> const &grad_yc,
                      scalar_t yo, scalar_t w)
    {
      SCITBX_ASSERT(grad_yc.size() == n_params);
      SCITBX_ASSERT(!finalised());
      add_equation(yc, grad_yc.begin(), yo, w);
    }

    /// Overload for when efficiency is paramount.
    /** This shall not be called after finalise() has been called
        but this is not enforced for speed.
     */
    void add_equation(scalar_t yc, scalar_t const *grad_yc,
                      scalar_t yo, scalar_t w)
    {
      add_residual(yc, yo, w);
      grad_yc_dot_grad_yc.add(grad_yc, w);
      for (int i=0; i<n_params; ++i) {
        yo_dot_grad_yc[i] += w * yo * grad_yc[i];
        yc_dot_grad_yc[i] += w * yc * grad_yc[i];
      }
    }

    /// Add many equations in one go
    void add_equations(af::const_ref<scalar_t> const &yc,
                       af::const_ref<scalar_t, af::mat_grid> const &jacobian_yc,
                       af::const_ref<scalar_t> const &yo,
                       af::const_ref<scalar_t> const &w)
    {
      SCITBX_ASSERT(   yc.size() == jacobian_yc.n_rows()
                    && (!w.size() || yc.size() == w.size()))
                   (yc.size())(jacobian_yc.n_rows())(w.size());
      SCITBX_ASSERT(jacobian_yc.n_columns() == n_parameters())
                   (jacobian_yc.n_columns())(n_parameters());
      for (int i=0; i<yc.size(); ++i) {
        add_equation(yc[i], &jacobian_yc(i, 0), yo[i], w.size() ? w[i] : 1);
      }
    }

    /// \f$\sum w y_o^2\f$
    /** This is the normalisation that guarantees
        that \f$L(K, x)\f$ is between 0 and 1.
     */
    scalar_t sum_w_yo_sq() const {
      SCITBX_ASSERT(finalised());
      return yo_sq;
    }

    /** \brief The value \f$ K^*(x) \f$ of the scale factor optimising the L.S. objective for a given constant \f$ x \f$.
     */
    scalar_t optimal_scale_factor() const {
      SCITBX_ASSERT(finalised());
      return yo_dot_yc/yc_sq;
    }

    /// The value of the minimised function, for the optimal scale factor
    /** This is \f$L(K^*(x), x)\f$, plus the contributions added to
        the reduced_problem().
     */
    scalar_t objective() const {
      SCITBX_ASSERT(finalised());
      return reduced_ls.objective();
    }

    /// \f$\chi^2\f$ of the fit.
    /** The \f$\chi^2\f$ for the fit of \f$y_c(x)\f$ to \f$y_o\f$.
        This does include the contributions added to the reduced_problem().
     */
    scalar_t chi_sq() const {
      SCITBX_ASSERT(finalised());
      return (r_sq + 2*(reduced_ls.objective() - objective_))/dof();
    }

    /// Equation accumulation is finished.
    /** The reduced normal equations for \f$ x \f$ as per step 2 are constructed
     */
    void finalise(bool objective_only=false) {
      SCITBX_ASSERT(!finalised() && n_equations())(n_equations());
      finalised_ = true;

      grad_yc_dot_grad_yc.finalise();
      a = grad_yc_dot_grad_yc;

      scalar_t k_star = optimal_scale_factor(), k_star_sq = k_star*k_star;
      r_sq = yo_sq*(1 - (k_star_sq * yc_sq)/yo_sq);
      objective_ = r_sq/2;
      if (normalised()) objective_ /= yo_sq;

      vector_owning_ref_t b = yo_dot_grad_yc;
      reduced_ls = non_linear_ls<scalar_t>(n_data,
                                           objective_, b.array(), a.array());

      if (objective_only) return;

      scalar_t r_dot_yc = yo_dot_yc - k_star*yc_sq;
      scalar_t inv_yc_sq = 1./yc_sq;
      for (int i=0; i<n_params; ++i) {
        scalar_t r_dot_grad_yc_i = yo_dot_grad_yc[i] - k_star*yc_dot_grad_yc[i];
        grad_k_star[i] = inv_yc_sq*(r_dot_grad_yc_i - k_star*yc_dot_grad_yc[i]);
        b[i] = k_star*r_dot_grad_yc_i + grad_k_star[i]*r_dot_yc;
      }
      scalar_t *pa = a.begin();
      for (int i=0; i<n_params; ++i) for (int j=i; j<n_params; ++j) {
        scalar_t a_ij = *pa;
        a_ij = k_star_sq*a_ij
             + k_star*(  yc_dot_grad_yc[i]*grad_k_star[j]
                       + yc_dot_grad_yc[j]*grad_k_star[i])
             + grad_k_star[i]*grad_k_star[j]*yc_sq;
        *pa++ = a_ij;
      }
      if (normalised()) {
        a /= yo_sq;
        b /= yo_sq;
      }
    }

    /// Whether finalise has been called.
    bool finalised() const { return finalised_; }

    /// The linear L.S. problem to solve for a step toward the minimum.
    linear_ls<scalar_t> &step_equations() {
      return reduced_problem().step_equations();
    }

    /// The non-linear problem with the scale factor already optimised away
    /** The main use of this function comes for an objective function

        \[ \tilde{L}(K, x) = L(K, x) + \frac{1}{2} \|r(x\|^2 \]

        for some residual vector \f$r(x)\f$ independent of the overall scale
        factor that the first term depends upon. The equations for that
        second term may then be accumulated into the object returned by this
        function, to produce the correct equations for
        \f$\tilde{L}(K^*(x), x)\f$.

        This would not be possible with step_equations() which looses
        sight of the objective value.

        Invariant: reduced_problem().step_equations() and this->step_equations()
        are identical (i.e. modify one, modifies the other).
     */
    non_linear_ls<scalar_t> &reduced_problem() {
      SCITBX_ASSERT(finalised());
      return reduced_ls;
    }

    /// Ready this for another computation of the normal equations
    void reset() {
      n_data = 0;
      yo_dot_yc = 0; yc_sq = 0; yo_sq = 0;
      grad_yc_dot_grad_yc.reset();
      std::fill(yo_dot_grad_yc.begin(), yo_dot_grad_yc.end(), scalar_t(0));
      std::fill(yc_dot_grad_yc.begin(), yc_dot_grad_yc.end(), scalar_t(0));
      std::fill(grad_k_star.begin(), grad_k_star.end(), scalar_t(0));
      finalised_ = false;
    }

  private:
    scalar_t yo_dot_yc, yo_sq, yc_sq, r_sq, objective_;
    int n_params;
    std::size_t n_data;
    bool normalised_;
    sum_of_rank_1_updates_t grad_yc_dot_grad_yc;
    symmetric_matrix_owning_ref_t a; // normal matrix stored
                                     // as packed upper diagonal
    vector_owning_ref_t yo_dot_grad_yc, yc_dot_grad_yc, grad_k_star;
    bool finalised_;
    non_linear_ls<scalar_t> reduced_ls;
  };

}}}

#endif // GUARD
