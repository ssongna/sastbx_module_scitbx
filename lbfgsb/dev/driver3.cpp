#include <scitbx/lbfgsb/raw.h>
#include <scitbx/array_family/shared.h>

namespace {

  using scitbx::af::shared;
  using scitbx::fn::absolute;
  using scitbx::fn::pow2;
  using namespace scitbx::lbfgsb::raw;

  template <typename ElementType>
  ref1<ElementType>
  make_ref1(shared<ElementType>& a)
  {
    return ref1<ElementType>(a.begin(), a.size());
  }

  void
  driver3()
  {
    std::string task, csave;
    shared<bool> lsave_(4);
    ref1<bool> lsave = make_ref1(lsave_);
    int n=1000;
    int m=10;
    int iprint;
    shared<int> nbd_(n);
    ref1<int> nbd = make_ref1(nbd_);
    shared<int> iwa_(3*n);
    ref1<int> iwa = make_ref1(iwa_);
    shared<int> isave_(44);
    ref1<int> isave = make_ref1(isave_);
    double f, factr, pgtol;
    shared<double> x_(n);
    ref1<double> x = make_ref1(x_);
    shared<double> l_(n);
    ref1<double> l = make_ref1(l_);
    shared<double> u_(n);
    ref1<double> u = make_ref1(u_);
    shared<double> g_(n);
    ref1<double> g = make_ref1(g_);
    shared<double> dsave_(29);
    ref1<double> dsave = make_ref1(dsave_);
    shared<double> wa_(2*m*n+4*n+12*m*m+12*m);
    ref1<double> wa = make_ref1(wa_);
    double t1, t2, time1, time2, tlimit;
    tlimit = 0.2;
    iprint = -1;
    factr=0.0e0;
    pgtol=0.0e0;
    for(int i=1;i<=n;i+=2) {
      nbd(i)=2;
      l(i)=1.0e0;
      u(i)=1.0e2;
    }
    for(int i=2;i<=n;i+=2) {
      nbd(i)=2;
      l(i)=-1.0e2;
      u(i)=1.0e2;
    }
    for(int i=1;i<=n;i++) {
      x(i)=3.0e0;
    }
    printf(
     "\n"
     "     Solving sample problem.\n"
     "      (f = 0.0 at the optimal solution.)\n"
     "\n");
    task = "START";
    timer(time1);
    lbl_111:
    setulb(
      n,m,x,l,u,nbd,f,g,factr,pgtol,wa,iwa,task,iprint,
      csave,lsave,isave,dsave);
    if (task.substr(0,2) == "FG") {
      timer(time2);
      if (time2-time1 > tlimit) {
        task="STOP: CPU EXCEEDING THE TIME LIMIT.";
        printf(" %-60.60s\n", task.c_str());
        int j = 3*n+2*m*n+12*m*m;
        printf(" Latest iterate X =");
        write_ref1(" ", wa.get1(j+1,n));
        printf("At latest iterate   f =%12.5E    |proj g| =%12.5E\n",
          dsave(2), dsave(13));
      }
      else {
        f=.25e0*pow2(x(1)-1.e0);
        for(int i=2;i<=n;i++) {
          f=f+pow2(x(i)-pow2(x(i-1)));
        }
        f=4.e0*f;
        t1=x(2)-pow2(x(1));
        g(1)=2.e0*(x(1)-1.e0)-1.6e1*x(1)*t1;
        for(int i=2;i<=n-1;i++) {
          t2=t1;
          t1=x(i+1)-pow2(x(i));
          g(i)=8.e0*t2-1.6e1*x(i)*t1;
        }
        g(n)=8.e0*t1;
      }
      goto lbl_111;
    }
    if (task.substr(0,5) == "NEW_X") {
      if (isave(34) >= 900) {
        task="STOP: TOTAL NO. of f AND g EVALUATIONS EXCEEDS LIMIT";
      }
      if (dsave(13) <= 1.e-10*(1.0e0 + absolute(f))) {
        task="STOP: THE PROJECTED GRADIENT IS SUFFICIENTLY SMALL";
      }
      printf(
        "Iterate%5d    nfg =%5d    f =%12.5E    |proj g| =%12.5E\n",
        isave(30), isave(34), f, dsave(13));
      if (task.substr(0,4) == "STOP") {
        printf(" %-60.60s\n", task.c_str());
        printf(" Final X=");
        write_ref1(" ", x);
      }
      goto lbl_111;
    }
  }

} // namespace <anonymous>

int main()
{
  try {
    driver3();
  }
  catch (std::exception const& e) {
    printf("%s\n", e.what());
  }
  return 0;
}
