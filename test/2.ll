; ModuleID = 'lumiere'
source_filename = "lumiere"

@glob = global double 1.200000e+01

define double @get_glob() {
entry:
  %glob = load double, double* @glob
  ret double %glob
}

define double @mutate_a(double %b) {
entry:
  %b1 = alloca double
  store double %b, double* %b1
  %a = alloca i64
  store i64 12, i64* %a
  %a2 = load i64, i64* %a
  %a3 = load i64, i64* %a
  %b4 = alloca double
  %b5 = load double, double* %b4
  %0 = sitofp i64 %a3 to double
  %addtmp = fadd double %b5, %0
  ret double %addtmp
}

define double @mutate_glob(double %a) {
entry:
  %a1 = alloca double
  store double %a, double* %a1
  %b = alloca double
  %b2 = load double, double* %b
  %a3 = alloca double
  %a4 = load double, double* %a3
  %addtmp = fadd double %a4, %b2
  %calltmp = call double @mutate_a(double %addtmp)
  %a5 = load double, double* %a3
  ret double %a5
}

define i8* @stest(i8* %a) {
entry:
  %a1 = alloca i8*
  store i8* %a, i8** %a1
  %a2 = alloca i8*
  %a3 = load i8*, i8** %a2
  ret i8* %a3
}
