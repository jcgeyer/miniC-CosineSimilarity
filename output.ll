; ModuleID = 'miniC'
source_filename = "miniC"
target triple = "x86_64-pc-linux-gnu"

declare void @print(i32)

declare i32 @read()

define i32 @func(i32 %0) {
entry:
  %b_2 = alloca i32, align 4
  %a_1 = alloca i32, align 4
  %i_0 = alloca i32, align 4
  %ret_ref = alloca i32, align 4
  store i32 %0, ptr %i_0, align 4
  store i32 -5, ptr %a_1, align 4
  store i32 2, ptr %b_2, align 4
  %1 = load i32, ptr %a_1, align 4
  %2 = load i32, ptr %i_0, align 4
  %3 = icmp slt i32 %1, %2
  br i1 %3, label %if_true, label %if_false

ret:                                              ; preds = %if_end
  %ret_val = load i32, ptr %ret_ref, align 4
  ret i32 %ret_val

if_true:                                          ; preds = %entry
  br label %while_cond

if_false:                                         ; preds = %entry
  %4 = load i32, ptr %b_2, align 4
  %5 = load i32, ptr %i_0, align 4
  %6 = icmp slt i32 %4, %5
  br i1 %6, label %if_true1, label %if_false2

while_cond:                                       ; preds = %while_true, %if_true
  %7 = load i32, ptr %b_2, align 4
  %8 = load i32, ptr %i_0, align 4
  %9 = icmp slt i32 %7, %8
  br i1 %9, label %while_true, label %while_false

while_true:                                       ; preds = %while_cond
  %10 = load i32, ptr %b_2, align 4
  %11 = sub i32 %10, -20
  store i32 %11, ptr %b_2, align 4
  br label %while_cond

while_false:                                      ; preds = %while_cond
  %12 = load i32, ptr %b_2, align 4
  %13 = add i32 -10, %12
  store i32 %13, ptr %a_1, align 4
  br label %if_end

if_true1:                                         ; preds = %if_false
  %14 = load i32, ptr %a_1, align 4
  store i32 %14, ptr %b_2, align 4
  br label %if_false2

if_false2:                                        ; preds = %if_true1, %if_false
  br label %if_end

if_end:                                           ; preds = %if_false2, %while_false
  store i32 1, ptr %ret_ref, align 4
  br label %ret
}
