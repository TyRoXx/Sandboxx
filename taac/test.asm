push con '?'
push con 'b'
push con 1.34
pull con 2
getPull adr loop
adr KEY := val loop

adr abc := con here
adr loop := con 1000
adr adr loop := con 456
goto val abc

1 loop con 0x12
2
3
4
abc con 'a'
str con "hallo"
strp con str

here nop
weiter nop
adr KEY := val val strp
adr strp += con 1
if val strp <> con strp then goto con weiter

adr loop := val abc + con 0
adr loop += val abc - con 1
adr loop *= val abc / con 2
adr loop %= val abc + con 4
adr loop /= val abc | con 1
adr loop |= val abc ^ con 4

adr loop -= val abc + con 1
adr loop /= val abc * con 2
adr loop ^= val abc % con 4
adr loop &= val abc / con 1
adr loop += val abc | con 4

adr a := con 2.0
adr b := con 0.5000
adr c := con 0.
adr c +f= val a +f val b
adr c -f= val a -f val b
adr c *f= val a *f val b
adr c /f= val a /f val b

adr loop := val loop + val abc
if val loop <> con 0x4567000034000 then goto con end
nop
gg nop
32 h nop
end nop

stop

arr con 0
con 1
con 2
con 3
con 4

a
b
c

KEY
STACK
