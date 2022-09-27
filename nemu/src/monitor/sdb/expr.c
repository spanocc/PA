#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {       //256 is behind ASCII        //TK_PTR represent pointer
             //  TK_REG代表寄存器$    TK_HEX代表十六进制0x/0X   TK_NEG代表负号
  TK_NOTYPE = 256, TK_EQ, TK_NUM, TK_PTR, TK_AND,  TK_REG, TK_HEX, TK_NEG


  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules. 因为是无符号数，所以负数-1与4294967295相等，但1+ -1 ==0 和 --1==1仍成立
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},     // spaces
  {"\\+", '+'},          // plus    前面要用两个\,不然会编译报错
  {"==", TK_EQ},         // equal ==
  {"&&", TK_AND},        // &&
  {"0[xX][0-9a-fA-F]+", TK_HEX},     //十六进制匹配要在数字前面，防止数字匹配
  {"\\-", '-'},          // sub
  {"\\*", '*'},          // multiply 
  {"\\/", '/'},          // div
  {"[0-9]{1,}", TK_NUM} ,// num
  {"\\(", '('},          // (
  {"\\)", ')'},          // )
  {"\\u", 'u'},           //匹配表达式生成器中的u无符号后缀
  {"\\$[$0-9a-z]+", TK_REG}        
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[65536] __attribute__((used)) = {}; //__attribute__((used))通知编译器在目标文件中保留一个静态函数，即使它没有被引用
static int nr_token __attribute__((used))  = 0;

word_t isa_reg_str2val(const char *s, bool *success);
uint8_t* guest_to_host(paddr_t paddr);   
//小端序
static uint32_t fetch_memory(paddr_t adr) {
    uint32_t result = 0;
    for(int i = 0; i < 4; ++i) {
        result += (*guest_to_host(adr+i))<<(i*8);
    }
    return result;
}


// [p,q)  紫书354页算法
static uint32_t eval(int p, int q) {  printf("%d   %d\n",p,q); 
    assert(p < q);
    if(p == q-1) {
        //assert(tokens[p].type == TK_NUM);
        uint32_t ret = 0;
        bool success = 0;
        switch(tokens[p].type) {
            case TK_NUM: return atoi(tokens[p].str);
            case TK_REG:
                ret = isa_reg_str2val(tokens[p].str+1, &success);
                if(success == true) return ret;
                printf("can't find %s!\n'",tokens[p].str);
                assert(0);
            case TK_HEX: sscanf(tokens[p].str,"%x",&ret); return ret;
        }
    }


    int divl = 0; //左括号减右括号的数
    int sym0 = -1; // sym0: "&&" "==" 优先级比加减还低
    int sym1 = -1, sym2 = -1; //sym1:'+','-'  sym2: '*','/'
    int sym3 = -1; ////负号和解引用，优先级最高，最后处理
    for(int i = p; i < q; ++i) {
        switch(tokens[i].type) {
            case '(': ++divl; break;
            case ')': --divl; assert(divl >= 0); break;
            case '-': case '+': if(!divl) sym1 = i; break;
            case '*': case '/': if(!divl) sym2 = i; break;
            case TK_AND: case TK_EQ: if(!divl) sym0 = i; break;
            //和其他三种情况不一样，负号和解引用 谁在前面 谁后计算 选第一个作为主运算符，最后计算    
            case TK_NEG: case TK_PTR: if(!divl && sym3 == -1) sym3 = i; break; 
            default: break;
        }
    }
    uint32_t l, r;
    if(sym0 < 0) {
        if(sym1 < 0) {
            if(sym2 < 0){
                if(sym3 < 0) {
                    assert(tokens[p].type == '(' && tokens[q-1].type == ')');
                    return eval(p+1, q-1);
                }else sym0 = sym3;//只剩下解引用和负号，其余都在括号内
            }
            else sym0 = sym2; //加减号都在括号内
        }
        else sym0 = sym1; //等号都在括号内
    }
                                                         printf("sym0: %d %s\n",sym0,tokens[sym0].str);
    l = eval(p, sym0);
    r = eval(sym0+1, q);
    switch(tokens[sym0].type) {
        case '+': return l+r;
        case '-': return l-r;
        case '*': return l*r;
        case '/': assert(r); return l/r;
        case TK_EQ: return l==r;
        case TK_AND: return l&&r;
        case TK_NEG: return -1 * eval(p+1, q);
        case TK_PTR: return fetch_memory(eval(p+1, q));
            
        default: assert(0);
    }
    return 0;
}


static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;
        
        //寄存器和十六进制作为整体匹配
        //负号和解引用作为单个符号匹配
       switch (rules[i].token_type) {
            case '-':
                if(nr_token == 0 || (tokens[nr_token-1].type != ')' && tokens[nr_token-1].type != TK_NUM && tokens[nr_token-1].type != TK_REG &&
                              tokens[nr_token-1].type != TK_HEX )) {
                    tokens[nr_token].type = TK_NEG;
                }else tokens[nr_token].type = '-';
                break;
            case '*':
                if(nr_token == 0 || (tokens[nr_token-1].type != ')' && tokens[nr_token-1].type != TK_NUM && tokens[nr_token-1].type != TK_REG &&
                              tokens[nr_token-1].type != TK_HEX )) {
                     tokens[nr_token].type = TK_PTR;
                }else tokens[nr_token].type = '*';
                break;
            //case TK_REG: tokens[nr_token].type = TK_REG; break;
            //case TK_HEX: tokens[nr_token].type = TK_HEX; break;
            case TK_NOTYPE: case 'u': break;
            default: 
                tokens[nr_token].type = rules[i].token_type;
                break;
               
       }

       if(rules[i].token_type == 'u' || rules[i].token_type == TK_NOTYPE) break;

       if(substr_len > 31) { //assert(substr_len <= 31);
            printf("The token is too long, please make sure the length of the token is less than 31\n");
            return false;
       }
       strncpy(tokens[nr_token].str, substr_start, substr_len);
       tokens[nr_token].str[substr_len] = '\0';                    //printf("%s\n",tokens[nr_token].str);
       ++nr_token;


/*        
        if(rules[i].token_type != TK_NOTYPE && rules[i].token_type != 'u') { 
            tokens[nr_token].type = rules[i].token_type; 
            if(substr_len > 31) { //assert(substr_len <= 31);
                printf("The token is too long, please make sure the length of the token is less than 31\n");
                return false;
            }
            strncpy(tokens[nr_token].str, substr_start, substr_len);   
            tokens[nr_token].str[substr_len] = '\0';                    //printf("%s\n",tokens[nr_token].str);
            ++nr_token;
        }
*/ 

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
/*
        switch (rules[i].token_type) {
          default: TODO();
        }
*/
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}


word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  /* for(int i = 0; i < nr_token; ++i) {
        printf("%s   %d\n",tokens[i].str,tokens[i].type);
   }assert(0);*/

  uint32_t ans = eval(0,nr_token);
  printf("%u\n",ans);
  assert(0);
  TODO();

  return ans;
}
