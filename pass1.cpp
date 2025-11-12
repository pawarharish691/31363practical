#include <bits/stdc++.h>
using namespace std;

// ---------------- STRUCTURES ----------------
struct Symbol {
    string name;
    int address;
};

struct Literal {
    string name;
    int address;
};

// ---------------- FUNCTION: Process Pass-II ----------------
void processPass2(vector<Symbol> symtab, vector<Literal> littab,
                  vector<int> pooltab, vector<string> intermediate) {
    ofstream machineCode("machinecode.txt");

    for (string line : intermediate) {
        if (line.empty()) continue; // skip empty lines

        stringstream ss(line);
        string LC, classOp, reg, operand;
        ss >> LC >> classOp;

        // ----------- 1️⃣ Skip Assembler Directives (AD) -----------
        if (classOp.find("AD") != string::npos) {
            // Handle ORIGIN or EQU if needed (address update only)
            if (classOp.find("AD,03") != string::npos) {
                // Example: (S,1)+3 or (S,2)+5
                string expr;
                ss >> expr;
                if (expr.find("+") != string::npos) {
                    size_t plusPos = expr.find('+');
                    string symPart = expr.substr(0, plusPos);
                    string offsetPart = expr.substr(plusPos + 1);
                    if (symPart.find("S,") != string::npos) {
                        int symIndex = stoi(symPart.substr(2)) - 1;
                        int offset = stoi(offsetPart);
                        int newAddr = symtab[symIndex].address + offset;
                        machineCode << LC << "\t(AD,03)\t(C," << newAddr << ")\n";
                    }
                }
            }
            continue;
        }

        // ----------- 2️⃣ Handle Declarative Statements (DL) -----------
        if (classOp.find("DL,01") != string::npos) {
            // Declare Constant (DC)
            ss >> operand;
            string lit = operand;
            if (!lit.empty() && lit[0] == '=') lit = lit.substr(1); // remove '='
            if (!lit.empty() && lit[0] == '\'') lit = lit.substr(1, lit.size() - 2); // remove quotes
            machineCode << LC << "\t00\t0\t" << lit << endl;
        } 
        else if (classOp.find("DL,02") != string::npos) {
            // Declare Storage (DS)
            machineCode << LC << "\t--\t--\t--" << endl;
        }

        // ----------- 3️⃣ Handle Imperative Statements (IS) -----------
        else if (classOp.find("IS") != string::npos) {
            ss >> reg >> operand;
            string opcode = classOp.substr(3);  // e.g. "04"
            string opfield = "000";             // default operand

            // Case 1: Operand is empty
            if (operand.empty() || operand == "-") {
                opfield = "000";
            }
            // Case 2: Operand refers to a Literal (L,index)
            else if (operand.find("L,") != string::npos) {
                string clean = operand.substr(2);
                int idx = 0;
                try { idx = stoi(clean) - 1; } catch (...) { idx = -1; }
                if (idx >= 0 && idx < littab.size())
                    opfield = to_string(littab[idx].address);
            }
            // Case 3: Operand refers to a Symbol (S,index)
            else if (operand.find("S,") != string::npos) {
                string clean = operand.substr(2);
                int idx = 0;
                try { idx = stoi(clean) - 1; } catch (...) { idx = -1; }
                if (idx >= 0 && idx < symtab.size())
                    opfield = to_string(symtab[idx].address);
            }
            // Case 4: Operand is constant
            else if (operand.find("C,") != string::npos) {
                opfield = operand.substr(3, operand.size() - 4); // extract constant value
            }

            // Output: LC  OPCODE  REG  OPERAND_ADDRESS
            machineCode << LC << "\t" << opcode << "\t" << reg << "\t" << opfield << endl;
        }

        // ----------- 4️⃣ Handle Invalid Lines -----------
        else {
            machineCode << LC << "\t--\t--\t--\t; Unsupported or invalid\n";
        }
    }

    machineCode.close();
    cout << "✅ PASS-II completed successfully. Check 'machinecode.txt'.\n";
}

// ---------------- MAIN FUNCTION ----------------
int main() {
    // ---------------- SYMBOL TABLE ----------------
    vector<Symbol> symtab = {
        {"L1", 205},
        {"NEXT", 209},
        {"BACK", 213},
        {"X", 218}
    };

    // ---------------- LITERAL TABLE ----------------
    vector<Literal> littab = {
        {"=5", 202},
        {"=2", 206},
        {"=1", 210},
        {"=2", 211},
        {"=4", 217}
    };

    // ---------------- POOL TABLE ----------------
    vector<int> pooltab = {1, 3, 5};

    // ---------------- INTERMEDIATE CODE ----------------
    vector<string> intermediate = {
        "201 (AD,01) (C,201)",
        "201 (IS,04) 1 (L,1)",
        "202 (IS,05) 1 (S,4)",
        "203 (IS,04) 2 (L,2)",
        "204 (AD,03) (S,1)+3",
        "207 (AD,05)",
        "208 (IS,01) 1 (L,3)",
        "209 (IS,02) 2 (L,4)",
        "210 (IS,07) 4 (S,3)",
        "211 (AD,05)",
        "212 (AD,04) (S,1)",
        "213 (AD,03) (S,2)+5",
        "214 (IS,03) 3 (L,5)",
        "215 (IS,00)",
        "216 (DL,02) 1",
        "217 (AD,02)"
    };

    // ---------------- RUN PASS-II ----------------
    processPass2(symtab, littab, pooltab, intermediate);

    return 0;
}
