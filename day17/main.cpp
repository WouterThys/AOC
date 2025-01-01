#include <iostream>
#include <algorithm>
#include <functional>
#include <iterator>

#include <fstream>
#include <sstream>

#include <string>
#include <vector>
#include <queue>
#include <array>
#include <set>

#include <memory>
#include <regex>
#include <cmath>
#include <chrono>
#include <thread>

#define TEST

#ifdef TEST
#define DATA "test.txt"
#define W 17
#define H W
#else
#define DATA "data.txt"
#define W 141
#define H W
#endif

/*
3-bit numbers
3 registers A B C
instructions: 'opcode' and 'operand'
instruction pointer: += 2

*/

struct IComputer 
{
    virtual int regA() const = 0;
    virtual int regB() const = 0;
    virtual int regC() const = 0;

    virtual void setA(int value) = 0;
    virtual void setB(int value) = 0;
    virtual void setC(int value) = 0;
    virtual void out(int value) = 0;

    /* Increment the instruction pointer */
    virtual void inc() = 0;
    virtual void setIPtr(int value) = 0;
};

class Operand 
{
private:
    int value;

public:

    Operand(int value) : value{value} 
    {

    }

    /*
    Combo operands 0 through 3 represent literal values 0 through 3.
    Combo operand 4 represents the value of register A.
    Combo operand 5 represents the value of register B.
    Combo operand 6 represents the value of register C.
    Combo operand 7 is reserved and will not appear in valid programs.
    */

   int getCombo(const IComputer* cmp) const 
   {
        switch (value)
        {
        case 0:
        case 1:
        case 2:
        case 3:
            return value;

        case 4: return cmp->regA();
        case 5: return cmp->regB();
        case 6: return cmp->regC();
        
        default:
            std::cout << "ERROR invalid value" << std::endl;
            break;
        }
        return -1;
   }

    int getLiteral() const 
    {
        return value;
    }

};

struct Instruction 
{
    virtual bool execute(IComputer* cmp, const Operand* operand) = 0;
};

/* 0: Division: A / (2^Oc) -> A */
struct adv : public Instruction 
{
    bool execute(IComputer* cmp, const Operand* operand) override
    {
        int a = cmp->regA();
        a = a >> operand->getCombo(cmp);
        cmp->setA(a);
        cmp->inc();
        return true; 
    }
};

/* 1: Bitwise XOR: B ^ Ol -> B*/
struct bxl : public Instruction 
{
    bool execute(IComputer* cmp, const Operand* operand) override
    {
        int val = cmp->regB() ^ operand->getLiteral();
        cmp->setB(val);
        cmp->inc();
        return true; 
    }
};

/* 2: Mod 8: Oc % 8 -> B*/
struct bst : public Instruction 
{
    bool execute(IComputer* cmp, const Operand* operand) override
    {
        int val = operand->getCombo(cmp) & 7;
        cmp->setB(val);
        cmp->inc();
        return true;  
    }
};

/* 3: nothing if A = 0, else instr.ptr. = Ol. */
struct jnz : public Instruction 
{
    bool execute(IComputer* cmp, const Operand* operand) override
    {
        if (cmp->regA() != 0) 
        {
            cmp->setIPtr(operand->getLiteral());
        }
        else 
        {
            cmp->inc();
        }
        return true;  
    }
};

/* 4: Bitwise XOR: B ^ C -> B */
struct bxc : public Instruction 
{
    bool execute(IComputer* cmp, const Operand* operand) override
    {
        int val = cmp->regB() ^ cmp->regC();
        cmp->setB(val);
        cmp->inc();
        return true;  
    }
};

/* 5: Oc % 8 -> Output */
struct out : public Instruction 
{
    bool execute(IComputer* cmp, const Operand* operand) override
    {
        int val = operand->getCombo(cmp) & 7;
        cmp->out(val);
        cmp->inc();
        return true;  
    }
};

/* 6: Division: A / (2^Oc) -> B */
struct bdv : public Instruction 
{
    bool execute(IComputer* cmp, const Operand* operand) override
    {
        int a = cmp->regA();
        a = a >> operand->getCombo(cmp);
        cmp->setB(a);
        cmp->inc();
        return true; 
    }
};

/* 7: Division: A / (2^Oc) -> C */
struct cdv : public Instruction 
{
    bool execute(IComputer* cmp, const Operand* operand) override
    {
        int a = cmp->regA();
        a = a >> operand->getCombo(cmp);
        cmp->setC(a);
        cmp->inc();
        return true; 
    }
};

/* Full instruction set in order of the opcode */
const std::array<std::unique_ptr<Instruction>, 8> INSTRUCTIONS
{
    std::unique_ptr<Instruction>(new adv()),
    std::unique_ptr<Instruction>(new bxl()),
    std::unique_ptr<Instruction>(new bst()),
    std::unique_ptr<Instruction>(new jnz()),
    std::unique_ptr<Instruction>(new bxc()),
    std::unique_ptr<Instruction>(new out()),
    std::unique_ptr<Instruction>(new bdv()),
    std::unique_ptr<Instruction>(new cdv()),
};

class Computer : public IComputer
{
public:
    Computer() 
    { 

    }

    void init(std::string input) 
    {
        std::istringstream f(input);
        std::string line;    
        
        // Register A:
        std::getline(f, line);
        regA_ = stoi(line.substr(line.find("A: ") + 3));

        // Register B:
        std::getline(f, line);
        regB_ = stoi(line.substr(line.find("B: ") + 3));

        // Register C:
        std::getline(f, line);
        regC_ = stoi(line.substr(line.find("C: ") + 3));

        // Empty line
        std::getline(f, line);

        // Program
        std::getline(f, line);
        std::istringstream prstr(line.substr(line.find("m: ") + 3));
        while (std::getline(prstr, line, ',')) 
        {
            program_.push_back(stoi(line));
        }
        size_ = program_.size();
    }

    bool iterate() 
    {
        if (iPtr_ >= 0 && iPtr_ < size_) 
        {
            auto instruction = INSTRUCTIONS[program_[iPtr_]].get();
            auto operand = Operand(program_[iPtr_+1]);

            auto res = instruction->execute(this, &operand);

            if (regA_ < 0 || regB_ < 0 || regC_ < 0) 
            {
                return false;
            }

            return res;
        }
        return false;
    }

    int recurse(int A, int digit) 
    {
        if (digit < 0) 
        {
            // We have found it!!
            std::cout << "Found solution for A=" << A << std::endl;
            return A;
        }

        // Shift
        A = A << 3;
        int target = program_[digit];

        // Each 3-bit part of A produces 1 output, so look for only these values
        for (int i = 0; i < 8; i++) 
        {
            reset();
            int newA = A + i;
            setA(newA);

            while(iterate()) { }

            int found = output_[0];
            if (found == target) 
            {
                int res = recurse(newA, digit-1);
                if (res >= 0) 
                {
                    return res;
                }
            }
        }

        return -1;
    }

    void print() 
    {
        system("clear");
        std::cout << "A[" << regA_ << "]" << std::endl; 
        std::cout << "B[" << regB_ << "]" << std::endl; 
        std::cout << "C[" << regC_ << "]" << std::endl; 
        std::cout << "P[" << iPtr_ << "]" << std::endl; 
        output(program_);
        std::cout << std::endl;
        if (iPtr_ > 0) 
        {
            std::cout << std::string(iPtr_*2, ' ');
        }
        std::cout << "^" << std::endl;
    }

    void printOutput() 
    {
        print();
        output(output_);
        std::cout << std::endl;
    }

    void output(const std::vector<int>& v) 
    {
        if (v.size() > 0) 
        {
            std::string res;
            int i = 0;
            for (; i < v.size() - 1; i++) 
            {
                res += std::to_string(v[i]) + ",";
            }
            res += std::to_string(v[i]);
            std::cout << res;
        }
    }

    void reset() 
    {
        regA_ = 0;
        regB_ = 0;
        regC_ = 0;
        iPtr_ = 0;
        output_.clear();
        size_ = program_.size();
    }

    int regA() const override { return regA_; }
    int regB() const override { return regB_; }
    int regC() const override { return regC_; }

    void setA(int value) override { regA_ = value; };
    void setB(int value) override { regB_ = value; };
    void setC(int value) override { regC_ = value; };
    void out(int value) override { output_.push_back(value); };

    void inc() override { iPtr_ = iPtr_ + 2; }
    void setIPtr(int value) override { iPtr_ = value; }

    size_t getSize() const { return size_; }

private:

    int regA_{0};
    int regB_{0};
    int regC_{0};

    int iPtr_ {0};

    std::vector<int> program_;
    std::vector<int> output_;
    size_t size_{0};
};

std::unique_ptr<Computer> read_input(std::string filename) 
{
    std::ifstream infile(filename);
    std::stringstream buffer;
    if (infile.good()) 
    {
        buffer << infile.rdbuf();
        auto ptr = std::unique_ptr<Computer>(new Computer());
        ptr->init(buffer.str());
        return std::move(ptr);
    }   
    else 
    {
        std::cout << "Bad input file " << filename << std::endl;
        return nullptr;
    }
}


void part1() 
{
    auto ptr = read_input("../day17/" DATA);
    while(ptr->iterate()) { }
    ptr->printOutput();
}

void part2() 
{
    auto ptr = read_input("../day17/" DATA);
    
    // Start at A = 0
    int a = ptr->recurse(0, ptr->getSize()-1);
    
    std::cout << "A = " << a << std::endl;;
    // for (const auto d : digits) 
    // {
    //     std::cout << d;
    // }
    // std::cout << std::endl;
    
}

int main(int argc, char* argv[]) 
{
    std::cout  << "Hallo day 17" << std::endl;
#ifdef TEST
    std::cout  << "TEST TEST TEST" << std::endl;
#endif

    //part1();
    part2();

    return 0;
}
