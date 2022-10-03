def_EHelper(jal) {
    rtl_li(s, ddest, s->snpc); //将snpc（pc+4）的值赋给目的寄存器(通常是返回地址寄存器 ra)，如果是x0寄存器，则什么都不做
    rtl_j(s, cpu.pc + id_src1->imm); //相对地址

//    printf("\n\n0x%x\n0x%x\n0x%x\n\n",cpu.pc,cpu.pc + id_src1->imm,id_src1->imm);

}


//lui + jalr 实现在 32 比特位绝对地址空间范围内的任意位置的跳转
//auipc + jalr  32 比特位 pc 相对寻址范围的任意地址跳转
//可以实现调用返回（只需 ra 作为源寄存器，零寄存器（x0）作为目的寄存器）
def_EHelper(jalr) {
    rtl_li(s, ddest, s->snpc); //把 jump 指令的下一地址（pc + 4）写入寄存器 rd
    //由寄存器 rs1 中的操作数加上 12 比特位的 I 格式的有符号立即数，然后把最小有效位设置为 0 来产生分支目标地址。
    rtl_j(s, (*dsrc1 + id_src2->imm) & 0xfffffffe); 
}

def_EHelper(beq) {

}

def_EHelper(bne) {

}

def_EHelper(blt) {

}

def_EHelper(bge) {

}

def_EHelper(bltu) {

}

def_EHelper(bgeu) {

}














