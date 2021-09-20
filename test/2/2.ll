; ModuleID = '2.lu'
source_filename = "2.lu"

@glob = global double 1.000000e+01
@0 = private unnamed_addr constant [17 x i8] c"test: glob = %f\0A\00", align 1
@1 = private unnamed_addr constant [15 x i8] c"test_int = %d\0A\00", align 1

declare void @printf(i8*, ...)

declare void @scanf(i8*, ...)

define double @get_glob() {
entry:
  %glob = load double, double* @glob, align 8
  ret double %glob
}

define void @main() {
entry:
  %calltmp = call double @get_glob()
  call void (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([17 x i8], [17 x i8]* @0, i64 0, i64 0), double %calltmp)
  call void (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([15 x i8], [15 x i8]* @1, i64 0, i64 0), i64 25)
  ret void
}
