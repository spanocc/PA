def_EHelper(lui) {
    rtl_li(s, ddest, id_src1->imm);
}

def_EHelper(auipc) {                   //将立即数的高位和(没加4的)pc相加 
    rtl_li(s, ddest, id_src1->imm);    //先把立即数赋值到目的寄存器
    rtl_addi(s, ddest, ddest, cpu.pc); //dest = dest + pc
}


//没有slt和sltu 
def_EHelper(add) {
    rtl_add(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sub) {
    rtl_sub(s, ddest, dsrc1, dsrc2);
}

def_EHelper(and) {
    rtl_and(s, ddest, dsrc1, dsrc2);
}

def_EHelper(or) {
    rtl_or(s, ddest, dsrc1, dsrc2);
}

def_EHelper(xor) {
    rtl_xor(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sll){
    rtl_sll(s, ddest, dsrc1, dsrc2);
}

def_EHelper(srl) {
    rtl_srl(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sra) {
    rtl_sra(s, ddest, dsrc1, dsrc2);
}

def_EHelper(slt) {
    rtl_setrelop(s, RELOP_LT, ddest, dsrc1, dsrc2);
}




def_EHelper(mul) {
    rtl_mulu_lo(s, ddest, dsrc1, dsrc2);
}

def_EHelper(divq) {                      
/*
printf("%d  ",*ddest);
printf("%d  ",*dsrc1);
printf("%d\n",*dsrc2);

*/
    rtl_divs_q(s, ddest, dsrc1, dsrc2);
}

def_EHelper(divu) {          
    rtl_divu_q(s, ddest, dsrc1, dsrc2);
}

def_EHelper(rem) {
    rtl_divs_r(s, ddest, dsrc1, dsrc2);
}

def_EHelper(mulh) { 
    rtl_muls_hi(s, ddest, dsrc1, dsrc2);
}

def_EHelper(mulhu) { 
    rtl_mulu_hi(s, ddest, dsrc1, dsrc2);
}

def_EHelper(remu) {
    rtl_divu_r(s, ddest, dsrc1, dsrc2);
}





def_EHelper(sltu) {
    rtl_setrelop(s, RELOP_LTU, ddest, dsrc1, dsrc2);
}

def_EHelper(addi) {
    rtl_addi(s, ddest, dsrc1, id_src2->imm);
}

//没有subi，顺便定义一下吧
def_EHelper(subi) {
    rtl_subi(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(andi) {
    rtl_andi(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(ori) {
    rtl_ori(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(xori) {
    rtl_xori(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(slli) {
    rtl_slli(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(srli) {
    rtl_srli(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(srai) {
    rtl_srai(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(slti) {
    rtl_setrelopi(s, RELOP_LT, ddest, dsrc1, id_src2->imm);
}

def_EHelper(sltiu) {
    rtl_setrelopi(s, RELOP_LTU, ddest, dsrc1, id_src2->imm);
}


