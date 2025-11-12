#include <bits/stdc++.h>           // Includes all standard headers (iostream, vector, string, etc.)
using namespace std;

// =========================== REGISTER MAP ===========================
// A map of registers to their corresponding register numbers.
unordered_map<string, int> regs = {
    {"AREG", 1}, {"BREG", 2}, {"CREG", 3}, {"DREG", 4}
};

// =========================== STRUCTURES =============================

// ---- Macro Name Table (MNT) ----
// Each macro has a name, counts of parameters, pointers to MDT and KPDTAB,
// and a Parameter Name Table (PNTAB).
struct MNT {
    string name;                    // Macro name
    int pp = 0;                     // Positional parameters count
    int kp = 0;                     // Keyword parameters count
    int mdtp = 0;                   // Pointer to Macro Definition Table
    int kpdtp = 0;                  // Pointer to Keyword Parameter Default Table
    vector<string> pntab;           // Parameter Name Table (holds parameter names)
    unordered_map<string, int> pnref; // Map parameter name to its index in PNTAB
};

// ---- Keyword Parameter Default Table (KPDTAB) ----
struct KPDTAB {
    string kpname;                  // Keyword parameter name (e.g., &B)
    string value;                   // Default value (e.g., =5)
};

// ---- Operand Representation ----
struct Operand {
    string param_type = "";         // R = Register, P = Parameter
    int param_ind = 0;              // Register number or parameter index
};

// ---- Macro Definition Table (MDT) ----
// Stores all macro body lines after parameter replacement.
struct MDT {
    vector<string> opcode;          // Operation code (e.g., MOVER, ADD)
    vector<Operand> op1;            // First operand (register or param)
    vector<Operand> op2;            // Second operand (register or param)
};

// =========================== HELPER FUNCTIONS ===========================

// ---- Split function ----
// Splits a given string by a delimiter (default: space)
vector<string> split(string s, char del = ' ') {
    vector<string> splitted;
    stringstream text(s);
    string word;
    while (getline(text, word, del)) {
        if (!word.empty())          // Avoid empty tokens
            splitted.emplace_back(word);
    }
    return splitted;
}

// ---- Operand Parser ----
// Determines if an operand is a register or a parameter,
// and returns an Operand structure accordingly.
Operand getOp(string s, MNT &currmnt) {
    Operand op;
    
    // Case 1: Register (AREG, BREG, etc.)
    if (regs.count(s)) {
        op.param_type = "R";         // Register type
        op.param_ind = regs[s];      // Store register number
    }
    else {                           // Case 2: Parameter (&X)
        if (!s.empty() && s[0] == '&')
            s = s.substr(1);          // Remove '&' symbol
        
        // Look up parameter index in current macro’s PNTAB
        if (currmnt.pnref.count(s)) {
            op.param_type = "P";      // Parameter type
            op.param_ind = currmnt.pnref[s]; // Get parameter index
        }
    }
    return op;
}

// =========================== MAIN FUNCTION ===========================
int main() {
    vector<MNT> mnt;                 // Macro Name Table
    vector<KPDTAB> kpdtab;           // Keyword Parameter Default Table
    MDT mdt;                         // Macro Definition Table

    ifstream file("macro.txt");      // Input source file containing macros
    if (!file.is_open()) {
        cout << "File error";        // Safety check
        return 0;
    }

    string line, line2;
    bool flag = false;               // Indicates if we are inside a macro definition

    // =========================== READ FILE LINE BY LINE ===========================
    while (getline(file, line)) {
        vector<string> statement = split(line, ' '); // Split the current line
        if (statement.empty()) continue;

        // -------- Detect start of a macro definition --------
        if (statement[0] == "MACRO") {
            flag = true;                             // Enable macro processing mode

            getline(file, line2);                    // Read macro header line (prototype)
            vector<string> statement2 = split(line2, ' '); // Example: INCR &A,&B=2

            MNT currmnt;                             // Create a new MNT entry
            currmnt.name = statement2[0];            // Macro name (e.g., INCR)
            currmnt.kpdtp = kpdtab.size();           // Starting index of KPDTAB for this macro
            currmnt.mdtp = mdt.opcode.size();        // Starting index in MDT

            // ---- Parse Parameters ----
            if (statement2.size() > 1) {
                vector<string> ops = split(statement2[1], ','); // Split &A,&B=2
                for (auto op : ops) {
                    if (op[0] == '&')
                        op = op.substr(1);            // Remove '&' for easier storage

                    // Keyword Parameter (contains '=')
                    if (op.find('=') != string::npos) {
                        vector<string> kpv = split(op, '=');
                        currmnt.kp++;                // Increment keyword parameter count
                        currmnt.pntab.push_back(kpv[0]); // Add to PNTAB
                        kpdtab.push_back({kpv[0], kpv[1]}); // Add to KPDTAB
                        currmnt.pnref[kpv[0]] = currmnt.pntab.size(); // Map name→index
                    }
                    // Positional Parameter
                    else {
                        currmnt.pp++;                // Increment positional parameter count
                        currmnt.pntab.push_back(op); // Add to PNTAB
                        currmnt.pnref[op] = currmnt.pntab.size(); // Map name→index
                    }
                }
            }

            // Add the macro entry to the MNT
            mnt.push_back(currmnt);
        }

        // -------- Inside macro body --------
        else if (flag) {
            // Detect end of macro
            if (statement[0] == "MEND") {
                flag = false;                        // End macro processing
                mdt.opcode.push_back("MEND");        // Add MEND to MDT
                mdt.op1.push_back({});               // Empty operand
                mdt.op2.push_back({});               // Empty operand
                continue;
            }

            // Add macro instruction to MDT
            mdt.opcode.push_back(statement[0]);      // Store opcode (e.g., MOVER)
            Operand op1, op2;
            vector<string> ops;

            // Split operands if present (e.g., AREG,&X)
            if (statement.size() > 1)
                ops = split(statement[1], ',');

            // Parse first operand (if exists)
            if (ops.size() >= 1)
                op1 = getOp(ops[0], mnt.back());     // Use helper to identify param/register

            // Parse second operand (if exists)
            if (ops.size() >= 2)
                op2 = getOp(ops[1], mnt.back());

            // Store in MDT
            mdt.op1.push_back(op1);
            mdt.op2.push_back(op2);
        }
    }

    // =========================== PRINT ALL TABLES ===========================

    // --- Print MNT ---
    cout << "MNT (Name PP KP MDTP KPDTP):\n";
    for (auto &m : mnt)
        cout << m.name << " " << m.pp << " " << m.kp << " "
             << m.mdtp << " " << m.kpdtp << endl;

    // --- Print KPDTAB ---
    cout << "\nKPDTAB (Index Name Value):\n";
    int k = 0;
    for (auto &kp : kpdtab)
        cout << k++ << " " << kp.kpname << " " << kp.value << endl;

    // --- Print MDT ---
    cout << "\nMDT (Index Opcode Op1 Op2):\n";
    for (size_t i = 0; i < mdt.opcode.size(); i++) {
        cout << i << " " << mdt.opcode[i] << "\t";

        // Operand 1
        if (mdt.op1[i].param_type != "")
            cout << "(" << mdt.op1[i].param_type << "," << mdt.op1[i].param_ind << ")";
        else
            cout << "-";
        cout << "\t";

        // Operand 2
        if (mdt.op2[i].param_type != "")
            cout << "(" << mdt.op2[i].param_type << "," << mdt.op2[i].param_ind << ")";
        else
            cout << "-";
        cout << endl;
    }
}
