#! /usr/bin/python2
import sys
import operator
import struct
from numpy import inf

#-----------------------------
PIXEL_CLOCK = 56250000
HTOTAL = 1048
#-----------------------------
THRESHOLD_VALUE = 3000
THRESHOLD_MIN_DIST = 11
SCORE_THRESHOLD = 4 



freq = dict()
freq['1'] = 523.0 / PIXEL_CLOCK * 2.0
freq['0'] = 1397.0 / PIXEL_CLOCK * 2.0
freq[' '] = 6272.0 / PIXEL_CLOCK * 2.0
freq['.'] = 277.0 / PIXEL_CLOCK * 2.0
separation_dist = {'1': 0, '.':0, '0':0, ' ':0}
for f_key in freq:
    t=0
    for y in range(1000):
        #print y, int(freq[f_key]*t)
        if int(freq[f_key] * t)%2 == 1:
            separation_dist[f_key] = y
            break
        t += HTOTAL
print separation_dist
#exit(0)
def convert_morse_to_char(bits):
    morseAlphabet ={
            "01": "A",
            "1000" : "B",
            "1010": "C",
            "100": "D",
            "0": "E",
            "0010": "F",
            "110": "G",
            "0000": "H",
            "00": "I",
            "0111": "J",
            "101": "K",
            "0100": "L",
            "11": "M",
            "10": "N",
            "111": "O",
            "0110": "P",
            "1101": "Q",
            "010": "R",
            "000": "S",
            "1": "T",
            "001": "U",
            "0001": "V",
            "011": "W",
            "1001": "X",
            "1011": "Y",
            "1100": "Z",
            "0101": " "
            }
    if bits not in morseAlphabet.keys():
        return -1
    else:
        return morseAlphabet[bits] 


silence = 0
current_bit = ''
score= {'1':0, '0':0, ' ':0, '.':0}
char_bits = ''
while True:
    sample = struct.unpack("h", sys.stdin.read(2))[0]
    if sample < THRESHOLD_VALUE:
        silence+=1
    elif sample > THRESHOLD_VALUE and silence:
        #print sample
        dist = silence
        silence = 0
        min_dist = float(inf)
        s = ' '
        for d in separation_dist:
            (s, min_dist) = (d, abs(dist - separation_dist[d])) if abs(dist - separation_dist[d]) <= min_dist else (s , min_dist)
            #if s == '.' == d and previous                
        #print ">", s, min_dist,
        if min_dist > THRESHOLD_MIN_DIST:
            continue
        if current_bit == s and new == 0:
            continue
        else:
            new = 1
        #print ""
        score[s]+=1
        max_bit = max(score.iteritems(), key=operator.itemgetter(1))[0]
        #print "\n"+max_bit,
        #print score[max_bit], "Bit", max_bit
        sys.stdout.flush()
        if score[max_bit] > SCORE_THRESHOLD:
            #print ">>",max_bit, max_bit in ('1', '0')
                current_bit = max_bit
                new = 0
                score = dict.fromkeys(score, 0)
                if current_bit in ('1', '0'):
                    char_bits += current_bit
                elif current_bit == ' ':
                    character = convert_morse_to_char(char_bits)
                    if character == -1:
                        print "[wrong bits " + char_bits + "]"
                    else:
                        print ">>",character
                    char_bits = ''
                elif current_bit == '.':
                    pass




