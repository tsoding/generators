#!/usr/bin/python

def firstn(n):
    i = 0
    while i < n:
        yield i
        i += 1

g = firstn(3)
print(g.__next__())
print(g.__next__())
print(g.__next__())
print(g.__next__())
