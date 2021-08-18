; ModuleID = 'lumiere'
source_filename = "lumiere"

@glob = global double 1.200000e+01
@0 = private unnamed_addr constant [13 x i8] c"my name jeff\00", align 1

define double @ftest1(double %a) {
entry:
  %a1 = alloca double
  store double %a, double* %a1
  %a2 = load double, double* %a1
  %0 = fneg double %a2
  %addtmp = fmul double %0, 2.000000e+00
  %addtmp3 = fadd double %addtmp, 1.200000e+01
  ret double %addtmp3
}

define double @ftest2(double %a) {
entry:
  %a1 = alloca double
  store double %a, double* %a1
  %a2 = load double, double* %a1
  %addtmp = fmul double %a2, 2.000000e+00
  %addtmp3 = fadd double %addtmp, -1.500000e+00
  ret double %addtmp3
}

define double @ftest3(double %a) {
entry:
  %a1 = alloca double
  store double %a, double* %a1
  %a2 = load double, double* %a1
  %addtmp = fsub double %a2, 1.200000e+01
  ret double %addtmp
}

define double @ftest4(double %a, double %y, i64 %p) {
entry:
  %p3 = alloca i64
  %y2 = alloca double
  %a1 = alloca double
  store double %a, double* %a1
  store double %y, double* %y2
  store i64 %p, i64* %p3
  %a4 = load double, double* %a1
  %p5 = load i64, i64* %p3
  %0 = sitofp i64 %p5 to double
  %addtmp = fmul double %a4, %0
  %p6 = load i64, i64* %p3
  %y7 = load double, double* %y2
  %1 = sitofp i64 %p6 to double
  %addtmp8 = fmul double %1, %y7
  %addtmp9 = fsub double %addtmp, %addtmp8
  %a10 = load double, double* %a1
  %y11 = load double, double* %y2
  %2 = fneg double %y11
  %addtmp12 = fdiv double %a10, %2
  %addtmp13 = fadd double %addtmp9, %addtmp12
  ret double %addtmp13
}

define double @ftest5(double %a) {
entry:
  %a1 = alloca double
  store double %a, double* %a1
  %a2 = load double, double* %a1
  %calltmp = call double @ftest4(double %a2, double 0x3FF0326180000000, i64 200)
  %calltmp3 = call double @ftest1(double %calltmp)
  %glob = load double, double* @glob
  %addtmp = fadd double %calltmp3, %glob
  ret double %addtmp
}

define double @ftest6(double %a) {
entry:
  %asd = alloca double
  %a1 = alloca double
  store double %a, double* %a1
  store double 2.000000e+00, double* %asd
  %asd2 = load double, double* %asd
  %a3 = load double, double* %a1
  store double %a3, double* %asd
  %asd4 = load double, double* %asd
  ret double %asd4
}

define double @ftest7(double %q) {
entry:
  %q1 = alloca double
  store double %q, double* %q1
  %q2 = load double, double* %q1
  %addtmp = fadd double %q2, 2.000000e+00
  %addtmp3 = fmul double %addtmp, 3.000000e+00
  ret double %addtmp3
}

define i8* @stest1(i8* %a) {
entry:
  %a1 = alloca i8*
  store i8* %a, i8** %a1
  %a2 = load i8*, i8** %a1
  ret i8* %a2
}

define i8* @stest2() {
entry:
  ret i8* getelementptr inbounds ([13 x i8], [13 x i8]* @0, i32 0, i32 0)
}

define i8* @stest3() {
entry:
  %calltmp = call i8* @stest2()
  ret i8* %calltmp
}
