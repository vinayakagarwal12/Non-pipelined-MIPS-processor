#include<bits/stdc++.h>
#define int long long 
using namespace std;

string instructions[100];  //storing instructions from instrmem(read from external file)
string datamem[30];        //storing instructions from datamem(read from external file)
int registermem[32]={0};   //Register file 
int num_data = 0,num_instr = 0;  
int ex_out =0; //output from execute stage
int mem_out=0; //output from memory stage
string typeInstr,rs,rt,rd,shamt,immediate,cur_instr;
int imm_value=0;  
int clockCycle=0;
int p =0;//PC p++ =PC+4
int binarytodecimal(string b) //2's complement form(converting binary string to 2's compliment form)
{      
    int l = b.length(); //length of binary string
    int ans = stoi(b.substr(1),0,2); //stoi(str,optional 0,b)converts string str of base b into integer
    if(b[0]=='1') //MSB is 1 i.e negative
        ans += -1*pow(2,l-1);
    return ans;
}
string decToBinary(int n) //converting decimal to binary
{   
    string s= ""; 
    while (n > 0) { 
        s = to_string(n%2) +s; 
        n = n/2;  
    } 
    int l = 32-s.length();
    while(l>0)
    {   
        s = "0" + s;
        l--;
    }
    return s;
}

void memRead() //function for reading from external file datamem
{
    fstream file2;
    string fname = "datamem.txt";
    file2.open(fname.c_str());
    while(file2>>datamem[num_data])
    {
        num_data++;
    }
    file2.close();
}

void memWrite() // writing to memory file
{
    ofstream fout;   
    string line; 
    fout.open("datamem.txt"); 
    int ind=0;
    while (fout) { 
        fout<<datamem[ind]<< endl;
        ind++; 
        if(ind==num_data)
        {
            break;
        }
    }  
    fout.close(); 
}
void readInstr()  // Reading from external file instmem
{   
    fstream file1;
    string filename = "instructionmem2.txt";
    file1.open(filename.c_str());
    while(file1>>instructions[num_instr])
    {
        num_instr++;
    }
    file1.close();
   // p++;
}
void fetchInstr()  //Fetching instruction from intruction mem and incrementing PC
{   
    cur_instr = instructions[p];
    p++;
    //clockCycle++;
}
void decodeInstr() //Decoding instruction i.e extracting opcode,funct,rs,rt etc and setting type of instruction based on funct and opcode
{   
    //clockCycle++;
    string opcode = cur_instr.substr(0,6);
    string funct = cur_instr.substr(26);
    rs = cur_instr.substr(6,5);
    rt = cur_instr.substr(11,5);
    rd = cur_instr.substr(16,5);
    shamt = cur_instr.substr(21,5);
    immediate = cur_instr.substr(16);
    imm_value = binarytodecimal(immediate);
    if(opcode=="000000") //INSTRUCTION IS R-TYPE
    {   
        if(funct=="100000") //FOR ADD
            typeInstr = "ADD";
        else if(funct=="100010") //FOR SUB
            typeInstr= "SUB";
        else if(funct=="000000") //For SLL
            typeInstr="SLL";
        else if(funct=="101010") //SLT
            typeInstr="SLT";
    }
    else // Instruction is I-type
    {  
        if(opcode=="001000")  // For ADDI
            typeInstr = "ADDI";
        else if(opcode=="000100") //For BEQ
            typeInstr = "BEQ";
        else if(opcode=="000101") //FOR BNE
            typeInstr = "BNE";
        else if(opcode=="100011")// For LW
            typeInstr = "LW";
        else if(opcode=="101011") //For SW
            typeInstr = "SW";
    }
}

void executeInstr()  //setting ex_out based on type of instruction
{
    //clockCycle++;
    if(typeInstr=="ADD") //FOR ADD
        ex_out= registermem[stoi(rs,0,2)] + registermem[stoi(rt,0,2)];
    else if(typeInstr=="SUB") //FOR SUB
        ex_out= registermem[stoi(rs,0,2)] - registermem[stoi(rt,0,2)];
    else if(typeInstr=="SLL") //For SLL
        ex_out= registermem[stoi(rt,0,2)]*pow(2,stoi(shamt,0,2));
    else if(typeInstr=="SLT")
    { //SLT
        if(registermem[stoi(rs,0,2)]<registermem[stoi(rt,0,2)])
            ex_out=1;
        else
            ex_out=0;
    }
    else if(typeInstr=="ADDI")  // For ADDI
        ex_out= registermem[stoi(rs,0,2)] + imm_value;
    else if(typeInstr=="BEQ") 
    {
        if(registermem[stoi(rt,0,2)]==registermem[stoi(rs,0,2)])
            p = p + imm_value;
    }
    else if(typeInstr=="BNE")
    { 
        if(registermem[stoi(rt,0,2)]!=registermem[stoi(rs,0,2)])
            p = p + imm_value;
    }
    else if(typeInstr=="LW")// For LW
        ex_out = registermem[stoi(rs,0,2)] + imm_value;
    else if(typeInstr=="SW") //For SW
        ex_out = registermem[stoi(rs,0,2)] + imm_value;
}
void memStage() //For LW and SW .. sets mem_out if required(in case of LW)
{
    //clockCycle++;
    if(typeInstr=="LW") 
        mem_out = binarytodecimal(datamem[ex_out]);
    else if(typeInstr=="SW") //For SW
        datamem[ex_out]= decToBinary(registermem[stoi(rt,0,2)]);
}
void writebackStage() // For R-type and LW instructions
{
    //clockCycle++;
    if((typeInstr=="ADD") || (typeInstr=="SUB")|| (typeInstr=="SLL"))
        registermem[stoi(rd,0,2)]=ex_out;
    else if(typeInstr=="ADDI")
        registermem[stoi(rt,0,2)]=ex_out;
    else if(typeInstr=="LW")
        registermem[stoi(rt,0,2)]=mem_out;
}
int32_t main()
{   
    memRead();
    readInstr();
    cout<<"PC           "<<"Clock Cycle    "<<"s0"<<endl;
    while(p<num_instr)
    {   
        clockCycle++;
        fetchInstr();
        decodeInstr();
        executeInstr();
        memStage();
        writebackStage();
        cout<<4*p<<"             "<<clockCycle<<"           "<<registermem[16]<<endl;
        //cout<<"clock cycle number: "<<clockCycle<<endl;
    }
    memWrite();
    for(int i=7;i<28;i++)
    {
        cout<<i<<" "<<registermem[i]<<"\n";
    }
    cout<<"ans_register : s0 "<<registermem[16]<<"\n";
    return 0;
}
