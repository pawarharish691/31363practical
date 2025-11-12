#include <bits/stdc++.h>          // Includes all standard C++ libraries
using namespace std;

// ==================== CLASS: Assembler ====================
class Assembler {
    // ----------------- INSTRUCTION TABLES -----------------
    // IS: Imperative Statements (actual machine instructions)
    map<string, string> IS = {
        {"STOP", "00"}, {"ADD", "01"}, {"SUB", "02"}, {"MULT", "03"},
        {"MOVER", "04"}, {"MOVEM", "05"}, {"COMP", "06"},
        {"BC", "07"}, {"DIV", "08"}, {"READ", "09"}, {"PRINT", "10"}
    };

    // AD: Assembler Directives (handled by assembler, not machine)
    map<string, string> AD = {
        {"START", "01"}, {"END", "02"}, {"ORIGIN", "03"},
        {"EQU", "04"}, {"LTORG", "05"}
    };

    // DL: Declarative Statements (for data definition)
    map<string, string> DL = {
        {"DC", "01"}, {"DS", "02"}
    };

    // REG: Register codes
    map<string, int> REG = {
        {"AREG", 1}, {"BREG", 2}, {"CREG", 3}, {"DREG", 4}
    };

    // ----------------- TABLES FOR PASS-1 -----------------
    map<string, int> SYMTAB;          // Symbol table → label name + address
    map<string, int> LITTAB;          // Literal table → literal name + address
    vector<string> LITERAL_LIST;      // To preserve order of literals
    vector<int> POOLTAB = {0};        // Starting index of each literal pool

    int LOCCTR = 0;                   // Location Counter (tracks memory address)
    int POOL_PTR = 0;                 // Tracks literal pool index

    // ----------------- HELPER: Split Input Line -----------------
    vector<string> split(string line) {
        replace(line.begin(), line.end(), ',', ' ');  // Replace commas with spaces
        stringstream ss(line);
        string word;
        vector<string> tokens;
        while (ss >> word) tokens.push_back(word);    // Split line into tokens
        return tokens;
    }

    // ----------------- HELPER: Assign Literal Addresses -----------------
    void assignLiterals(ofstream &out) {
        for (int i = POOLTAB.back(); i < LITERAL_LIST.size(); i++) {
            string lit = LITERAL_LIST[i];
            if (LITTAB[lit] == -1) {                 // If literal unassigned
                LITTAB[lit] = LOCCTR++;              // Assign current LOCCTR
                string value = lit.substr(2, lit.size() - 3); // ='5' → 5
                // Write this literal to intermediate file
                out << LITTAB[lit] << "\t(DL,01)\t(C," << value << ")\n";
            }
        }
        POOLTAB.push_back(LITERAL_LIST.size());      // Update POOLTAB
    }

public:
    // ----------------- SOURCE CODE STORAGE -----------------
    vector<string> source;
    void readSource(string file) {
        ifstream fin(file);
        string line;
        while (getline(fin, line)) source.push_back(line);  // Read each line
    }

    // ================== PASS-1 STARTS ==================
    void pass1() {
        ofstream out("intermediate.txt");            // Output intermediate file

        for (string line : source) {
            if (line.empty()) continue;
            auto tokens = split(line);               // Break line into tokens

            string label = "", opcode = tokens[0];   // Assume first word = opcode
            vector<string> operands;

            // ---------- If first token is NOT opcode → it’s a label ----------
            if (!IS.count(opcode) && !AD.count(opcode) && !DL.count(opcode)) {
                label = opcode;                      // First word is label
                opcode = tokens[1];                  // Next word is opcode
                for (int i = 2; i < tokens.size(); i++) operands.push_back(tokens[i]);
            } else {
                for (int i = 1; i < tokens.size(); i++) operands.push_back(tokens[i]);
            }

            // ---------- If label exists, store its address ----------
            if (!label.empty()) SYMTAB[label] = LOCCTR;

            // ---------- HANDLE ASSEMBLER DIRECTIVES ----------
            if (AD.count(opcode)) {
                if (opcode == "START") {
                    LOCCTR = stoi(operands[0]);       // Initialize LOCCTR
                    out << LOCCTR << "\t(AD,01)\t(C," << LOCCTR << ")\n";
                } 
                else if (opcode == "END" || opcode == "LTORG") {
                    assignLiterals(out);              // Assign addresses to literals
                    out << LOCCTR << "\t(AD," << AD[opcode] << ")\n";
                    if (opcode == "END") break;       // Stop processing after END
                }
                else if (opcode == "ORIGIN") {
                    LOCCTR = SYMTAB[operands[0]];     // Set LOCCTR to symbol address
                    out << LOCCTR << "\t(AD,03)\t(C," << LOCCTR << ")\n";
                }
                else if (opcode == "EQU") {
                    SYMTAB[label] = SYMTAB[operands[0]]; // Assign same address
                    out << "-\t(AD,04)\t(S," << operands[0] << ")\n";
                }
                continue;
            }

            // ---------- HANDLE DECLARATIVE STATEMENTS ----------
            if (DL.count(opcode)) {
                SYMTAB[label] = LOCCTR;               // Save label in SYMTAB
                out << LOCCTR << "\t(DL," << DL[opcode] << ")\t(C," << operands[0] << ")\n";
                // Increment LOCCTR
                LOCCTR += (opcode == "DS") ? stoi(operands[0]) : 1;
                continue;
            }

            // ---------- HANDLE IMPERATIVE STATEMENTS ----------
            if (IS.count(opcode)) {
                out << LOCCTR << "\t(IS," << IS[opcode] << ")\t";

                // --- REGISTER CODE ---
                int regCode = (operands.size() > 0 && REG.count(operands[0])) ? REG[operands[0]] : 0;
                out << "(R," << regCode << ")\t";

                // --- SYMBOL OR LITERAL ---
                if (operands.size() > 1) {
                    string op = operands[1];
                    if (op[0] == '=') {               // It’s a literal
                        if (!LITTAB.count(op)) {
                            LITTAB[op] = -1;          // Placeholder
                            LITERAL_LIST.push_back(op);
                        }
                        out << "(L," << LITERAL_LIST.size() << ")"; // literal index
                    } else {                          // It’s a symbol
                        if (!SYMTAB.count(op)) SYMTAB[op] = -1;     // Placeholder
                        out << "(S," << op << ")";     // symbol reference
                    }
                }
                out << "\n";
                LOCCTR++;                              // Move to next address
            }
        }

        out.close();                                   // Intermediate file ready
        writeTables();                                 // Generate symbol/literal/pool tables
    }

    // ================== WRITE TABLES ==================
    void writeTables() {
        ofstream s("symtab.txt"), l("littab.txt"), p("pooltab.txt");

        // --- SYMBOL TABLE ---
        s << "Index\tSymbol\tAddress\n";
        int i = 1;
        for (auto &x : SYMTAB) s << i++ << "\t" << x.first << "\t" << x.second << "\n";

        // --- LITERAL TABLE ---
        l << "Index\tLiteral\tAddress\n";
        i = 1;
        for (auto &x : LITERAL_LIST) l << i++ << "\t" << x << "\t" << LITTAB[x] << "\n";

        // --- POOL TABLE ---
        p << "Pool Index\n";
        for (int val : POOLTAB) p << val << "\n";
    }
};

// ================== MAIN FUNCTION ==================
int main() {
    Assembler a;                         // Create Assembler object
    a.readSource("input.txt");           // Read source program from file
    a.pass1();                           // Execute Pass-1
}
