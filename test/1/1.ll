; ModuleID = './test/1/1.lu'
source_filename = "./test/1/1.lu"

@glob = global double 1.000000e+01
@0 = private unnamed_addr constant [13 x i8] c"my name jeff\00", align 1
@1 = private unnamed_addr constant [5 x i8] c"asdf\00", align 1
@2 = private unnamed_addr constant [5 x i8] c"test\00", align 1

declare void @printf(i8*, ...)

define double @ftest1(double %a, ...) {
entry:
  %addtmp = fmul double %a, 2.000000e+00
  %0 = fsub double 1.200000e+01, %addtmp
  ret double %0
}

define double @ftest2(double %a) {
entry:
  %addtmp = fmul double %a, 2.000000e+00
  %addtmp3 = fadd double %addtmp, -1.500000e+00
  ret double %addtmp3
}

define double @ftest3(double %a) {
entry:
  %addtmp = fadd double %a, -1.200000e+01
  ret double %addtmp
}

define double @ftest4(double %a, double %y, i64 %p) {
entry:
  store double 1.200000e+01, double* @glob, align 8
  %0 = sitofp i64 %p to double
  %addtmp = fmul double %a, %0
  %addtmp7 = fmul double %y, %0
  %addtmp8 = fsub double %addtmp, %addtmp7
  %1 = fdiv double %a, %y
  %addtmp10 = fsub double %addtmp8, %1
  ret double %addtmp10
}

define double @ftest5(double %a) {
entry:
  %calltmp = call double @ftest4(double %a, double 0x3FF1F7CEE0000000, i64 200)
  %calltmp3 = call double (double, ...) @ftest1(double %calltmp)
  %glob = load double, double* @glob, align 8
  %addtmp = fadd double %calltmp3, %glob
  ret double %addtmp
}

define double @ftest6(double %a) {
entry:
  %glob = load double, double* @glob, align 8
  %addtmp = fadd double %glob, 2.000000e+00
  %addtmp3 = fadd double %addtmp, -2.000000e+00
  ret double %addtmp3
}

define double @ftest7(double %q) {
entry:
  store double 1.000000e+01, double* @glob, align 8
  %addtmp = fadd double %q, 2.000000e+00
  %addtmp3 = fmul double %addtmp, 3.000000e+00
  ret double %addtmp3
}

define void @qwerrewq() {
entry:
  %glob = load double, double* @glob, align 8
  %addtmp1 = fadd double %glob, -1.000000e+00
  store double %addtmp1, double* @glob, align 8
  ret void
}

define double @ftest8() {
entry:
  call void @qwerrewq()
  %glob = load double, double* @glob, align 8
  ret double %glob
}

define double @ftest9() {
entry:
  %glob = load double, double* @glob, align 8
  %addtmp = fdiv double %glob, 1.000000e+01
  ret double %addtmp
}

define i8* @stest1(i8* %a) {
entry:
  ret i8* %a
}

define i8* @stest2() {
entry:
  ret i8* getelementptr inbounds ([13 x i8], [13 x i8]* @0, i64 0, i64 0)
}

define i8* @stest3() {
entry:
  %calltmp = call i8* @stest2()
  ret i8* %calltmp
}

define i8* @stest4() {
entry:
  ret i8* getelementptr inbounds ([5 x i8], [5 x i8]* @2, i64 0, i64 0)
}
