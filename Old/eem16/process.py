str='''1110111
1111100
1011000
1011110
1111001
1110001
1101111
1110110
0000110
0011110
1111000
0111000
0010101
1010100
1011100
1110011
1100111
1010000
1101101
1000110
0111110
0011100
0101010
1001001
1101110
1011011'''
let=list("1000000")
print(let[1])
str=str.split('\n')
for start in range(7):
    print()
    print("case (letter)")
    for x in range(len(str)):
        print("5'h{} : assign {} = 1'b{};".format(hex(x)[2:],chr(ord('a')+start),str[x][start]))
    print("default : assign {} = 1'b{};".format(chr(ord('a')+start),let[start]))
    print("endcase")