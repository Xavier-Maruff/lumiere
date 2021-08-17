; ModuleID = 'lumiere'
source_filename = "lumiere"

@0 = private unnamed_addr constant [13 x i8] c"my name jeff\00", align 1

define double @ftest1(double %a) {
entry:
  %0 = fneg double %a
  %addtmp = fmul double %0, 2.000000e+00
  %addtmp1 = fadd double %addtmp, 1.200000e+01
  ret double %addtmp1
}

define double @ftest2(double %a) {
entry:
  %addtmp = fmul double %a, 2.000000e+00
  %addtmp1 = fadd double %addtmp, -1.500000e+00
  ret double %addtmp1
}

define double @ftest3(double %a) {
entry:
  %addtmp = fsub double %a, 1.200000e+01
  ret double %addtmp
}

define double @ftest4(double %a, double %y, i64 %p) {
entry:
  %0 = sitofp i64 %p to double
  %addtmp = fmul double %a, %0
  %1 = sitofp i64 %p to double
  %addtmp1 = fmul double %1, %y
  %addtmp2 = fsub double %addtmp, %addtmp1
  %2 = fneg double %y
  %addtmp3 = fdiv double %a, %2
  %addtmp4 = fadd double %addtmp2, %addtmp3
  ret double %addtmp4
}

define double @asdf() {
entry:
  %calltmp = call double @ftest1(double 1.000000e+00)
  ret double %calltmp
}

define i8* @stest1(i8* %a) {
entry:
  ret i8* %a
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
