// RUN: %clang_cc1 %s -fsyntax-only -fsycl-is-device -internal-isystem %S/Inputs -sycl-std=2020 -Wno-sycl-2017-compat -triple spir64 -DTRIGGER_ERROR -verify
// RUN: %clang_cc1 %s -fsyntax-only -ast-dump -fsycl-is-device -internal-isystem %S/Inputs -sycl-std=2017 -Wno-sycl-2017-compat -triple spir64 | FileCheck %s

// The test checks support and functionality of [[intel::max_global_work_dim()]] attribute.

#include "sycl.hpp"

using namespace cl::sycl;
queue q;

#ifndef __SYCL_DEVICE_ONLY__
struct FuncObj {
  [[intel::max_global_work_dim(1)]] // expected-no-diagnostics
  void
  operator()() const {}
};

void foo() {
  q.submit([&](handler &h) {
    h.single_task<class test_kernel1>(FuncObj());
  });
}

#else // __SYCL_DEVICE_ONLY__

[[intel::max_global_work_dim(2)]] void func_do_not_ignore() {}

struct FuncObj {
  [[intel::max_global_work_dim(1)]] void operator()() const {}
};

struct Func {
  // expected-warning@+1 {{unknown attribute 'max_global_work_dim' ignored}}
  [[intelfpga::max_global_work_dim(2)]] void operator()() const {}
};

// No diagnostic is emitted because the arguments match.
[[intel::max_global_work_dim(1)]] void bar();
[[intel::max_global_work_dim(1)]] void bar() {}

// Checking of different argument values.
[[intel::max_global_work_dim(2)]] void baz();  // expected-note {{previous attribute is here}}
[[intel::max_global_work_dim(1)]] void baz();  // expected-warning {{attribute 'max_global_work_dim' is already applied with different arguments}}

struct TRIFuncObj {
  [[intel::max_global_work_dim(0)]] void operator()() const; // expected-note {{previous attribute is here}}
};
[[intel::max_global_work_dim(1)]] void TRIFuncObj::operator()() const {} // expected-warning {{attribute 'max_global_work_dim' is already applied with different arguments}}

// Checks correctness of mutual usage of different work_group_size attributes:
// reqd_work_group_size, max_work_group_size, and max_global_work_dim.
// In case the value of 'max_global_work_dim' attribute equals to 0 we shall
// ensure that if max_work_group_size and reqd_work_group_size attributes exist,
// they hold equal values (1, 1, 1).

struct TRIFuncObjGood1 {
  [[intel::max_global_work_dim(0)]]
  [[intel::max_work_group_size(1, 1, 1)]]
  [[sycl::reqd_work_group_size(1, 1, 1)]] void
  operator()() const {}
};

struct TRIFuncObjGood2 {
  [[intel::max_global_work_dim(3)]]
  [[intel::max_work_group_size(8, 1, 1)]]
  [[sycl::reqd_work_group_size(4, 1, 1)]] void
  operator()() const {}
};

struct TRIFuncObjGood3 {
  [[sycl::reqd_work_group_size(1)]]
  [[intel::max_global_work_dim(0)]] void
  operator()() const {}
};

struct TRIFuncObjGood4 {
  [[sycl::reqd_work_group_size(1, 1, 1)]]
  [[intel::max_global_work_dim(0)]] void
  operator()() const {}
};

struct TRIFuncObjGood5 {
  [[intel::max_work_group_size(1, 1, 1)]]
  [[intel::max_global_work_dim(0)]] void
  operator()() const {}
};

struct TRIFuncObjGood6 {
  [[sycl::reqd_work_group_size(4, 1, 1)]]
  [[intel::max_global_work_dim(3)]] void
  operator()() const {}
};

struct TRIFuncObjGood7 {
  [[sycl::reqd_work_group_size(4, 1, 1)]]
  [[intel::max_global_work_dim(3)]] void
  operator()() const {}
};

struct TRIFuncObjGood8 {
  [[intel::max_work_group_size(8, 1, 1)]]
  [[intel::max_global_work_dim(3)]] void
  operator()() const {}
};

struct TRIFuncObjGood9 {
  [[intel::max_work_group_size(4, 4, 4)]] void // OK
  operator()() const;
};

[[intel::max_global_work_dim(1)]] void TRIFuncObjGood9::operator()() const {}

#ifdef TRIGGER_ERROR
struct TRIFuncObjBad1 {
  [[sycl::reqd_work_group_size(4, 4, 4)]] void // expected-error {{all 'reqd_work_group_size' attribute arguments must be '1' when the 'max_global_work_dim' attribute argument is '0'}}
  operator()() const;
};

[[intel::max_global_work_dim(0)]]
void TRIFuncObjBad1::operator()() const {}

// Checks correctness of mutual usage of different work_group_size attributes:
// reqd_work_group_size, max_work_group_size and max_global_work_dim.
// In case the value of 'max_global_work_dim' attribute equals to 0 we shall
// ensure that if max_work_group_size and reqd_work_group_size attributes exist,
// they hold equal values (1, 1, 1).

struct TRIFuncObjBad2 {
  [[intel::max_global_work_dim(0)]]
  [[intel::max_work_group_size(8, 8, 8)]] // expected-error{{all 'max_work_group_size' attribute arguments must be '1' when the 'max_global_work_dim' attribute argument is '0'}}
  [[sycl::reqd_work_group_size(4, 4, 4)]] // expected-error{{all 'reqd_work_group_size' attribute arguments must be '1' when the 'max_global_work_dim' attribute argument is '0'}}
  void
  operator()() const {}
};

struct TRIFuncObjBad3 {
  [[intel::max_work_group_size(8, 8, 8)]] // expected-error{{all 'max_work_group_size' attribute arguments must be '1' when the 'max_global_work_dim' attribute argument is '0'}}
  [[intel::max_global_work_dim(0)]] void
  operator()() const {}
};

struct TRIFuncObjBad4 {
  [[sycl::reqd_work_group_size(4, 4, 4)]]   // expected-error{{all 'reqd_work_group_size' attribute arguments must be '1' when the 'max_global_work_dim' attribute argument is '0'}}
  [[intel::max_global_work_dim(0)]] void
  operator()() const {}
};

struct TRIFuncObjBad5 {
  [[sycl::reqd_work_group_size(4)]]   // expected-error{{all 'reqd_work_group_size' attribute arguments must be '1' when the 'max_global_work_dim' attribute argument is '0'}}
  [[intel::max_global_work_dim(0)]] void
  operator()() const {}
};

struct TRIFuncObjBad6 {
  [[intel::max_global_work_dim(0)]] void
  operator()() const;
};

[[sycl::reqd_work_group_size(4, 4, 4)]] // expected-error{{all 'reqd_work_group_size' attribute arguments must be '1' when the 'max_global_work_dim' attribute argument is '0'}}
void
TRIFuncObjBad6::operator()() const {}

struct TRIFuncObjBad7 {
  [[intel::max_global_work_dim(0)]] void
  operator()() const;
};

[[sycl::reqd_work_group_size(4, 4, 4)]] // expected-error{{all 'reqd_work_group_size' attribute arguments must be '1' when the 'max_global_work_dim' attribute argument is '0'}}
void
TRIFuncObjBad7::operator()() const {}

struct TRIFuncObjBad8 {
  [[intel::max_global_work_dim(0)]] void
  operator()() const;
};

[[intel::max_work_group_size(4, 4, 4)]] // expected-error{{all 'max_work_group_size' attribute arguments must be '1' when the 'max_global_work_dim' attribute argument is '0'}}
void
TRIFuncObjBad8::operator()() const {}

// Tests for incorrect argument values for Intel FPGA function attributes:
// reqd_work_group_size, max_work_group_size and max_global_work_dim.

struct TRIFuncObjBad9 {
  // expected-error@+1{{'reqd_work_group_size' attribute requires a positive integral compile time constant expression}}
  [[sycl::reqd_work_group_size(-4, 1)]]
  [[intel::max_global_work_dim(0)]] void
  operator()() const {}
};

struct TRIFuncObjBad10 {
  [[intel::max_work_group_size(4, 4, 4.f)]] // expected-error{{integral constant expression must have integral or unscoped enumeration type, not 'float'}}
  [[intel::max_global_work_dim(0)]] void
  operator()() const {}
};

struct TRIFuncObjBad11 {
  [[sycl::reqd_work_group_size(0, 4, 4)]] // expected-error{{'reqd_work_group_size' attribute requires a positive integral compile time constant expression}}
  [[intel::max_global_work_dim(0)]] void
  operator()() const {}
};

struct TRIFuncObjBad12 {
  [[sycl::reqd_work_group_size(4)]]
  [[intel::max_global_work_dim(-2)]] // expected-error{{'max_global_work_dim' attribute requires integer constant between 0 and 3 inclusive}}
  void operator()() const {}
};

struct TRIFuncObjBad13 {
  [[intel::max_work_group_size(4, 4, 4)]]
  [[intel::max_global_work_dim(4.f)]] // expected-error{{integral constant expression must have integral or unscoped enumeration type, not 'float'}}
  void operator()() const {}
};

struct TRIFuncObjBad14 {
  [[intel::max_work_group_size(4, 4, 4)]] void // expected-error{{all 'max_work_group_size' attribute arguments must be '1' when the 'max_global_work_dim' attribute argument is '0'}}
  operator()() const;
};

[[intel::max_global_work_dim(0)]] void TRIFuncObjBad14::operator()() const {}
#endif // TRIGGER_ERROR

int main() {
  q.submit([&](handler &h) {
    // CHECK-LABEL: FunctionDecl {{.*}}test_kernel1
    // CHECK:       SYCLIntelMaxGlobalWorkDimAttr {{.*}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 1
    // CHECK-NEXT:  IntegerLiteral{{.*}}1{{$}}
    h.single_task<class test_kernel1>(FuncObj());

    // CHECK-LABEL: FunctionDecl {{.*}}test_kernel2
    // CHECK:       SYCLIntelMaxGlobalWorkDimAttr {{.*}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 2
    // CHECK-NEXT:  IntegerLiteral{{.*}}2{{$}}
    h.single_task<class test_kernel2>(
        []() [[intel::max_global_work_dim(2)]]{});

    // CHECK-LABEL: FunctionDecl {{.*}}test_kernel3
    // CHECK:       SYCLIntelMaxGlobalWorkDimAttr {{.*}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 2
    // CHECK-NEXT:  IntegerLiteral{{.*}}2{{$}}
    h.single_task<class test_kernel3>(
        []() { func_do_not_ignore(); });

    h.single_task<class test_kernel4>(TRIFuncObjGood1());
    // CHECK-LABEL: FunctionDecl {{.*}}test_kernel4
    // CHECK:       SYCLIntelMaxGlobalWorkDimAttr {{.*}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 0
    // CHECK-NEXT:  IntegerLiteral{{.*}}0{{$}}
    // CHECK:       SYCLIntelMaxWorkGroupSizeAttr {{.*}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 1
    // CHECK-NEXT:  IntegerLiteral{{.*}}1{{$}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 1
    // CHECK-NEXT:  IntegerLiteral{{.*}}1{{$}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 1
    // CHECK-NEXT:  IntegerLiteral{{.*}}1{{$}}
    // CHECK:       ReqdWorkGroupSizeAttr {{.*}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 1
    // CHECK-NEXT:  IntegerLiteral{{.*}}1{{$}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 1
    // CHECK-NEXT:  IntegerLiteral{{.*}}1{{$}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 1
    // CHECK-NEXT:  IntegerLiteral{{.*}}1{{$}}

    h.single_task<class test_kernel5>(TRIFuncObjGood2());
    // CHECK-LABEL: FunctionDecl {{.*}}test_kernel5
    // CHECK:       SYCLIntelMaxGlobalWorkDimAttr {{.*}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 3
    // CHECK-NEXT:  IntegerLiteral{{.*}}3{{$}}
    // CHECK:       SYCLIntelMaxWorkGroupSizeAttr {{.*}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 8
    // CHECK-NEXT:  IntegerLiteral{{.*}}8{{$}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 1
    // CHECK-NEXT:  IntegerLiteral{{.*}}1{{$}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 1
    // CHECK-NEXT:  IntegerLiteral{{.*}}1{{$}}
    // CHECK:       ReqdWorkGroupSizeAttr {{.*}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 4
    // CHECK-NEXT:  IntegerLiteral{{.*}}4{{$}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 1
    // CHECK-NEXT:  IntegerLiteral{{.*}}1{{$}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 1
    // CHECK-NEXT:  IntegerLiteral{{.*}}1{{$}}

    h.single_task<class test_kernel5>(TRIFuncObjGood3());
    // CHECK-LABEL: FunctionDecl {{.*}}test_kernel5
    // CHECK:       ReqdWorkGroupSizeAttr {{.*}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 1
    // CHECK-NEXT:  IntegerLiteral{{.*}}1{{$}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 1
    // CHECK-NEXT:  IntegerLiteral{{.*}}1{{$}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 1
    // CHECK-NEXT:  IntegerLiteral{{.*}}1{{$}}
    // CHECK:       SYCLIntelMaxGlobalWorkDimAttr {{.*}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 0
    // CHECK-NEXT:  IntegerLiteral{{.*}}0{{$}}

    h.single_task<class test_kernel6>(TRIFuncObjGood4());
    // CHECK-LABEL: FunctionDecl {{.*}}test_kernel6
    // CHECK:       ReqdWorkGroupSizeAttr {{.*}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 1
    // CHECK-NEXT:  IntegerLiteral{{.*}}1{{$}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 1
    // CHECK-NEXT:  IntegerLiteral{{.*}}1{{$}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 1
    // CHECK-NEXT:  IntegerLiteral{{.*}}1{{$}}
    // CHECK:       SYCLIntelMaxGlobalWorkDimAttr {{.*}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 0
    // CHECK-NEXT:  IntegerLiteral{{.*}}0{{$}}

    h.single_task<class test_kernel7>(TRIFuncObjGood5());
    // CHECK-LABEL: FunctionDecl {{.*}}test_kernel7
    // CHECK:       SYCLIntelMaxWorkGroupSizeAttr {{.*}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 1
    // CHECK-NEXT:  IntegerLiteral{{.*}}1{{$}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 1
    // CHECK-NEXT:  IntegerLiteral{{.*}}1{{$}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 1
    // CHECK-NEXT:  IntegerLiteral{{.*}}1{{$}}
    // CHECK:       SYCLIntelMaxGlobalWorkDimAttr {{.*}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 0
    // CHECK-NEXT:  IntegerLiteral{{.*}}0{{$}}

    h.single_task<class test_kernel8>(TRIFuncObjGood6());
    // CHECK-LABEL: FunctionDecl {{.*}}test_kernel8
    // CHECK:       ReqdWorkGroupSizeAttr {{.*}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 4
    // CHECK-NEXT:  IntegerLiteral{{.*}}4{{$}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 1
    // CHECK-NEXT:  IntegerLiteral{{.*}}1{{$}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 1
    // CHECK-NEXT:  IntegerLiteral{{.*}}1{{$}}
    // CHECK:       SYCLIntelMaxGlobalWorkDimAttr {{.*}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 3
    // CHECK-NEXT:  IntegerLiteral{{.*}}3{{$}}

    h.single_task<class test_kernel9>(TRIFuncObjGood7());
    // CHECK-LABEL: FunctionDecl {{.*}}test_kernel9
    // CHECK:       ReqdWorkGroupSizeAttr {{.*}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 4
    // CHECK-NEXT:  IntegerLiteral{{.*}}4{{$}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 1
    // CHECK-NEXT:  IntegerLiteral{{.*}}1{{$}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 1
    // CHECK-NEXT:  IntegerLiteral{{.*}}1{{$}}
    // CHECK:       SYCLIntelMaxGlobalWorkDimAttr {{.*}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 3
    // CHECK-NEXT:  IntegerLiteral{{.*}}3{{$}}

    h.single_task<class test_kernel10>(TRIFuncObjGood8());
    // CHECK-LABEL: FunctionDecl {{.*}}test_kernel10
    // CHECK:       SYCLIntelMaxWorkGroupSizeAttr {{.*}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 8
    // CHECK-NEXT:  IntegerLiteral{{.*}}8{{$}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 1
    // CHECK-NEXT:  IntegerLiteral{{.*}}1{{$}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 1
    // CHECK-NEXT:  IntegerLiteral{{.*}}1{{$}}
    // CHECK:       SYCLIntelMaxGlobalWorkDimAttr {{.*}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 3
    // CHECK-NEXT:  IntegerLiteral{{.*}}3{{$}}

    h.single_task<class test_kernel11>(TRIFuncObjGood9());
    // CHECK-LABEL: FunctionDecl {{.*}}test_kernel11
    // CHECK:       SYCLIntelMaxWorkGroupSizeAttr {{.*}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 4
    // CHECK-NEXT:  IntegerLiteral{{.*}}4{{$}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 4
    // CHECK-NEXT:  IntegerLiteral{{.*}}4{{$}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 4
    // CHECK-NEXT:  IntegerLiteral{{.*}}4{{$}}
    // CHECK:       SYCLIntelMaxGlobalWorkDimAttr {{.*}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 1
    // CHECK-NEXT:  IntegerLiteral{{.*}}1{{$}}

#ifdef TRIGGER_ERROR
    h.single_task<class test_kernel12>(TRIFuncObjBad1());
    h.single_task<class test_kernel13>(TRIFuncObjBad2());
#endif // TRIGGER_ERROR

    // Ignore duplicate attribute with same argument value.
    h.single_task<class test_kernell4>(
    // CHECK-LABEL: FunctionDecl {{.*}}test_kernell4 'void ()'
    // CHECK:       SYCLIntelMaxGlobalWorkDimAttr {{.*}}
    // CHECK-NEXT:  ConstantExpr {{.*}} 'int'
    // CHECK-NEXT:  value: Int 3
    // CHECK-NEXT:  IntegerLiteral{{.*}}3{{$}}
        []() [[intel::max_global_work_dim(3),
               intel::max_global_work_dim(3)]]{}); // Ok

#ifdef TRIGGER_ERROR
    [[intel::max_global_work_dim(1)]] int Var = 0; // expected-error{{'max_global_work_dim' attribute only applies to functions}}

    h.single_task<class test_kernel15>(
        []() [[intel::max_global_work_dim(-8)]]{}); // expected-error{{'max_global_work_dim' attribute requires integer constant between 0 and 3 inclusive}}

    h.single_task<class test_kernell6>(
        []() [[intel::max_global_work_dim(3),      // expected-note {{previous attribute is here}}
               intel::max_global_work_dim(2)]]{}); // expected-warning{{attribute 'max_global_work_dim' is already applied with different arguments}}

    h.single_task<class test_kernel17>(TRIFuncObjBad3());
    h.single_task<class test_kernel18>(TRIFuncObjBad4());
    h.single_task<class test_kernel19>(TRIFuncObjBad5());
    h.single_task<class test_kernel20>(TRIFuncObjBad6());
    h.single_task<class test_kernel21>(TRIFuncObjBad7());
    h.single_task<class test_kernel22>(TRIFuncObjBad8());
    h.single_task<class test_kernel23>(TRIFuncObjBad9());
    h.single_task<class test_kernel24>(TRIFuncObjBad10());
    h.single_task<class test_kernel25>(TRIFuncObjBad11());
    h.single_task<class test_kernel26>(TRIFuncObjBad12());
    h.single_task<class test_kernel27>(TRIFuncObjBad13());
    h.single_task<class test_kernel28>(TRIFuncObjBad14());

    h.single_task<class test_kernel28>(
        []() [[intel::max_global_work_dim(4)]] {}); // expected-error{{'max_global_work_dim' attribute requires integer constant between 0 and 3 inclusive}}
#endif // TRIGGER_ERROR
  });
  return 0;
}
#endif // __SYCL_DEVICE_ONLY__
