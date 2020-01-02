/****************************************************************************
  FileName     [ cmdParser.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command parsing member functions for class CmdParser ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include "../util/util.h"
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    External funcitons
//----------------------------------------------------------------------
void mybeep();


//----------------------------------------------------------------------
//    Member Function for class cmdParser
//----------------------------------------------------------------------
// return false if file cannot be opened
// Please refer to the comments in "DofileCmd::exec", cmdCommon.cpp
bool
CmdParser::openDofile(const string& dof)
{
   	// TODO...
    _dofile = new ifstream(dof.c_str());

   	if (!*_dofile)
    {
       // take original dofile
       if (_dofileStack.size() > 0) _dofile = _dofileStack.top();
       else
       {
          delete _dofile;
          _dofile = 0;
       }
   		 return false;
    }
    else if (_dofileStack.size() >= 1024)
    {
        _dofile->close();
        delete _dofile;
        _dofile = 0;
    }
   	
	  _dofileStack.push(_dofile);   
    
    return true;
}

// Must make sure _dofile != 0
void
CmdParser::closeDofile()
{
    // TODO
    assert(_dofile != 0);
    _dofile->close();
    _dofileStack.pop();
    delete _dofile;

    if (!_dofileStack.empty())
   		_dofile = _dofileStack.top();
   	else _dofile = 0;

}

// Return false if registration fails
bool
CmdParser::regCmd(const string& cmd, unsigned nCmp, CmdExec* e)
{
   // Make sure cmd hasn't been registered and won't cause ambiguity
   string str = cmd;
   unsigned s = str.size();
   if (s < nCmp) return false;
   while (true) {
      if (getCmd(str)) return false;
      if (s == nCmp) break;
      str.resize(--s);
   }

   // Change the first nCmp characters to upper case to facilitate
   //    case-insensitive comparison later.
   // The strings stored in _cmdMap are all upper case
   //
   assert(str.size() == nCmp);  // str is now mandCmd
   string& mandCmd = str;
   for (unsigned i = 0; i < nCmp; ++i)
      mandCmd[i] = toupper(mandCmd[i]);
   string optCmd = cmd.substr(nCmp);
   assert(e != 0);
   e->setOptCmd(optCmd);

   // insert (mandCmd, e) to _cmdMap; return false if insertion fails.
   return (_cmdMap.insert(CmdRegPair(mandCmd, e))).second;
}

// Return false on "quit" or if excetion happens
CmdExecStatus
CmdParser::execOneCmd()
{
   bool newCmd = false;
   if (_dofile != 0){
      newCmd = readCmd (*_dofile);
   }
   else
      newCmd = readCmd(cin);

   // execute the command
   if (newCmd) {
      string option;
      CmdExec* e = parseCmd(option);
      if (e != 0)
         return e->exec(option);
   }
   return CMD_EXEC_NOP;
}

// For each CmdExec* in _cmdMap, call its "help()" to print out the help msg.
// Print an endl at the end.
void
CmdParser::printHelps() const
{
	// since the function is const, iterator must also be const
	// use const_iterator instead...
    for (map<const string, CmdExec*>::const_iterator it = _cmdMap.begin(); it != _cmdMap.end(); ++it)
   		it->second->usage(cout);
}

void
CmdParser::printHistory(int nPrint) const
{
   assert(_tempCmdStored == false);
   if (_history.empty()) {
      cout << "Empty command history!!" << endl;
      return;
   }
   int s = _history.size();
   if ((nPrint < 0) || (nPrint > s))
      nPrint = s;
   for (int i = s - nPrint; i < s; ++i)
      cout << "   " << i << ": " << _history[i] << endl;
}


//
// Parse the command from _history.back();
// Let string str = _history.back();
//
// 1. Read the command string (may contain multiple words) from the leading
//    part of str (i.e. the first word) and retrive the corresponding
//    CmdExec* from _cmdMap
//    ==> If command not found, print to cerr the following message:
//        Illegal command!! "(string cmdName)"
//    ==> return it at the end.
// 2. Call getCmd(cmd) to retrieve command from _cmdMap.
//    "cmd" is the first word of "str".
// 3. Get the command options from the trailing part of str (i.e. second
//    words and beyond) and store them in "option"
//
CmdExec*
CmdParser::parseCmd(string& option)
{
   assert(_tempCmdStored == false);
   assert(!_history.empty());
   string str = _history.back();

   // TODO...
   assert(str[0] != 0 && str[0] != ' ');

   // get the first token
   string sFirst("");
   myStrGetTok (str, sFirst);

   CmdExec* cmdOption = getCmd(sFirst);

   if (cmdOption != 0)
   {
      size_t pBeg = str.find (" ") + 1;
      size_t pEnd = str.npos;
      if (pBeg - 1 == pEnd) option = "";
      else option = str.substr (pBeg, pEnd - pBeg);

      return cmdOption;
   }

   cerr << "Illegal command!! \"" << sFirst << "\"\n"; 
   return NULL;
}

// Remove this function for TODO...
//
// This function is called by pressing 'Tab'.
// It is to list the partially matched commands.
// "str" is the partial string before current cursor position. It can be 
// a null string, or begin with ' '. The beginning ' ' will be ignored.
//
// Several possibilities after pressing 'Tab'
// (Let $ be the cursor position)
// 1. LIST ALL COMMANDS
//    --- 1.1 ---
//    [Before] Null cmd
//    cmd> $
//    --- 1.2 ---
//    [Before] Cmd with ' ' only
//    cmd>     $
//    [After Tab]
//    ==> List all the commands, each command is printed out by:
//           cout << setw(12) << left << cmd;
//    ==> Print a new line for every 5 commands
//    ==> After printing, re-print the prompt and place the cursor back to
//        original location (including ' ')
//
// 2. LIST ALL PARTIALLY MATCHED COMMANDS
//    --- 2.1 ---
//    [Before] partially matched (multiple matches)
//    cmd> h$                   // partially matched
//    [After Tab]
//    HELp        HIStory       // List all the parially matched commands
//    cmd> h$                   // and then re-print the partial command
//    --- 2.2 ---
//    [Before] partially matched (multiple matches)
//    cmd> h$llo                // partially matched with trailing characters
//    [After Tab]
//    HELp        HIStory       // List all the parially matched commands
//    cmd> h$llo                // and then re-print the partial command
//
// 3. LIST THE SINGLY MATCHED COMMAND
//    ==> In either of the following cases, print out cmd + ' '
//    ==> and reset _tabPressCount to 0
//    --- 3.1 ---
//    [Before] partially matched (single match)
//    cmd> he$
//    [After Tab]
//    cmd> heLp $               // auto completed with a space inserted
//    --- 3.2 ---
//    [Before] partially matched with trailing characters (single match)
//    cmd> he$ahah
//    [After Tab]
//    cmd> heLp $ahaha
//    ==> Automatically complete on the same line
//    ==> The auto-expanded part follow the strings stored in cmd map and
//        cmd->_optCmd. Insert a space after "heLp"
//    --- 3.3 ---
//    [Before] fully matched (cursor right behind cmd)
//    cmd> hElP$sdf
//    [After Tab]
//    cmd> hElP $sdf            // a space character is inserted
//
// 4. NO MATCH IN FITST WORD
//    --- 4.1 ---
//    [Before] No match
//    cmd> hek$
//    [After Tab]
//    ==> Beep and stay in the same location
//
// 5. FIRST WORD ALREADY MATCHED ON FIRST TAB PRESSING
//    --- 5.1 ---
//    [Before] Already matched on first tab pressing
//    cmd> help asd$gh
//    [After] Print out the usage for the already matched command
//    Usage: HELp [(string cmd)]
//    cmd> help asd$gh
//
// 6. FIRST WORD ALREADY MATCHED ON SECOND AND LATER TAB PRESSING
//    ==> Note: command usage has been printed under first tab press
//    ==> Check the word the cursor is at; get the prefix before the cursor
//    ==> So, this is to list the file names under current directory that
//        match the prefix
//    ==> List all the matched file names alphabetically by:
//           cout << setw(16) << left << fileName;
//    ==> Print a new line for every 5 commands
//    ==> After printing, re-print the prompt and place the cursor back to
//        original location
//    Considering the following cases in which prefix is empty:
//    --- 6.1.1 ---
//    [Before] if prefix is empty, and in this directory there are multiple
//             files and they do not have a common prefix,
//    cmd> help $sdfgh
//    [After] print all the file names
//    .               ..              Homework_3.docx Homework_3.pdf  Makefile
//    MustExist.txt   MustRemove.txt  bin             dofiles         include
//    lib             mydb            ref             src             testdb
//    cmd> help $sdfgh
//    --- 6.1.2 ---
//    [Before] if prefix is empty, and in this directory there are multiple
//             files and all of them have a common prefix,
//    cmd> help $orld
//    [After]
//    ==> auto insert the common prefix and make a beep sound
//    // e.g. in hw3/ref
//    cmd> help mydb-$orld
//    ==> DO NOT print the matched files
//    ==> If "tab" is pressed again, see 6.2
//    --- 6.1.3 ---
//    [Before] if prefix is empty, and only one file in the current directory
//    cmd> help $ydb
//    [After] print out the single file name followed by a ' '
//    // e.g. in hw3/bin
//    cmd> help mydb $
//    ==> If "tab" is pressed again, make a beep sound and DO NOT re-print 
//        the singly-matched file
//    --- 6.2 ---
//    [Before] with a prefix and with mutiple matched files
//    cmd> help M$Donald
//    [After]
//    Makefile        MustExist.txt   MustRemove.txt
//    cmd> help M$Donald
//    --- 6.3 ---
//    [Before] with a prefix and with mutiple matched files,
//             and these matched files have a common prefix
//    cmd> help Mu$k
//    [After]
//    ==> auto insert the common prefix and make a beep sound
//    ==> DO NOT print the matched files
//    cmd> help Must$k
//    --- 6.4 ---
//    [Before] with a prefix and with a singly matched file
//    cmd> help MustE$aa
//    [After] insert the remaining of the matched file name followed by a ' '
//    cmd> help MustExist.txt $aa
//    ==> If "tab" is pressed again, make a beep sound and DO NOT re-print 
//        the singly-matched file
//    --- 6.5 ---
//    [Before] with a prefix and NO matched file
//    cmd> help Ye$kk
//    [After] beep and stay in the same location
//    cmd> help Ye$kk
//
//    [Note] The counting of tab press is reset after "newline" is entered.
//
// 7. FIRST WORD NO MATCH
//    --- 7.1 ---
//    [Before] Cursor NOT on the first word and NOT matched command
//    cmd> he haha$kk
//    [After Tab]
//    ==> Beep and stay in the same location
void
CmdParser::listCmd(const string& str)
{
   	// TODO...
   	// CASE
   	// 1. FISRTSPACE -> print all
   	// 2. FIRSTMATCH -> later decide printing format
   	// 3. SECONDMACH -> later deal with tab number
   	// 4. NONE -> no match

    // fuction variables
    string sTemp ("");
    for (int i = 0; i < _readBufEnd - _readBuf; ++i)
        sTemp += *(_readBuf + i);

    string* sFirstArg = new string("");
    size_t* nFirstArg = new size_t(0);
    *nFirstArg = myStrGetTok (str, *sFirstArg);
    if (*nFirstArg == string::npos) *nFirstArg = str.length();

    int nBufPtr = _readBufPtr - _readBuf;
    int nTABState;
    vector<string> sPossibleCmd;
    if (sPossibleCmd.size()) sPossibleCmd.clear();

    // analize str
    if (str == "" || str == " ") nTABState = 1;
    else // check the first argument / string before cursor corresponds to possible CMD
    {
        bool isFind = false;

        CmdExec* e;
        for (map<const string, CmdExec*>::iterator it = _cmdMap.begin(); it != _cmdMap.end(); ++it)
        {

            e = _cmdMap.at (it->first);
            string sCmd = e->getOptCmd();
            sCmd = it->first + sCmd;

            // check matched in cmdMap
            if (*nFirstArg > sCmd.length()) isFind = false;
            else if ((unsigned) nBufPtr <= *nFirstArg) // if cursor at the first arg.
            {
                if (myStrNCmp(sCmd, *sFirstArg, min(sCmd.length(), *nFirstArg)) == 0)
                {
                    isFind = true;
                    sPossibleCmd.push_back(it->first);
                } 
            }
            else // if not at the first arg.
            {
                // must exactly case-insensitive
                if (myStrNCmp (sCmd, *sFirstArg, it->first.length()) == 0)
                {
                    isFind = true;
                    sPossibleCmd.push_back(it->first);

                    // exactly only one mapping
                    if (sPossibleCmd.size() > 1) {isFind = false; break;}
                }
            }
        }

        if (!isFind) nTABState = 4;
        else if ((unsigned) nBufPtr <= *nFirstArg) nTABState = 2;
        else nTABState = 3;  
    }
    
    // TAB implement
    switch (nTABState)
    {
        case 1:
        {
            // deleteline, including _prompt
            deleteLine();
            cout << endl;
            //for (unsigned i = 0; i < _prompt.size(); ++i)
                //cout << '\b' << ' ' << '\b';

            int* nCount = new int(0);
            for (map<const string, CmdExec*>::iterator it = _cmdMap.begin(); it != _cmdMap.end(); ++it, ++*nCount)
            {
                CmdExec* cmdOpt = it->second;
                string sCmdComm = it->first + cmdOpt->getOptCmd();
                cout << setw(12) << left << sCmdComm;
                if ((*nCount + 1) % 5 == 0)
                    cout << endl;
            }
            delete nCount;
        
            // re-print
            cout << endl;
            resetBufAndPrintPrompt();
            strcpy(_readBuf, sTemp.c_str());
            cout << _readBuf;
            _readBufPtr = _readBufEnd = _readBuf + sTemp.size();

            // reset tabpress
            _tabPressCount = 0;
            break;
        }

        case 2:
        {
            string sCmd = "";
            if (sPossibleCmd.size() == 1) // if only a possible match CMD
            {
                sCmd = sPossibleCmd[0];
                sCmd += _cmdMap.at(sCmd)->getOptCmd();
                for (unsigned i = nBufPtr; i < sCmd.length(); ++i)
                    insertChar (sCmd[i]);

                insertChar (' ');
            }
            else
            {
                // deleteline, including _prompt
                deleteLine();
                cout << endl;
                //for (unsigned i = 0; i < _prompt.size(); ++i)
                    //cout << '\b' << ' ' << '\b';

                int* nCount = new int(0);
                for (unsigned i = 0; i < sPossibleCmd.size(); ++i)
                {
                    ++*nCount;
                    sCmd = sPossibleCmd[i];
                    sCmd += _cmdMap.at(sCmd)->getOptCmd();
                    cout << setw(12) << left << sCmd;
                    if (*nCount % 5 == 0)
                        cout << endl;
                }
                delete nCount;

                // re-print 
                cout << endl;
                resetBufAndPrintPrompt();
                strcpy(_readBuf, sTemp.c_str());
                cout << _readBuf;
                _readBufPtr = _readBufEnd = _readBuf + sTemp.size();
              
                for (int i = 0; i < _readBufEnd - _readBuf - nBufPtr; ++i)
                {
                    _readBufPtr--;
                    cout << '\b';
                }
            }

            // reset tabpress
            _tabPressCount = 0;
            
            break;
        }

        case 3:
        {
            // Second TAB State
            // CMDMATCHNONE   = 1
            // CMDMATCHSINGLE = 2
            // CMDNOMOREPRX   = 3
            // CMDMOREPRRX    = 4
            int nSecTABState = 0;

            assert (sPossibleCmd.size() == 1);
            assert ((unsigned) nBufPtr > *nFirstArg);

            vector<string> sFile;
            vector<string> sAllFile;

            // find prefix
            string sCmdPrx ("");
            for (unsigned i = *nFirstArg + 1; i < (unsigned) nBufPtr; ++i)
                sCmdPrx.push_back (*(_readBuf + i));

            string sMorePrx (sCmdPrx);

            // load file
            listDir (sFile, sCmdPrx, ".");

            // determine second tab case
            if (sFile.size() == 0) nSecTABState = 1; // no match
            else if (sFile.size() == 1) nSecTABState = 2; // match single
            else
            {
                // find more prefix, need match all
                bool isMatchAll = true;
                for (unsigned i = sCmdPrx.length(); i < sFile[0].length() && isMatchAll; ++i)
                {
                    isMatchAll = true;
                    sMorePrx.push_back (sFile[0][i]);
                    for (unsigned j = 1; j < sFile.size(); ++j)
                        if (sFile[0][i] != sFile[j][i]) {isMatchAll = false; break;}
                }

                if (!isMatchAll) sMorePrx.pop_back();
                if (sMorePrx.size() == sCmdPrx.size()) nSecTABState = 3;
                else
                {
                    nSecTABState = 4;
                    
                    // get only additional part of prefix'
                    string sTemp("");
                    for (unsigned i = sCmdPrx.length(); i < sMorePrx.length(); ++i)
                        sTemp.push_back (sMorePrx[i]);
                    
                    sMorePrx = sTemp;
                }
            }

            // implement TAB -> press 1 and press 2 (or more) TAB
            if (_tabPressCount == 1)
            {
                // deleteline, including _prompt
                deleteLine();
                cout << endl;
                //for (unsigned i = 0; i < _prompt.size(); ++i)
                    //cout << '\b' << ' ' << '\b';

                // print usage
                _cmdMap.at(sPossibleCmd[0])->usage(cout);

                // re-print
                cout << endl;
                resetBufAndPrintPrompt();
                _tabPressCount = 1; // restore tab count
                strcpy(_readBuf, sTemp.c_str());
                cout << _readBuf;
                _readBufPtr = _readBufEnd = _readBuf + sTemp.size();

                for (int i = 0; i < _readBufEnd - _readBuf - nBufPtr; ++i)
                {
                    _readBufPtr--;
                    cout << '\b';
                }

                break;
            }
            else if (_tabPressCount >= 2)
            {
                if (nSecTABState == 1) {mybeep(); break;}// no match...
                else if (nSecTABState == 2) // single file
                {
                    if (_tabPressCount > 2) {mybeep(); break;}

                    // print file name
                    for (unsigned i = sCmdPrx.length(); i < sFile[0].length(); ++i)
                        insertChar (sFile[0][i]);

                    insertChar(' ');
                    break;
                }
                else if (nSecTABState == 3) // no more common prefix
                {
                    // print all file
                    // deleteline, including _prompt
                    deleteLine();
                    cout << endl;
                    //for (unsigned i = 0; i < _prompt.size(); ++i)
                        //cout << '\b' << ' ' << '\b';

                    int* nCount = new int(0);
                    for (unsigned i = 0; i < sFile.size(); ++i)
                    {
                        ++*nCount;
                        cout << setw(16) << left << sFile[i];
                        if (*nCount % 5 == 0) cout << endl;
                    }
                    delete nCount;

                    // re-print 
                    cout << endl;

                    int* TABTEMP = new int(_tabPressCount);
                    resetBufAndPrintPrompt();
                    _tabPressCount = *TABTEMP;
                    delete TABTEMP;

                    strcpy(_readBuf, sTemp.c_str());
                    cout << _readBuf;
                    _readBufPtr = _readBufEnd = _readBuf + sTemp.size();
              
                    for (int i = 0; i < _readBufEnd - _readBuf - nBufPtr; ++i)
                    {
                        _readBufPtr--;
                        cout << '\b';
                    }
                    break;
                }
                else if (nSecTABState == 4)
                {
                    // directly add sMorePrx
                    for (unsigned i = 0; i < sMorePrx.length(); ++i)
                        insertChar(sMorePrx[i]);

                    mybeep();
                    break;
                }
                else {cerr << "Some case is not classified!\n"; break;}
            }
            
        }
        case 4:
            // reset tabpress
            _tabPressCount = 0;
            mybeep();
            break;
    
        default:
            cerr << "Some condition is not classified!\n";
            // reset tabpress
            _tabPressCount = 0;
            break;
    }
    
}

// cmd is a copy of the original input
//
// return the corresponding CmdExec* if "cmd" matches any command in _cmdMap
// return 0 if not found.
//
// Please note:
// ------------
// 1. The mandatory part of the command string (stored in _cmdMap) must match
// 2. The optional part can be partially omitted.
// 3. All string comparison are "case-insensitive".
// STEPS:
// 1. check mandatory
// 2. compare with _cmdMap
// 3. check optional
// 4. return CmdExec*
//
CmdExec*
CmdParser::getCmd(string cmd)
{
   CmdExec* e = 0;
   // TODO...
   // Mandatory
   string sMand;
   bool isFind = false;
   for (map<const string, CmdExec*>::iterator it = _cmdMap.begin(); it != _cmdMap.end(); ++it)
   {
      // find numbers for mandatory part
      int nMandNum = 0;
      sMand = it->first;
      for (string::iterator sit = sMand.begin(); sit != sMand.end(); ++sit)
      {
         // find CAPITAL CHAR or "-"
         char* cCommand = new char;
         *cCommand = *sit;

         if (*cCommand == 45 || (*cCommand >= 65 && *cCommand <= 90)) // "-" is 45 
            ++nMandNum;

         delete cCommand;
      }

      // get optional
      e = _cmdMap.at(sMand);
      const string sOpt = e->getOptCmd();
      sMand = sMand + sOpt;
      
      // check matched in cmdMap 
      if (myStrNCmp(sMand, cmd, nMandNum) == 0)
      {
         isFind = true;
         break;
      }
   }

   if (!isFind) {return 0;}

   return e;
}


//----------------------------------------------------------------------
//    Member Function for class CmdExec
//----------------------------------------------------------------------
// return false if option contains an token
bool
CmdExec::lexNoOption(const string& option) const
{
   string err;
   myStrGetTok(option, err);
   if (err.size()) {
      errorOption(CMD_OPT_EXTRA, err);
      return false;
   }
   return true;
}

// Return false if error options found
// "optional" = true if the option is optional XD
// "optional": default = true
//
bool
CmdExec::lexSingleOption
(const string& option, string& token, bool optional) const
{
   size_t n = myStrGetTok(option, token);
   if (!optional) {
      if (token.size() == 0) {
         errorOption(CMD_OPT_MISSING, "");
         return false;
      }
   }
   if (n != string::npos) {
      errorOption(CMD_OPT_EXTRA, option.substr(n));
      return false;
   }
   return true;
}

// if nOpts is specified (!= 0), the number of tokens must be exactly = nOpts
// Otherwise, return false.
//
bool
CmdExec::lexOptions
(const string& option, vector<string>& tokens, size_t nOpts) const
{
   string token;
   size_t n = myStrGetTok(option, token);
   while (token.size()) {
      tokens.push_back(token);
      n = myStrGetTok(option, token, n);
   }
   if (nOpts != 0) {
      if (tokens.size() < nOpts) {
         errorOption(CMD_OPT_MISSING, "");
         return false;
      }
      if (tokens.size() > nOpts) {
         errorOption(CMD_OPT_EXTRA, tokens[nOpts]);
         return false;
      }
   }
   return true;
}

CmdExecStatus
CmdExec::errorOption(CmdOptionError err, const string& opt) const
{
   switch (err) {
      case CMD_OPT_MISSING:
         cerr << "Error: Missing option";
         if (opt.size()) cerr << " after (" << opt << ")";
         cerr << "!!" << endl;
      break;
      case CMD_OPT_EXTRA:
         cerr << "Error: Extra option!! (" << opt << ")" << endl;
      break;
      case CMD_OPT_ILLEGAL:
         cerr << "Error: Illegal option!! (" << opt << ")" << endl;
      break;
      case CMD_OPT_FOPEN_FAIL:
         cerr << "Error: cannot open file \"" << opt << "\"!!" << endl;
      break;
      default:
         cerr << "Error: Unknown option error type!! (" << err << ")" << endl;
      exit(-1);
   }
   return CMD_EXEC_ERROR;
}

