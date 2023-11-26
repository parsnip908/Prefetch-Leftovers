import sys

if len(sys.argv) != 3:
    print("USAGE: parse.py input_file ouput_dir")
    exit(-1)
input = sys.argv[1]
output = sys.argv[2]
read_file = open(input, "r")

threads_num = 0
threads_id = {}

types = set()
ln = read_file.readline()

f = open(output + "/hello.out", "a+")
threads_file = f
while ln:
    tk = ln.split('\t')
    
    id = int(tk[1])
    types.add(len(tk))
    if len(tk) == 3 and tk[2] == "tr\n":
        threads_id[id] = threads_num 
        threads_num += 1
    else:
        lamport_clk = tk[0]
        td_id = tk[1]
        base = lamport_clk + ',' + str(threads_id[id])
        i = 3
        line = ''
        while i < len(tk):
            if tk[i] == 'r':
                line =  ',' +  tk[i] + ',' + hex(int(tk[i + 1])) + ',' + tk[i + 2]
                i += 3
            elif tk[i] == 'r2':
                line =  ',' +  tk[i] + ',' + hex(int(tk[i + 1]))
                i += 2
            elif tk[i] == 'w':
                line =  ',' +  tk[i] + ',' + hex(int(tk[i + 1])) + ',' + tk[i + 2]
                i += 3
            wr = base + line
            if wr[-1] != '\n':
                wr += '\n'
            threads_file.write(wr )
    ln = read_file.readline()
print(threads_id)
print(threads_num)
print(types)