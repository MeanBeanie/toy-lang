# toy-lang

A very poorly functioning interpreted coding language

the interpreter runs in c++

example script, runs the fibonacci sequence, do note that variables cannot be assigned to other variables without a +0
```
int x = 1
int y = 1
int t = 0

loop:
print x
t = x + y
x = y + 0
y = t + 0
if x > 1000
return
goto loop
```
