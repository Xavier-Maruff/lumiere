; ModuleID = 'lumiere'
source_filename = "lumiere"

@glob = global double 1.200000e+01

define double @get_glob() {
entry:
  %glob = load double, double* @glob
  ret double %glob
}

define double @test_f(double %a) {
entry:
  %a2 = alloca double
  %a1 = alloca double
  store double %a, double* %a1
  %a3 = load double, double* %a2
  ret double %a3
}

define i8* @test_s(i8* %a) {
entry:
  %a2 = alloca i8*
  %a1 = alloca i8*
  store i8* %a, i8** %a1
  %a3 = load i8*, i8** %a2
  ret i8* %a3
}

define double @mutate_a(double %b) {
entry:
  %b2 = alloca double
  %b1 = alloca double
  store double %b, double* %b1
  %b3 = load double, double* %b2
  %b4 = load double, double* %b2
  store double %b3, double %b4
  %b5 = load double, double* %b2
  ret double %b5
}

define double @mutate_glob(double %a) {
entry:
  %a2 = alloca double
  %a1 = alloca double
  store double %a, double* %a1
  %a3 = load double, double* %a2
  %calltmp = call double @mutate_a(double %a3)
  %a4 = load double, double* %a2
  ret double %a4
}
