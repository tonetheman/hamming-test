#!/usr/bin/env python
'''
SHA1 digest search
Copyright 2009 by Brian C. Lane <bcl@brianlane.com>
All Rights Reserved

This application uses brute force to find the closest SHA1 digest to a target digest of an
unknown string.

'''
from hashlib import sha1
import random
import sys
# from multiprocessing import Process

# Number of concurrent processes to run
NUM_CORES=2
WORD_DICT_SIZE=1000


class hamming(object):
    '''
    Calculate the binary Hamming difference (number of different bits) using a table of
    bit counts and XOR method
    '''
    def __init__(self):
        # Build a table of bitcounts for bytes 0-255
        self.bitcount = []
        for x in range(256):
            self.bitcount.append(self.bits(x))

    def bits(self, n):
        '''
        Calculate number of 1 bits in an integer

        @param n integer value
        '''
        x = 0
        while n > 0:
            x = x + (n & 1)
            n = n >> 1
        return x

    def distance(self, d1, d2):
        '''
        Calculate the Hamming distance using table and XOR method

        @param d1 digest as a binary string
        @param d2 digest as a binary string
        '''
        assert len(d1) == len(d2)
        return sum([self.bitcount[ord(x)^ord(y)] for x,y in zip(d1, d2)])


class contest(object):
    '''
    Execute the contest code.
    Pick 12 random words from a 1000 word dictionary and compare its SHA1 digest
    to a target digest

    @param id Core id, used for writing a unique logfile
    @param wordfile location of 1000 word dictionary, 1 per line
    @param targetHexdigest ASCII HEX digest to try and match
    '''
    def __init__(self, id, wordfile, targetHexdigest):
        self.id = id
        print "Running #%d" % (self.id)

        self.wordlist = open(wordfile, 'r').readlines()
        newWordlist = [w.strip() for w in self.wordlist]
        self.wordlist = newWordlist
        if len(self.wordlist) != WORD_DICT_SIZE:
            sys.stderr.write("Error, %s dictionary is %d long, not %d!\n" % (wordfile,len(self.wordlist),WORD_DICT_SIZE))
            return

        target = targetHexdigest.decode('hex')
        f = open('./core-%d.log' % (self.id), 'a')
        f.write("Target Hexdigest: %s" % (targetHexdigest))
        f.close()
        
        h = hamming()
        tries = 0
        bestHamming = 160
        bestWords = ''
        while 1:
            t = self.generateWordList()
            d = h.distance(sha1(t).digest(), target)
            if d < bestHamming:
                bestHamming = d
                bestWords = t
                f = open('./core-%d.log' % (self.id), 'a')
                f.write("%d (%s)\n(%s)\n\n" % (bestHamming, sha1(t).hexdigest(), bestWords))
                f.close()

            tries += 1
            if (tries % 100000) == 0:
                sys.stdout.write("%d" % (self.id))
                sys.stdout.flush()

    def generateWordList(self):
        """
        Generate a list of 12 random words from the word list
        """
        return " ".join([random.choice(self.wordlist) for x in range(0,12)])


def run_contest(id, wordfile, targetHexdigest):
    c = contest(id, wordfile, targetHexdigest)


if __name__ == '__main__':
	import threading
	t = threading.Thread(None, run_contest, "thread-1", (1, "./words", 'd325e29428175a863b105555f086d0fd08bc5a27'))
	t.start()

	t2 = threading.Thread(None, run_contest, "thread-2", (2, "./words", 'd325e29428175a863b105555f086d0fd08bc5a27'))
	t2.start()
