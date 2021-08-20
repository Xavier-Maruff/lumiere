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
  store double 0x3FF3AE1480000000, double* @glob
  %a = alloca i64
  store i64 12, i64* %a
  %b2 = load double, double* %b1
  %a3 = load i64, i64* %a
  %0 = sitofp i64 %a3 to double
  %addtmp = fadd double %b2, %0
  ret double %addtmp
}

define double @mutate_glob(double %a) {
entry:
  %a1 = alloca double
  store double %a, double* %a1
  %a2 = load double, double* %a1
  %calltmp = call double @mutate_a(double %a2)
  %a3 = load double, double* %a1
  ret double %a3
}

define i8* @stest(i8* %a) {
entry:
  %a1 = alloca i8*
  store i8* %a, i8** %a1
  %a2 = load i8*, i8** %a1
  ret i8* %a2
}
