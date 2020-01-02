import sys
import random
import string
import math
'''
# cmdParse = ['adtr', 'adta', 'adtd', 'adtq', 'adts', 'adtp', 'do']
strnum = 10000
_strLen = 5
file = open ("./b06901086_hw5/tests/dox", "w")
for i in range (1, strnum+1):
    addstr = ''.join(random.choice (string.ascii_letters + string.digits) for x in range(_strLen))
    file.write ("adta -s " + addstr + "\n")
'''

_strLen = 5
cmdNum_max = 1001
doNum_max = 17
recurdo = False
isPrint = False

for dofile in range (12, doNum_max):
    file = open ("./b06901086_hw5/tests/do" + str(dofile), "w")
    for cmdnum in range (1, random.randint (50, cmdNum_max)):
        cmd = random.randint (0, 7)
        
        # reset print
        if cmdnum % 8 == 0:
            isPrint = False
        
        if cmd != 2: # reset delete
            cmd = random.choice ([2, cmd, cmd, 2, 2, cmd, 2, cmd])
        elif isPrint == False:
            cmd = random.choice ([5, 5, cmd, 5, cmd, 5, 5, cmd]) 

        if cmd == 0: # reset len
            _strLen = random.randint (2, 8)
            file.write ("adtr " + str (_strLen) + "\n")
        elif cmd == 1: # add string
            addrnd = math.ceil (random.gauss (5, 3))
            if addrnd <= 0:
                addrnd = 1
            elif (addrnd > 15):
                addrnd = 15

            for addnum in range (1, addrnd):
                addstr = ''.join(random.choice (string.ascii_letters + string.digits) for x in range(_strLen))
                file.write ("adta -s " + addstr + "\n")
        elif cmd == 2: # delete
            # delcase = ['-a', '-f', '-b', '-s']
            delcmd = random.randint (0, 4)
            if delcmd == 0:
                file.write ("adtd -a\n")
            elif delcmd == 1:
                file.write ("adtd -f " + str (random.randint (1, 4)) + "\n")
            elif delcmd == 2:
                file.write ("adtd -b " + str(random.randint (1, 4)) + "\n")
            else:
                delstr = ''.join(random.choice (string.ascii_letters + string.digits) for x in range(_strLen))
                file.write ("adtd -s " + delstr + "\n")
        elif cmd == 3: # queue
            findstr = ''.join(random.choice (string.ascii_letters + string.digits) for x in range(_strLen))
            file.write ("adtq " + findstr + "\n")
        elif cmd == 4: # sort
            file.write ("adts\n")
        elif cmd == 5: # print
            prtstr = random.choice (['-v', '', '-r'])
            file.write ("adtp " + prtstr + "\n")
            isPrint = True
        elif cmd == 6: # do recursive
            file.write ("usage\n")
        else:
            AssertionError ("random false!\n")

    file.write ("adtp\n")
    file.write ("q -f\n")
    file.close()