import sys

if len(sys.argv) != 3:
    print("USAGE: parse.py input_file ouput_dir")
    exit(-1)
input = sys.argv[1]
output = sys.argv[2]
read_file = open(input, "r")

threads_num = 0
threads_id = []
threads_file = {}
types = set()
ln = read_file.readline()
while ln:
    tk = ln.split('\t')
    
    id = int(tk[1])
    types.add(len(tk))
    if len(tk) == 3 and tk[2] == "tr\n":
        # print(tk[2])
        f = open(output + str(threads_num) + ".out", "a+")
        threads_file[id] = f
        threads_num += 1
    else:
        lamport_clk = tk[0]
        td_id = tk[1]
        base = lamport_clk + ',' + td_id
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
            threads_file[id].write(wr )
    ln = read_file.readline()

print(types)