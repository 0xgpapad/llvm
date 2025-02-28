//===- LoongArchInstrFormats.td - LoongArch Instr. Formats -*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Describe LoongArch instructions format
//
//  opcode       - operation code.
//  {r/f}d       - destination operand.
//  {r/f}{j/k/a} - source operand.
//  immN         - immediate data.
//
//===----------------------------------------------------------------------===//

class LAInst<dag outs, dag ins, string asmstr, list<dag> pattern = []>
    : Instruction {
  field bits<32> Inst;
  // SoftFail is a field the disassembler can use to provide a way for
  // instructions to not match without killing the whole decode process. It is
  // mainly used for ARM, but Tablegen expects this field to exist or it fails
  // to build the decode table.
  field bits<32> SoftFail = 0;

  let Namespace = "LoongArch";
  let Size = 4;
  let OutOperandList = outs;
  let InOperandList = ins;
  let AsmString = asmstr;
  let Pattern = pattern;
}

// Pseudo instructions
class Pseudo<dag outs, dag ins, string asmstr> : LAInst<outs, ins, asmstr> {
  let isPseudo = 1;
}

// 2R-type
// <opcode | rj | rd>
class Fmt2R<bits<22> op, dag outs, dag ins, string asmstr,
            list<dag> pattern = []> : LAInst<outs, ins, asmstr, pattern> {
  bits<5> rj;
  bits<5> rd;

  let Inst{31-10} = op;
  let Inst{9-5} = rj;
  let Inst{4-0} = rd;
}

// 3R-type
// <opcode | rk | rj | rd>
// <opcode | fk | fj | fd>
class Fmt3R<bits<17> op, dag outs, dag ins, string asmstr,
            list<dag> pattern = []> : LAInst<outs, ins, asmstr, pattern> {
  bits<5> rk;
  bits<5> rj;
  bits<5> rd;

  let Inst{31-15} = op;
  let Inst{14-10} = rk;
  let Inst{9-5} = rj;
  let Inst{4-0} = rd;
}

class Fmt3FR<bits<17> op, dag outs, dag ins, string asmstr,
             list<dag> pattern = []> : LAInst<outs, ins, asmstr, pattern> {
  bits<5> fk;
  bits<5> fj;
  bits<5> fd;

  let Inst{31-15} = op;
  let Inst{14-10} = fk;
  let Inst{9-5} = fj;
  let Inst{4-0} = fd;
}

// 4R-type
// <opcode | ra | rk | rj | rd>
class Fmt4R<bits<12> op, dag outs, dag ins, string asmstr,
            list<dag> pattern = []> : LAInst<outs, ins, asmstr, pattern> {
  bits<5> ra;
  bits<5> rk;
  bits<5> rj;
  bits<5> rd;

  let Inst{31-20} = op;
  let Inst{19-15} = ra;
  let Inst{14-10} = rk;
  let Inst{9-5} = rj;
  let Inst{4-0} = rd;
}

// 3RI2-type
// <opcode | I2 | rk | rj | rd>
class Fmt3RI2<bits<15> op, dag outs, dag ins, string asmstr,
              list<dag> pattern = []> : LAInst<outs, ins, asmstr, pattern> {
  bits<2> imm2;
  bits<5> rk;
  bits<5> rj;
  bits<5> rd;

  let Inst{31-17} = op;
  let Inst{16-15} = imm2;
  let Inst{14-10} = rk;
  let Inst{9-5} = rj;
  let Inst{4-0} = rd;
}

// 3RI3-type
// <opcode | I3 | rk | rj | rd>
class Fmt3RI3<bits<14> op, dag outs, dag ins, string asmstr,
              list<dag> pattern = []> : LAInst<outs, ins, asmstr, pattern> {
  bits<3> imm3;
  bits<5> rk;
  bits<5> rj;
  bits<5> rd;

  let Inst{31-18} = op;
  let Inst{17-15} = imm3;
  let Inst{14-10} = rk;
  let Inst{9-5} = rj;
  let Inst{4-0} = rd;
}

// 2RI5-type
// <opcode | I5 | rj | rd>
class Fmt2RI5<bits<17> op, dag outs, dag ins, string asmstr,
              list<dag> pattern = []> : LAInst<outs, ins, asmstr, pattern> {
  bits<5> imm5;
  bits<5> rj;
  bits<5> rd;

  let Inst{31-15} = op;
  let Inst{14-10} = imm5;
  let Inst{9-5} = rj;
  let Inst{4-0} = rd;
}

// 2RI6-type
// <opcode | I6 | rj | rd>
class Fmt2RI6<bits<16> op, dag outs, dag ins, string asmstr,
              list<dag> pattern = []> : LAInst<outs, ins, asmstr, pattern> {
  bits<6> imm6;
  bits<5> rj;
  bits<5> rd;

  let Inst{31-16} = op;
  let Inst{15-10} = imm6;
  let Inst{9-5} = rj;
  let Inst{4-0} = rd;
}

// 2RI8-type
// <opcode | I8 | rj | rd>
class Fmt2RI8<bits<14> op, dag outs, dag ins, string asmstr,
              list<dag> pattern = []> : LAInst<outs, ins, asmstr, pattern> {
  bits<8> imm8;
  bits<5> rj;
  bits<5> rd;

  let Inst{31-18} = op;
  let Inst{17-10} = imm8;
  let Inst{9-5} = rj;
  let Inst{4-0} = rd;
}

// 2RI12-type
// <opcode | I12 | rj | rd>
class Fmt2RI12<bits<10> op, dag outs, dag ins, string asmstr,
               list<dag> pattern = []> : LAInst<outs, ins, asmstr, pattern> {
  bits<12> imm12;
  bits<5> rj;
  bits<5> rd;

  let Inst{31-22} = op;
  let Inst{21-10} = imm12;
  let Inst{9-5} = rj;
  let Inst{4-0} = rd;
}

// 2RI14-type
// <opcode | I14 | rj | rd>
class Fmt2RI14<bits<8> op, dag outs, dag ins, string asmstr,
               list<dag> pattern = []> : LAInst<outs, ins, asmstr, pattern> {
  bits<14> imm14;
  bits<5> rj;
  bits<5> rd;

  let Inst{31-24} = op;
  let Inst{23-10} = imm14;
  let Inst{9-5} = rj;
  let Inst{4-0} = rd;
}

// 2RI16-type
// <opcode | I16 | rj | rd>
class Fmt2RI16<bits<6> op, dag outs, dag ins, string asmstr,
               list<dag> pattern = []> : LAInst<outs, ins, asmstr, pattern> {
  bits<16> imm16;
  bits<5> rj;
  bits<5> rd;

  let Inst{31-26} = op;
  let Inst{25-10} = imm16;
  let Inst{9-5} = rj;
  let Inst{4-0} = rd;
}

// 1RI20-type
// <opcode | I20 | rd>
class Fmt1RI20<bits<7> op, dag outs, dag ins, string asmstr,
               list<dag> pattern = []> : LAInst<outs, ins, asmstr, pattern> {
  bits<20> imm20;
  bits<5> rd;

  let Inst{31-25} = op;
  let Inst{24-5} = imm20;
  let Inst{4-0} = rd;
}

// 1RI21-type
// <opcode | I21[15:0] | rj | I21[20:16]>
class Fmt1RI21<bits<6> op, dag outs, dag ins, string asmstr,
               list<dag> pattern = []> : LAInst<outs, ins, asmstr, pattern> {
  bits<21> imm21;
  bits<5> rj;

  let Inst{31-26} = op;
  let Inst{25-10} = imm21{15-0};
  let Inst{9-5} = rj;
  let Inst{4-0} = imm21{20-16};
}

// I15-type
// <opcode | I15>
class FmtI15<bits<17> op, dag outs, dag ins, string asmstr,
             list<dag> pattern = []> : LAInst<outs, ins, asmstr, pattern> {
  bits<15> imm15;

  let Inst{31-15} = op;
  let Inst{14-0} = imm15;
}

// I26-type
// <opcode | I26[15:0] | I26[25:16]>
class FmtI26<bits<6> op, dag outs, dag ins, string asmstr,
             list<dag> pattern = []> : LAInst<outs, ins, asmstr, pattern> {
  bits<26> imm26;

  let Inst{31-26} = op;
  let Inst{25-10} = imm26{15-0};
  let Inst{9-0} = imm26{25-16};
}

// FmtBSTR_W
// <opcode[11:1] | msbw | opcode[0] | lsbw | rj | rd>
class FmtBSTR_W<bits<12> op, dag outs, dag ins, string asmstr,
                list<dag> pattern = []> : LAInst<outs, ins, asmstr, pattern> {
  bits<5> msbw;
  bits<5> lsbw;
  bits<5> rj;
  bits<5> rd;

  let Inst{31-21} = op{11-1};
  let Inst{20-16} = msbw;
  let Inst{15} = op{0};
  let Inst{14-10} = lsbw;
  let Inst{9-5} = rj;
  let Inst{4-0} = rd;
}

// FmtBSTR_D
// <opcode | msbd | lsbd | rj | rd>
class FmtBSTR_D<bits<10> op, dag outs, dag ins, string asmstr,
                list<dag> pattern = []> : LAInst<outs, ins, asmstr, pattern> {
  bits<6> msbd;
  bits<6> lsbd;
  bits<5> rj;
  bits<5> rd;

  let Inst{31-22} = op;
  let Inst{21-16} = msbd;
  let Inst{15-10} = lsbd;
  let Inst{9-5} = rj;
  let Inst{4-0} = rd;
}

// FmtASRT
// <opcode | rk | rj | 0x0>
class FmtASRT<bits<17> op, dag outs, dag ins, string asmstr,
              list<dag> pattern = []> : LAInst<outs, ins, asmstr, pattern> {
  bits<5> rk;
  bits<5> rj;

  let Inst{31-15} = op;
  let Inst{14-10} = rk;
  let Inst{9-5} = rj;
  let Inst{4-0} = 0x0;
}

// FmtPRELD
// < 0b0010101011 | I12 | rj | I5>
class FmtPRELD<dag outs, dag ins, string asmstr,
               list<dag> pattern = []> : LAInst<outs, ins, asmstr, pattern> {
  bits<12> imm12;
  bits<5> rj;
  bits<5> imm5;

  let Inst{31-22} = 0b0010101011;
  let Inst{21-10} = imm12;
  let Inst{9-5} = rj;
  let Inst{4-0} = imm5;
}

// FmtPRELDX
// < 0b00111000001011000 | rk | rj | I5>
class FmtPRELDX<dag outs, dag ins, string asmstr,
                list<dag> pattern = []> : LAInst<outs, ins, asmstr, pattern> {
  bits<5> rk;
  bits<5> rj;
  bits<5> imm5;

  let Inst{31-15} = 0b00111000001011000;
  let Inst{14-10} = rk;
  let Inst{9-5} = rj;
  let Inst{4-0} = imm5;
}
