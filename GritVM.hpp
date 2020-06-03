#include <iostream>
#include <list>
#include "GritVMBase.hpp"

class GritVM : public GritVMInterface{
    public:
    GritVM() : dataMem(), instructMem(), currentInstruct(), machineStatus(WAITING), accumulator() {}
    STATUS              load(const std::string filename, const std::vector<long> &initialMemory);
    STATUS              run();
    std::vector<long>   getDataMem();
    STATUS              reset();
    
    //testing functions
    STATUS getStatus(){return machineStatus;}
    void printVM(bool printData, bool printInstruction); //prints contents of the instruction memory and data memory

    private:
    //data members
    std::vector<long> dataMem; 
    std::list<Instruction> instructMem; 
    std::list<Instruction>::iterator currentInstruct; 
    STATUS machineStatus; 
    long accumulator;

    //private functions
    long evaluate(Instruction instrct);//evaluates the current instruction while iterating through instructMem
    void advance(long jump); //moves the position in the instruction list by the proper amount
};