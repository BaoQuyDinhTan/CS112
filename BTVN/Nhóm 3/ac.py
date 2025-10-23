import sys
input = sys.stdin.readline
n = int(input())
x = []
for i in range(n):
    a, b = map(int, input().split())
    x.append((b-a,b+a))
x.sort()
ans = 0
mxy = 0
for i in reversed(range(n)):
    xi, yi = x[i]
    if i > 0:
        xj, yj = x[i-1]
        if xi == xj and yi == yj:
            continue
    if yi > mxy:
        ans += 1
        mxy = yi
print(ans)