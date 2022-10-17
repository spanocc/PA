#ifdef CONFIG_FTRACE
enum func_type { CALL_TYPE, RET_TYPE } ;
void ftrace_display();
#endif


def_EHelper(jal) {
    rtl_li(s, ddest, s->snpc); //将snpc（pc+4）的值赋给目的寄存器(通常是返回地址寄存器 ra)，如果是x0寄存器，则什么都不做
    rtl_j(s, cpu.pc + id_src1->imm); //相对地址
        
//    printf("\n\n0x%x\n0x%x\n0x%x\n\n",cpu.pc,cpu.pc + id_src1->imm,id_src1->imm);

#ifdef CONFIG_FTRACE
    ftrace_display(cpu.pc + id_src1->imm, CALL_TYPE);
#endif

}


//lui + jalr 实现在 32 比特位绝对地址空间范围内的任意位置的跳转
//auipc + jalr  32 比特位 pc 相对寻址范围的任意地址跳转
//可以实现调用返回（只需 ra 作为源寄存器，零寄存器（x0）作为目的寄存器）
def_EHelper(jalr) {
    uint32_t addr = (*dsrc1 + id_src2->imm) & 0xfffffffe;  //有可能目的寄存器和源寄存器是一个，所以要把地址先存下来
    rtl_j(s, addr); //先更新pc，有可能目的寄存器和源寄存器是一个，所以要把地址先存下来
    rtl_li(s, ddest, s->snpc); //把 jump 指令的下一地址（pc + 4）写入寄存器 rd
    //由寄存器 rs1 中的操作数加上 12 比特位的 I 格式的有符号立即数，然后把最小有效位设置为 0 来产生分支目标地址。

#ifdef CONFIG_FTRACE 
    if(dsrc1 == &gpr(1) && id_src2->imm == 0) { //ret
        ftrace_display(cpu.pc, RET_TYPE);
    }
    else {   //printf("64646545"); 
        ftrace_display(addr, CALL_TYPE);
    }
#endif

}

def_EHelper(beq) {
    rtl_jrelop(s, RELOP_EQ, dsrc1, dsrc2, cpu.pc + id_dest->imm);
}

def_EHelper(bne) {
    rtl_jrelop(s, RELOP_NE, dsrc1, dsrc2, cpu.pc + id_dest->imm);
}

def_EHelper(blt) {
    rtl_jrelop(s, RELOP_LT, dsrc1, dsrc2, cpu.pc + id_dest->imm);
}

def_EHelper(bge) {
    rtl_jrelop(s, RELOP_GE, dsrc1, dsrc2, cpu.pc + id_dest->imm);
}

def_EHelper(bltu) {
    rtl_jrelop(s, RELOP_LTU, dsrc1, dsrc2, cpu.pc + id_dest->imm);
}

def_EHelper(bgeu) {
    rtl_jrelop(s, RELOP_GEU, dsrc1, dsrc2, cpu.pc + id_dest->imm);
}














