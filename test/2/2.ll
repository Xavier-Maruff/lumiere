; ModuleID = '2.lu'
source_filename = "2.lu"

@glob = global double 1.000000e+01
@0 = private unnamed_addr constant [6 x i8] c"test\0A\00", align 1

declare void @printf(i8*, ...)

define double @get_glob() {
entry:
  %glob = load double, double* @glob
  ret double %glob
}

define void @main() {
entry:
  %calltmp = call double @get_glob()
  call void (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @0, i32 0, i32 0))
  ret void
}
