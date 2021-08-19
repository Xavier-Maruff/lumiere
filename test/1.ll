; ModuleID = 'lumiere'
source_filename = "lumiere"

@0 = private unnamed_addr constant [13 x i8] c"my name jeff\00", align 1
@1 = private unnamed_addr constant [5 x i8] c"asdf\00", align 1
@2 = private unnamed_addr constant [5 x i8] c"test\00", align 1

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
  %glob = load
  store double 1.200000e+01, double %glob
  %y4 = load double, double* %y2
  %0 = fneg double %y4
  %a5 = load double, double* %a1
  %addtmp = fdiv double %a5, %0
  %y6 = load double, double* %y2
  %p7 = load i64, i64* %p3
  %1 = sitofp i64 %p7 to double
  %addtmp8 = fmul double %1, %y6
  %p9 = load i64, i64* %p3
  %a10 = load double, double* %a1
  %2 = sitofp i64 %p9 to double
  %addtmp11 = fmul double %a10, %2
  %addtmp12 = fsub double %addtmp11, %addtmp8
  %addtmp13 = fadd double %addtmp12, %addtmp
  ret double %addtmp13
}

define double @ftest5(double %a) {
entry:
  %a1 = alloca double
  store double %a, double* %a1
  %glob = load
  %a2 = load double, double* %a1
  %calltmp = call double @ftest4(double %a2, double 0x3FF0326180000000, i64 200)
  %calltmp3 = call double @ftest1(double %calltmp)
  %addtmp = fadd double %calltmp3, %glob
  ret double %addtmp
}

define double @ftest6(double %a) {
entry:
  %zxcv = alloca double
  %asd = alloca double
  %a1 = alloca double
  store double %a, double* %a1
  store double 2.000000e+00, double* %asd
  %asd2 = load double, double* %asd
  %zxcv3 = load double, double* %zxcv
  %zxcv4 = load double, double* %zxcv
  %glob = load
  store double %glob, double %zxcv4
  %asd5 = load double, double* %asd
  %zxcv6 = load double, double* %zxcv
  store double %zxcv6, double %asd5
  %asd7 = load double, double* %asd
  ret double %asd7
}

define double @ftest7(double %q) {
entry:
  %q1 = alloca double
  store double %q, double* %q1
  %glob = load
  store double 1.000000e+01, double %glob
  %q2 = load double, double* %q1
  %addtmp = fadd double %q2, 2.000000e+00
  %addtmp3 = fmul double %addtmp, 3.000000e+00
  ret double %addtmp3
}

define double @ftest8() {
entry:
  %glob = load
  ret double %glob
}

define double @ftest9() {
entry:
  ret double 0x3FF3333340000000
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

define i8* @stest4() {
entry:
  %qawsed = alloca i8*
  store i8* getelementptr inbounds ([5 x i8], [5 x i8]* @1, i32 0, i32 0), i8** %qawsed
  %qawsed1 = load i8*, i8** %qawsed
  %qawsed2 = load i8*, i8** %qawsed
  store i8* getelementptr inbounds ([5 x i8], [5 x i8]* @2, i32 0, i32 0), i8* %qawsed2
  %qawsed3 = load i8*, i8** %qawsed
  ret i8* %qawsed3
}
