#include "GritVM.hpp"
#include <fstream>
#include <sstream>

STATUS GritVM::load(const std::string filename, const std::vector<long> &initialMemory){

    //opening file given with filename
    std::ifstream file;
    file.open(filename);

    //if file could not be opened, throw and exit load function
    if(!file.is_open()){throw "error reading file"; return machineStatus;}
    std::cout << "file opened successfully" << std::endl;

    //read contents of file into instruction list "instructMem"
    for(std::string str; std::getline(file, str);){//getline() returns false if therer are no more lines
        if(str[0] == '#' || str[0] == '\n' || str[0] == ' ' || str[0] == '\0'){ continue; } //skip comments or empty lines or lines that begin with whitespace
        std::stringstream ss(str);

        std::string operation; //each non comment line has a operation
        long argument = 0;     //possibly followed by an argument
        ss >> operation >> argument; //reading into variables from stream from the getline

        //emplace instruction into the list using "operation" and "argument"
        instructMem.emplace_back(GVMHelper::stringtoInstruction(operation), argument);
    }

    //loading initial memory into dataMem 
    dataMem = initialMemory;

    //close file
    file.close();

    //load completed: set machineStatus to ready and return status
    machineStatus = READY;
    return machineStatus;
}

STATUS GritVM::run(){
    //if no file is loaded, machineStatus is WAITING rather than READY, so running should do nothing
    if(machineStatus == WAITING) return machineStatus;
    machineStatus = RUNNING; //set status to RUNNING
    std::cout << "GritVM has started running..." << std::endl;

    currentInstruct = instructMem.begin(); //set iterator to beginning of list

    while(machineStatus == RUNNING){

        //evaluate the current instruction, and determine the distance to continue in the instructions
        long jumpDistance = evaluate(*currentInstruct);

        //advance the current instruction using the jumpDistance determined by evaluate()
        advance(jumpDistance);
    }

    std::cout << "GritVM has finished running - Now in HALTED status" << std::endl;
    return machineStatus;
}

std::vector<long> GritVM::getDataMem(){ //returns a copy of dataMem vector
    return dataMem;
}

STATUS GritVM::reset(){ //resets GritVM object for another run cycle
    std::cout << "reseting GritVM object" << std::endl;

    //reset accumulator
    accumulator = 0;

    //clear data vector and instruction list
    dataMem.clear();
    instructMem.clear();

    //set machine status to WAITING
    machineStatus = WAITING;
    return machineStatus;
}

long GritVM::evaluate(Instruction instrct){ //evaluates an instruction and returns distance to advance in the program
    long d = 1; //variable for distance to advance in instruction list - default is 1 step

    //evaluate the current instruction with switch statement
    switch(instrct.operation){
        case CLEAR : accumulator = 0;
        std::cout << "operation : CLEAR" << std::endl;
        std::cout << "contents of accumulator after op: " << accumulator << std::endl;
            break;
        case AT : accumulator = dataMem[instrct.argument];
        std::cout << "operation : AT " << instrct.argument << std::endl;
        std::cout << "contents of accumulator after op: " << accumulator << std::endl;
            break;
        case SET : dataMem[instrct.argument] = accumulator;
        std::cout << "operation : SET " << instrct.argument << std::endl;
        std::cout << "contents of accumulator after op: " << accumulator << std::endl;
            break;
        //inserts contents of the accumulator into specified index of dataMem vector
        case INSERT : dataMem.insert(dataMem.begin() + instrct.argument, accumulator);
        std::cout << "operation : INSERT " << instrct.argument << std::endl;
        std::cout << "contents of accumulator after op: " << accumulator << std::endl;    
            break;
        //deletes one entry in the dataMem vector
        case ERASE : dataMem.erase(dataMem.begin() + instrct.argument);
        std::cout << "operation : ERASE" << std::endl;
        std::cout << "contents of accumulator after op: " << accumulator << std::endl;
            break;
        case ADDCONST : accumulator += instrct.argument;
        std::cout << "operation : ADDCONST " << instrct.argument << std::endl;
        std::cout << "contents of accumulator after op: " << accumulator << std::endl;
            break;
        case SUBCONST: accumulator -= instrct.argument;
        std::cout << "operation : SUBCONST " << instrct.argument << std::endl;
        std::cout << "contents of accumulator after op: " << accumulator << std::endl;
            break;
        case MULCONST : accumulator *= instrct.argument;
        std::cout << "operation : MULCONST " << instrct.argument << std::endl;
        std::cout << "contents of accumulator after op: " << accumulator << std::endl;
            break;
        case DIVCONST : accumulator /= instrct.argument;
        std::cout << "operation : DIVCONS T " << instrct.argument << std::endl;
        std::cout << "contents of accumulator after op: " << accumulator << std::endl;
            break;
        case ADDMEM : accumulator += dataMem[instrct.argument];
        std::cout << "operation : ADDMEM " << instrct.argument << std::endl;
        std::cout << "contents of accumulator after op: " << accumulator << std::endl;
            break;
        case SUBMEM : accumulator -= dataMem[instrct.argument];
        std::cout << "operation : SUBMEM " << instrct.argument << std::endl;
        std::cout << "contents of accumulator after op: " << accumulator << std::endl;
            break;
        case MULMEM : accumulator *= dataMem[instrct.argument];
        std::cout << "operation : MULMEM " << instrct.argument << std::endl;
        std::cout << "contents of accumulator after op: " << accumulator << std::endl;
            break;
        case DIVMEM : accumulator /= dataMem[instrct.argument];
        std::cout << "operation : DIVMEM " << instrct.argument << std::endl;
        std::cout << "contents of accumulator after op: " << accumulator << std::endl;
            break;
        //these 3 operations may modify "d" from default value of 1
        case JUMPREL : d = instrct.argument;
        std::cout << "operation : JUMPREL " << instrct.argument << std::endl;
        std::cout << "contents of accumulator after op: " << accumulator << std::endl;
            break;
        case JUMPZERO : if(accumulator == 0){d = instrct.argument;};
        std::cout << "operation : JUMPZERO " << instrct.argument << std::endl;
        std::cout << "contents of accumulator after op: " << accumulator << std::endl;
            break;
        case JUMPNZERO : if(accumulator != 0){d = instrct.argument;};
        std::cout << "operation : JUMPNZERO " << instrct.argument << std::endl;
        std::cout << "contents of accumulator after op: " << accumulator << std::endl;
            break;
        //performs no operation, instruction list still gets iterated by 1
        case NOOP : ;
        std::cout << "operation : NOOP" << std::endl;
        std::cout << "contents of accumulator after op: " << accumulator << std::endl;
            break;
        case HALT : machineStatus = HALTED;
        std::cout << "operation : HALT" << std::endl;
        std::cout << "contents of accumulator after op: " << accumulator << std::endl;
            break;
        case OUTPUT : std::cout << accumulator;
        std::cout << "operation : OUTPUT" << std::endl;
        std::cout << "contents of accumulator after op: " << accumulator << std::endl;
            break;
        //checks if dataMem vector is greater than or equal to specified size
        case CHECKMEM : if((long)dataMem.size() < instrct.argument){ machineStatus = ERRORED; };
        std::cout << "operation : CHECKMEM " << instrct.argument << std::endl;
        std::cout << "contents of accumulator after op: " << accumulator << std::endl;
            break;
        case UNKNOWN_INSTRUCTION : machineStatus = ERRORED;
        std::cout << "operation : UNKNOWN_INSTRUCTION" << std::endl;
        std::cout << "contents of accumulator after op: " << accumulator << std::endl;
            break;

    }

    return d; //return distance to advance in the program
}
void GritVM::advance(long jump){ //advances the program by jump amount via currentInstruct iterator
    std::cout << "  Advancing by: " << jump << std::endl << std::endl;
    std::advance(currentInstruct, jump);
    return;
}

void GritVM::printVM(bool printData, bool printInstruction){ //prints the contents of the data and instruction members of the GritVM object

    //printing status of the vm and contents of the accumulator
    std::cout << "****** Output Dump ******" << std::endl << std::endl;
    std::cout << "Status: " << 
        GVMHelper::statusToString(machineStatus) << std::endl; 
    std::cout << "Accumulator: " << accumulator << std::endl; 

    //printing data memory
    if(printData){
        std::cout << "*** Data Memory ***" << std::endl; 
        int index = 0; 
        std::vector<long>::iterator it = dataMem.begin(); 
        while(it != dataMem.end()) { 
            long item = (*it); 
            std::cout << "Location " << index++ << ": " << item << std::endl;
            it++;
        }
    }

    //printing instruction memory
    if(printInstruction){
        std::cout << "*** Instruction Memory ***" << std::endl; 
        int index = 0; 
        std::list<Instruction>::iterator it = instructMem.begin(); 
        while(it != instructMem.end()) { 
            Instruction item = (*it); 
            std::cout << "Instruction " << index++ << ": " << 
            GVMHelper::instructionToString(item.operation) << 
            " " << item.argument << std::endl; 
            it++;
        }
    }

}