#include <bits/stdc++.h>          // Includes all standard C++ libraries
using namespace std;

// -------------------- STRUCTURES --------------------
struct Symbol {                  // Structure to hold a symbol's details
    string name;                 // Name of symbol (like label or variable)
    int address;                 // Its assigned memory address
};

struct Literal {                 // Structure to hold literal's details
    string name;                 // Literal name (like =’5’)
    int address;                 // Its assigned address in memory
};

// -------------------- PASS 2 FUNCTION --------------------
void processPass2(vector<Symbol> symtab, vector<Literal> littab, 
                  vector<int> pooltab, vector<string> intermediate) {
    
    ofstream machineCode("machinecode.txt");  // Output file for final machine code

    // Iterate through each line of intermediate code (output of pass-1)
    for (string line : intermediate) {
        stringstream ss(line);                // Used to break line into words
        string LC, classOp, reg, operand;     // LC = Location Counter
        ss >> LC >> classOp;                  // Read first two fields

        // ---------- 1️⃣ Skip Assembler Directives (AD) ----------
        if (classOp.find("AD") != string::npos)
            continue;                         // AD lines don’t generate machine code

        // ---------- 2️⃣ Handle Declarative Statements (DL) ----------
        // DL,01 → Declare Constant (like DC)
        if (classOp.find("DL,01") != string::npos) {
            ss >> operand;                    // Get the constant operand
            string lit = operand;             // Copy to 'lit'
            if (lit[0] == '=') lit = lit.substr(1);  // Remove '=' if it’s a literal
            machineCode << LC << "\t00\t0\t" << lit << endl;  
            // Format: LC  opcode=00  reg=0  operand=value
        }

        // DL,02 → Declare Storage (like DS)
        else if (classOp.find("DL,02") != string::npos) {
            machineCode << LC << "\t--\t--\t--" << endl;  
            // DS just reserves space → no real machine instruction
        }

        // ---------- 3️⃣ Handle Imperative Statements (IS) ----------
        else if (classOp.find("IS") != string::npos) {
            ss >> reg >> operand;             // Example: (IS,04) 1 (L,1)
            string opcode = classOp.substr(3); // Extract '04' from (IS,04)
            string opfield = "000";           // Default operand field

            // Case 1: No operand
            if (operand == "-" || operand.empty()) {
                opfield = "000";
            }
            // Case 2: Operand refers to literal (L, index)
            else if (operand.find("L,") != string::npos) {
                int idx = stoi(operand.substr(2)) - 1;  // Get literal index
                opfield = to_string(littab[idx].address); // Get its address
            }
            // Case 3: Operand refers to symbol (S, index)
            else if (operand.find("S,") != string::npos) {
                int idx = stoi(operand.substr(2)) - 1;  // Get symbol index
                opfield = to_string(symtab[idx].address); // Get its address
            }

            // Write final machine code line: LC opcode reg operand-address
            machineCode << LC << "\t" << opcode << "\t" << reg << "\t" << opfield << endl;
        }

        // ---------- 4️⃣ Handle Invalid Lines ----------
        else {
            machineCode << LC << "\t--\t--\t--\t; Invalid or unsupported line\n";
        }
    }

    machineCode.close();                       // Close output file
    cout << "✅ PASS-II completed. Check machinecode.txt\n"; // Confirmation message
}

// -------------------- MAIN FUNCTION --------------------
int main() {
    // ---------------- SYMBOL TABLE ----------------
    vector<Symbol> symtab = {                  // Hardcoded symbols and their addresses
        {"L1", 205},
        {"NEXT", 209},
        {"BACK", 213},
        {"X", 218}
    };

    // ---------------- LITERAL TABLE ----------------
    vector<Literal> littab = {                 // Hardcoded literals and addresses
        {"=5", 202},
        {"=2", 206},
        {"=1", 210},
        {"=2", 211},
        {"=4", 217}
    };

    // ---------------- POOL TABLE ----------------
    vector<int> pooltab = {1, 3, 5};           // Starting index of each literal pool
    // Pool 1 → literals 1 & 2
    // Pool 2 → literals 3 & 4
    // Pool 3 → literal 5

    // ---------------- INTERMEDIATE CODE (from PASS-I) ----------------
    vector<string> intermediate = {            // Each line represents Pass-I output
        "201 (AD,01) (C,201)",                // START 201
        "201 (IS,04) 1 (L,1)",                // MOVEM 1, =5
        "202 (IS,05) 1 (S,4)",                // ADD 1, X
        "203 (IS,04) 2 (L,2)",                // MOVEM 2, =2
        "204 (AD,03) (S,1)+3",                // ORIGIN L1+3
        "207 (AD,05)",                        // LTORG
        "208 (IS,01) 1 (L,3)",                // STOP
        "209 (IS,02) 2 (L,4)",
        "210 (IS,07) 4 (S,3)",
        "211 (AD,05)",
        "212 (AD,04) (S,1)",
        "213 (AD,03) (S,2)+5",
        "214 (IS,03) 3 (L,5)",
        "215 (IS,00)",
        "216 (DL,02) 1",                      // DS 1
        "217 (AD,02)"                         // END
    };

    // ---------------- RUN PASS-II ----------------
    processPass2(symtab, littab, pooltab, intermediate);

    return 0;
}
