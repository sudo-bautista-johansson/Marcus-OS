; ModuleID = 'kryo'
source_filename = "kryo.kyo"
target datalayout = "e-m:e-p:64:64-i64:64-n32:64-S128"
target triple = "x86_64-pc-none-elf"

@.str.0 = private unnamed_addr constant [42 x i8] c"========================================\0A\00"
@.str.1 = private unnamed_addr constant [28 x i8] c"          MARCUS OS KERNEL\0A\00"
@.str.2 = private unnamed_addr constant [43 x i8] c"========================================\0A\0A\00"
@.str.3 = private unnamed_addr constant [22 x i8] c"Architecture: x86_64\0A\00"
@.str.4 = private unnamed_addr constant [16 x i8] c"Language: Kryo\0A\00"
@.str.5 = private unnamed_addr constant [10 x i8] c"Boot: OK\0A\00"
@.str.6 = private unnamed_addr constant [9 x i8] c"CPU: OK\0A\00"
@.str.7 = private unnamed_addr constant [31 x i8] c"Memory: identity mapped 2 MiB\0A\00"
@.str.8 = private unnamed_addr constant [15 x i8] c"Serial: COM1\0A\0A\00"
@.str.9 = private unnamed_addr constant [31 x i8] c"MARCUS OS kernel initialized.\0A\00"

declare void @kernel_puts(i8* %arg0)

define dso_local void @kernel_main() {
  entry:
  %0 = getelementptr [42 x i8], [42 x i8]* @.str.0, i64 0, i64 0
  call void @kernel_puts(i8* %0)
  %1 = getelementptr [28 x i8], [28 x i8]* @.str.1, i64 0, i64 0
  call void @kernel_puts(i8* %1)
  %2 = getelementptr [43 x i8], [43 x i8]* @.str.2, i64 0, i64 0
  call void @kernel_puts(i8* %2)
  %3 = getelementptr [22 x i8], [22 x i8]* @.str.3, i64 0, i64 0
  call void @kernel_puts(i8* %3)
  %4 = getelementptr [16 x i8], [16 x i8]* @.str.4, i64 0, i64 0
  call void @kernel_puts(i8* %4)
  %5 = getelementptr [10 x i8], [10 x i8]* @.str.5, i64 0, i64 0
  call void @kernel_puts(i8* %5)
  %6 = getelementptr [9 x i8], [9 x i8]* @.str.6, i64 0, i64 0
  call void @kernel_puts(i8* %6)
  %7 = getelementptr [31 x i8], [31 x i8]* @.str.7, i64 0, i64 0
  call void @kernel_puts(i8* %7)
  %8 = getelementptr [15 x i8], [15 x i8]* @.str.8, i64 0, i64 0
  call void @kernel_puts(i8* %8)
  %9 = getelementptr [31 x i8], [31 x i8]* @.str.9, i64 0, i64 0
  call void @kernel_puts(i8* %9)
  ret void
}

define dso_local i32 @main() {
  entry:
  call void @kernel_main()
  ret i32 0
}

