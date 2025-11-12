#include <bits/stdc++.h>
using namespace std;

// --- DATA STRUCTURES ---

unordered_map<string, int> regs = {{"AREG", 1}, {"BREG", 2}, {"CREG", 3}, {"DREG", 4}};
string reg_names[] = {"", "AREG", "BREG", "CREG", "DREG"}; // Reverse lookup for printing

struct MNT
{
    string name;
    int pp = 0, kp = 0, mdtp = 0, kpdtp = 0;
    vector<string> pntab;
    unordered_map<string, int> pnref; // Maps param name -> index
};

struct KPDTAB
{
    string kpname;
    string value;
};

struct Operand
{
    string param_type = ""; // "R"=Reg, "P"=Param, "C"=Const
    int param_ind = 0;
    string literal = "";
};

struct MDT
{
    vector<string> opcode;
    vector<Operand> op1;
    vector<Operand> op2;
};

// --- HELPERS ---

vector<string> split(string s, char del = ' ')
{
    vector<string> splitted;
    stringstream text(s);
    string word;
    while (getline(text, word, del))
    {
        if (!word.empty())
            splitted.emplace_back(word);
    }
    return splitted;
}

Operand getOp(string s, MNT &currmnt)
{
    Operand op;
    if (regs.count(s))
    {
        op.param_type = "R";
        op.param_ind = regs[s];
    }
    else
    {
        string temp = s;
        if (temp[0] == '&')
            temp = temp.substr(1);

        if (currmnt.pnref.count(temp))
        {
            op.param_type = "P";
            op.param_ind = currmnt.pnref[temp];
        }
        else
        {
            op.param_type = "C"; // Constant/Label
            op.literal = s;
        }
    }
    return op;
}

// --- MAIN ---

int main()
{
    vector<MNT> mnt;
    vector<KPDTAB> kpdtab;
    MDT mdt;

    // ================= PASS 1: POPULATE TABLES =================
    ifstream file("macro.txt");
    if (!file.is_open())
    {
        cout << "Error: macro.txt missing";
        return 0;
    }

    string line, line2;
    bool flag = false;

    while (getline(file, line))
    {
        vector<string> statement = split(line, ' ');
        if (statement.empty())
            continue;

        if (statement[0] == "MACRO")
        {
            flag = true;
            getline(file, line2);
            vector<string> statement2 = split(line2, ' ');

            MNT currmnt;
            currmnt.name = statement2[0];
            currmnt.kpdtp = kpdtab.size();
            currmnt.mdtp = mdt.opcode.size();

            if (statement2.size() > 1)
            {
                vector<string> ops = split(statement2[1], ',');
                for (auto op : ops)
                {
                    if (op[0] == '&')
                        op = op.substr(1);
                    if (op.find('=') != string::npos)
                    {
                        vector<string> kpv = split(op, '=');
                        currmnt.kp++;
                        currmnt.pntab.push_back(kpv[0]);
                        kpdtab.push_back({kpv[0], kpv[1]});
                        currmnt.pnref[kpv[0]] = currmnt.pntab.size() - 1; // 0-based
                    }
                    else
                    {
                        currmnt.pp++;
                        currmnt.pntab.push_back(op);
                        currmnt.pnref[op] = currmnt.pntab.size() - 1;
                    }
                }
            }
            mnt.push_back(currmnt);
        }
        else if (flag)
        {
            if (statement[0] == "MEND")
            {
                flag = false;
                mdt.opcode.push_back("MEND");
                mdt.op1.push_back({});
                mdt.op2.push_back({});
                continue;
            }
            mdt.opcode.push_back(statement[0]);
            vector<string> ops;
            if (statement.size() > 1)
                ops = split(statement[1], ',');
            mdt.op1.push_back(ops.size() >= 1 ? getOp(ops[0], mnt.back()) : Operand());
            mdt.op2.push_back(ops.size() >= 2 ? getOp(ops[1], mnt.back()) : Operand());
        }
    }

    // ================= PASS 2: EXPANSION =================

    cout << "--- PASS 2 OUTPUT ---" << endl;

    // Reset file reading
    file.clear();
    file.seekg(0);

    bool inside_def = false;

    while (getline(file, line))
    {
        vector<string> stmt = split(line, ' ');
        if (stmt.empty())
            continue;

        // 1. Skip Macro Definitions
        if (stmt[0] == "MACRO")
        {
            inside_def = true;
            continue;
        }
        if (inside_def)
        {
            if (stmt[0] == "MEND")
                inside_def = false;
            continue;
        }

        // 2. Check if it's a Macro Call
        int mnt_idx = -1;
        for (size_t i = 0; i < mnt.size(); i++)
        {
            if (mnt[i].name == stmt[0])
            {
                mnt_idx = i;
                break;
            }
        }

        if (mnt_idx != -1)
        {
            // --- EXPAND MACRO ---
            MNT &m = mnt[mnt_idx];
            vector<string> ala(m.pntab.size(), "");

            // A. Fill Defaults from KPDTAB
            // The macro's defaults are stored from m.kpdtp to m.kpdtp + m.kp
            int kp_count = 0;
            for (int i = m.kpdtp; i < m.kpdtp + m.kp; i++)
            {
                string pname = kpdtab[i].kpname;
                string pval = kpdtab[i].value;
                ala[m.pnref[pname]] = pval;
            }

            // B. Process Call Arguments
            vector<string> args;
            if (stmt.size() > 1)
                args = split(stmt[1], ',');

            int pp_counter = 0; // Positional counter

            for (auto arg : args)
            {
                // Check for Keyword Argument (contains '=')
                if (arg.find('=') != string::npos)
                {
                    vector<string> kv = split(arg, '=');
                    string key = kv[0];
                    if (key[0] == '&')
                        key = key.substr(1); // Safety cleanup
                    string val = kv[1];
                    ala[m.pnref[key]] = val;
                }
                else
                {
                    // Positional Argument
                    if (pp_counter < m.pp)
                    {
                        ala[pp_counter] = arg;
                        pp_counter++;
                    }
                }
            }

            // C. Execute from MDT
            int ptr = m.mdtp;
            while (true)
            {
                string op = mdt.opcode[ptr];
                if (op == "MEND")
                    break;

                cout << "+ " << op << "\t"; // '+' indicates expansion

                // Lambda to print operand
                auto printOp = [&](Operand o)
                {
                    if (o.param_type == "P")
                        cout << ala[o.param_ind];
                    else if (o.param_type == "R")
                        cout << reg_names[o.param_ind];
                    else
                        cout << o.literal;
                };

                // Print Op1
                if (mdt.op1[ptr].param_type != "" || mdt.op1[ptr].literal != "")
                {
                    printOp(mdt.op1[ptr]);
                }

                // Print Op2
                if (mdt.op2[ptr].param_type != "" || mdt.op2[ptr].literal != "")
                {
                    cout << ", ";
                    printOp(mdt.op2[ptr]);
                }
                cout << endl;
                ptr++;
            }
        }
        else
        {
            // 3. Regular Line - Just Print
            cout << line << endl;
        }
    }

    return 0;
}
