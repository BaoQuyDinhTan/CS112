import sys
input = sys.stdin.readline
n = int(input())
x = []
for i in range(n):
    a, b = map(int, input().split())
    x.append((b - a, b + a))
x = list(set(x))

ans = 0
for i in range(len(x)):
    Flag = True
    ai, bi = x[i]
    for j in range(len(x)):
        if i == j:
            continue
        aj, bj = x[j]
        if ai <= aj and bi <= bj:
            Flag = False
    ans += Flag

print(ans)
