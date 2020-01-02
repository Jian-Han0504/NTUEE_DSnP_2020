/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "../cmd/cmdParser.h"
#include "../util/util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   	// TODO
	// check option
    vector<string> tokens;
    
    string token;
    size_t n = myStrGetTok(option, token);
    while (token.size()) {
        tokens.push_back(token);
        n = myStrGetTok(option, token, n);
    }

    try
    {
        if (tokens.size() == 0)
            return CmdExec::errorOption (CMD_OPT_MISSING, "");
        else
        {
        	// first find -a, but not at the last one
        	bool isOption = false;
        	unsigned nLocOption = 0;
        	for (unsigned i = 0; i < tokens.size(); ++i)
        		if (myStrNCmp ("-Array", tokens[i], 2) == 0) { isOption = true; nLocOption = i; }

        	if (isOption && tokens.size() < 3) // only -a specified... MISSING
        		return CmdExec::errorOption(CMD_OPT_MISSING, tokens[nLocOption]);
        	else if (isOption && tokens.size() > 3) // Extra inputs
        	{
        		string sExtra("");
        		for (unsigned i = 3; i < tokens.size(); ++i) 
        		{
        			if (i == tokens.size() - 1) sExtra += tokens[i];
        			else sExtra += tokens[i] + " ";
        		}

        		return CmdExec::errorOption(CMD_OPT_EXTRA, sExtra);
        	}	
        	else if (isOption && nLocOption == 2) // -a is at the end... taken as extra case on the 2nd input
				return CmdExec::errorOption(CMD_OPT_EXTRA, tokens[1]);
			
			if (!isOption) // no array
			{
				int nObjNum;
            	if (!myStr2Int (tokens[0], nObjNum) || nObjNum < 0)
                	return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[0]);

                if (tokens.size() > 1) // must be single
                	return CmdExec::errorOption(CMD_OPT_EXTRA, tokens[1]);

                mtest.newObjs ((size_t) nObjNum);
			}
			else
			{
				assert(tokens.size() == 3);
				assert(nLocOption < 2 && nLocOption >= 0);

				int nIdxArrNum, nIdxArrSize;
				if (nLocOption == 0) { nIdxArrNum = 2; nIdxArrSize = 1;}
				else {nIdxArrNum = 0; nIdxArrSize = 2;}

				// check number legal
            	int nArrNum;
            	if (!myStr2Int (tokens[nIdxArrNum], nArrNum) || nArrNum < 0)
                	return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[nIdxArrNum]);

                // check size legal
            	int nArrSize;
           		if (!myStr2Int (tokens[nIdxArrSize], nArrSize) || nArrSize <= 0)
                		return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[nIdxArrSize]);

                // allocate array
            	mtest.newArrs ((size_t) nArrNum, (size_t) nArrSize);
			}
        	
        }
    }
    catch (std::bad_alloc& a) {return CMD_EXEC_ERROR;}
    catch (...) { cerr << "Missing exceptions!!!\n"; }
	
    // Use try-catch to catch the bad_alloc exception
    return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO

   vector<string> tokens;
    
    string token;
    size_t n = myStrGetTok(option, token);
    while (token.size()) {
        tokens.push_back(token);
        n = myStrGetTok(option, token, n);
    }

    if (tokens.size() == 0)
        return CmdExec::errorOption(CMD_OPT_MISSING, "");
    else if (tokens.size() <= 3)
    {
        // first find -a, but not at the last one
       	bool isOption = false;
       	unsigned nLocOption = 0;
       	for (unsigned i = 0; i < tokens.size(); ++i)
        	if (myStrNCmp ("-Array", tokens[i], 2) == 0) { isOption = true; nLocOption = i; }

        int nIdxSpecify, nIdxNum;

        if (!isOption && tokens.size() == 3) 
        	return CmdExec::errorOption(CMD_OPT_EXTRA, tokens[2]);
        else if (!isOption) { nIdxSpecify = 0; nIdxNum = 1;}
        else if (isOption && tokens.size() < 3)
        	return CmdExec::errorOption(CMD_OPT_MISSING, "");
        else
        {
        	if (nLocOption == 0) { nIdxSpecify = 1; nIdxNum = 2;}
        	else if(nLocOption == 2) { nIdxSpecify = 0; nIdxNum = 1;}
        	else return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[nLocOption]);
        }
        
        // check random or specific
        if (myStrNCmp ("-Index", tokens[nIdxSpecify], 2) == 0)
        {
            // missing size case
            if (tokens.size() == 1) 
                return CmdExec::errorOption(CMD_OPT_MISSING, tokens[nIdxSpecify]);

            // check id is legal
            int idI;
            if (!myStr2Int (tokens[nIdxNum], idI) || idI < 0)
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[nIdxNum]);

            // deleting...
            if (tokens.size() == 2 && !isOption)
            {
                // object case
                if (idI >= (int)(mtest.getObjListSize()))
                {
                    cerr << "Size of object list (" << mtest.getObjListSize() << ") is <= " << idI << "!!" << endl;
                    return CmdExec::errorOption(CMD_OPT_ILLEGAL, to_string(idI));
                }

                mtest.deleteObj ((size_t) idI);
            }  
            else if (isOption)
            {
                // array case
                if(idI >= (int)(mtest.getArrListSize()))
                {
                    cerr << "Size of array list (" << mtest.getArrListSize() << ") is <= " << idI << "!!" << endl;
                    return CmdExec::errorOption(CMD_OPT_ILLEGAL, to_string(idI));
                }

                mtest.deleteArr ((size_t) idI);
            }
            else return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[2]);
        }
        else if (myStrNCmp ("-Random", tokens[nIdxSpecify], 2) == 0)
        {
            // missing size case
            if (tokens.size() == 1) 
                return CmdExec::errorOption(CMD_OPT_MISSING, tokens[nIdxSpecify]);

            // check id is legal
            int idR;
            if (!myStr2Int (tokens[nIdxNum], idR) || idR < 0)
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[nIdxNum]);

            //deleting...
            if (!isOption && tokens.size() == 2)
            {
                // object case
                if (mtest.getObjListSize() == 0)
                {
                    cerr << "Size of object list is 0!!" << endl;
                    return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[nIdxNum]);
                }

                size_t rndSize;
                for (int i = 0; i < idR; ++i)
                {
                    rndSize = rnGen (mtest.getObjListSize());
                    //cout << "**** random ***** " << rndSize << endl;
                    mtest.deleteObj(rndSize);
                }
            }
            else if (isOption)
            { 
                // array case
                if (mtest.getArrListSize() == 0)
                {
                    cerr << "Size of object list is 0!!" << endl;
                    return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[nIdxNum]);
                }

                size_t rndSize;
                for (int i = 0; i < idR; ++i)
                {
                    rndSize = rnGen (mtest.getArrListSize());
                    //cout << "**** random ***** " << rndSize << endl;
                    mtest.deleteArr(rndSize);
                }
            }
            else return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[2]);
        }
        else return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[nIdxSpecify]);        
    }
    else return CmdExec::errorOption(CMD_OPT_EXTRA, tokens[3]);
    
    return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


